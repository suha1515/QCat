#include <QCat.h>

class Sandbox : public QCat::Application
{
public:
	Sandbox()
	{
		
	}
	~Sandbox()
	{
	}
};

QCat::Application* QCat::CreateApplication()
{
	return new Sandbox();
}