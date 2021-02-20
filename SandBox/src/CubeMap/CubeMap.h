#pragma once
#include "QCat.h"

namespace QCat
{
	class CubeMap
	{
	public:
		CubeMap(const std::vector<std::string> imgPathes, const Ref<Shader> shader);
		~CubeMap()=default;
	public:
		void Draw(const Ref<Shader>& shader);
	private:
		Ref<VertexArray> m_VertexArray;
		Ref<TextureCube> m_cubeTexture;
	};
}