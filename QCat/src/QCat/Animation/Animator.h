#pragma once
#include "QCat/Core/QCatCore.h"
#include "Animation.h"


namespace QCat
{
	class Animator
	{
	public:
		Animator() = default;
		void Initialize(const aiScene* scene, std::vector<std::pair<uint32_t, std::string>>& nodes, std::unordered_map<std::string, BoneInfo>& boneMap, uint32_t& boneCount);
		
		void PlayAnimation(int index)
		{
			m_CurrentAnimation = index;
			m_CurrentTime = 0.0f;
		}
		void UpdateAnimation(Scene* pScene,float dt);
		void CalculateBoneTransform(Scene* pScene);
	private:
		void ReadHierarchy(Entity& node);
	public:
		std::unordered_map<uint32_t, std::string> m_MapNodeID;
	private:
		std::vector<std::pair<uint32_t, std::string>> m_Nodes;
		std::vector<Animation> animationClips;
		int m_CurrentAnimation=0;
		float m_CurrentTime;
		float m_DeltaTime;
		bool isStop = true;
	};
}