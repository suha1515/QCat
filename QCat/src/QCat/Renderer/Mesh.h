#pragma once
#include "QCat/Core/QCatCore.h"
#include "VertexArray.h"

namespace QCat
{
	class ModelLibrary
	{
	public:
		static void LoadModel(const std::string& modelPath)
		{
			Get().Load_(modelPath);
		}
		static std::vector<std::string>& GetModelList() { return Get().modelPath; };
	private:
		static ModelLibrary& Get()
		{
			static ModelLibrary lib;
			return lib;
		}
		void Load_(const std::string& modelPath);
	private:
		std::vector<std::string> modelPath;
	};
	class MeshLibrary
	{
	public:
		static Ref<VertexArray> Load(const std::string& meshName)
		{
			return Get().Load_(meshName);
		}
		static bool Set(const std::string& meshName, Ref<VertexArray>& vertexArray)
		{
			return Get().Set_(meshName, vertexArray);
		}
		static std::unordered_map<std::string, Ref<VertexArray>>& GetMeshes() { return Get().m_vertexArray; }
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
		bool Set_(const std::string& meshName, Ref<VertexArray>& vertexArray)
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
		std::unordered_map<std::string, std::vector<Ref<VertexArray>>> m_vertexSet;
	};
}