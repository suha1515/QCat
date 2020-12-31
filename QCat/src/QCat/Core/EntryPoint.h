#pragma once

#ifdef QCAT_PLATFORM_WINDOWS

extern QCat::Application* QCat::CreateApplication();


int main(int argc,char** argv)
{
	QCat::Log::Init();
	QCAT_CORE_WARN("Initailized Log!");

	QCAT_PROFILE_BEGIN_SESSION("Startup", "QCatProfile-Startup.json");
	auto app = QCat::CreateApplication();
	QCAT_PROFILE_END_SESSION();

	QCAT_PROFILE_BEGIN_SESSION("Runtime", "QCatProfile-Runtime.json");
	app->Run();
	QCAT_PROFILE_END_SESSION();

	QCAT_PROFILE_BEGIN_SESSION("Shutdown", "QCatProfile-Shutdown.json");
	delete app;
	QCAT_PROFILE_END_SESSION();
}


#endif
