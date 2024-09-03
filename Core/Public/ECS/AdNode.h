#ifndef ADNODE_H
#define ADNODE_H

#include "AdUUID.h"

namespace ade{
    class AdNode{
    public:
        AdNode() = default;
        virtual ~AdNode() = default;

        AdUUID GetId() const;
        void SetId(const AdUUID &nodeId);
        const std::string &GetName() const;
        void SetName(const std::string &name);

        const std::vector<AdNode *> &GetChildren() const;
        bool HasParent();
        bool HasChildren();
        void SetParent(AdNode *node);
        AdNode *GetParent() const;
        void AddChild(AdNode *node);
        void RemoveChild(AdNode *node);
    private:
        AdUUID mId;
        std::string mName;
        AdNode *mParent = nullptr;
        std::vector<AdNode*> mChildren;
    };
}

#endif