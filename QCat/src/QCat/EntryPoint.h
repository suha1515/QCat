#pragma once

#ifdef QCAT_PLATFORM_WINDOWS

extern QCat::Application* QCat::CreateApplication();
int main(int argc,char** argv)
{
	QCat::Log::Init();
	QCat::Log::GetCoreLogger()->warn("Initialized Log!");
	QCat::Log::GetClientLogger()->info("Hi");

	auto app = QCat::CreateApplication();
	app->Run();
	delete app;
}

#endif
