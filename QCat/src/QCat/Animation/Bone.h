#pragma once
#include "QCat/Core/QCatCore.h"
#include <assimp/scene.h>
#include <glm/gtx/quaternion.hpp>
#include "QCat/Uitiliy/Assimphelper.h"

namespace QCat
{
	struct KeyPosition
	{
		glm::vec3 position;
		float timeStamp;
	};
	struct KeyRotation
	{
		glm::quat orientation;
		float timeStamp;
	};
	struct KeyScale
	{
		glm::vec3 Scale;
		float timeStamp;
	};
	class Bone
	{
	public:
		Bone(const std::string  name, int id, const aiNodeAnim* channel)
			:m_Name(name), m_ID(id), m_LocalTransform(1.0f)
		{
			//Position Key
			m_NumPositions = channel->mNumPositionKeys;
			for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
			{
				aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
				float timeStamp = channel->mPositionKeys[positionIndex].mTime;
				KeyPosition data;
				data.position = Utils::ConvertToGlm(aiPosition);
				data.timeStamp = timeStamp;
				m_Positions.push_back(data);
			}

			//Rotation Key
			m_NumRotations = channel->mNumRotationKeys;
			for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
			{
				aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
				float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
				KeyRotation data;
				data.orientation = Utils::ConvertToGlm(aiOrientation);
				data.timeStamp = timeStamp;
				m_Rotations.push_back(data);
			}

			//Scale Key
			m_NumScalings = channel->mNumScalingKeys;
			for (int scaleIndex = 0; scaleIndex < m_NumScalings; ++scaleIndex)
			{
				aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
				float timeStamp = channel->mScalingKeys[scaleIndex].mTime;
				KeyScale data;
				data.Scale = Utils::ConvertToGlm(aiScale);
				data.timeStamp = timeStamp;
				m_Scales.push_back(data);
			}

		}

		void Update(float animTime)
		{
			glm::mat4 translation = InterPolatePosition(animTime);
			glm::mat4 rotation = InterPolateRotation(animTime);
			glm::mat4 scale = InterPolateScaling(animTime);
			m_LocalTransform = translation * rotation * scale;
		}
		glm::mat4 GetLocalTransform() { return m_LocalTransform; }
		std::string GetBoneName() const { return m_Name; }
		int GetBoneID() { return m_ID; }

		int GetPositionIndex(float animTime)
		{
			for (int index = 0; index < m_NumPositions - 1; ++index)
			{
				if (animTime < m_Positions[index + 1].timeStamp)
					return index;
			}
			QCAT_ASSERT(false);
		}
		int GetRotationIndex(float animTime)
		{
			for (int index = 0; index < m_NumRotations - 1; ++index)
			{
				if (animTime < m_Rotations[index + 1].timeStamp)
					return index;
			}
			QCAT_ASSERT(false);
		}
		int GetScaleIndex(float animTime)
		{
			for (int index = 0; index < m_NumScalings - 1; ++index)
			{
				if (animTime < m_Scales[index + 1].timeStamp)
					return index;
			}
			QCAT_ASSERT(false);
		}
	private:
		float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animTime)
		{
			float scaleFactor = 0.0f;
			float midWayLength = animTime - lastTimeStamp;
			float frameDiff = nextTimeStamp - lastTimeStamp;
			scaleFactor = midWayLength / frameDiff;
			return scaleFactor;
		}
		// InterPolation Position
		glm::mat4 InterPolatePosition(float animTime)
		{
			if (1 == m_NumPositions)
				return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

			int index1 = GetPositionIndex(animTime);
			int index2 = index1 + 1;
			float scaleFactor = GetScaleFactor(m_Positions[index1].timeStamp, m_Positions[index2].timeStamp, animTime);
			glm::vec3 finalPosition = glm::mix(m_Positions[index1].position, m_Positions[index2].position,scaleFactor);
			return glm::translate(glm::mat4(1.0f), finalPosition);
		}
		// InterPolation Rotation
		glm::mat4 InterPolateRotation(float animTime)
		{
			if (1 == m_NumRotations)
			{
				auto rotation = glm::normalize(m_Rotations[0].orientation);
				return glm::toMat4(rotation);
			}
			int index1 = GetRotationIndex(animTime);
			int index2 = index1 + 1;
			float scaleFactor = GetScaleFactor(m_Rotations[index1].timeStamp, m_Rotations[index2].timeStamp, animTime);
			glm::quat finalRotation = glm::slerp(m_Rotations[index1].orientation, m_Rotations[index2].orientation, scaleFactor);
			finalRotation = glm::normalize(finalRotation);
			return glm::toMat4(finalRotation);
		}

		//InterPolateScale
		glm::mat4 InterPolateScaling(float animTime)
		{
			if (1 == m_NumScalings)
				return glm::scale(glm::mat4(1.0f), m_Scales[0].Scale);
			int index1 = GetScaleIndex(animTime);
			int index2 = index1 + 1;
			float scaleFactor = GetScaleFactor(m_Scales[index1].timeStamp, m_Scales[index2].timeStamp, animTime);
			glm::vec3 finalScale = glm::mix(m_Scales[index1].Scale, m_Scales[index2].Scale, scaleFactor);
			return glm::scale(glm::mat4(1.0f), finalScale);
		}

		std::vector<KeyPosition> m_Positions;
		std::vector<KeyRotation> m_Rotations;
		std::vector<KeyScale> m_Scales;

		int m_NumPositions;
		int m_NumRotations;
		int m_NumScalings;

		glm::mat4 m_LocalTransform;
		std::string m_Name;
		int m_ID;
	};
	/*class BoneLibrary
	{
	public:
		static BoneInfo GetBoneMatrix(const std::string& modelPath, const std::string boneName)
		{
			return Get().GetBoneMatrix_(modelPath, boneName);
		}
		static BoneInfo GetBoneMatrix(const std::string& modelPath, const uint32_t entityId)
		{
			return Get().GetBoneMatrix_(modelPath, entityId);
		}
		static void SetBoneMap(const std::string& modelPath, std::unordered_map<std::string, BoneInfo>& boneMap)
		{
			Get().SetBoneMap_(modelPath, boneMap);
		}
		static void SetEntityMap(const uint32_t id, std::string boneName)
		{
			Get().SetEntityMap_(id, boneName);
		}
	private:

		static BoneLibrary& Get()
		{
			static BoneLibrary lib;
			return lib;
		}
		void SetBoneMap_(const std::string& modelPath, std::unordered_map<std::string, BoneInfo>& boneMap)
		{
			auto iter = m_boneMaps.find(modelPath);
			if (iter == m_boneMaps.end())
				m_boneMaps[modelPath] = boneMap;
			else
				QCAT_CORE_ERROR("There is aready bonemap on modelPath {0} ", modelPath);
		}
		void SetEntityMap_(const uint32_t id, std::string boneName)
		{
			auto iter = m_nodeToBoneNameMap.find(id);
			if (iter == m_nodeToBoneNameMap.end())
				m_nodeToBoneNameMap[id] = boneName;
			else
				QCAT_CORE_ERROR("There is aready Entity on EntityMap : {0} ", id);
		}

		BoneInfo GetBoneMatrix_(const std::string& modelPath, const std::string boneName)
		{
			auto iter = m_boneMaps.find(modelPath);
			if (iter != m_boneMaps.end())
			{
				auto& bonemap = m_boneMaps[modelPath];
				auto iter2 = bonemap.find(boneName);
				if (iter2 != bonemap.end())
					return bonemap[boneName];
				else
				{
					QCAT_CORE_ERROR("There is no BoneName '{0}' on ModelPath : {1}",boneName,modelPath);
					return BoneInfo();
				}
			}
			else
			{
				QCAT_CORE_ERROR("There is no modelPath '{0}' ", modelPath);
				return BoneInfo();
			}
		}

		BoneInfo GetBoneMatrix_(const std::string& modelPath, const uint32_t entityId)
		{
			auto iter_node = m_nodeToBoneNameMap.find(entityId);
			std::string boneName;
			if (iter_node != m_nodeToBoneNameMap.end())
				boneName = iter_node->second;
			else
			{
				QCAT_CORE_ERROR("There is no entityID '{0}' on nodeMap", entityId);
				return BoneInfo();
			}
			auto iter = m_boneMaps.find(modelPath);
			if (iter != m_boneMaps.end())
			{
				auto& bonemap = m_boneMaps[modelPath];
				auto iter2 = bonemap.find(boneName);
				if (iter2 != bonemap.end())
					return bonemap[boneName];
				else
				{
					QCAT_CORE_ERROR("There is no BoneName '{0}' on ModelPath : {1}", boneName, modelPath);
					return BoneInfo();
				}
			}
			else
			{
				QCAT_CORE_ERROR("There is no modelPath '{0}' ", modelPath);
				return BoneInfo();
			}
		}

	private:
		std::unordered_map<std::string,std::unordered_map<std::string, BoneInfo>> m_boneMaps;
		std::unordered_map<uint32_t, std::string> m_nodeToBoneNameMap;
	};*/
}