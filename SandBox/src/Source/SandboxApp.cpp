#include <QCat.h>

// ---- Entry Point----//
#include "QCat/Core/EntryPoint.h"
// --------------------//
#include <Imgui/imgui.h>
#include "API/Opengl/OpenGLShader.h"
#include "API/DirectX11/DX11_Blender.h"

#include "QCat/Renderer/Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

#include "ExamLayer.h"

class Sandbox : public QCat::Application
{
public:
	Sandbox()
		:Application("QCat App",QCat::RenderAPI::API::OpenGL)
	{
		PushLayer(new QCat::ExamLayer());
	}
	~Sandbox()
	{
	}

};

QCat::Application* QCat::CreateApplication()
{
	return new Sandbox();
}