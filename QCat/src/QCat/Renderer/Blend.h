#pragma once
#include <vector>
namespace QCat
{
	enum class BlendFunc
	{
		BLEND_ZERO=0,BLEND_ONE,
		BLEND_SRC_COLOR,BLEND_INV_SRC_COLOR,
		BLEND_SRC_ALPHA,BLEND_INV_SRC_ALPHA,
		BLEND_DST_COLOR,BLEND_INV_DST_COLOR,
		BLEND_DST_ALPHA,BLEND_INV_DST_ALPHA,
		BLEND_FACTOR,BLEND_INV_FACTOR,
		BLEND_SRC1_COLOR,BLEND_SRC1_ALPHA,BLEND_INV_SRC1_COLOR,BLEND_INV_SRC1_ALPHA
	};
	enum class BlendOp
	{
		BLEND_ADD=0,BLEND_SUBTRACT,BLEND_REVERSE_SUBTRACT,BLEND_MIN,BLEND_MAX
	};
	enum class ColorWriteMask
	{
		COLOR_WRITE_RED=0,COLOR_WRITE_GREEN,COLOR_WRITE_BLUE,COLOR_WRITE_ALPHA,COLOR_WRITE_ALL
	};
	struct Blend_Target_Desc
	{
		bool blendEnable			= false;
		BlendFunc	srcBlend		= BlendFunc::BLEND_ONE;
		BlendFunc	dstBlend		= BlendFunc::BLEND_ZERO;
		BlendOp		blendOp			= BlendOp::BLEND_ADD;
		BlendFunc	srcBlendAlpha	= BlendFunc::BLEND_ONE;
		BlendFunc	dstBlendAlpha	= BlendFunc::BLEND_ZERO;
		BlendOp		blendOpAlpha	= BlendOp::BLEND_ADD;
		ColorWriteMask	writeMask	= ColorWriteMask::COLOR_WRITE_ALL;
	};
	struct BlendDesc
	{
		bool AlphaToCoverage		= false;
		bool IndependentBlendEnable = false;
		Blend_Target_Desc BlendDesc[8];
	};
	class Blender 
	{
	public:
		Blender() = default;
		virtual ~Blender() = default;

		virtual void Bind()const =0;
		virtual void UnBind() const = 0;

		virtual void SetEnableBlend(uint32_t index, bool enable) = 0;
		virtual void SetBlendOp(uint32_t index, BlendOp colorOp, BlendOp alphaOp) = 0;
		virtual void SetBlendFunc(uint32_t index, BlendFunc srcColor, BlendFunc dstColor, BlendFunc srcAlpha, BlendFunc dstAlpha) = 0;

		virtual void SetIndependentBlend(bool enable) = 0;
		virtual void SetAlphaToCoverage(bool enable) = 0;

		static Ref<Blender> Create();
	protected:
		BlendDesc desc;
		std::array<float, 4> factor = { 0.0f,0.0f,0.0f,0.0f };
	};
}