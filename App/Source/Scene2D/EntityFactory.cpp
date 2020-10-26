#include "EntityFactory.h"
#include <stdlib.h>
#include <sstream>
#include "Enemy2D.h"

EntityFactory::EntityFactory(): currKey(0){}

EntityFactory::~EntityFactory(){
	for(std::map<int, Entity*>::iterator it = entityMap.begin(); it != entityMap.end(); ++it){
		if(it->second){
			delete it->second; //Deallocate mem if not done somewhere else
			it->second = NULL;
		}
	}
}

Entity* const& EntityFactory::Create(const Entity::ENTITY_TYPE& type){
	switch(type){
		case Entity::ENTITY_TYPE::PLAYER: (void)AddEntity(new CPlayer2D); break;
		case Entity::ENTITY_TYPE::ENEMY: (void)AddEntity(new CEnemy2D); break;
		default: exit(1);
	}
	return entityMap[currKey - 1];
}

Entity* const EntityFactory::GetEntity(const short& key){
	return entityMap.count(key) ? entityMap[key] : nullptr;
}

bool EntityFactory::AddEntity(const Entity* const& entity){
	if(!entity){ //Trivial Acceptance (valid ptr provided)
		return false;
	}
	entityMap[currKey] = const_cast<Entity* const&>(entity);
	++currKey;
	return true;
}

bool EntityFactory::RemoveEntity(const short& key){
	Entity* entity = GetEntity(key);
	if(!entity){
		return false;
	}
	delete entity;
	entityMap.erase(key);
	return true;
}

size_t EntityFactory::GetEntityMapSize() const{
	return entityMap.size();
}

void EntityFactory::Update(const float& dt){
	for(auto iter = entityMap.begin(); iter != entityMap.end(); ++iter){
		switch(iter->second->type){
			case Entity::ENTITY_TYPE::PLAYER: dynamic_cast<CPlayer2D*>(iter->second)->Update(dt); break;
			case Entity::ENTITY_TYPE::ENEMY: dynamic_cast<CEnemy2D*>(iter->second)->Update(dt); break;
		}
	}
};

void EntityFactory::PreRender(){
	for(auto iter = entityMap.begin(); iter != entityMap.end(); ++iter){
		switch(iter->second->type){
			case Entity::ENTITY_TYPE::PLAYER: dynamic_cast<CPlayer2D*>(iter->second)->PreRender(); break;
			case Entity::ENTITY_TYPE::ENEMY: dynamic_cast<CEnemy2D*>(iter->second)->PreRender(); break;
		}
	}
}

void EntityFactory::Render(){
	for(auto iter = entityMap.begin(); iter != entityMap.end(); ++iter){
		switch(iter->second->type){
			case Entity::ENTITY_TYPE::PLAYER: dynamic_cast<CPlayer2D*>(iter->second)->Render(); break;
			case Entity::ENTITY_TYPE::ENEMY: dynamic_cast<CEnemy2D*>(iter->second)->Render(); break;
		}
	}
}

void EntityFactory::PostRender(){
	for(auto iter = entityMap.begin(); iter != entityMap.end(); ++iter){
		switch(iter->second->type){
			case Entity::ENTITY_TYPE::PLAYER: dynamic_cast<CPlayer2D*>(iter->second)->PostRender(); break;
			case Entity::ENTITY_TYPE::ENEMY: dynamic_cast<CEnemy2D*>(iter->second)->PostRender(); break;
		}
	}
}