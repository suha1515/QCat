#pragma once
#include"QCat/Core/QCatCore.h"
#include "QCat/Renderer/Texture.h"
namespace QCat
{
	class PassOutput
	{
	public:
	
		virtual ~PassOutput() = default;

		std::string GetName() { return name; }
		//std::string GetPassName() { return passName; }
		//std::string GetInputName() { return inputName; }
		ResourceType GetType() { return type; }
	protected:
		PassOutput(const std::string& name,ResourceType type)
			:name(name),type(type)
		{}
	private:
		std::string name;
		//std::string passName;
		//std::string inputName;
		ResourceType type;

	private:

	};

	class TextureOutput : public PassOutput
	{
	public:
		static Scope<PassOutput> Make(const std::string& name, Ref<Texture>& texture)
		{
			return CreateScope<TextureOutput>(name, texture);
		}
		TextureOutput(const std::string& name, Ref<Texture>& texture)
			:PassOutput(name,ResourceType::Texture),m_texture(texture)
		{}

		Ref<Texture>& GetTexture() { return m_texture; };
	private:
		Ref<Texture>& m_texture;
	};

	template<typename T>
	class DataOutput : public PassOutput
	{
	public:
		static Scope<PassOutput> Make(const std::string& name , Ref<T>& pdata,DataType type)
		{
			return CreateScope<DataOutput>(name, pdata, type);
		}
		DataOutput(const std::string& name, Ref<T>& pdata,DataType type)
			:PassOutput(name,ResourceType::Data),type(type),pData(pdata)
		{}
		Ref<T>& GetData() { return pData; }
		DataType GetDataType() { return type; }
	private:
		Ref<T>& pData=nullptr;
		DataType type = DataType::None;
	};

}
