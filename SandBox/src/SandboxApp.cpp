#include <QCat.h>


class ExamLayer : public QCat::Layer
{
public:
	ExamLayer()
		:Layer("Example")
	{}
	void OnUpdate() override
	{
		//QCAT_INFO("ExampleLayer::Update");
	}
	void OnEvent(QCat::Event& event) override
	{
		QCAT_TRACE("{0}", event);
	}
};

class Sandbox : public QCat::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExamLayer());
		PushOverlay(new QCat::ImGuiLayer());
	}
	~Sandbox()
	{
	}
};

QCat::Application* QCat::CreateApplication()
{
	return new Sandbox();
}