#pragma once
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class DX11Blender
	{
	public:
		DX11Blender();
		~DX11Blender();

		void Bind() const ;
	private:
		Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
	};
}
