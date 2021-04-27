#pragma once
#include "QCat/Core/QCatCore.h"
#include "PassInputOutPut/PassInput.h"
#include "PassInputOutPut/PassOutput.h"
#include "QCat/Scene/Scene.h"

namespace QCat
{
	class Pass
	{
	public:
		Pass();
		virtual ~Pass();

		virtual void Initialize()=0;
		virtual void Execute(Ref<Scene>& scene)=0;

		PassInput& GetInput(const std::string& name) const;
		const std::vector<Scope<PassInput>>& GetInputs() const { return inputs; }
		PassOutput& GetOutput(const std::string& name) const;
		const std::vector<Scope<PassOutput>>& GetOutputs() const { return outputs; }

		void SetInputLink(const std::string& inputName,const std::string& target);
		void RegisterInput(Scope<PassInput> input);
		void RegisterOutput(Scope<PassOutput> output);

		unsigned int GetLevel() { return level; }
		std::string GetName() const{ return name; }
	protected:
		unsigned int level=0;
		std::string name;
		std::vector<Scope<PassInput>> inputs;
		std::vector<Scope<PassOutput>> outputs;
	};
}
