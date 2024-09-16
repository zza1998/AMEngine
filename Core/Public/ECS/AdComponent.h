#ifndef ADCOMPONENT_H
#define ADCOMPONENT_H

#include "ECS/AdEntity.h"
#include "entt/meta/factory.hpp"

namespace ade{
#define HS(s) entt::hashed_string(s)
#define FUNC_NAME_HAS_COMPONENT       "HasComponent"
#define FUNC_NAME_GET_COMPONENT       "GetComponent"
#define FUNC_NAME_ADD_COMPONENT       "AddComponent"
#define FUNC_NAME_REMOVE_COMPONENT    "RemoveComponent"
#define FUNC_NAME_DRAW_GUI            "OnDrawGui"

#define REG_NODE_FUNCS(className) \
.func<&ade::AdEntity::HasComponent<className>, entt::as_is_t>(HS(FUNC_NAME_HAS_COMPONENT))                          \
.func<&ade::AdEntity::GetComponent<className>, entt::as_ref_t>(HS(FUNC_NAME_GET_COMPONENT))                         \
.func<&ade::AdEntity::AddComponent<className>, entt::as_void_t>(HS(FUNC_NAME_ADD_COMPONENT))                        \
.func<&ade::AdEntity::RemoveComponent<className>, entt::as_void_t>(HS(FUNC_NAME_REMOVE_COMPONENT))                  \

#define REG_COMPONENT(className) static void OnRegisterReflections(){ \
entt::meta<className>().prop(HS("Label"), STR(className)) \
.func<&ade::AdComponent::OnDrawGui, entt::as_void_t>(HS(FUNC_NAME_DRAW_GUI))REG_NODE_FUNCS(className); \
}

    class AdComponent{
    public:
        void SetOwner(AdEntity *owner) { mOwner = owner;}
        AdEntity *GetOwner() const { return mOwner; }

        virtual void OnDrawGui(){}
    private:
        AdEntity *mOwner = nullptr;
    };
}
#endif