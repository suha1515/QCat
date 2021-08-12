#pragma once
#include "QCat.h"


namespace QCat
{
	class PBRRenderGraph : public RenderGraph
	{
	public:
		PBRRenderGraph();
		~PBRRenderGraph()=default;
		void Initialize();
		void SetHdrImg(Ref<Texture>& texture) { hdrImage = texture; }
		void SetView(Ref<glm::mat4>& viewMat) { viewMatrix = viewMat; }
		void SetProj(Ref<glm::mat4>& projMat) { projectionMatrix = projMat; }
		void SetSize(uint32_t width, uint32_t height);
		void SetCameraProperties(Ref<glm::vec4>& camData) { forArNearFar = camData; }

		void SetCascadeSplits(float v) { *m_CascadeSplits = v; }
		void SetDebugMode(bool debug) { *m_DebugShadow = debug; }
		void SetSoftShadow(bool v) { *m_SoftShadow = v; }
		Ref<Texture>& GetColorBuffer() { return m_Colorbuffer; }
		Ref<Texture>& GetIDBuffer() { return m_EntityIDBuffer; }
	public:
		uint32_t width, height;
	private:
		Ref<Texture> hdrImage;
		Ref<glm::mat4> viewMatrix;
		Ref<glm::mat4> projectionMatrix;
		Ref<glm::vec4> forArNearFar;


		Ref<Texture> m_Colorbuffer;
		Ref<Texture> m_DepthBuffer;
		Ref<Texture> m_EntityIDBuffer;

		Ref<float> m_CascadeSplits;
		Ref<float> m_ShadowMaxDistance;
		Ref<bool> m_DebugShadow;
		Ref<bool> m_SoftShadow;
		
	};
}