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
	private:
		Ref<Texture> hdrImage;
		Ref<glm::mat4> viewMatrix;
	};
}