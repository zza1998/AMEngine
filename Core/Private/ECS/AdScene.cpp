#include "ECS/AdScene.h"
#include "ECS/AdEntity.h"
#include "ECS/Component/AdTransformComponent.h"

namespace ade{
    AdScene::AdScene() {
        mRootNode = std::make_shared<AdNode>();
    }

    AdScene::~AdScene() {
        mRootNode.reset();
        DestroyAllEntity();
        mEntities.clear();
    }

    AdEntity *AdScene::CreateEntity(const std::string &name) {
        return CreateEntityWithUUID(AdUUID(), name);
    }

    AdEntity *AdScene::CreateEntityWithUUID(const AdUUID &id, const std::string &name) {
        auto enttEntity = mEcsRegistry.create();
        mEntities.insert({ enttEntity, std::make_shared<AdEntity>(enttEntity, this) });
        mEntities[enttEntity]->SetParent(mRootNode.get());
        mEntities[enttEntity]->SetId(id);
        mEntities[enttEntity]->SetName(name.empty() ? "Entity" : name);

        // add default components
        mEntities[enttEntity]->AddComponent<AdTransformComponent>();

        return mEntities[enttEntity].get();
    }

    void AdScene::DestroyEntity(const AdEntity *entity) {
        if(entity && entity->IsValid()){
            mEcsRegistry.destroy(entity->GetEcsEntity());
        }

        auto it = mEntities.find(entity->GetEcsEntity());
        if(it != mEntities.end()){
            AdNode *parent = it->second->GetParent();
            if(parent){
                parent->RemoveChild(it->second.get());
            }
            mEntities.erase(it);
        }
    }

    void AdScene::DestroyAllEntity() {
        mEcsRegistry.clear();
        mEntities.clear();
    }

    AdEntity *AdScene::GetEntity(entt::entity enttEntity) {
        if(mEntities.find(enttEntity) != mEntities.end()){
            return mEntities.at(enttEntity).get();
        }
        return nullptr;
    }
}