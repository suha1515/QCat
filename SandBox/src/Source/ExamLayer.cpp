#include "ExamLayer.h"
#include <chrono>
namespace QCat
{
	void ExamLayer::OnAttach()
	{
		RenderCommand::SetClearColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
		computeShader =Shader::Create("Asset/shaders/hlsl/ComputShaderPractice.hlsl");

		std::vector<Data> dataA(32);
		std::vector<Data> dataB(32);

		for (int i = 0; i < 32; ++i)
		{
			dataA[i].v1 = glm::vec3(i, i, i);
			dataA[i].v2 = glm::vec2(i, 0);

			dataB[i].v1 = glm::vec3(-i, i, 0.0f);
			dataB[i].v2 = glm::vec2(0, -i);
		}

		
		
		InputBufferA = ShaderBuffer::Create(sizeof(Data), 32, BufferType::Read, dataA.data());
		InputBufferB = ShaderBuffer::Create(sizeof(Data), 32, BufferType::Read, dataB.data());
		OutputBufferA =ShaderBuffer::Create(sizeof(Data), 32, BufferType::ReadWrite);

		computeShader->Bind();
		InputBufferA->Bind(0, ShaderType::CS);
		InputBufferB->Bind(1, ShaderType::CS);
		OutputBufferA->Bind(0,ShaderType::CS);

		std::chrono::system_clock::time_point time1 = std::chrono::system_clock::now();
		RenderCommand::DispatchCompute(1, 1, 1);
		std::chrono::system_clock::time_point time2 = std::chrono::system_clock::now();
		std::chrono::nanoseconds t =time2- time1;
		QCAT_CORE_INFO("Compute Shader time : {0}", t.count());
		
		std::vector<Data> dataC(32);
		time1 = std::chrono::system_clock::now();
		for (int i = 0; i < 32; ++i)
		{
			dataC[i].v1 = (dataA[i].v1 + dataB[i].v1);
			dataC[i].v2 = (dataA[i].v2 + dataB[i].v2);
		}
		time2 = std::chrono::system_clock::now();
		t = time2 - time1;
		QCAT_CORE_INFO("CPU time : {0}", t.count() );


		

		std::vector<char> data(sizeof(Data) * 32);
		OutputBufferA->ReadData(data);

		std::ofstream outfile("output.txt");
		Data* pData = reinterpret_cast<Data*>(data.data());
		for (int i = 0; i < 32; ++i)
		{		
			outfile << "( V1: " << pData[i].v1.x << "," << pData[i].v1.y << "," << pData[i].v1.z
				<< ") ( V2 :" << pData[i].v2.x << "," << pData[i].v2.y << ")" << std::endl;
		}

		outfile.close();
	}

	void ExamLayer::OnUpdate(Timestep step)
	{
		RenderCommand::Clear();

		/*std::vector<Data> dataA(32);
		std::vector<Data> dataB(32);

		for (int i = 0; i < 32; ++i)
		{
			dataA[i].v1 = glm::vec3(i, i, i);
			dataA[i].v2 = glm::vec2(i, 0);

			dataB[i].v1 = glm::vec3(-i, i, 0.0f);
			dataB[i].v2 = glm::vec2(0, -i);
		}

		InputBufferA = QCat::ShaderBuffer::Create(sizeof(Data), 32, QCat::BufferType::Read, dataA.data());
		InputBufferB = QCat::ShaderBuffer::Create(sizeof(Data), 32, QCat::BufferType::Read, dataB.data());
		OutputBufferA = QCat::ShaderBuffer::Create(sizeof(Data), 32, QCat::BufferType::ReadWrite);

		computeShader->Bind();
		InputBufferA->Bind(0, QCat::ShaderType::CS);
		InputBufferB->Bind(1, QCat::ShaderType::CS);
		OutputBufferA->Bind(0, QCat::ShaderType::CS);

		QCat::RenderCommand::DispatchCompute(1, 1, 1);

		std::vector<char> data(sizeof(Data) * 32);
		OutputBufferA->ReadData(data);*/

		/*std::ofstream outfile("output.txt");

		if (outfile.is_open());

		for (int i = 0; i < 32; ++i)
		{
			Data* pData = reinterpret_cast<Data*>(data[sizeof(Data) * i]);
			outfile << "( V1: " << pData->v1.x << "," << pData->v1.y << "," << pData->v1.z
				<< ") ( V2 :" << pData->v2.x << "," << pData->v2.y << ")" << std::endl;
		}

		outfile.close();*/

	}

}
