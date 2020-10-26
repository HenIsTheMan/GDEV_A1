#include "Enemy2D.h"
#include <iostream>
using namespace std;

#include <GL/glew.h>
#include "System\ImageLoader.h"
#include "Map2D.h"

CEnemy2D::CEnemy2D(): cMap2D(nullptr), cPlayer2D(nullptr), currState(FSM::IDLE), Entity(ENTITY_TYPE::ENEMY){
	spawnIndex = glm::i32vec2(0, 0);
	patrolRightTexID = patrolLeftTexID = 0;
	i32vec2Dir = glm::i32vec2(1, 0);
	attackBT = 0.f;
	changeStateBT = 0.f;
	updateDirBT = 0.f;
	moveBT = 0.f;
	moveDelay = .08f;
}

CEnemy2D::~CEnemy2D(){
	cPlayer2D = nullptr; //Don't delete as...
	cMap2D = nullptr; //...
	cShader = nullptr; //...
}

bool CEnemy2D::Init(){
	Entity::Init();
	cMap2D = CMap2D::GetInstance();
	cSettings = CSettings::GetInstance();

	if(!LoadTexture("Images/GhostIdleRight.tga", idleRightTexID)){
		std::cout << "Failed to load GhostIdleRight.tga" << std::endl;
		return false;
	}
	if(!LoadTexture("Images/GhostIdleLeft.tga", idleLeftTexID)){
		std::cout << "Failed to load GhostIdleLeft.tga" << std::endl;
		return false;
	}
	if(!LoadTexture("Images/GhostPatrolRight.tga", patrolRightTexID)){
		std::cout << "Failed to load GhostPatrolRight.tga" << std::endl;
		return false;
	}
	if(!LoadTexture("Images/GhostPatrolLeft.tga", patrolLeftTexID)){
		std::cout << "Failed to load GhostPatrolLeft.tga" << std::endl;
		return false;
	}

	if(!LoadTexture("Images/GhostAttackRight1.tga", currTexID)){
		std::cout << "Failed to load GhostAttackRight1.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_RIGHT, currTexID);
	if(!LoadTexture("Images/GhostAttackRight2.tga", currTexID)){
		std::cout << "Failed to load GhostAttackRight2.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_RIGHT, currTexID);
	if(!LoadTexture("Images/GhostAttackRight3.tga", currTexID)){
		std::cout << "Failed to load GhostAttackRight3.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_RIGHT, currTexID);
	if(!LoadTexture("Images/GhostAttackRight4.tga", currTexID)){
		std::cout << "Failed to load GhostAttackRight4.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_RIGHT, currTexID);
	if(!LoadTexture("Images/GhostAttackRight5.tga", currTexID)){
		std::cout << "Failed to load GhostAttackRight5.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_RIGHT, currTexID);

	if(!LoadTexture("Images/GhostAttackLeft1.tga", currTexID)){
		std::cout << "Failed to load GhostAttackLeft1.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_LEFT, currTexID);
	if(!LoadTexture("Images/GhostAttackLeft2.tga", currTexID)){
		std::cout << "Failed to load GhostAttackLeft2.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_LEFT, currTexID);
	if(!LoadTexture("Images/GhostAttackLeft3.tga", currTexID)){
		std::cout << "Failed to load GhostAttackLeft3.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_LEFT, currTexID);
	if(!LoadTexture("Images/GhostAttackLeft4.tga", currTexID)){
		std::cout << "Failed to load GhostAttackLeft4.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_LEFT, currTexID);
	if(!LoadTexture("Images/GhostAttackLeft5.tga", currTexID)){
		std::cout << "Failed to load GhostAttackLeft5.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::ATTACK_LEFT, currTexID);

	currTexID = idleRightTexID;
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL); //Enemy falls by default
	return true;
}

void CEnemy2D::Update(const float& dt){
	aliveTime += dt;

	///Handle state change
	if(cPhysics2D.CalculateDist(i32vec2Index, cPlayer2D->Geti32vec2Index()) < 5.f && attackBT <= aliveTime){
		currState = FSM::ATTACK;
	} else if(changeStateBT <= aliveTime){
		currState = currState == FSM::IDLE ? FSM::PATROL : FSM::IDLE;
		changeStateBT = aliveTime + (currState == FSM::IDLE ? 1.2f : 5.f);
	}
	switch(currState){
		case FSM::IDLE: currTexID = currDir == DIR::LEFT ? idleLeftTexID : idleRightTexID; break;
		case FSM::PATROL:
			if(updateDirBT <= aliveTime){
				UpdateDir();
				updateDirBT = aliveTime + 2.f;
			}
			UpdatePos();
			currTexID = currDir == DIR::LEFT ? patrolLeftTexID : patrolRightTexID;
			break;
		case FSM::ATTACK:
			UpdateDir();
			UpdatePos();
			InteractWithPlayer();
			currTexID = currDir == DIR::LEFT ? cAnimatedSprites.Get(CAnimatedSprites::TYPE::ATTACK_LEFT) : cAnimatedSprites.Get(CAnimatedSprites::TYPE::ATTACK_RIGHT);
	}
	UpdateJumpFall(this, dt);
	vec2UVCoords.x = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, i32vec2Index.x, false, i32vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoords.y = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::y, i32vec2Index.y, false, i32vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

void CEnemy2D::SetSpawnIndex(const int& x, const int& y){
	spawnIndex.x = x;
	spawnIndex.y = y;
}

void CEnemy2D::SetPlayer2D(CPlayer2D* cPlayer2D){
	this->cPlayer2D = cPlayer2D;
}

void CEnemy2D::InteractWithPlayer(){
	const glm::i32vec2&& i32vec2PlayerPos = cPlayer2D->Geti32vec2Index();
	if(i32vec2Index.x >= i32vec2PlayerPos.x - .1f && i32vec2Index.x <= i32vec2PlayerPos.x + .1f && i32vec2Index.y >= i32vec2PlayerPos.y - .1f && i32vec2Index.y <= i32vec2PlayerPos.y + .1f){
		if(cPlayer2D->healFromDmg < 10){
			cPlayer2D->SetHealth(cPlayer2D->GetHealth() + 2);
			++cPlayer2D->healFromDmg;
		} else{
			cPlayer2D->SetHealth(cPlayer2D->GetHealth() - 2);
		}
		currState = FSM::IDLE;
		attackBT = aliveTime + 3.f;
	}
}

void CEnemy2D::UpdateDir(){
	if(currState == FSM::ATTACK){
		i32vec2Dir = cPlayer2D->Geti32vec2Index() - i32vec2Index;
		float&& dist = cPhysics2D.CalculateDist(i32vec2Index, cPlayer2D->Geti32vec2Index());
		if(!dist){
			i32vec2Dir = glm::i32vec2(currDir == DIR::LEFT ? -1 : 1, 0);
		} else{
			i32vec2Dir.x = (int)round(i32vec2Dir.x / dist); //Easier to work with whole no. for movement so round
			i32vec2Dir.y = (int)round(i32vec2Dir.y / dist); //...
		}
	} else{
		i32vec2Dir.x = -i32vec2Dir.x;
		i32vec2Dir.y = 0;
	}
}

void CEnemy2D::UpdatePos(){
	if(i32vec2Dir.x < 0){ //If player is to the left of enemy2D...
		currDir = DIR::LEFT;
		if(moveBT <= aliveTime){
			const int iOldIndex = i32vec2Index.x;
			if(i32vec2Index.x >= 0){
				--i32vec2NumMicroSteps.x;
				if(i32vec2NumMicroSteps.x < 0){
					i32vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
					--i32vec2Index.x;
				}
			}
			Constraint(this);
			FindFeasiblePos(this, iOldIndex);
			moveBT = aliveTime + moveDelay;
		}
	}
	if(i32vec2Dir.x > 0){ //Else if player is to the right of enemy2D...
		currDir = DIR::RIGHT;
		if(moveBT <= aliveTime){
			const int iOldIndex = i32vec2Index.x;
			if(i32vec2Index.x < (int)cSettings->NUM_TILES_XAXIS){
				++i32vec2NumMicroSteps.x;
				if(i32vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS){
					i32vec2NumMicroSteps.x = 0;
					++i32vec2Index.x;
				}
			}
			Constraint(this);
			FindFeasiblePos(this, iOldIndex);
			moveBT = aliveTime + moveDelay;
		}
	}
	if(IsMidAir(this)){
		cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
	}
	if(i32vec2Dir.y > 0 && cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE){ //Jump to attack if player is above enemy2D and...
		cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
		cPhysics2D.SetInitialVelocity(glm::vec2(0.f, 3.f));
	}
}