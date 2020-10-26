#pragma once
#include "RenderControl\shader.h"
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>
#include "GameControl\Settings.h"
#include "AnimatedSprites.h"
#include "Physics2D.h"

class Entity{
public:
	enum class DIR{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		AMT
	};
	enum class ENTITY_TYPE{
		PLAYER,
		ENEMY,
		AMT,
	};
	template<class T>
	friend bool IsMidAir(const T* const& entity);
	template<class T>
	friend void Constraint(T* const& entity);
	template<class T>
	friend void FindFeasiblePos(T* const& entity, const int oldIndex);
	template<class T>
	friend void UpdateJumpFall(T* const& entity, const float& dt);

	Entity(const ENTITY_TYPE& type);
	virtual ~Entity();
	virtual bool Init();
	virtual void PreRender();
	virtual void Render();
	virtual void PostRender();
	DIR currDir;
	ENTITY_TYPE type;

	///Getters
	glm::i32vec2 Geti32vec2Index() const;
	glm::i32vec2 Geti32vec2NumMicroSteps() const;
	glm::vec2 Getvec2UVCoords() const;

	///Setters
	virtual void SetShader(Shader* cShader);
	void Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis);
	void Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis);
	void Setvec2UVCoords(const float fUVCoords_XAxis, const float fUVCoords_YAxis);
protected:
	Shader* cShader;
	CSettings* cSettings;
	unsigned int VBO, VAO, EBO;

	glm::i32vec2 i32vec2Index;
	glm::i32vec2 i32vec2NumMicroSteps;
	glm::vec2 vec2UVCoords;
	virtual bool LoadTexture(const char* fPath, GLuint& iTextureID);

	CAnimatedSprites cAnimatedSprites;
	CPhysics2D cPhysics2D;
	float aliveTime;
	glm::mat4 transform;
	unsigned int currTexID;
	unsigned int idleLeftTexID;
	unsigned int idleRightTexID;
	unsigned int jumpLeftTexID;
	unsigned int jumpRightTexID;
	unsigned int fallLeftTexID;
	unsigned int fallRightTexID;
};

class Map2D;
template<class T>
bool IsMidAir(const T* const& entity){
	return (entity->i32vec2Index.y != 0 /*If entity is not at the bottom row...*/ && entity->i32vec2NumMicroSteps.x == 0 &&
		(entity->cMap2D->GetMapInfo(entity->i32vec2Index.y - 1, entity->i32vec2Index.x) < 100 || entity->cMap2D->GetMapInfo(entity->i32vec2Index.y - 1, entity->i32vec2Index.x) > 199) ? true : false);
}

template<class T>
void Constraint(T* const& entity){
	if(entity->currDir == Entity::DIR::LEFT && entity->i32vec2Index.x < 0){
		entity->i32vec2Index.x = 0;
		entity->i32vec2NumMicroSteps.x = 0;
		entity->currTexID = entity->idleLeftTexID;
	}
	if(entity->currDir == Entity::DIR::RIGHT && entity->i32vec2Index.x >= int(entity->cSettings->NUM_TILES_XAXIS) - 1){
		entity->i32vec2Index.x = int(entity->cSettings->NUM_TILES_XAXIS) - 1;
		entity->i32vec2NumMicroSteps.x = 0;
		entity->currTexID = entity->idleRightTexID;
	}
	if(entity->cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP && entity->i32vec2Index.y >= int(entity->cSettings->NUM_TILES_YAXIS) - 1){
		entity->i32vec2Index.y = int(entity->cSettings->NUM_TILES_YAXIS) - 1;
		entity->i32vec2NumMicroSteps.y = 0;
	}
	if(entity->cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL && entity->i32vec2Index.y < 0){
		entity->i32vec2Index.y = 0;
		entity->i32vec2NumMicroSteps.y = 0;
		entity->cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
	}
	if((short)entity->currDir > 3 || (short)entity->currDir < 0){
		cout << "Constraint: Unknown movement dir." << endl;
	}
}

template<class T>
void FindFeasiblePos(T* const& entity, const int oldIndex){
	if(entity->currDir == Entity::DIR::LEFT){
		if(entity->i32vec2NumMicroSteps.x != 0){
			if(entity->cMap2D->GetMapInfo(entity->i32vec2Index.y, entity->i32vec2Index.x) > 99 && entity->cMap2D->GetMapInfo(entity->i32vec2Index.y, entity->i32vec2Index.x) < 200){ //If new pos is not accessible...
				entity->currTexID = entity->idleLeftTexID;
				entity->i32vec2Index.x = oldIndex; //Snap to prev index
				entity->i32vec2NumMicroSteps.x = 0; //Reset entity->i32vec2NumMicroSteps.x so entity is aligned to the column
			} //Go back to OG pos

			if(entity->i32vec2NumMicroSteps.y != 0){ //If new pos is between 2 rows...
				if(entity->cMap2D->GetMapInfo(entity->i32vec2Index.y + 1, entity->i32vec2Index.x) > 99 && entity->cMap2D->GetMapInfo(entity->i32vec2Index.y + 1, entity->i32vec2Index.x) < 200){ //If new pos is not feasible...
					entity->currTexID = entity->idleLeftTexID;
					entity->i32vec2Index.x = oldIndex; //...
					entity->i32vec2NumMicroSteps.x = 0; //...
				} //Go back to OG pos
			} //Check other row also
		}
		return;
	}
	if(entity->currDir == Entity::DIR::RIGHT){
		if(entity->i32vec2NumMicroSteps.x != 0){
			if(entity->cMap2D->GetMapInfo(entity->i32vec2Index.y, entity->i32vec2Index.x + 1) > 99 && entity->cMap2D->GetMapInfo(entity->i32vec2Index.y, entity->i32vec2Index.x + 1) < 200){ //If new pos is not accessible...
				entity->currTexID = entity->idleRightTexID;
				entity->i32vec2Index.x = oldIndex; //Snap to prev index
				entity->i32vec2NumMicroSteps.x = 0; //Reset entity->i32vec2NumMicroSteps.x so entity is aligned to the column
			} //Go back to OG pos

			if(entity->i32vec2NumMicroSteps.y != 0){ //If new pos is between 2 rows...
				if(entity->cMap2D->GetMapInfo(entity->i32vec2Index.y + 1, entity->i32vec2Index.x + 1) > 99 && entity->cMap2D->GetMapInfo(entity->i32vec2Index.y + 1, entity->i32vec2Index.x + 1) < 200){ //If new pos is not feasible...
					entity->currTexID = entity->idleRightTexID;
					entity->i32vec2Index.x = oldIndex; //...
					entity->i32vec2NumMicroSteps.x = 0; //...
				} //Go back to OG pos
			} //Check other row also
		}
		return;
	}
	cout << "FindFeasiblePos: Unknown dir." << endl;
}

template<class T>
void UpdateJumpFall(T* const& entity, const float& dt){
	entity->cPhysics2D.AddElapsedTime(dt);
	entity->cPhysics2D.Update();
	glm::vec2 v2Displacement = entity->cPhysics2D.GetDisplacement();
	int iIndex_YAxis_OLD = entity->i32vec2Index.y;
	entity->i32vec2Index.y += (int)v2Displacement.y; //Update with change

	if(entity->cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP){
		entity->i32vec2NumMicroSteps.y += glm::i32(abs((v2Displacement.y - (int)v2Displacement.y) * entity->cSettings->NUM_STEPS_PER_TILE_YAXIS));
		if(entity->i32vec2NumMicroSteps.y > 0){
			++entity->i32vec2Index.y;
			entity->i32vec2NumMicroSteps.y = 0;
		}
		Constraint(entity);

		for(int i = iIndex_YAxis_OLD; i <= entity->i32vec2Index.y; ++i){ //Make entity stop jumping when he/she hits a solid tile
			//If entity is within a column, check only one column. If entity is between 2 columns, check both columns.
			if((entity->i32vec2NumMicroSteps.x == 0 && entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x) > 99 && entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x) < 200) ||
				(entity->i32vec2NumMicroSteps.x != 0 && ((entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x) > 99 && entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x) < 200) ||
					(entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x + 1) > 99 && entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x + 1) < 200)))){
				entity->i32vec2Index.y = i - 1; //Assign solid tile index to entity->i32vec2Index.y
				entity->i32vec2NumMicroSteps.y = 0; //Since entity stops at a tile pos
				entity->cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				break;
			}
		}
		if((entity->cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (entity->cPhysics2D.GetInitialVelocity().y <= 0.f)){
			entity->cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL); //Fall after reaching max jump height
		}
	} else if(entity->cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL){
		entity->i32vec2NumMicroSteps.y -= glm::i32(abs((v2Displacement.y - (int)v2Displacement.y) * entity->cSettings->NUM_STEPS_PER_TILE_YAXIS));
		if(entity->i32vec2NumMicroSteps.y < 0){
			entity->i32vec2Index.y -= 1;
			entity->i32vec2NumMicroSteps.y = glm::i32(entity->cSettings->NUM_STEPS_PER_TILE_YAXIS + entity->i32vec2NumMicroSteps.y - 1.f);
		}
		Constraint(entity);

		for(int i = iIndex_YAxis_OLD; i >= entity->i32vec2Index.y; --i){ //Make entity stop falling when he/she lands on a solid tile
			//If entity is within a column, check only one column. If entity is between 2 columns, check both columns.
			if((entity->i32vec2NumMicroSteps.x == 0 && entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x) > 99 && entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x) < 200) ||
				(entity->i32vec2NumMicroSteps.x != 0 && ((entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x) > 99 && entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x) < 200) ||
					(entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x + 1) > 99 && entity->cMap2D->GetMapInfo(i, entity->i32vec2Index.x + 1) < 200)))){
				entity->i32vec2Index.y = i + 1; //...
				entity->i32vec2NumMicroSteps.y = 0; //...
				entity->cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
				break;
			}
		}
	}
}