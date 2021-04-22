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
		PassOutput(const std::string& name) { this->name = name; }
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
			:PassOutput(std::move(name)),m_texture(texture)
		{}

		Ref<Texture>& GetTexture() { return m_texture; };
	private:
		Ref<Texture> m_texture;
	};


}
