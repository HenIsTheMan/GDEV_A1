#pragma once
#include "DesignPatterns\SingletonTemplate.h"
#include <map>
#include "Entity.h"

class EntityFactory final: public CSingletonTemplate<EntityFactory>{
	friend CSingletonTemplate<EntityFactory>;
	EntityFactory();
	std::map<int, Entity*> entityMap;
public:
	~EntityFactory();
	Entity* const& Create(const Entity::ENTITY_TYPE& type);
	Entity* const GetEntity(const short& key);
	bool AddEntity(const Entity* const& entity);
	bool RemoveEntity(const short& key);
	size_t GetEntityMapSize() const;
	short currKey;
	void Update(const float& dt);
	void PreRender();
	void Render();
	void PostRender();
};