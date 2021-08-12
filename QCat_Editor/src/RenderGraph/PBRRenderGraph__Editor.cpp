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

        m_CascadeSplits = CreateRef<float>(0.5f);
        m_DebugShadow = CreateRef<bool>(false);
        m_SoftShadow = CreateRef<bool>(false);

    }
    void PBRRenderGraph::Initialize()
    {
        //Input
        AddGlobalInput(TextureInput::Make("hdrImage", hdrImage));
        AddGlobalInput(DataInput<glm::mat4>::Make("viewMatrix", viewMatrix,DataType::Matrix));
        AddGlobalInput(DataInput<glm::mat4>::Make("projectionMatrix", projectionMatrix, DataType::Matrix));
        AddGlobalInput(DataInput<glm::vec4>::Make("forArNearFar",forArNearFar, DataType::Float4));

        //Output
        AddGlobalOutput(TextureOutput::Make("hdrImage", hdrImage));
        AddGlobalOutput(DataOutput<glm::mat4>::Make("viewMatrix", viewMatrix, DataType::Matrix));
        AddGlobalOutput(DataOutput<glm::mat4>::Make("projectionMatrix", projectionMatrix, DataType::Matrix));
        AddGlobalOutput(DataOutput<glm::vec4>::Make("forArNearFar", forArNearFar, DataType::Float4));
        AddGlobalOutput(DataOutput<bool>::Make("DebugMode", m_DebugShadow, DataType::Bool));
        AddGlobalOutput(DataOutput<bool>::Make("SoftShadow", m_SoftShadow, DataType::Bool));
        AddGlobalOutput(DataOutput<float>::Make("CasacadeSplits", m_CascadeSplits, DataType::Float));
        
        //Buffer
        AddGlobalOutput(TextureOutput::Make("ColorBuffer", m_Colorbuffer));
        AddGlobalOutput(TextureOutput::Make("DepthBuffer", m_DepthBuffer));
        AddGlobalOutput(TextureOutput::Make("IDBuffer", m_EntityIDBuffer));
    
        //Color Buffer , DepthBuffer , IDBuffer
        Sampler_Desc smpDesc;
        m_Colorbuffer = Texture2D::Create(TextureFormat::RGB8, smpDesc, width, height);
        m_DepthBuffer = Texture2D::Create(TextureFormat::DEPTH24STENCIL8, smpDesc, width, height);
        smpDesc.MIN = Filtering::POINT;
        smpDesc.MAG = Filtering::POINT;
        m_EntityIDBuffer = Texture2D::Create(TextureFormat::RED32_INTEGER, smpDesc,width,height);

        Ref<Pass> preComputPass = CreateRef<PBRPreComputePass>(0, "precomputepass");
        preComputPass->SetInputLink("HdrImage", "$.hdrImage");
        AppendPass(preComputPass);
        
        //Shadow Pass
        Ref<Pass> ShadowPass = CreateRef<ShadowMappingPass>(0, "shadowmappingpass");
        ShadowPass->SetInputLink("viewMatrix", "$.viewMatrix");
        ShadowPass->SetInputLink("projectionMatrix", "$.projectionMatrix");
        ShadowPass->SetInputLink("forArNearFar", "$.forArNearFar");
        ShadowPass->SetInputLink("CasacadeSplits", "$.CasacadeSplits");

        AppendPass(ShadowPass);

        //PbrShading Pass
        Ref<Pass> pbrShaderPass = CreateRef<PBRShaderPass>(1, "pbrrenderpass");
        pbrShaderPass->SetInputLink("HdrCubeMap", "precomputepass.HdrCubeMap");
        pbrShaderPass->SetInputLink("IrradianceCubeMap", "precomputepass.IrradianceCubeMap");
        pbrShaderPass->SetInputLink("BRDFLutTexture", "precomputepass.BRDFLutTexture");
        pbrShaderPass->SetInputLink("PrefilterMap", "precomputepass.PrefilterMap");
        pbrShaderPass->SetInputLink("viewMatrix", "$.viewMatrix");
        pbrShaderPass->SetInputLink("projectionMatrix", "$.projectionMatrix");
        pbrShaderPass->SetInputLink("ColorBuffer", "$.ColorBuffer");
        pbrShaderPass->SetInputLink("DepthBuffer", "$.DepthBuffer");
        pbrShaderPass->SetInputLink("IDBuffer", "$.IDBuffer");

        //Bool
        pbrShaderPass->SetInputLink("DebugMode", "$.DebugMode");
        pbrShaderPass->SetInputLink("SoftShadow", "$.SoftShadow");
        // Shadowmap Link from ShadowPass
        pbrShaderPass->SetInputLink("DirlightTransform", "shadowmappingpass.DirlightTransform");
        pbrShaderPass->SetInputLink("DirectionalLightShadowMap", "shadowmappingpass.DirectionalLightShadowMap");
        pbrShaderPass->SetInputLink("PointLightShadowMap", "shadowmappingpass.PointLightShadowMap");
        pbrShaderPass->SetInputLink("SpotLightShadowMap", "shadowmappingpass.SpotLightShadowMap");

        

        AppendPass(pbrShaderPass);
    }
    void PBRRenderGraph::SetSize(uint32_t width, uint32_t height)
    {
        if (this->width != width || this->height != height)
        {
            this->width = width;
            this->height = height;
            m_Colorbuffer->SetSize(width, height);
            m_DepthBuffer->SetSize(width, height);
            m_EntityIDBuffer->SetSize(width, height);
        }
    }
}