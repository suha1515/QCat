#include "ExamLayer.h"
#include <chrono>
#include<random>

namespace QCat
{
	struct Particle
	{
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec2 size;
		float lifttime;
	};

	void ExamLayer::OnAttach()
	{
		RenderCommand::SetClearColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
		
		dynamicBuffer = VertexArray::Create();

		/*std::vector<Particle> particles;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<float> dist(1.0f, 2.0f);
		for (int i = 0; i < 1000; ++i)
		{
			Particle info;
			
			info.position = glm::vec3(0.0f, 0.0f, 0.0f);
			info.velocity = glm::vec3(dist(gen), dist(gen), dist(gen));
		}*/

		vertexBuffer = VertexBuffer::Create()

	}

	void ExamLayer::OnUpdate(Timestep step)
	{
		RenderCommand::Clear();

		

	}

}
