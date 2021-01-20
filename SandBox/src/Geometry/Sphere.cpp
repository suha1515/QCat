#include "Sphere.h"

QCat::Sphere::Sphere(Scene* activeScene, const char* name, const glm::vec3& position, const char* texturePath)
{
	entity = activeScene->CreateEntity(name);
	auto& tc = entity.GetComponent<TransformComponent>();
	tc.Translation = position;

	shader = Shader::Create("Asset/shaders/glsl/Solid.glsl");
	if (texturePath != "")
		m_Texture = QCat::Texture2D::Create(texturePath);

	shader->Bind();
	shader->SetInt("u_Texture", 0);
}

void QCat::Sphere::SetScale(const glm::vec3& scale)
{
}

void QCat::Sphere::SetRotation(const glm::vec3& rotation)
{
}

void QCat::Sphere::Draw(Entity& camera)
{
}

void QCat::Sphere::ImguiRender(const char* name)
{
}
