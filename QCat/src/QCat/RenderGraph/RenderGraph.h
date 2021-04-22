#pragma once
#include "QCat/Core/QCatCore.h"
#include "QCat/Renderer/Texture.h"
#include "Pass.h"
namespace QCat
{
	class RenderGraph
	{
	public:
		RenderGraph();
		virtual ~RenderGraph();

		void Execute();
		void AppendPass(unsigned int level, Ref<Pass> pass);
	private:
		std::map<unsigned int, std::vector<Ref<Pass>>> passes;
	};
}