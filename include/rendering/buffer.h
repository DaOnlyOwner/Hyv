#pragma once
#include "definitions.h"
#include "Buffer.h"
#include "global.h"
#include <string>
#include "MapHelper.hpp"

namespace hyv
{
	namespace rendering
	{

		// TODO: Implement explicit resource transition

		/// <summary>
		/// Implements an structured buffer. The buffer dynamically grows.
		/// Right now, there is no way to delete data
		/// </summary>
		/// <typeparam name="T">The type of the buffer</typeparam>
		template<typename T>
		class struct_buffer
		{
		private:
			struct_buffer(const std::string& name, dl::BIND_FLAGS bind, u64 capacity)
				:name(name), bind(bind), capacity(capacity) {}
		public:
			struct_buffer() = default;
			struct_buffer(const std::string& name, const T* a, int a_size, dl::BIND_FLAGS bind = dl::BIND_FLAGS::BIND_SHADER_RESOURCE)
				:struct_buffer(name, a, a_size, a_size, bind)
			{ }

			struct_buffer(const std::string& name, const T* a, int a_size, int capacity, dl::BIND_FLAGS bind = dl::BIND_FLAGS::BIND_SHADER_RESOURCE)
				:struct_buffer(name, bind, capacity)
			{
				auto desc = get_desc();
				dl::BufferData data;
				data.DataSize = a_size * sizeof(T);
				data.pData = a;
				dataInRam.assign(a, a + a_size);
				Dev->CreateBuffer(desc, &data, &bufferHandle);
			}

			struct_buffer(const std::string& name, u64 capacity, dl::BIND_FLAGS bind = dl::BIND_FLAGS::BIND_SHADER_RESOURCE)
				:struct_buffer(name, bind, capacity)
			{
				auto desc = get_desc();
				Dev->CreateBuffer(desc, nullptr, &bufferHandle);
			}
			struct_buffer(const std::string& name, dl::BIND_FLAGS bind = dl::BIND_FLAGS::BIND_SHADER_RESOURCE)
				:name(name), bind(bind), capacity(0)
			{}

			dl::IBuffer* operator ->()
			{
				return bufferHandle.RawPtr();
			}

			bool add(const T* a, int a_size, float growth = 1.f)
			{
				bool newBufferHandle = false;

				int i;
				// TODO: Optimize for adjacent slots
				for (i = 0; i < a_size; i++)
				{
					const T* obj = a[i];
					if (free_slots.empty()) break;
					u32 slot = free_slots.pop_back();
					update(slot, *obj);
				}

				const T* rest = a + i;
				int rest_size = a_size - i;

				if (rest_size == 0) return false;

				if (dataInRam.size() + rest_size >= capacity)
				{
					grow(std::ceil((rest_size + dataInRam.size()) * growth));
					newBufferHandle = true;
				}

				Imm->UpdateBuffer(bufferHandle, dataInRam.size() * sizeof(T), rest_size * sizeof(T), rest, dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

				dataInRam.insert(dataInRam.end(), rest, rest + rest_size);
				return newBufferHandle;
			}

			void update(u64 idx, const T& val)
			{
				Imm->UpdateBuffer(bufferHandle, idx * sizeof(T), sizeof(T), &val, dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				dataInRam[idx] = val;
			}

			const T& operator[](u64 idx)
			{
				return dataInRam[idx];
			}

			dl::IBuffer* get_buffer()
			{
				return bufferHandle.RawPtr();
			}

			bool remove(u64 pos)
			{
				/*std::swap(dataInRam.back(), dataInRam.data + pos);
				dataInRam.pop_back();
				bufferHandle.Release();
				dl::BufferData bdata;
				bdata.DataSize = dataInRam.size() * sizeof(T);
				bdata.pData = dataInRam.data();
				Dev->CreateBuffer(get_desc(), bdata, &bufferHandle);
				return { dataInRam.size(),pos };*/
				
				if (pos >= dataInRam.size()) return false;
				free_slots.push_back(pos);
			}

			/*void compact()
			{
				for (auto& slot : free_slots)
				{

				}
			}*/

			void reserve(u64 amount)
			{
				if (amount > dataInRam.size())
				{
					grow(amount);
				}
			}

			u64 get_size()
			{
				return dataInRam.size();
			}

			u64 get_capacity()
			{
				return capacity;
			}

		private:

			dl::BufferDesc get_desc()
			{
				dl::BufferDesc desc;
				desc.Mode = dl::BUFFER_MODE_STRUCTURED;
				desc.Size = capacity * sizeof(T);
				desc.Name = name.c_str();
				desc.ElementByteStride = sizeof(T);
				desc.BindFlags = bind;
				return desc;
			}

			void grow(u64 newCapacity)
			{
				//u64 oldCapacity = capacity;
				capacity = newCapacity;
				auto desc = get_desc();
				
				dl::BufferData bd;
				bd.DataSize = dataInRam.size() * sizeof(T);
				bd.pData = dataInRam.data();
				dl::RefCntAutoPtr<dl::IBuffer> newBuffer;
				Dev->CreateBuffer(desc, bd, &newBuffer);
				/*if (size > 0)
				{
					Imm->CopyBuffer(bufferHandle, 0, dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
						newBuffer, 0, size * sizeof(T), dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
				}*/
				dataInRam.reserve(newCapacity);
				bufferHandle = newBuffer;
			}

		private:
			dl::RefCntAutoPtr<dl::IBuffer> bufferHandle;
			std::vector<T> dataInRam;
			std::vector<u32> free_slots;
			u64 capacity = 0;
			std::string name;
			dl::BIND_FLAGS bind;

		};

		template<typename T>
		class uniform_buffer
		{
		public:
			static_assert(sizeof(T) % 16 == 0, "A uniform buffer has to be a multiple of 16 bytes");
			uniform_buffer() {}
			uniform_buffer(const std::string& name, dl::USAGE usage = dl::USAGE_DYNAMIC)
			{
				dl::BufferDesc bdesc;
				bdesc.Name = name.c_str();
				bdesc.Usage = usage;
				bdesc.BindFlags = dl::BIND_UNIFORM_BUFFER;
				bdesc.Size = sizeof(T);
				bdesc.CPUAccessFlags = usage == dl::USAGE_DYNAMIC ? dl::CPU_ACCESS_WRITE : dl::CPU_ACCESS_NONE;
				Dev->CreateBuffer(bdesc, nullptr, &buffer_handle);
				dl::StateTransitionDesc descs[1] = { { buffer_handle,
					dl::RESOURCE_STATE_UNKNOWN,
					dl::RESOURCE_STATE_CONSTANT_BUFFER,
					dl::STATE_TRANSITION_FLAG_UPDATE_STATE } };
				Imm->TransitionResourceStates(_countof(descs), descs);
			}

			uniform_buffer(const std::string& name, dl::USAGE usage, const T& t)
			{
				dl::BufferDesc bdesc;
				bdesc.Name = name.c_str();
				bdesc.Usage = usage;
				bdesc.BindFlags = dl::BIND_UNIFORM_BUFFER;
				bdesc.Size = sizeof(T);
				bdesc.CPUAccessFlags = usage == dl::USAGE_DYNAMIC ? dl::CPU_ACCESS_WRITE : dl::CPU_ACCESS_NONE;
				dl::BufferData bdata;
				bdata.pData = &t;
				bdata.DataSize = sizeof(T);
				Dev->CreateBuffer(bdesc, &bdata, &buffer_handle);
				dl::StateTransitionDesc desc[1] = { {buffer_handle, dl::RESOURCE_STATE_UNKNOWN, dl::RESOURCE_STATE_CONSTANT_BUFFER} };
				Imm->TransitionResourceStates(_countof(desc), desc);
			}

			dl::MapHelper<T> map(dl::IDeviceContext* ctxt = Imm.RawPtr())
			{
				return dl::MapHelper<T>(ctxt, buffer_handle, dl::MAP_WRITE, dl::MAP_FLAG_DISCARD);
			}

			dl::IBuffer* get_buffer() { return buffer_handle.RawPtr(); }

			/*void Update(const T& val)
			{
				Imm->UpdateBuffer(buffer_handle, 0, sizeof(T), &val, dl::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
			}*/

		private:
			dl::RefCntAutoPtr<dl::IBuffer> buffer_handle;

		};
	}
}
