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
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.BorderColor[0] = 1.0f;
		samplerDesc.BorderColor[1] = 1.0f;
		samplerDesc.BorderColor[2] = 1.0f;
		samplerDesc.BorderColor[3] = 1.0f;

		pgfx->BlendStateEnable(true);
		pgfx->SetSamplerState(samplerDesc);
		pgfx->BindSamplerState();

		pgfx->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DepthStencil::DepthStencilDesc DepthStencildesc;
		{
			DepthStencildesc.depthEnable = true;
			DepthStencildesc.depthFunc = COMPARISON_FUNC::LESS;
			DepthStencildesc.depthWriteMask =DEPTH_WRITE_MASK::MASK_ALL;

			DepthStencildesc.stencilEnable = false;
			DepthStencildesc.stencilReadMask = 0xFF;
			DepthStencildesc.stencilWriteMask = 0xFF;

			DepthStencildesc.FrontstencilFunc = COMPARISON_FUNC::ALWAYS;
			DepthStencildesc.FrontstencilFail = STENCIL_OP::KEEP;
			DepthStencildesc.FrontdepthFail = STENCIL_OP::KEEP;
			DepthStencildesc.FrontbothPass = STENCIL_OP::KEEP;

			DepthStencildesc.BackstencilFunc = COMPARISON_FUNC::ALWAYS;
			DepthStencildesc.BackstencilFail = STENCIL_OP::KEEP;
			DepthStencildesc.BackdepthFail = STENCIL_OP::KEEP;
			DepthStencildesc.BackbothPass = STENCIL_OP::KEEP;
			DepthStencildesc.referenceValue = 1;
		}
		m_DepthStencilState = DepthStencil::Create(DepthStencildesc);
		m_DepthStencilState->Bind();

		m_BlenderState = Blender::Create();
		m_BlenderState->SetIndependentBlend(true);

		Rasterizer::Rasterizer_Desc Rasterdesc;
		m_RasterizState = Rasterizer::Create(Rasterdesc);
		m_RasterizState->SetCullMode(CullMode::Back);
		m_RasterizState->SetFrontClockWise(false);
	}
	void DX11RenderAPI::SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
	{
		pgfx->SetViewPort(width, height);
	}
	void DX11RenderAPI::ResizeFrameBuffer(unsigned int width, unsigned int height)
	{
		pgfx->SetWidthHeight(width, height);
		pgfx->CleanRenderTarget();
		pgfx->GetSwapChain()->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
		pgfx->CreateRenderTarget();
		pgfx->SetRenderTarget();
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
		m_RasterizState->Bind();
		pgfx->GetContext()->DrawIndexed(indexCount,0u,0u);
	}
	void DX11RenderAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, unsigned int indexCount, DrawMode mode)
	{
		unsigned int count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		DrawIndexed(count);
		//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> nullSRV =  nullptr;
		//pgfx->GetContext()->PSSetShaderResources(0u, 1u, nullSRV.GetAddressOf());
	}
	void DX11RenderAPI::SetDepthTest(bool enable)
	{
		m_DepthStencilState->EnableDepth(enable);
	}
	void DX11RenderAPI::SetDepthFunc(COMPARISON_FUNC func)
	{
		m_DepthStencilState->SetDepthFunc(func);
	}
	void DX11RenderAPI::SetStencilTest(bool enable)
	{
		m_DepthStencilState->EnableStencil(enable);
	}
	void DX11RenderAPI::SetFrontStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
	{
		m_DepthStencilState->SetFrontStencilOperator(stencilFail, depthFail, bothPass);
	}
	void DX11RenderAPI::SetFrontStencilFunc(COMPARISON_FUNC func, int value)
	{
		m_DepthStencilState->SetFrontStencilFunc(func, value, 0xFF);
	}
	void DX11RenderAPI::SetBackStencilOp(STENCIL_OP stencilFail, STENCIL_OP depthFail, STENCIL_OP bothPass)
	{
		m_DepthStencilState->SetBackStencilOperator(stencilFail, depthFail, bothPass);
	}
	void DX11RenderAPI::SetBackStencilFunc(COMPARISON_FUNC func, int value)
	{
		m_DepthStencilState->SetBackStencilFunc(func, value, 0xFF);
	}
	void DX11RenderAPI::SetDepthWriteMask(DEPTH_WRITE_MASK mask)
	{
		m_DepthStencilState->SetDepthWriteMask(mask);
	}
	void DX11RenderAPI::SetStencilWriteMask(int value)
	{
		m_DepthStencilState->SetStencilWriteMask(value);
	}
	void DX11RenderAPI::SetFillMode(FillMode mode)
	{
		m_RasterizState->SetFillMode(mode);
	}
	void DX11RenderAPI::SetCullMode(CullMode mode)
	{
		m_RasterizState->SetCullMode(mode);
	}
	void DX11RenderAPI::SetFrontClockWise(bool enable)
	{
		m_RasterizState->SetFrontClockWise(enable);
	}
	void DX11RenderAPI::SetDepthBias(int value)
	{
		m_RasterizState->SetDepthBias(value);
	}
	void DX11RenderAPI::SetDepthBiasClamp(float value)
	{
		m_RasterizState->SetDepthBiasClamp(value);
	}
	void DX11RenderAPI::SetSlopeScaledDepthBias(float value)
	{
		m_RasterizState->SetSlopeScaledDepthBias(value);
	}
	void DX11RenderAPI::SetDepthClip(bool enable)
	{
		m_RasterizState->SetDepthClip(enable);
	}
	void DX11RenderAPI::SetSissor(bool enable)
	{
		m_RasterizState->SetSissor(enable);
	}
	void DX11RenderAPI::SetMultiSample(bool enable)
	{
		m_RasterizState->SetMultiSample(enable);
	}
	void DX11RenderAPI::SetAntialiasedLine(bool enable)
	{
		m_RasterizState->SetAntialiasedLine(enable);
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
	void* DX11RenderAPI::GetDefaultFrameBuffer()
	{
		return nullptr;
	}
}