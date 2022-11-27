#include "rendering/shader.h"

hyv::rendering::shader::shader(shader_type st, const std::string& file, const std::string& name)
	:m_type(st),m_filename(file),m_name(name)
{
}

void hyv::rendering::shader::ok()
{
	dl::ShaderCreateInfo ci;
	ci.SourceLanguage = dl::SHADER_SOURCE_LANGUAGE_HLSL;
	ci.ShaderCompiler = dl::SHADER_COMPILER_DXC;
	
	m_sm.Finalize();

	ci.Macros = m_sm;
	ci.pShaderSourceStreamFactory = ShaderStream;

	dl::SHADER_TYPE shader_type_dl;

	switch (m_type)
	{
	case shader_type::Vertex:
		shader_type_dl = dl::SHADER_TYPE_VERTEX;
	case shader_type::Pixel:
		shader_type_dl = dl::SHADER_TYPE_PIXEL;
	case shader_type::Compute:
		shader_type_dl = dl::SHADER_TYPE_COMPUTE;
	default:
		abort(); // WTF
	}

	ci.Desc.ShaderType = shader_type_dl;
	ci.EntryPoint = "main";
	ci.Desc.Name = m_name.c_str();
	ci.FilePath = m_filename.c_str();
	Dev->CreateShader(ci, &m_shader_handle);
}

hyv::rendering::shader hyv::rendering::shader::new_permutation()
{
	shader cpy = *this;
	cpy.m_sm.Reopen();
	return cpy;
}
