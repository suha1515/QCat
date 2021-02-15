#include "qcpch.h"
#include "DX11_RenderAPI.h"
#include "API/DirectX11/QGfxDeviceDX11.h"

namespace QCat
{
	DX11RenderAPI::DX11RenderAPI()
		:color(glm::vec4(0.0f))
	{
 		pgfx = nullptr;		
	}
	DX11RenderAPI::~DX11RenderAPI()
	{
	}
	void DX11RenderAPI::Init()
	{
		QCAT_PROFILE_FUNCTION();
		pgfx = QGfxDeviceDX11::GetInstance();
		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;

		pgfx->BlendStateEnable(true);
		pgfx->SetSamplerState(samplerDesc);
		pgfx->BindSamplerState();

		pgfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DepthStencil::DepthStencilDesc desc;
		{
			desc.depthEnable = true;
			desc.depthFunc = COMPARISON_FUNC::LESS;
			desc.depthWriteMask =DEPTH_WRITE_MASK::MASK_ALL;

			desc.stencilEnable = true;
			desc.stencilFunc = COMPARISON_FUNC::ALWAYS;
			desc.stencilReadMask = 0xFF;
			desc.stencilWriteMask = 0xFF;
			desc.stencilFail = STENCIL_OP::KEEP;
			desc.depthFail = STENCIL_OP::KEEP;
			desc.bothPass = STENCIL_OP::REPLACE;
			desc.referenceValue = 1;
		}
		m_DepthStencilState = DepthStencil::Create(desc);
		m_DepthStencilState->Bind();

		m_BlenderState = Blender::Create();
		m_BlenderState->SetIndependentBlend(true);
	}
	void DX11RenderAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		pgfx->SetWidthHeight(width, height);
		pgfx->CleanRenderTarget();
		pgfx->GetSwapChain()->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		pgfx->CreateRenderTarget();
		pgfx->SetViewPort(width, height);
	}
	void DX11RenderAPI::SetDefaultFrameBuffer()
	{
		pgfx->SetRenderTarget();
	}
	void DX11RenderAPI::SetClearColor(const glm::vec4& color)
	{
		this->color = color;
	}
	void DX11RenderAPI::Clear()
	{
		pgfx->Clear(color);
	}
	void DX11RenderAPI::DrawIndexed(const unsigned int indexCount)
	{
		m_BlenderState->Bind();
		m_DepthStencilState->Bind();
		pgfx->GetContext()->DrawIndexed(indexCount,0u,0u);
	}
	void DX11RenderAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount)
	{
		unsigned int count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		DrawIndexed(count);
		//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nullSRV =  nullptr;
		//pgfx->GetContext()->PSSetShaderResources(0u, 1u, nullSRV.GetAddressOf());
	}
	void DX11RenderAPI::SetWireFrameMode()
	{
		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterDesc.FrontCounterClockwise = true;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		pgfx->GetDevice()->CreateRasterizerState(&rasterDesc, &pRasterizer);
		pgfx->GetContext()->RSSetState(pRasterizer.Get());
	}
	void DX11RenderAPI::SetFillMode()
	{
		D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = true;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
		pgfx->GetDevice()->CreateRasterizerState(&rasterDesc, &pRasterizer);
		pgfx->GetContext()->RSSetState(pRasterizer.Get());
	}
	void DX11RenderAPI::SetDepthTest(bool enable)
	{
		m_DepthStencilState->EnableDepth(enable);
	}
	void DX11RenderAPI::SetStencilTest(bool enable)
	{
		m_DepthStencilState->EnableStencil(enable);
	}
	void DX11RenderAPI::SetStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
	{
		m_DepthStencilState->SetStencilOperator(stencilFail, depthFail, bothPass);
	}
	void DX11RenderAPI::SetStencilFunc(COMPARISON_FUNC func, int value)
	{
		m_DepthStencilState->SetStencilFunc(func, value, 0xFF);
	}
	void DX11RenderAPI::SetDepthWriteMask(DEPTH_WRITE_MASK mask)
	{
		m_DepthStencilState->SetDepthWriteMask(mask);
	}
	void DX11RenderAPI::SetStencilWriteMask(int value)
	{
		m_DepthStencilState->SetStencilWriteMask(value);
	}
	void DX11RenderAPI::SetBlend(bool enable)
	{
		m_BlenderState->SetEnableBlend(0, enable);
	}
	void DX11RenderAPI::SetBlend(uint32_t index, bool enable)
	{
		m_BlenderState->SetEnableBlend(index, enable);
	}
	void DX11RenderAPI::SetBlendFunc(BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha)
	{
		m_BlenderState->SetBlendFunc(0, srcColor, dstColor, srcAlpha, dstAlpha);
	}
	void DX11RenderAPI::SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha)
	{
		m_BlenderState->SetBlendFunc(index, srcColor, dstColor, srcAlpha, dstAlpha);
	}
	void DX11RenderAPI::SetBlendOp(BlendOp colorOp, BlendOp alphaOp)
	{
		m_BlenderState->SetBlendOp(0, colorOp, alphaOp);
	}
	void DX11RenderAPI::SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp)
	{
		m_BlenderState->SetBlendOp(index, colorOp, alphaOp);
	}
}