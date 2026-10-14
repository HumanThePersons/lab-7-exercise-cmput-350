#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <cstddef>  // size_t
#include <array>
#include <cassert>
#include <limits>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "pack.h"  // Pack from prep
#include "registry.h"

using EntityID = size_t;

template <typename... ComponentTypes>
class EntityManager {
public:
    EntityID createEntity() {
        EntityID id;
        if (!freeEntities.empty()) {
            id = freeEntities.back();
            freeEntities.pop_back();
            aliveEntities[id] = true;
            for (auto& column : componentIndices) {
                column[id] = INVALID_COMPONENT_ID;  // no components by default
            }
        } else {
            id = aliveEntities.size();
            aliveEntities.push_back(true);
            for (auto& column : componentIndices) {
                column.push_back(INVALID_COMPONENT_ID);
            }
        }
        return id;
    }

    bool validEntity(EntityID id) const {
        return (id < aliveEntities.size()) && aliveEntities[id];
    }

    template <typename ComponentType>
    bool hasComponent(EntityID id) const {
        constexpr size_t typeIndex = componentTypeIndex<ComponentType>();
        assert(validEntity(id));

        return (componentIndices[typeIndex][id] != INVALID_COMPONENT_ID);
    }

    // Used AI in trying to understand the difference between const and regular of the following method - why we need both
    template <typename ComponentType>
    ComponentType& getComponent(EntityID id) {
        constexpr size_t componentIndex = componentTypeIndex<ComponentType>();
        assert(hasComponent<ComponentType>(id));

        return (std::get<componentIndex>(registries)[componentIndices[componentIndex][id]]);
    }

    template <typename ComponentType>
    const ComponentType& getComponent(EntityID id) const {
        constexpr size_t componentIndex = componentTypeIndex<ComponentType>();
        assert(hasComponent<ComponentType>(id));

        return (std::get<componentIndex>(registries)[componentIndices[componentIndex][id]]);
    }

    template <typename ComponentType>
    void addComponent(EntityID id, ComponentType&& component) {
        using T = std::remove_reference_t<ComponentType>;

        constexpr size_t componentIndex = componentTypeIndex<T>();
        assert(validEntity(id));
        assert(!hasComponent<T>(id));

        componentIndices[componentIndex][id] =
            std::get<componentIndex>(registries).store(std::forward<ComponentType>(component));
    }

private:
    static constexpr size_t INVALID_COMPONENT_ID = std::numeric_limits<size_t>::max();
    static constexpr size_t NUM_COMPONENT_TYPES = sizeof...(ComponentTypes);

    template <typename ComponentType>
    static constexpr size_t componentTypeIndex() {
        static_assert(Pack<ComponentTypes...>::template contains<ComponentType>, "Not a component type in this entity manager");
        
        return static_cast<size_t>(Pack<ComponentTypes...>::template index<ComponentType>);
    }

    std::vector<bool> aliveEntities;
    std::vector<EntityID> freeEntities;

    // one Registry per component type
    std::tuple<Registry<ComponentTypes>...> registries;

    // array with [component types index][entity id]
    std::array<std::vector<size_t>, NUM_COMPONENT_TYPES> componentIndices;
};

#endif  // ENTITY_MANAGER_H
