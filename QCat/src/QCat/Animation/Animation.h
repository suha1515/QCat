#pragma once
#include "QCat/Core/QCatCore.h"
#include "Bone.h"
#include "AnimData.h"

#include <assimp/scene.h>

namespace QCat
{
	
	class Animation
	{
	public:
		Animation() = default;
		Animation(const aiAnimation* animationClip, std::map<std::string, BoneInfo>& boneMap, int& boneCount)
		{
			m_Duration = animationClip->mDuration;
			m_TicksPerSecond = animationClip->mTicksPerSecond;
			clipName = animationClip->mName.C_Str();
			SetBone(animationClip, boneMap, boneCount);
		}
		Bone* FindBone(const std::string& name)
		{
			auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
				[&](const Bone& Bone)
				{
					return Bone.GetBoneName() == name;
				});

			if (iter == m_Bones.end()) return nullptr;
			else return &(*iter);
		}
		inline float GetTicksPerSecond() { return m_TicksPerSecond; }
		inline float GetDuration() { return m_Duration; }

	private:
		void SetBone(const aiAnimation* animationClip,std::map<std::string, BoneInfo>& boneMap,int& boneCount)
		{
			int size = animationClip->mNumChannels;
			for (int i = 0; i < size; ++i)
			{
				auto channel = animationClip->mChannels[i];
				std::string boneName = channel->mNodeName.data;
				if (boneMap.find(boneName) == boneMap.end())
				{
					boneMap[boneName].id = boneCount;
					boneCount++;
				}
				m_Bones.push_back(Bone(boneName, boneMap[boneName].id, channel));
			}
		}

	private:
		float m_Duration;
		int m_TicksPerSecond;
		std::string clipName;
		std::vector<Bone> m_Bones;
	};
}