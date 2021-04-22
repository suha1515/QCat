#pragma once
#include "QCat/Core/QCatCore.h"
#include "PassInputOutPut/PassInput.h"
#include "PassInputOutPut/PassOutput.h"

namespace QCat
{
	class Pass
	{
	public:
		Pass()=default;
		virtual ~Pass() = default;

		virtual void Initialize()=0;
		virtual void Execute()=0;

		PassInput& GetInput(const std::string& name) const;
		PassOutput& GetOutput(const std::string& name) const;

		void SetInputLink(const std::string& inputName,const std::string& target);
		void RegisterInput(Scope<PassInput> input);
		void RegisterOutput(Scope<PassOutput> output);
	private:
		std::string name;
		std::vector<Scope<PassInput>> inputs;
		std::vector<Scope<PassOutput>> outputs;
	};
}
