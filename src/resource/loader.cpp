#include "resource/loader.h"
#include "global.h"
#include "assimp/matrix4x4.h"
#include "definitions.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "resource/resource_components.h"

#include "Buffer.h"

namespace
{
	void create_vertices(aiMesh* p_assMesh, const aiMatrix4x4& trans, std::vector<hyv::vertex>& vertices, glm::vec3 scale)
	{
		for (unsigned int i = 0; i < p_assMesh->mNumVertices; i++)
		{
			hyv::vertex v;

			aiVector3D assPos = trans * p_assMesh->mVertices[i];

			if (!p_assMesh->HasTextureCoords(0))
			{
				throw hyv::resource::no_texture_coords_error("Imported mesh" + std::string(p_assMesh->mName.C_Str()) + " has no texture coordinates");
			}

			glm::vec3 pos{ assPos.x * scale.x, assPos.y * scale.y, assPos.z * scale.z };
			v.pos = pos;

			auto normalMatrix = trans;
			normalMatrix.Inverse();
			normalMatrix.Transpose();
			aiVector3D assNormal = normalMatrix * p_assMesh->mNormals[i];
			aiVector3D assTangent = trans * p_assMesh->mTangents[i];
			aiVector3D assBitangent = trans * p_assMesh->mBitangents[i];
			glm::vec3 normal{ assNormal.x, assNormal.y, assNormal.z };
			glm::vec3 tangent{ assTangent.x, assTangent.y, assTangent.z };
			glm::vec3 bitangent{ assBitangent.x,assBitangent.y,assBitangent.z };


			v.normal = glm::normalize(normal);
			//v.tangent = glm::normalize(tangent);
			//v.bitangent = glm::normalize(bitangent);

			auto assUV = p_assMesh->mTextureCoords[0][i];
			glm::vec2 uvMap{ assUV.x,1 - assUV.y };
			v.uv = uvMap;

			vertices.push_back(v);
		}
	}

	void create_indices(aiMesh* p_assMesh, std::vector<hyv::u32>& indices)
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < p_assMesh->mNumFaces; i++)
		{
			aiFace& face = p_assMesh->mFaces[i];

			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
				count++;
			}
		}
	}
}

hyv::resource::static_mesh_bundle hyv::resource::asset_loader::create_mesh(hyv::u64 vertices_size, hyv::u64 indices_size, hyv::u64 iAt, hyv::u64 vAt, const char* name)
{
	static_mesh_gpu m_gpu;
	m_gpu.numIndices = indices_size;
	m_gpu.offsetIndex = iAt;
	m_gpu.numVertices = vertices_size;
	m_gpu.offsetVertex = vAt;

	static_mesh_cpu m_cpu;
	m_cpu.vertices = std::make_shared<std::vector<vertex>>(vAt, vAt + vertices_size);
	m_cpu.indices = std::make_shared<std::vector<u32>>(iAt, iAt + indices_size);

	static_mesh_bundle sm = { std::move(m_cpu),m_gpu };

	res.name_to_static_mesh[std::string(name)] = sm;
	return sm;
}

void hyv::resource::asset_loader::process_node_static_mesh(std::vector<static_mesh_bundle>& bundles, const aiNode& node, const aiScene& scene, static_mesh_loader_options options)
{
	for (unsigned int i = 0; i < node.mNumChildren; i++)
	{
		aiNode& child = *node.mChildren[i];
		child.mTransformation = options.pretransform ? node.mTransformation * child.mTransformation : child.mTransformation;
		process_node_static_mesh(bundles, child, scene, options);
		for (unsigned int j = 0; j < child.mNumMeshes; j++)
		{
			auto* mesh = scene.mMeshes[child.mMeshes[j]];
			u64 global_vertices_at = vertices.size();
			u64 global_indices_at = indices.size();
			create_vertices(mesh, child.mTransformation, vertices, options.size_factor);
			create_indices(mesh, indices);
			auto vertices_size = vertices.size() - global_vertices_at;
			auto indices_size = indices.size() - global_indices_at;
			auto bundle = create_mesh(global_vertices_at, global_indices_at, indices_size, vertices_size, mesh->mName.C_Str());
			bundles.push_back(std::move(bundle));
		}
	}
}

hyv::resource::asset_loader::asset_loader(resource& res) : res(res) {
	auto mb = res.get_world().get_mut<global_mesh_buffer>();
	mb->index_buffer.Release();
	mb->vertex_buffer.Release();
	res.name_to_static_mesh.clear();
}

std::vector<hyv::resource::static_mesh_bundle> hyv::resource::asset_loader::load_static_mesh(const char* file, static_mesh_loader_options options)
{
	Assimp::Importer imp;

	int assimp_options =
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_FlipWindingOrder |
		(options.optimize ? aiProcess_OptimizeMeshes : 0) |
		(options.merge ? aiProcess_PreTransformVertices : 0)
		;

	const aiScene* scene = imp.ReadFile(file, assimp_options);
	std::vector<static_mesh_bundle> bundles;

	if (scene)
	{
		process_node_static_mesh(bundles, *scene->mRootNode, *scene, options);
	}

	else
	{
		throw mesh_import_error("Something went wrong parsing the model" + std::string(file));
	}

	return bundles;
}

hyv::resource::asset_loader::~asset_loader()
{
	auto mb = res.get_world().get_mut<global_mesh_buffer>();

	//See: https://github.com/DiligentGraphics/DiligentSamples/blob/master/Tutorials/Tutorial02_Cube/src/Tutorial02_Cube.cpp
	dl::BufferDesc vbd;
	vbd.Name = "Global Vertex Buffer";
	vbd.Usage = dl::USAGE_IMMUTABLE;
	vbd.BindFlags = dl::BIND_VERTEX_BUFFER;
	vbd.Size = vertices.size() + sizeof(vertex);
	dl::BufferData vdata;
	vdata.pData = vertices.data();
	vdata.DataSize = vbd.Size;
	Dev->CreateBuffer(vbd, &vdata, &mb->vertex_buffer);

	dl::BufferDesc ibd;
	ibd.Name = "Global Index Buffer";
	ibd.Usage = dl::USAGE_IMMUTABLE;
	ibd.BindFlags = dl::BIND_INDEX_BUFFER;
	ibd.Size = sizeof(u32) * indices.size();
	dl::BufferData idata;
	idata.pData = indices.data();
	idata.DataSize = ibd.Size;
	Dev->CreateBuffer(ibd, &idata, &mb->index_buffer);
	res.get_world().modified<global_mesh_buffer>();
}