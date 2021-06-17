#include "qcpch.h"
#include "Animator.h"
#include "QCat/Scene/Components.h"

namespace QCat
{
    void ReadHierarchy(Entity& node, NodeData& data)
    {
        data.entity = node;
        data.nodeName = node.GetComponent<TagComponent>().Tag;
        auto child = node.GetComponent<RelationShipComponent>().firstChild;

        while (child != Entity::emptyEntity)
        {
            NodeData node;
            ReadHierarchy(child, node);
            child = child.GetComponent<RelationShipComponent>().nextSibling;
            data.children.push_back(node);
        }
    }
    void Animator::Initialize(const aiScene* scene, Entity& rootNode)
    {
        ReadHierarchy(rootNode, m_RootNode);
        m_CurrentTime = 0.0f;
        m_Transforms.reserve(100);
        for (int i = 0; i < 100; ++i)
            m_Transforms.push_back(glm::mat4(1.0f));
        for (int i = 0; i < scene->mNumAnimations; ++i)
        {
            animationClips.push_back(Animation(scene->mAnimations[i],m_OffsetMatMap,m_BoneCount));
        }
        if(scene->mNumAnimations>0)
            m_CurrentAnimation = &animationClips[0];
    }
    void Animator::UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        if (m_CurrentAnimation)
        {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            auto& entity = m_RootNode.entity;
            glm::mat4 parentMat = glm::mat4(1.0f);
            auto& parent = entity.GetComponent<RelationShipComponent>().parent;
            if (parent != Entity::emptyEntity)
                parentMat = parent.GetComponent<TransformComponent>().GetTransform();
            CalculateBoneTransform(m_RootNode, parentMat);
        }
    }
    void Animator::CalculateBoneTransform(NodeData& node, glm::mat4 parentTransform)
    {
        auto& entity = node.entity;
        std::string nodeName = node.nodeName;
        glm::mat4 nodeTransform = entity.GetComponent<TransformComponent>().GetLocalTransform();

        Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }
        glm::mat4 globalTransformation = parentTransform * nodeTransform;
        if (m_OffsetMatMap.find(nodeName) != m_OffsetMatMap.end())
        {
            int index = m_OffsetMatMap[nodeName].id;
            glm::mat4 offset = m_OffsetMatMap[nodeName].offsetMatrix;
            m_Transforms[index] = globalTransformation * offset;
          // m_Transforms[index] = offset;

        }
        for (int i = 0; i < node.children.size(); ++i)
            CalculateBoneTransform(node.children[i], globalTransformation);
    }
   

}