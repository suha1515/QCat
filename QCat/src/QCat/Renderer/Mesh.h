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
		static void Reset()
		{
			Get().Reset_();
		}
		static std::vector<std::string>& GetModelList() { return Get().modelPath; };
	private:
		static ModelLibrary& Get()
		{
			static ModelLibrary lib;
			return lib;
		}
		void Reset_()
		{
			modelPath.clear();
		}
		void Load_(const std::string& modelPath);
	private:
		std::vector<std::string> modelPath;
	};
	class MeshLibrary
	{
	public:
		static void Reset()
		{
			Get().Reset_();
		}
		static Ref<VertexArray> Load(const std::string& meshName)
		{
			return Get().Load_(meshName);
		}
		static bool Set(const std::string& meshName, Ref<VertexArray>& vertexArray)
		{
			return Get().Set_(meshName, vertexArray);
		}
		static std::unordered_map<std::string, Ref<VertexArray>>& GetMeshes() { return Get().m_vertexArray; }
		static bool IsExist(const std::string& meshName)
		{
			return Get().IsExist_(meshName);
		}
	private:
		static MeshLibrary& Get()
		{
			static MeshLibrary lib;
			return lib;
		}
		void Reset_()
		{
			m_vertexArray.clear();
		}
		bool IsExist_(const std::string& meshName) const
		{
			return m_vertexArray.find(meshName) != m_vertexArray.end();
		}
		Ref<VertexArray> Load_(const std::string& meshName)
		{
			if (IsExist_(meshName))
			{
				return m_vertexArray[meshName];
			}
			else
				return nullptr;
		}
		bool Set_(const std::string& meshName, Ref<VertexArray>& vertexArray)
		{
			if (IsExist_(meshName))
			{
				QCAT_CORE_ERROR("There is already same meshname :{0}", meshName);
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