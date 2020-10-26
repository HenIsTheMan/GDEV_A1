#pragma once
#include "RenderControl\shader.h"
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>
#include "GameControl\Settings.h"
#include "Player2D.h"
#include "Entity.h"

class CEnemy2D: public Entity{
	template<class T>
	friend bool IsMidAir(const T* const& entity);
	template<class T>
	friend void FindFeasiblePos(T* const& entity, const int oldIndex);
	template<class T>
	friend void UpdateJumpFall(T* const& entity, const float& dt);
public:
	CEnemy2D();
	virtual ~CEnemy2D();
	bool Init();
	void Update(const float& dt);
	void SetSpawnIndex(const int& x, const int& y);
	void SetPlayer2D(CPlayer2D* cPlayer2D);
	glm::i32vec2 spawnIndex;
protected:
	enum class FSM{
		IDLE,
		PATROL,
		ATTACK,
		NUM
	};
	CPlayer2D* cPlayer2D;
	CMap2D* cMap2D;
	FSM currState;
	glm::i32vec2 i32vec2Dir; //Stores dir of enemy2D movement in Map2D

	float attackBT;
	float changeStateBT;
	float updateDirBT;
	float moveBT;
	float moveDelay;
	unsigned int patrolLeftTexID;
	unsigned int patrolRightTexID;
	void InteractWithPlayer();
	void UpdateDir();
	void UpdatePos();
};