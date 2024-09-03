#include "ECS/AdNode.h"

namespace ade{
    AdUUID AdNode::GetId() const {
        return mId;
    }

    void AdNode::SetId(const AdUUID &nodeId) {
        mId = nodeId;
    }

    const std::string &AdNode::GetName() const {
        return mName;
    }

    void AdNode::SetName(const std::string &name) {
        mName = name;
    }

    const std::vector<AdNode *> &AdNode::GetChildren() const {
        return mChildren;
    }

    bool AdNode::HasParent() {
        return mParent != nullptr;
    }

    bool AdNode::HasChildren() {
        return !mChildren.empty();
    }

    void AdNode::SetParent(AdNode *node) {
        node->AddChild(this);
    }

    AdNode *AdNode::GetParent() const {
        return mParent;
    }

    void AdNode::AddChild(AdNode *node) {
        if(node->HasParent()){
            node->GetParent()->RemoveChild(node);
        }
        node->mParent = this;
        mChildren.push_back(node);
    }

    void AdNode::RemoveChild(AdNode *node) {
        if(!HasChildren()){
            return;
        }
        for(auto it = mChildren.begin(); it != mChildren.end(); ++it){
            if(node == *it){
                mChildren.erase(it);
                break;
            }
        }
    }
}