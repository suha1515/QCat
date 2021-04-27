#pragma once
#include "QCat/Core/QCatCore.h"
#include "QCat/Renderer/Texture.h"
#include "Pass.h"
namespace QCat
{
	class RenderGraph
	{
	public:
		RenderGraph() = default;
		virtual ~RenderGraph()=default;

		virtual void Execute(Ref<Scene>& scene);
		virtual void AppendPass( Ref<Pass>& pass);
		void LinkInput(Ref<Pass>& pass);
		void AddGlobalInput(Scope<PassInput> input);
		void AddGlobalOutput(Scope<PassOutput> output);
	private:
		std::vector<Ref<Pass>> passes;
		std::vector<Scope<PassInput>> globalInputs;
		std::vector<Scope<PassOutput>> globalOutputs;
	};
}