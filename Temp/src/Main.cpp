#include <QCat.h>
//--EntryPoint begin--//
#include "QCat/Core/EntryPoint.h"
//--EntryPoint end--//

class TempClass : public QCat::Application
{
public:
	TempClass()
		:Application("QCat App Test", QCat::RenderAPI::API::OpenGL)
	{
		
	}
	~TempClass()
	{
	}

};

QCat::Application* QCat::CreateApplication()
{
	return new TempClass();
}

//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/stdout_color_sinks.h"
//#include "spdlog/fmt/ostr.h"
//
//int main()
//{
//	std::shared_ptr<spdlog::logger> corelogger;
//	spdlog::set_pattern("%^[%T] %n: %v%$");
//	corelogger = spdlog::stdout_color_mt("Test");
//	corelogger->set_level(spdlog::level::trace);
//	 std::string name = " sdawd";
//	 std::string name2 = "awdawd";
//	corelogger->error("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name.c_str(), name2.c_str());
//
//	return 0;
//}