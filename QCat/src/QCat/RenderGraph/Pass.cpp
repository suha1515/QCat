#include "qcpch.h"
#include "Pass.h"
#include "QCat/Uitiliy/StringUtil.h"

namespace QCat
{
	Pass::Pass()
	{
	}
	Pass::~Pass()
	{
	}
	PassInput& Pass::GetInput(const std::string& name) const
	{
		for (auto& input : inputs)
		{
			if (input->GetName() == name)
				return *input;
		}
		QCAT_CORE_ERROR("{0} Pass Error : Cant find input name:{1}",this->name,name);
	}
	PassOutput& Pass::GetOutput(const std::string& name) const
	{
		for (auto& output : outputs)
		{
			if (output->GetName() == name)
				return *output;
		}
		QCAT_CORE_ERROR("{0} Pass Error : Cant find output name:{1}", this->name, name);

	}
	void Pass::SetInputLink(const std::string& inputName, const std::string& target)
	{
		auto& input = GetInput(inputName);
		auto targetSplit = SplitString(target, ".");
		if (targetSplit.size() != 2u)
		{
			QCAT_CORE_ERROR("{0} Pass Error :Split Target Error!");
		}
		input.SetLink(std::move(targetSplit[0]), std::move(targetSplit[1]));
	}
	void Pass::RegisterInput(Scope<PassInput> input)
	{
		//check overlap
		bool overlap = false;
		for (auto& in : inputs)
		{
			if (in->GetName() == input->GetName())
			{
				QCAT_CORE_ERROR("{0} Pass Error : there is same input in pass passname:{1}", this->name, in->GetName().c_str());
				overlap = true;
				break;
			}
		}
		if (!overlap)
			inputs.push_back(std::move(input));
	}
	void Pass::RegisterOutput(Scope<PassOutput> output)
	{
		//check overlap
		bool overlap = false;
		for (auto& out : outputs)
		{
			if (out->GetName() == output->GetName())
			{
				QCAT_CORE_ERROR("{0} Pass Error :there is same output in pass passname:{1}", this->name, out->GetName().c_str());
				overlap = true;
				break;
			}
		}
		if (!overlap)
			outputs.push_back(std::move(output));
	}
}