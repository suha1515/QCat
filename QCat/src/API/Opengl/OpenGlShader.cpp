#include "qcpch.h"
#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include "QCat/Uitiliy/Timer.h"


namespace QCat
{
	namespace Utils
	{
		static std::string SPirvBaseTypeToString(spirv_cross::SPIRType::BaseType type)
		{
			switch(type)
			{
			case spirv_cross::SPIRType::BaseType::Char:		return "Char"; break;
			case spirv_cross::SPIRType::BaseType::Float:	return "Float"; break;
			case spirv_cross::SPIRType::BaseType::Half:		return "Half-Float"; break;
			case spirv_cross::SPIRType::BaseType::Int:		return "Int"; break;
			case spirv_cross::SPIRType::BaseType::Int64:	return "Int64"; break;
			case spirv_cross::SPIRType::BaseType::Short:	return "Short"; break;
			case spirv_cross::SPIRType::BaseType::SByte:	return "Signed-Byte"; break;
			case spirv_cross::SPIRType::BaseType::Struct:	return "Struct"; break;
			case spirv_cross::SPIRType::BaseType::UByte:	return "Unsigned-Byte"; break;
			case spirv_cross::SPIRType::BaseType::UInt:		return "Unsigned-Int"; break;
			case spirv_cross::SPIRType::BaseType::UInt64:	return "Unsigned-Int64"; break;
			case spirv_cross::SPIRType::BaseType::UShort:	return "Unsigned-Short"; break;
			}
		}
		/*static ShaderDataType SPirvBaseTypeToString(spirv_cross::SPIRType::BaseType type)
		{
			
		}*/
		
		ShaderDataType GetDataType(size_t vecSize, size_t column, spirv_cross::SPIRType::BaseType type)
		{
			switch (vecSize)
			{
			case 1: 
				switch (type)
				{
				case spirv_cross::SPIRType::BaseType::Float:	return  ShaderDataType::Float; break;
				case spirv_cross::SPIRType::BaseType::Int:		return  ShaderDataType::Int; break;
				}
				break;
			case 2: 
				switch (column)
				{
				case 1: 
					switch (type)
					{
					case spirv_cross::SPIRType::BaseType::Float:	return  ShaderDataType::Float2; break;
					case spirv_cross::SPIRType::BaseType::Int:		return  ShaderDataType::Int2; break;
					}
					break;
				case 2: break;
				case 3: break;
				case 4: break;
				}
				break;
			case 3: 
				switch (column)
				{
				case 1:
					switch (type)
					{
					case spirv_cross::SPIRType::BaseType::Float:	return  ShaderDataType::Float3; break;
					case spirv_cross::SPIRType::BaseType::Int:		return  ShaderDataType::Int3; break;
					}
					break;
				case 2: break;
				case 3: return ShaderDataType::Mat3; break;
				case 4: break;
				}
				break;
			case 4: 
				switch (column)
				{
				case 1:
					switch (type)
					{
					case spirv_cross::SPIRType::BaseType::Float:	return  ShaderDataType::Float4; break;
					case spirv_cross::SPIRType::BaseType::Int:		return  ShaderDataType::Int4; break;
					}
					break;
				case 2: break;
				case 3: break;
				case 4: return ShaderDataType::Mat4; break;
				}
				break;
			}
		}
		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;
			if (type == "geometry")
				return GL_GEOMETRY_SHADER;

			QCAT_CORE_ASSERT(false, "Unknown shader Type!");
			return 0;
		}
		static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER: return shaderc_glsl_vertex_shader;
			case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
			case GL_GEOMETRY_SHADER: return shaderc_glsl_geometry_shader;
			}
			QCAT_CORE_ASSERT(false,"GL_TO_SHADERC Stage type error!");
			return (shaderc_shader_kind)0;
		}
		static const char* GLShaderStageToString(GLenum stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER";
			case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER";
			case GL_GEOMETRY_SHADER: return "GL_GEOMETRY_SHADER";
			}
			QCAT_CORE_ASSERT(false, "GL_TO_SHADERC Stage type error!");
			return nullptr;
		}
		static const char* GetCacheDirectory()
		{
			return "Asset/cache/shaders/opengl";
		}
		static void CreateCacheDirectory()
		{
			std::string cacheDirectory = GetCacheDirectory();
			if (!std::filesystem::exists(cacheDirectory))
				std::filesystem::create_directories(cacheDirectory);
		}
		static const char* GLShaderStatgeCachedOpenGLFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER: return ".cached_opengl.vert";
			case GL_FRAGMENT_SHADER: return ".cached_opengl.frag";
			}
			QCAT_CORE_ASSERT(false);
			return "";
		}
		static const char* GLShaderStageCachedVulkanFileExtension(uint32_t stage)
		{
			switch (stage)
			{
			case GL_VERTEX_SHADER:    return ".cached_vulkan.vert";
			case GL_FRAGMENT_SHADER:  return ".cached_vulkan.frag";
			}
			QCAT_CORE_ASSERT(false);
			return "";
		}

	}
	

	OpenGLShader::OpenGLShader(const std::string& filepath)
		:m_FilePath(filepath)
	{
		QCAT_PROFILE_FUNCTION();

		Utils::CreateCacheDirectory();

		auto begin = filepath.find_last_of('.');
		std::string extension = filepath.substr(begin + 1, filepath.length());
		QCAT_CORE_ASSERT(extension == "glsl", "Wrong format for DXShader");

		//Compile(shaderSources);
		// assets/shaders/Texture.glsl
		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');

		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_name = filepath.substr(lastSlash, count);

		std::string source = ReadFile(filepath);
		auto shaderSources = PreProcess(source);

		{
			Timer timer;
			CompileVulkanBinary(shaderSources);
			CompileOpenGLBinary();
			CreateProgram();
			QCAT_CORE_WARN("{0} : Shader creation took {1}", m_name,timer.Peek());
		}

		
	
	}
	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& geometrySrc, const std::string& pixelSrc)
		:m_name(name)
	{
		QCAT_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> sources;
		std::string source;
		if (vertexSrc != "")
		{
			source = ReadFile(vertexSrc);
			sources[GL_VERTEX_SHADER] = source;
		}
		if (pixelSrc != "")
		{
			source = ReadFile(pixelSrc);
			sources[GL_FRAGMENT_SHADER] = source;
		}
		if (geometrySrc != "")
		{
			source = ReadFile(geometrySrc);
			sources[GL_GEOMETRY_SHADER] = source;
		}
		
		CompileVulkanBinary(sources);
		CompileOpenGLBinary();
		CreateProgram();
		//Compile(sources);
	}
	OpenGLShader::~OpenGLShader()
	{
		QCAT_PROFILE_FUNCTION();

		glDeleteProgram(m_renderID);
	}
	// Read from Shader fild .glsl
	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		QCAT_PROFILE_FUNCTION();

		std::string result;
		// ios::in means read mode, ios::binary means it will read data as binary
		std::ifstream in(filepath, std::ios::in | std::ios::binary);// ifstream closes itself due to RAII
		if (in)
		{
			// seekg is move the cursor ios::end means end of file 
			// so based on end of file this func will move the cursor as first parameter value
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			// after reading you have to call close function
		}
		else
		{
			QCAT_CORE_ERROR("Could not open file '{0}'", filepath);
		}
		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		QCAT_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> shaderSources;

		// To specify type find '#type' in file, and get position of that string
		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			//find first end of the line in #type line , P.S in Windows platform all string end with '\r\n' in Linux end with '\n'
			size_t eol = source.find_first_of("\r\n", pos);
			QCAT_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			// without #type's length and space(+1) we can get first position of type info
			size_t begin = pos + typeTokenLength + 1;
			// and using substr can get us name of type 
			std::string type = source.substr(begin, eol - begin);
			QCAT_CORE_ASSERT(Utils::ShaderTypeFromString(type), "Invalid shader type specified!");

			// find_first_not_of will find position without '\r\n' character so this will find first position of next line
			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			// and find first position of typetoken(#type)
			pos = source.find(typeToken, nextLinePos);
			// and get string from nextLinePos as amount of pos - nextLinePos
			shaderSources[Utils::ShaderTypeFromString(type)] =
				source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return shaderSources;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		QCAT_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();
		QCAT_CORE_ASSERT(shaderSources.size() <= 3, "we only support 3 shaders");
		std::array<GLenum, 3> glShaderIDs = { 0,0,0 };
		int glShaderIDIndex = 0;
		for (auto& kv : shaderSources)
		{
			GLenum type = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(type);

			const GLchar* sourceCstr = source.c_str();
			glShaderSource(shader, 1, &sourceCstr, 0);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader,GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				QCAT_CORE_ERROR("{0}", infoLog.data());
				QCAT_CORE_ASSERT(false, "Shader compilation failure!");
				break;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}
		m_renderID = program;

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);

			for (auto id : glShaderIDs)
				glDeleteShader(id);

			QCAT_CORE_ERROR("{0}", infoLog.data());
			QCAT_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			if(id!=0)
				glDetachShader(program, id);
		}
			
	}

	void OpenGLShader::CompileOpenGLBinary()
	{
		auto& shaderData = m_openglSPIRV;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		shaderData.clear();
		m_openGLSourceCode.clear();
		for (auto&& [stage, spirv] : m_vulkanSPIRV)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() 
							+ Utils::GLShaderStatgeCachedOpenGLFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				spirv_cross::CompilerGLSL glslCompilrer(spirv);
				m_openGLSourceCode[stage] = glslCompilrer.compile();
				auto& source = m_openGLSourceCode[stage];
				shaderc::SpvCompilationResult Module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (Module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					QCAT_CORE_ERROR(Module.GetErrorMessage());
					QCAT_CORE_ASSERT(false);
				}
				shaderData[stage] = std::vector<uint32_t>(Module.cbegin(), Module.cend());
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
	}

	void OpenGLShader::CompileVulkanBinary(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		GLuint program = glCreateProgram();

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = false;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		std::filesystem::path cacheDirectory = Utils::GetCacheDirectory();

		auto& shaderData = m_vulkanSPIRV;
		shaderData.clear();
		for (auto&& [stage, source] : shaderSources)
		{
			std::filesystem::path shaderFilePath = m_FilePath;
			std::filesystem::path cachedPath = cacheDirectory / (shaderFilePath.filename().string() 
							 + Utils::GLShaderStageCachedVulkanFileExtension(stage));

			std::ifstream in(cachedPath, std::ios::in | std::ios::binary);
			if (in.is_open())
			{
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				auto& data = shaderData[stage];
				data.resize(size / sizeof(uint32_t));
				in.read((char*)data.data(), size);
			}
			else
			{
				shaderc::SpvCompilationResult Module = compiler.CompileGlslToSpv(source, Utils::GLShaderStageToShaderC(stage), m_FilePath.c_str(), options);
				if (Module.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					QCAT_CORE_ERROR(Module.GetErrorMessage());
					QCAT_CORE_ASSERT(false);
				}
				shaderData[stage] = std::vector<uint32_t>(Module.cbegin(), Module.cend());
				std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = shaderData[stage];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
				}
			}
		}
		for (auto&& [stage, data] : shaderData)
			Reflect(stage, data);
	}

	void OpenGLShader::CreateProgram()
	{
		GLuint program = glCreateProgram();
		
		std::vector<GLuint> shaderIDs;
		for (auto&& [stage, spirv] : m_openglSPIRV)
		{
			GLuint shaderID = shaderIDs.emplace_back(glCreateShader(stage));
			glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
			glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
			glAttachShader(program, shaderID);
		}

		glLinkProgram(program);
		GLint isLinked;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());
			QCAT_CORE_ERROR("Shader linking faild({0} : \n{1}", m_FilePath, infoLog.data());
			glDeleteProgram(program);
			for (auto id : shaderIDs)
				glDeleteShader(id);
		}

		for (auto id : shaderIDs)
		{
			glDetachShader(program, id);
			glDeleteShader(id);
		}
		m_renderID = program;
	}
	LayoutElement* SetArrayInfo(LayoutElement* playout, size_t arrayCount)
	{
		playout->ArraySet(ShaderDataType::Array, arrayCount);
		return &playout->GetArrayElement();
	}
	void GetStructInformation(LayoutElement* playout,const spirv_cross::Resource& resource,spirv_cross::Compiler& comp, const spirv_cross::SPIRType memberType)
	{
		int memberCount = memberType.member_types.size();
		for (int i = 0; i < memberCount; ++i)
		{
			auto member_type = comp.get_type(memberType.member_types[i]);

			int arraySize = member_type.array[0];
			int width = member_type.width;
			int vecsize = member_type.vecsize;
			int column = member_type.columns;
			std::string type = Utils::SPirvBaseTypeToString(member_type.basetype);
			// Get name within this struct
			//std::string name = comp.get_name(memberType.member_types[i]);
			std::string name = comp.get_member_name(memberType.self, i);
			// Get size within this struct
			size_t member_size = comp.get_declared_struct_member_size(memberType, i);
			// Get offset within this struct
			size_t offset = comp.type_struct_member_offset(memberType, i);

			QCAT_CORE_TRACE("====Member{0}========================", i);
			QCAT_CORE_TRACE("	^---Name  = {0}", name);
			QCAT_CORE_TRACE("	^---Offset= {0}", offset);
			QCAT_CORE_TRACE("	^---Size  = {0}", member_size);
			QCAT_CORE_TRACE("	^---Type  = {0}", type);
			// 배열크기
			QCAT_CORE_TRACE("	^---arraySize  = {0}", arraySize);
			QCAT_CORE_TRACE("	^---width  = {0}", width);
			// 벡터일경우 벡터 크기
			QCAT_CORE_TRACE("	^---vecsize  = {0}", vecsize);
			// 행렬일경우 행을의미
			QCAT_CORE_TRACE("	^---column  = {0}", column);
	
			ShaderDataType dataType = Utils::GetDataType(vecsize, column, member_type.basetype);
			if (type == "Struct")
				dataType =  ShaderDataType::Struct;
			// 배열의 경우.
			if (member_type.array.size() > 0)
			{
				(*playout).Add(ShaderDataType::Array, name);
				LayoutElement* layout = &(*playout)[name];
				//LayoutElement* layout = &(*playout)[name].GetArrayElement();
				for (int i = 0; i <member_type.array.size()-1; ++i)
				{
					layout = SetArrayInfo(layout, member_type.array[i]);
				}
				size_t index = member_type.array.size() - 1;
				size_t count = member_type.array[index];
				if (dataType == ShaderDataType::Struct)
				{
					if (member_type.member_types.size() > 0)
					{
						layout->ArraySet(dataType, count);
						GetStructInformation(&(layout->GetArrayElement()), resource, comp, member_type);
					}
				}
				else
					layout->ArraySet(dataType, count);
			}
			else if (member_type.member_types.size() > 0)
			{
			// 구조체일경우 멤버 사이즈를 의미.
				QCAT_CORE_TRACE("	^---MemberSize={0}", member_type.member_types.size());
				playout->Add(dataType, name);
				GetStructInformation(&(*playout)[name], resource, comp, member_type);
			}			
			// 구조체도 배열도 아닌경우
			else
			{
				ShaderDataType dataType = Utils::GetDataType(vecsize, column, member_type.basetype);
				(*playout).Add(dataType, name);
			}
				
		}
	}
	LayoutElement GetConstantBufferLayout(spirv_cross::Compiler& comp, const spirv_cross::Resource& resource)
	{
		const auto& bufferType = comp.get_type(resource.base_type_id);
		LayoutElement layout(ShaderDataType::Struct,resource.name);
		GetStructInformation(&layout,resource, comp, bufferType);
		return layout;
	}

	void OpenGLShader::Reflect(GLenum stage, const std::vector<uint32_t>& shaderData)
	{
		spirv_cross::Compiler compiler(shaderData);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		QCAT_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::GLShaderStageToString(stage), m_FilePath);
		QCAT_CORE_TRACE("		{0} uniform buffers", resources.uniform_buffers.size());
		QCAT_CORE_TRACE("		{0} resources", resources.sampled_images.size());
		QCAT_CORE_TRACE("-Texture Resources-");
		for (const auto& texture : resources.sampled_images)
		{
			const auto& textureType = compiler.get_type(texture.base_type_id);
			uint32_t binding = compiler.get_decoration(texture.id, spv::DecorationBinding);
			std::string name = compiler.get_name(texture.id);
			QCAT_CORE_TRACE("	Name    = {0}", name);
			QCAT_CORE_TRACE("	Binding = {0}", binding);
		}
		QCAT_CORE_TRACE("-Uniform Buffers-");
		
		for (const auto& resource : resources.uniform_buffers)
		{
			const auto& bufferType = compiler.get_type(resource.base_type_id);
			uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			int memberCount = bufferType.member_types.size();

			QCAT_CORE_TRACE("	Name     = {0}", resource.name);
			QCAT_CORE_TRACE("	Size     = {0}", bufferSize);
			QCAT_CORE_TRACE("	Binding  = {0}", binding);
			QCAT_CORE_TRACE("	Memebers = {0}", memberCount);
			LayoutElement layout = GetConstantBufferLayout(compiler, resource);
			//layout.Finalize();
			m_constantBuffer.emplace(resource.name, ElementBuffer(layout));
		}
	}

	void OpenGLShader::Bind() const
	{
		QCAT_PROFILE_FUNCTION();

		glUseProgram(m_renderID);
	}
	void OpenGLShader::UnBind() const
	{
		QCAT_PROFILE_FUNCTION();

		glUseProgram(0);
	}
	void OpenGLShader::SetInt(const std::string& name, int value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}
	void OpenGLShader::SetIntArray(const std::string& name, int* values, unsigned int count, ShaderType type)
	{
		UploadUniformIntArray(name, values, count);
	}
	void OpenGLShader::SetFloat(const std::string& name, const float& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}
	void OpenGLShader::SetFloatArray(const std::string name, float* values, unsigned int count, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();

		UploadUniformFloatArray(name, values, count);
	}
	void OpenGLShader::SetFloat2(const std::string& name, const glm::vec2& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();

		UploadUniformFloat2(name, value);
	}
	void OpenGLShader::SetFloat3(const std::string& name,const glm::vec3& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}
	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}
	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}
	void OpenGLShader::SetBool(const std::string& name, const bool& value, ShaderType type)
	{
		QCAT_PROFILE_FUNCTION();

		UploadUniformBool(name, value);
	}
	void OpenGLShader::SetFloat3u(const std::string& uniformname, const std::string& valuename, const glm::vec3& value)
	{
	}
	void OpenGLShader::SetFloat4u(const std::string& uniformname, const std::string& valuename, const glm::vec4& value)
	{
	}
	void OpenGLShader::SetMat4u(const std::string& uniformname, const std::string& valuename, const glm::mat4& value)
	{
	}
	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, unsigned int count)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniform1iv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloatArray(const std::string& name, float* values, unsigned int count)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniform1fv(location, count, values);
	}
	void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniform1f(location, value);
	}
	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniform2f(location, value.x,value.y);
	}
	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniform3f(location, value.x, value.y, value.z);
	}
	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
	void OpenGLShader::UploadUniformBool(const std::string& name, const bool& value)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniform1i(location, value);
	}
	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		GLint location = glGetUniformLocation(m_renderID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}
	// VertexShader
	OpenGLVertexShader::OpenGLVertexShader(const std::string& vertexSrc)
	{
		// Create an empty vertex shader handle
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = vertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			QCAT_CORE_ERROR("{0}", infoLog.data());
			QCAT_CORE_ASSERT(false, "Vertex shader compilation failure!");
			return;
		}
	}
	OpenGLVertexShader::~OpenGLVertexShader()
	{
		glDeleteShader(vertexShader);
	}
	// PixelShader
	OpenGLPixelShader::OpenGLPixelShader(const std::string& pixelSrc)
	{
		GLint isCompiled = 0;
		// Create an empty fragment shader handle
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = pixelSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);

			QCAT_CORE_ERROR("{0}", infoLog.data());
			QCAT_CORE_ASSERT(false, "Fragment shader compilation failure!");
			return;
		}
	}
	OpenGLPixelShader::~OpenGLPixelShader()
	{
		glDeleteShader(fragmentShader);
	}
	OpenGLGeoMetryShader::OpenGLGeoMetryShader(const std::string& geometrySrc)
	{
		// Create an empty vertex shader handle
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = geometrySrc.c_str();
		glShaderSource(geometryShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(geometryShader);

		GLint isCompiled = 0;
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(geometryShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(geometryShader);

			QCAT_CORE_ERROR("{0}", infoLog.data());
			QCAT_CORE_ASSERT(false, "Vertex shader compilation failure!");
			return;
		}
	}
	OpenGLGeoMetryShader::~OpenGLGeoMetryShader()
	{
		glDeleteShader(geometryShader);
	}
}