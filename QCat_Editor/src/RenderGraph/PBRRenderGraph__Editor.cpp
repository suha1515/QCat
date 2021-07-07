#include "PBRRenderGraph_Editor.h"
#include "PBRShaderPass_Editor.h"
#include "PBRPreComputePass_Editor.h"
#include "ShadowMappingPass.h"
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
        //Input
        AddGlobalInput(TextureInput::Make("hdrImage", hdrImage));
        AddGlobalInput(DataInput<glm::mat4>::Make("viewMatrix", viewMatrix,DataType::Matrix));
        AddGlobalInput(DataInput<glm::mat4>::Make("projectionMatrix", projectionMatrix, DataType::Matrix));

        //Output
        AddGlobalOutput(TextureOutput::Make("hdrImage", hdrImage));
        AddGlobalOutput(DataOutput<glm::mat4>::Make("viewMatrix", viewMatrix, DataType::Matrix));
        AddGlobalOutput(DataOutput<glm::mat4>::Make("projectionMatrix", projectionMatrix, DataType::Matrix));

        AddGlobalOutput(TextureOutput::Make("ColorBuffer", m_Colorbuffer));
        AddGlobalOutput(TextureOutput::Make("DepthBuffer", m_DepthBuffer));

        
        Sampler_Desc smpDesc;
        m_Colorbuffer = Texture2D::Create(TextureFormat::RGB8, smpDesc, width, height);
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

        //Shadow Pass
        Ref<Pass> ShadowPass = CreateRef<ShadowMappingPass>(0, "shadowmappingpass");
       AppendPass(ShadowPass);
    }
    void PBRRenderGraph::SetSize(uint32_t width, uint32_t height)
    {
        if (this->width != width || this->height != height)
        {
            this->width = width;
            this->height = height;
            m_Colorbuffer->SetSize(width, height);
            m_DepthBuffer->SetSize(width, height);
        }
    }
}