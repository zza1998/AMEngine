#ifndef ADSCENE_H
#define ADSCENE_H

#include "AdUUID.h"
#include "entt/entity/registry.hpp"

namespace ade{
    class AdNode;
    class AdEntity;

    class AdScene{
    public:
        AdScene();
        ~AdScene();

        AdEntity* CreateEntity(const std::string &name = "");
        AdEntity* CreateEntityWithUUID(const AdUUID &id, const std::string &name = "");
        void DestroyEntity(const AdEntity *entity);
        void DestroyAllEntity();

        entt::registry &GetEcsRegistry() { return mEcsRegistry; }
        AdNode *GetRootNode() const { return mRootNode.get(); }
        AdEntity *GetEntity(entt::entity enttEntity);
    private:
        std::string mName;
        entt::registry mEcsRegistry;

        std::unordered_map<entt::entity, std::shared_ptr<AdEntity>> mEntities;
        std::shared_ptr<AdNode> mRootNode;

        friend class AdEntity;
    };
}

#endif