#include "PBRRenderGraph.h"
#include "../Pass/PBRShaderPass.h"
#include "../Pass/PBRPreComputePass.h"
#include "glm/gtc/type_ptr.hpp"
namespace QCat
{
    PBRRenderGraph::PBRRenderGraph()
    {
        width = 1600;
        height = 900;
    }
    void PBRRenderGraph::Initialize()
    {
        AddGlobalInput(TextureInput::Make("hdrImage", hdrImage));
        AddGlobalInput(DataInput<glm::mat4>::Make("viewMatrix", viewMatrix, DataType::Matrix));
        AddGlobalInput(DataInput<glm::mat4>::Make("projectionMatrix", projectionMatrix, DataType::Matrix));


        AddGlobalOutput(TextureOutput::Make("hdrImage", hdrImage));
        AddGlobalOutput(DataOutput<glm::mat4>::Make("viewMatrix", viewMatrix, DataType::Matrix));
        AddGlobalOutput(DataOutput<glm::mat4>::Make("projectionMatrix", projectionMatrix, DataType::Matrix));

        AddGlobalOutput(TextureOutput::Make("ColorBuffer", m_ColorBuffer));
        AddGlobalOutput(TextureOutput::Make("DepthBuffer", m_DepthBuffer));


        Sampler_Desc smpDesc;
        m_ColorBuffer = Texture2D::Create(TextureFormat::RGB8, smpDesc, width, height);
        m_DepthBuffer = Texture2D::Create(TextureFormat::DEPTH24STENCIL8, smpDesc, width, height);

        Ref<Pass> preComputPass = CreateRef<PBRPreComputePass>(0, "precomputepass");
        preComputPass->SetInputLink("HdrImage", "$.hdrImage");
        AppendPass(preComputPass);
        
        Ref<Pass> pbrShaderPass = CreateRef<PBRShaderPass>(1, "pbrrenderpass");
        pbrShaderPass->SetInputLink("HdrCubeMap", "precomputepass.HdrCubeMap");
        pbrShaderPass->SetInputLink("IrradianceCubeMap", "precomputepass.IrradianceCubeMap");
        pbrShaderPass->SetInputLink("BRDFLutTexture", "precomputepass.BRDFLutTexture");
        pbrShaderPass->SetInputLink("PrefilterMap", "precomputepass.PrefilterMap");
        pbrShaderPass->SetInputLink("viewMatrix", "$.viewMatrix");
        pbrShaderPass->SetInputLink("projectionMatrix", "$.projectionMatrix");
        pbrShaderPass->SetInputLink("ColorBuffer", "$.ColorBuffer");
        pbrShaderPass->SetInputLink("DepthBuffer", "$.DepthBuffer");
        AppendPass(pbrShaderPass);
    }
    void PBRRenderGraph::SetSize(uint32_t width, uint32_t height)
    {
        if (this->width != width || this->height != height)
        {
            this->width = width;
            this->height = height;
            m_ColorBuffer->SetSize(width, height);
            m_DepthBuffer->SetSize(width, height);
        }
    }
}