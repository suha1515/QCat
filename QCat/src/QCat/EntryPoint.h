#pragma once

#ifdef QCAT_PLATFORM_WINDOWS

extern QCat::Application* QCat::CreateApplication();


int main(int argc,char** argv)
{
	QCat::Log::Init();
	QCAT_CORE_WARN("Initailized Log!");

	auto app = QCat::CreateApplication();
	app->Run();
	delete app;
}


#endif
