#pragma once
#include "QCat.h"
#include "../../Light/Light.h"
#include "../../Geometry/Sphere.h"
#include "../../Geometry/Cube.h"
namespace QCat
{
	class PBRShaderPass : public Pass
	{
	public:
		PBRShaderPass(unsigned int level, const std::string& name);
		~PBRShaderPass() = default;

		virtual void Initialize() override;
		virtual void Execute(Ref<Scene>& scene) override;

	private:
		Ref<Texture> m_HdrCubeMap;
		Ref<Texture> m_IrradianceCubeMap;
		Ref<Texture> m_BRDFLutTextrue;
		Ref<Texture> m_PrefilterMap;

		Ref<Shader>  m_PBRShader;
		Ref<Shader>  m_FlatColorShader;
		Ref<Shader>  m_SkyBoxShader;
		
		Ref<glm::mat4> viewMatrix;
		//Material materials[5];
		Light light[4];

		Ref<Cube> cube;
		Ref<Sphere> sphere;
	};
}