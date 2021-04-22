#include "qcpch.h"
#include "PassInput.h"
namespace QCat
{
	void TextureInput::Bind(PassOutput& output)
	{
		auto type = output.GetType();
		if (GetType() != type)
		{
			QCAT_CORE_ERROR("{0} Pass Error : Input and output type missmatch");
		}
		else
		{
			m_texture = dynamic_cast<TextureOutput*>(&output)->GetTexture();
		}

	}
}