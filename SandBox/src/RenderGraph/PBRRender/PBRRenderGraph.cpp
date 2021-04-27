#include "PBRRenderGraph.h"
#include "../Pass/PBRShaderPass.h"
#include "../Pass/PBRPreComputePass.h"
#include "glm/gtc/type_ptr.hpp"
namespace QCat
{
    PBRRenderGraph::PBRRenderGraph()
    {
    }
    void PBRRenderGraph::Initialize()
    {
        AddGlobalInput(TextureInput::Make("hdrImage", hdrImage));
        AddGlobalInput(DataInput<glm::mat4>::Make("viewMatrix", viewMatrix,DataType::Matrix));
        AddGlobalOutput(TextureOutput::Make("hdrImage", hdrImage));
        AddGlobalOutput(DataOutput<glm::mat4>::Make("viewMatrix", viewMatrix, DataType::Matrix));

        Ref<Pass> preComputPass = CreateRef<PBRPreComputePass>(0, "precomputepass");
        preComputPass->SetInputLink("HdrImage", "$.hdrImage");
        AppendPass(preComputPass);
        

        Ref<Pass> pbrShaderPass = CreateRef<PBRShaderPass>(1, "pbrrenderpass");
        pbrShaderPass->SetInputLink("HdrCubeMap", "precomputepass.HdrCubeMap");
        pbrShaderPass->SetInputLink("IrradianceCubeMap", "precomputepass.IrradianceCubeMap");
        pbrShaderPass->SetInputLink("BRDFLutTexture", "precomputepass.BRDFLutTexture");
        pbrShaderPass->SetInputLink("PrefilterMap", "precomputepass.PrefilterMap");
        pbrShaderPass->SetInputLink("viewMatrix", "$.viewMatrix");
        AppendPass(pbrShaderPass);
    }
}