#pragma once
#include "QCat/Core/QCatCore.h"
#include "QCat/Renderer/Texture.h"
#include "QCat/Renderer/enums.h"
#include "PassOutput.h"
namespace QCat
{
	class PassInput
	{
	public:
		virtual ~PassInput() = default;

		virtual void Bind(PassOutput& output) = 0;

		void SetLink(const std::string& passName, const std::string& outputName)
		{
			this->passName = passName;
			this->outputName = outputName;
		}
		std::string GetName() { return name; }
		std::string GetPassName() { return passName; }
		std::string GetOutputName() { return outputName; }
		ResourceType GetType() { return type; }

	protected:
		PassInput(const std::string& name, ResourceType type)
			:name(name),type(type)
		{}
	private:
		std::string name;
		std::string passName;
		std::string outputName;
		ResourceType type;
	};

	class TextureInput : public PassInput
	{
	public:
		static Scope<PassInput> Make(const std::string& name, Ref<Texture>& texture)
		{
			return CreateScope<TextureInput>(name, texture);
		}
		TextureInput(const std::string& name,Ref<Texture>& texture)
			:PassInput(name,ResourceType::Texture),m_texture(texture)
		{}
		void Bind(PassOutput& output);
	private:
		Ref<Texture>& m_texture;
	
	};
	template<typename T>
	class DataInput : public PassInput
	{
	public:
		static Scope<PassInput> Make(const std::string& name, Ref<T>& pdata,DataType type)
		{
			return CreateScope<DataInput>(name, pdata, type);
		}
		DataInput(const std::string& name, Ref<T>& pdata,DataType type)
			:PassInput(name,ResourceType::Data),type(type),pData(pdata)
		{}
		void Bind(PassOutput& output)
		{
			auto type = output.GetType();
			if (GetType() != type)
			{
				QCAT_CORE_ERROR("{0} Pass Error : Input and output type missmatch", GetPassName().c_str());
			}
			else
			{
				DataOutput<T>* dataoutput = dynamic_cast<DataOutput<T>*>(&output);

				DataType type = dataoutput->GetDataType();
				if (this->type != type)
					QCAT_CORE_ERROR("{0} Pass Error : InputData Type and OutputData Type missmatch!", GetPassName().c_str());

				pData = dataoutput->GetData();
			}
		}
	private:
		Ref<T>& pData;
		DataType type;
	};
}