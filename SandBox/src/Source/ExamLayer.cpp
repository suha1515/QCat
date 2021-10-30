#include "ExamLayer.h"
#include <chrono>
#include<random>


namespace QCat
{
	struct Particle
	{
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 speed;
		glm::vec3 color;

		float size=1.0f;
		float lifttime=0.0f;
		float cameraDistance = -1.0f;

		bool operator<(const Particle& other) const
		{
			return this->cameraDistance > other.cameraDistance;
		}
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 uv;
	};

	const int MaxParticles = 1000;
	std::vector<Particle> particles;
	std::vector<Vertex> vertices;
	int LastUsedParticle = 0;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<float> dist(-1.0f, 1.0f);

	uint32_t FindUnusedParticle()
	{
		for (int i = LastUsedParticle; i < MaxParticles; ++i)
		{
			if (particles[i].lifttime <= 0)
			{
				LastUsedParticle = i;
				return i;
			}
		}

		for (int i = 0; i < LastUsedParticle; ++i)
		{
			if (particles[i].lifttime <= 0)
			{
				LastUsedParticle = i;
				return i;
			}
		}

		return 0;
	}

	void SortParticle()
	{
		std::sort(particles.begin(), particles.end());
	}

	void ExamLayer::OnAttach()
	{
		RenderCommand::SetClearColor(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));

		particles.resize(MaxParticles);
		vertices.resize(MaxParticles * 6);
		cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);

		dynamicBuffer = VertexArray::Create();

		cube = CreateRef<Cube>(glm::vec3(0.0f, 0.0f, 3.f));

		vertexBuffer = VertexBuffer::Create(nullptr, sizeof(Vertex) * vertices.size());
		vertexBuffer->SetLayout(BufferLayout::Create({
			{ShaderDataType::Float3,"a_Position"},
			{ShaderDataType::Float3,"a_Color"},
			{ShaderDataType::Float2,"a_TextureCoords"} }
			, particleShader
			));

		dynamicBuffer->AddVertexBuffer(vertexBuffer);


		particleShader = Shader::Create("Asset/shaders/glsl/FlatColor.glsl");

		transformBuffer = ConstantBuffer::Create(sizeof(modelInfo), 0);
		cameraBuffer = ConstantBuffer::Create(sizeof(cameraInfo), 0);

		viewMatrix = glm::lookAt(cameraPosition, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		projMatrix = glm::perspective(60.f, 1.778f, 0.1f, 100.0f);
	}

	void ExamLayer::OnUpdate(Timestep step)
	{
		RenderCommand::Clear();

		int newParticles = (int)(step * 10000.0);
		if (newParticles > (int)(0.016f * 10000.0))
			newParticles = (int)(0.016f * 10000.0);

		glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
		float spread = 1.5f;
		for (int i = 0; i < newParticles; ++i)
		{
			int particleIndex = FindUnusedParticle();
			particles[particleIndex].lifttime = 3.0f;
			particles[particleIndex].position = glm::vec3(0.0f, 0.0f, 4.0f);

			glm::vec3 randomdir = glm::vec3(dist(gen), dist(gen), dist(gen));
			particles[particleIndex].speed = maindir + randomdir * 1.5f;
			//particles[particleIndex].color = glm::vec3(dist(gen) + 1.0f, dist(gen) + 1.0f, dist(gen)+1.0f) / 2.0f;
			particles[particleIndex].color = glm::vec3(1.0f, 1.0f, 1.0f);
		}

		int particlesCount = 0;
		for (int i = 0; i < MaxParticles; ++i)
		{
			Particle& p = particles[i];
			if (p.lifttime > 0.0f)
			{
				p.lifttime -= step;
				if (p.lifttime > 0.0f)
				{
					p.speed += glm::vec3(0.0f, -9.8f, 0.0f) * (float)step * 0.5f;
					p.position += p.speed * (float)step;
					p.cameraDistance = glm::length2(p.position - cameraPosition);
					//1
					vertices[i].position = glm::vec3(-0.5f, 0.5f, 0.0f) + p.position;
					vertices[i].uv = glm::vec2(0.0f, 0.0f);
					vertices[i].color = p.color;

					//2
					vertices[i+1].position = glm::vec3(0.5f, 0.5f, 0.0f) + p.position;
					vertices[i+1].uv = glm::vec2(1.0f, 0.0f);
					vertices[i+1].color = p.color;

					//3
					vertices[i+2].position = glm::vec3(0.5f, -0.5f, 0.0f) + p.position;
					vertices[i+2].uv = glm::vec2(1.0f, 1.0f);
					vertices[i+2].color = p.color;

					//4
					vertices[i+3].position = glm::vec3(-0.5f, 0.5f, 0.0f) + p.position;
					vertices[i+3].uv = glm::vec2(0.0f, 0.0f);
					vertices[i+3].color = p.color;

					//5
					vertices[i+4].position = glm::vec3(0.5f, -0.5f, 0.0f) + p.position;
					vertices[i+4].uv = glm::vec2(1.0f, 1.0f);
					vertices[i+4].color = p.color;

					//6
					vertices[i+5].position = glm::vec3(-0.5f, -0.5f, 0.0f) + p.position;
					vertices[i+5].uv = glm::vec2(0.0f, 1.0f);
					vertices[i+5].color = p.color;
				}
				else
				{
					p.cameraDistance = -1.0f;
				}
				particlesCount++;
			}
		}
		SortParticle();


		//vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertices.size());

		//cameraInfo caminfo;
		//caminfo.projection = projMatrix;
		//caminfo.view = viewMatrix;

		//modelInfo modelinfo;
		//modelinfo.transform = glm::mat4(1.0f);
		//
		

		//particleShader->Bind();
		////dynamicBuffer->Bind();
		//vertexBuffer->Bind();

		
		//
		//RenderCommand::Draw(0, vertices.size());

		//dynamicBuffer->UnBind();
		//particleShader->UnBind();

		cameraInfo caminfo;
		caminfo.projection = projMatrix;
		caminfo.view = viewMatrix;

		modelInfo modelinfo;
		modelinfo.transform = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,3.0f));
		transformBuffer->SetData(&modelinfo, sizeof(modelInfo));
		cameraBuffer->SetData(&caminfo, sizeof(cameraInfo));

		particleShader->Bind();
		cameraBuffer->Bind(0);
		transformBuffer->Bind(1);
		cube->Draw();

		particleShader->UnBind();

	}

}
