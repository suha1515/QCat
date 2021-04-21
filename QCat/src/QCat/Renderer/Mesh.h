#pragma once
#include "QCat/Core/QCatCore.h"
#include "VertexArray.h"

namespace QCat
{
	/*class Mesh
	{
	public:
		Mesh(std::string meshName,Ref<VertexArray> vertexArray);
		Mesh(std::string meshName = "");
		virtual ~Mesh() = default;


		void Bind();
		void UnBind();
;	private:
		Ref<VertexArray> m_vertexArray;
	};*/

	class MeshLibrary
	{
	public:
		static Ref<VertexArray> Load(const std::string& meshName)
		{
			return Get().Load_(meshName);
		}
		static bool Set(const std::string& meshName, Ref<VertexArray> vertexArray)
		{
			return Get().Set_(meshName, vertexArray);
		}
	private:
		static MeshLibrary& Get()
		{
			static MeshLibrary lib;
			return lib;
		}
		bool IsExist(const std::string& meshName) const
		{
			return m_vertexArray.find(meshName) != m_vertexArray.end();
		}
		Ref<VertexArray> Load_(const std::string& meshName)
		{
			if (IsExist(meshName))
			{
				return m_vertexArray[meshName];
			}
			else
				return nullptr;
		}
		bool Set_(const std::string& meshName, Ref<VertexArray> vertexArray)
		{
			if (IsExist(meshName))
			{
				return false;
			}
			else
			{
				m_vertexArray[meshName] = vertexArray;
				return true;
			}
		}
	private:
		std::unordered_map<std::string, Ref<VertexArray>> m_vertexArray;
	};
}