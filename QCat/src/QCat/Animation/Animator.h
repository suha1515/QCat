#pragma once
#include "QCat/Core/QCatCore.h"
#include "Animation.h"


namespace QCat
{
	class Animator
	{
	public:
		Animator() = default;
		void Initialize(const aiScene* scene,Entity& rootNode);
		
		void PlayAnimation(int index)
		{
			m_CurrentAnimation = &animationClips[index];
			m_CurrentTime = 0.0f;
		}
		void UpdateAnimation(float dt);
		void CalculateBoneTransform(NodeData& node, glm::mat4 parentTransform);

		std::vector<glm::mat4>& GetTransforms()
		{
			return m_Transforms;
		}
	public:
		std::map<std::string, BoneInfo> m_OffsetMatMap;
		int m_BoneCount=0;
	private:
		std::vector<glm::mat4> m_Transforms;
		NodeData m_RootNode;
		std::vector<Animation> animationClips;
		Animation* m_CurrentAnimation;
		float m_CurrentTime;
		float m_DeltaTime;
	};
}