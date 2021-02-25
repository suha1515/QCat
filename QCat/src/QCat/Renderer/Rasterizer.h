#pragma once

namespace QCat
{
	enum class FillMode
	{
		Solid = 0, WireFrame, Point
	};
	enum class CullMode
	{
		None = 0, Front, Back, Front_And_Back
	};

	class Rasterizer
	{
	public:
		struct Rasterizer_Desc
		{
			FillMode fillmode = FillMode::Solid;
			CullMode cullmode = CullMode::Back;
			bool     frontCounterClockWise=false;
			int		 DepthBias = 0;
			float    DepthBiasClamp=0;
			float    SlopeScaledDepthBias=0;
			bool     DepthClipEnable=true;
			bool     ScissorEnable=false;
			bool     MultisampleEnable=false;
			bool     AntialiasedLineEnable=false;
		};
	public:
		Rasterizer() = default;
		virtual ~Rasterizer() = default;

		virtual void SetFillMode(FillMode mode) = 0;
		virtual void SetCullMode(CullMode mode) = 0;
		virtual void SetFrontClockWise(bool enable) = 0;
		virtual void SetDepthBias(int value) = 0;
		virtual void SetDepthBiasClamp(float value) = 0;
		virtual void SetSlopeScaledDepthBias(float value) = 0;
		virtual void SetDepthClip(bool enable) = 0;
		virtual void SetSissor(bool enable) = 0;
		virtual void SetMultiSample(bool enable) = 0;
		virtual void SetAntialiasedLine(bool enable) = 0;

		virtual void Bind() = 0;

		static Ref<Rasterizer> Create(Rasterizer_Desc desc);
	protected:
		Rasterizer_Desc desc;

	};
}