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

		Ref<Texture>& GetColorBuffer() { return m_ColorBuffer; }
	private:
		Ref<Texture> hdrImage;
		Ref<glm::mat4> viewMatrix;
		Ref<glm::mat4> projectionMatrix;

		Ref<Texture> m_ColorBuffer;
		Ref<Texture> m_DepthBuffer;
		uint32_t width, height;
	};
}
