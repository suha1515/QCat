#include "qcpch.h"
#include "Animator.h"
#include "QCat/Scene/Components.h"

namespace QCat
{
    void Animator::ReadHierarchy(Entity& node)
    {
        std::string nodeName = node.GetComponent<TagComponent>().Tag;
        uint32_t id = node.GetComponent<GuidComponent>().uid;
        auto iter = m_MapNodeID.find(id);
        if (iter != m_MapNodeID.end())
            m_MapNodeID[id] = nodeName;

        auto child = node.GetComponent<RelationShipComponent>().firstChild;


        Scene* currentScene = node.GetScene();
        Entity entity;
        while ((entity = currentScene->GetEntityById(child)) !=Entity::emptyEntity)
        {
            NodeData node;
            ReadHierarchy(entity);
            child = entity.GetComponent<RelationShipComponent>().nextSibling;
        }
    }
    void Animator::Initialize(const aiScene* scene, std::vector<std::pair<uint32_t, std::string>>& nodes, std::unordered_map<std::string, BoneInfo>& boneMap,uint32_t& boneCount)
    {
       
        for (int i = 0; i < scene->mNumAnimations; ++i)
        {
            animationClips.push_back(Animation(scene->mAnimations[i], boneMap, boneCount));
        }

        for (auto iter = nodes.begin(); iter != nodes.end();)
        {
            auto find_iter = boneMap.find(iter->second);
            if (find_iter == boneMap.end())
            {
                iter = nodes.erase(iter);
            }
            else
                iter++;
        }
        m_Nodes = nodes;
    }
    void Animator::UpdateAnimation(Scene* pScene,float dt)
    {
        m_DeltaTime = dt;
        m_CurrentTime += animationClips[m_CurrentAnimation].GetTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, animationClips[m_CurrentAnimation].GetDuration());

        glm::mat4 parentMat = glm::mat4(1.0f);
        CalculateBoneTransform(pScene);
    }
    void Animator::CalculateBoneTransform(Scene* pScene )
	{
		for (auto& entity : m_Nodes)
		{
			std::string& nodeName = entity.second;
			Bone* Bone = animationClips[m_CurrentAnimation].FindBone(nodeName);
			if (Bone)
			{
				Bone->Update(m_CurrentTime);
				glm::mat4 nodeTransform = Bone->GetLocalTransform();
				pScene->GetEntityById(entity.first).GetComponent<TransformComponent>().SetTransform(nodeTransform);
			}
		}
        //glm::mat4 globalTransformation = parentTransform * nodeTransform;
       /* if (m_OffsetMatMap.find(nodeName) != m_OffsetMatMap.end())
        {
            int index = m_OffsetMatMap[nodeName].id;
            glm::mat4 offset = m_OffsetMatMap[nodeName].offsetMatrix;
            m_Transforms[index] = offset;
        }*/
        /*for (int i = 0; i < node.children.size(); ++i)
            CalculateBoneTransform(node.children[i], globalTransformation);*/
    }
   

}