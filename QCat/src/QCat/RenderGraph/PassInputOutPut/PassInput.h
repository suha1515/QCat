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
		PassInput(const std::string& name) {this->name = name;}
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
			:PassInput(std::move(name)),m_texture(texture)
		{}
		void Bind(PassOutput& output);
	private:
		Ref<Texture> m_texture;
	
	};
}