#pragma once

#ifdef QCAT_PLATFORM_WINDOWS

extern QCat::Application* QCat::CreateApplication();


int main(int argc,char** argv)
{
	QCat::Log::Init();
	QCAT_CORE_WARN("Initailized Log!");
	int a = 5;
	QCAT_INFO("Hello var = {0}", a);

	auto app = QCat::CreateApplication();
	app->Run();
	delete app;
}


#endif
