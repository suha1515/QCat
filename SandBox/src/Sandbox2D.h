#pragma once

#include "QCat.h"

class Sandbox2D : public QCat::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(QCat::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(QCat::Event& e) override;
private:
	//camera
	QCat::OrthographicCameraController m_CameraController;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

	QCat::Ref<QCat::Texture2D> m_Texture;
};