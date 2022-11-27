#pragma once
#include "global.h"
#include "ShaderMacroHelper.hpp"

namespace hyv
{
	namespace rendering
	{
		enum class shader_type
		{
			Compute, Vertex, Pixel
		};

		class shader
		{
		public:
			shader() = default;
			shader(shader_type st, const std::string& file, const std::string& name);
			
			template<typename T>
			shader& add_macro(const std::string& name, T val)
			{
				m_sm.AddShaderMacro(name, val);
				return *this;
			}

			template<typename T>
			shader& update_macro(const std::string& name, T val)
			{
				m_sm.UpdateMacro(name, val);
				return *this;			
			}
			void ok();
			shader new_permutation();

			dl::IShader* get_shader_handle() { return m_shader_handle; }

		private:
			dl::RefCntAutoPtr<dl::IShader> m_shader_handle;
			dl::ShaderMacroHelper m_sm;
			shader_type m_type;
			std::string m_filename;
			std::string m_name;
		};
	}
}