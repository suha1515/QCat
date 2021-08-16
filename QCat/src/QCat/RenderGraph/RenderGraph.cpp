#include "qcpch.h"
#include "RenderGraph.h"
#include "QCat/Scene/Scene.h"
namespace QCat
{
	void RenderGraph::Execute(Ref<Scene>& scene)
	{
		unsigned int Maxlevels=0;
		std::map<unsigned int, std::vector<Ref<Pass>>> passLevel;
		for (auto& pass : passes)
		{
			if(Maxlevels < pass->GetLevel())
				Maxlevels = pass->GetLevel();
			passLevel[pass->GetLevel()].push_back(pass);
		} 
		for (int i = 0; i <= Maxlevels; ++i)
		{
			for (auto& pass : passLevel[i])
				pass->Execute(scene);
		}
	}
	void RenderGraph::AppendPass( Ref<Pass>& pass)
	{
		pass->Initialize();
		LinkInput(pass);
		passes.push_back(pass);
	}
	void RenderGraph::LinkInput(Ref<Pass>& pass)
	{
		for (auto& input : pass->GetInputs())
		{
			const auto& inputPassName = input->GetPassName();
			if (inputPassName.empty())
			{
				QCAT_CORE_ERROR("input doesnt have passName !");
				std::runtime_error("RenderGraph LinkInput Error!");
			}
			if (inputPassName == "$")
			{
				bool bound = false;
				for (auto& goutput : globalOutputs)
				{
					if (goutput->GetName() == input->GetOutputName())
					{
						input->Bind(*goutput);
						bound = true;
						break;
					}
				}
				if (!bound)
				{
					QCAT_CORE_ERROR("output named {0} not found in globaloutput",input->GetOutputName());
					std::runtime_error("RenderGraph LinkInput Error!");
				}
			}
			else
			{
				bool bound = false;
				for (auto& existpass : passes)
				{
					if (existpass->GetName() == inputPassName)
					{
						auto& output = existpass->GetOutput(input->GetOutputName());
						input->Bind(output);
						bound = true;
						break;
					}
				}
				if (!bound)
				{
					QCAT_CORE_ERROR("PassName {0} not found", inputPassName);
					std::runtime_error("RenderGraph LinkInput Error!");
				}
			}
		}

	}
	void RenderGraph::AddGlobalInput(Scope<PassInput> input)
	{
		globalInputs.push_back(std::move(input));
	}
	void RenderGraph::AddGlobalOutput(Scope<PassOutput> output)
	{
		globalOutputs.push_back(std::move(output));
	}
}