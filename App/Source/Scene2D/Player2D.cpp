#include "Player2D.h"
#include <iostream>
#include <GL/glew.h>
#include "System\ImageLoader.h"
#include "Scene2D.h"
using namespace std;

CPlayer2D::CPlayer2D(): VAO(0), VBO(0), EBO(0), score(0), health(10), lives(3), maxHealth(10), maxLives(3), gotKey(false),
	cMap2D(nullptr), cKeyboardController(nullptr), Entity(ENTITY_TYPE::PLAYER)
{
	touchedCheckpoint = false;
	inv.reserve(4);
	inv = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
	animationBT = acidDmgBT = lavaDmgBT = moveBT = moveDelay = acidDmgTime = cannotDieTime = higherJumpsTime = infiniteJumpsTime = periodicHealTime = 0.f;
	maxJumps = 0;
	acidDmgCount = 3;
	healFromDmg = 10;
}

CPlayer2D::~CPlayer2D(){
	cKeyboardController = nullptr; //Don't do delete as created elsewhere
	cMap2D = nullptr; //...
	cShader = nullptr; //...
}

bool CPlayer2D::Init(){
	Entity::Init();
	cKeyboardController = CKeyboardController::GetInstance();
	cMap2D = CMap2D::GetInstance();
	cSettings = CSettings::GetInstance();

	if(!LoadTexture("Images/PlayerIdleLeft.tga", idleLeftTexID)){
		std::cout << "Failed to load PlayerIdleLeft.tga" << std::endl;
		return false;
	}
	if(!LoadTexture("Images/PlayerIdleRight.tga", idleRightTexID)){
		std::cout << "Failed to load PlayerIdleRight.tga" << std::endl;
		return false;
	}
	if(!LoadTexture("Images/PlayerJumpLeft.tga", jumpLeftTexID)){
		std::cout << "Failed to load PlayerJumpLeft.tga" << std::endl;
		return false;
	}
	if(!LoadTexture("Images/PlayerJumpRight.tga", jumpRightTexID)){
		std::cout << "Failed to load PlayerJumpRight.tga" << std::endl;
		return false;
	}
	if(!LoadTexture("Images/PlayerFallLeft.tga", fallLeftTexID)){
		std::cout << "Failed to load PlayerFallLeft.tga" << std::endl;
		return false;
	}
	if(!LoadTexture("Images/PlayerFallRight.tga", fallRightTexID)){
		std::cout << "Failed to load PlayerFallRight.tga" << std::endl;
		return false;
	}

	if(!LoadTexture("Images/PlayerWalkLeft1.tga", currTexID)){
		std::cout << "Failed to load PlayerWalkLeft1.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::WALK_LEFT, currTexID);
	if(!LoadTexture("Images/PlayerWalkLeft2.tga", currTexID)){
		std::cout << "Failed to load PlayerWalkLeft2.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::WALK_LEFT, currTexID);
	if(!LoadTexture("Images/PlayerWalkRight1.tga", currTexID)){
		std::cout << "Failed to load PlayerWalkRight1.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::WALK_RIGHT, currTexID);
	if(!LoadTexture("Images/PlayerWalkRight2.tga", currTexID)){
		std::cout << "Failed to load PlayerWalkRight2.tga" << std::endl;
		return false;
	}
	cAnimatedSprites.Add(CAnimatedSprites::TYPE::WALK_RIGHT, currTexID);

	currTexID = idleRightTexID;
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
	return true;
}

void CPlayer2D::Update(const float& dt){
	aliveTime += dt;
	const short initialHealth = health;
	if(higherJumpsTime > 0.f){
		higherJumpsTime -= dt;
	}
	if(infiniteJumpsTime > 0.f){
		infiniteJumpsTime -= dt;
	}
	for(short i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i){
		if(cKeyboardController->IsKeyPressed(inputs[i]) && inv[i].first){
			switch(inv[i].first){
				case 207:
					health += 7;
					if(health > maxHealth){
						health = maxHealth;
					}
					break; //Medkit (heal 7)
				case 208: cannotDieTime = 10.f; break; //Black Potion (health can't go below 1)
				case 209: infiniteJumpsTime = 5.f; break; //Blue Potion (infinite jumps)
				case 210: higherJumpsTime = 7.f; break; //Green Potion (higher jumps)
				case 211: score *= 2; break; //Orange Potion (score gets doubled)
				case 212: healFromDmg = 0; break; //Purple Potion (heal from dmg taken [max 7])
				case 213: periodicHealTime = 100.f; break; //Red Potion (heal 1 every 10 sec for 100 sec)
			}
			--inv[i].second;
			if(!inv[i].second){
				inv[i].first = 0;
			}
		}
	}

	const short&& leftRight = short((cKeyboardController->IsKeyDown(VK_LEFT) || cKeyboardController->IsKeyDown('A')) - (cKeyboardController->IsKeyDown(VK_RIGHT) || cKeyboardController->IsKeyDown('D')));
	if(!leftRight){
		if(currDir == DIR::LEFT){
			currTexID = idleLeftTexID;
		}
		if(currDir == DIR::RIGHT){
			currTexID = idleRightTexID;
		}
		InteractWithMap();
	}
	if(leftRight > 0){
		if(currDir == DIR::RIGHT){ //So can flip facing dir instantly
			animationBT = 0.f;
		}
		currDir = DIR::LEFT;
		if(moveBT <= aliveTime){
			if(animationBT <= aliveTime){
				currTexID = cAnimatedSprites.Get(CAnimatedSprites::TYPE::WALK_LEFT);
				animationBT = aliveTime + .2f;
			}
			const int iOldIndex = i32vec2Index.x;
			--i32vec2NumMicroSteps.x;
			if(i32vec2NumMicroSteps.x < 0){ //Constraint the player's pos within the screen boundary
				i32vec2NumMicroSteps.x = (int)cSettings->NUM_STEPS_PER_TILE_XAXIS - 1;
				--i32vec2Index.x;
			}
			Constraint(this);
			FindFeasiblePos(this, iOldIndex);
			if(IsMidAir(this) && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP){
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			}
			InteractWithMap();
			moveBT = aliveTime + moveDelay;
		}
	}
	if(leftRight < 0){
		if(currDir == DIR::LEFT){ //So can flip facing dir instantly
			animationBT = 0.f;
		}
		currDir = DIR::RIGHT;
		if(moveBT <= aliveTime){
			if(animationBT <= aliveTime){
				currTexID = cAnimatedSprites.Get(CAnimatedSprites::TYPE::WALK_RIGHT);
				animationBT = aliveTime + .2f;
			}
			const int iOldIndex = i32vec2Index.x;
			++i32vec2NumMicroSteps.x;
			if(i32vec2NumMicroSteps.x > (int)cSettings->NUM_STEPS_PER_TILE_XAXIS){ //Constraint...
				i32vec2NumMicroSteps.x = 0;
				++i32vec2Index.x;
			}
			Constraint(this);
			FindFeasiblePos(this, iOldIndex);
			if(IsMidAir(this) && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP){
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			}
			InteractWithMap();
			moveBT = aliveTime + moveDelay;
		}
	}

	if(cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE){
		maxJumps = 2;
	}
	if((cKeyboardController->IsKeyPressed(VK_UP) || cKeyboardController->IsKeyPressed('W') || cKeyboardController->IsKeyPressed('X')) && maxJumps){
		cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
		cPhysics2D.SetInitialVelocity(glm::vec2(0.f, higherJumpsTime > 0.f ? 5.f : 3.f));
		if(infiniteJumpsTime <= 0.f){
			--maxJumps;
		}
	}
	if(cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP){
		if(!cKeyboardController->IsKeyDown(VK_UP) && !cKeyboardController->IsKeyDown('W') && !cKeyboardController->IsKeyPressed('X')){
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		} else{
			if(currDir == DIR::LEFT){
				currTexID = jumpLeftTexID;
			}
			if(currDir == DIR::RIGHT){
				currTexID = jumpRightTexID;
			}
		}
	}
	if(cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL){
		if(currDir == DIR::LEFT){
			currTexID = fallLeftTexID;
		}
		if(currDir == DIR::RIGHT){
			currTexID = fallRightTexID;
		}
	}
	UpdateJumpFall(this, dt);
	InteractWithMap();

	if(cKeyboardController->IsKeyDown(VK_SHIFT) || cKeyboardController->IsKeyDown('Z')){ //Sprint
		moveDelay = .01f;
	} else{
		moveDelay = .03f;
	}
	if(acidDmgBT <= aliveTime && acidDmgCount < 3){
		--health;
		++acidDmgCount;
		acidDmgBT = aliveTime + 2.f;
	}
	if(healFromDmg < 10){
		const short& heal = initialHealth - health;
		healFromDmg += heal;
		health = initialHealth + heal;
		if(health > maxHealth){
			health = maxHealth;
		}
	}
	if(periodicHealTime > 0.f){
		const float prevTime = periodicHealTime;
		periodicHealTime -= dt;
		if(int(periodicHealTime / 10.f) != int(prevTime / 10.f)){
			++health;
			if(health > maxHealth){
				health = maxHealth;
			}
		}
	}
	if(cannotDieTime > 0.f){
		cannotDieTime -= dt;
		if(!health){
			health = 1;
		}
	}
	if(health <= 0){
		inv = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
		if(lives > 0){
			ifstream savedLvl("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv");
			CScene2D::ChangeMap((savedLvl.good() && touchedCheckpoint ? "Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv" : "Maps/Lvl_" + to_string(cMap2D->GetLvl()) + ".csv").c_str());
		} else{
			short currLvl = cMap2D->GetLvl();
			while(currLvl > 0){
				(void)remove(("Maps/Save_Lvl_" + to_string(currLvl) + ".csv").c_str());
				--currLvl;
			}
			CScene2D::showMenu = true;
		}
	}

	///Update UV coords
	vec2UVCoords.x = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, i32vec2Index.x, false, i32vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoords.y = cSettings->ConvertIndexToUVSpace(CSettings::AXIS::y, i32vec2Index.y, false, i32vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

bool CPlayer2D::GetGotKey() const{
	return gotKey;
}

glm::i32vec2 CPlayer2D::Geti32vec2Index() const{
	return i32vec2Index;
}

glm::i32vec2 CPlayer2D::Geti32vec2NumMicroSteps() const{
	return i32vec2NumMicroSteps;
}

glm::vec2 CPlayer2D::Getvec2UVCoords() const{
	return vec2UVCoords;
}

const short& CPlayer2D::GetHealth() const{
	return health;
}

const short& CPlayer2D::GetScore() const{
	return score;
}

const vector<pair<int, int>>& CPlayer2D::GetInv(){
	return inv;
}

void CPlayer2D::SetHealth(const short& health){
	this->health = health;
}

void CPlayer2D::SetShader(Shader* cShader){
	this->cShader = cShader;
}

void CPlayer2D::Seti32vec2Index(const int iIndex_xAxis, const int iIndex_yAxis){
	i32vec2Index.x = iIndex_xAxis;
	i32vec2Index.y = iIndex_yAxis;
}

void CPlayer2D::Seti32vec2NumMicroSteps(const int iNumMicroSteps_xAxis, const int iNumMicroSteps_yAxis){
	i32vec2NumMicroSteps.x = iNumMicroSteps_xAxis;
	i32vec2NumMicroSteps.y = iNumMicroSteps_yAxis;
}

void CPlayer2D::Setvec2UVCoords(const float fUVCoord_xAxis, const float fUVCoord_yAxis){
	vec2UVCoords.x = fUVCoord_xAxis;
	vec2UVCoords.y = fUVCoord_yAxis;
}

void CPlayer2D::InteractWithMap(){
	if(i32vec2Index.y > 0){
		///Lock
		if(gotKey && i32vec2NumMicroSteps.y == 0){
			if(cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) == 103){
				cMap2D->SetMapInfo(i32vec2Index.y - 1, i32vec2Index.x, 0);
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				gotKey = false;
			}
			if(i32vec2NumMicroSteps.x != 0 && cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1) == 103){
				cMap2D->SetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1, 0);
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				gotKey = false;
			}
		}

		///Bomb
		if(i32vec2NumMicroSteps.y == 0){
			if(cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) == 105){
				cMap2D->SetMapInfo(i32vec2Index.y - 1, i32vec2Index.x, 0);
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				health = 0;
			}
			if(i32vec2NumMicroSteps.x != 0 && cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1) == 105){
				cMap2D->SetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1, 0);
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				health = 0;
			}
		}

		///Princess
		if(i32vec2Index.y > 0 && (cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) == 106 || cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) == 107)){
			cMap2D->SetMapInfo(i32vec2Index.y - 1, i32vec2Index.x, 0);
			ifstream nextLvl("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv");
			if(nextLvl.good()){
				CScene2D::ChangeMap(("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv").c_str());
			} else{
				short currLvl = cMap2D->GetLvl();
				while(currLvl > 0){
					(void)remove(("Maps/Save_Lvl_" + to_string(currLvl) + ".csv").c_str());
					--currLvl;
				}
				CScene2D::showMenu = true;
			}
		}
		if(i32vec2Index.y > 0 && i32vec2Index.x < int(cSettings->NUM_TILES_XAXIS) - 1 && i32vec2NumMicroSteps.x != 0 &&
			(cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1) == 106 || cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1) == 107)){
			cMap2D->SetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1, 0);
			ifstream nextLvl("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv");
			if(nextLvl.good()){
				CScene2D::ChangeMap(("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv").c_str());
			} else{
				short currLvl = cMap2D->GetLvl();
				while(currLvl > 0){
					(void)remove(("Maps/Save_Lvl_" + to_string(currLvl) + ".csv").c_str());
					--currLvl;
				}
				CScene2D::showMenu = true;
			}
		}
	}

	switch(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x)){ //For when player is stationary
		case 200: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0); score += 10; break; //Coin
		case 201: //Health Heart
			if(health < maxHealth){
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
			}
			health += 2;
			if(health > maxHealth){
				health = maxHealth;
			}
			break;
		case 202: //Life Heart
			if(lives < maxLives){
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
			}
			++lives;
			if(lives > maxLives){
				lives = maxLives;
			}
			break;
		case 203: acidDmgCount = 0; break; //Acid
		case 204: //Lava
			if(lavaDmgBT <= aliveTime){
				--health;
				lavaDmgBT = aliveTime + .2f;
			}
			break;
		case 205: //Checkpoint
			touchedCheckpoint = true;
			for(short r = 0; r < (int)cSettings->NUM_TILES_YAXIS; ++r){
				for(int c = 0; c < (int)cSettings->NUM_TILES_XAXIS; ++c){
					if(cMap2D->GetMapInfo(r, c) == 206){
						cMap2D->SetMapInfo(r, c, 205);
					}
				}
			}
			cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 206);
			try{
				if(!cMap2D->SaveMap("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv")){
					throw runtime_error("Unable to save game to a file");
				}
			} catch(const runtime_error& e){
				cout << "Runtime Error: " << e.what();
				return;
			}
			break;
		case 207:
		case 208:
		case 209:
		case 210:
		case 211:
		case 212:
		case 213:
			for(auto iter = inv.begin(); iter != inv.end(); ++iter){
				if(iter->first == cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x)){
					cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
					++iter->second;
					break;
				} else if(!iter->first){
					iter->first = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x);
					++iter->second;
					cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
					break;
				}
			}
			break;
		case 214: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0); gotKey = true; break; //Key
	}

	if(currDir == DIR::LEFT && i32vec2Index.x > 0){
		///Lock
		if(gotKey){
			if(i32vec2NumMicroSteps.x == 0 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x - 1) == 103){
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x - 1, 0);
				gotKey = false;
			}
		}

		///Bomb
		if(i32vec2NumMicroSteps.x == 0 && cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x - 1) == 105){
			cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x - 1, 0);
			health = 0;
		}

		///Princess
		if(i32vec2NumMicroSteps.x == 0 && (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x - 1) == 106 || cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x - 1) == 107)){
			cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x - 1, 0);
			ifstream nextLvl("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv");
			if(nextLvl.good()){
				CScene2D::ChangeMap(("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv").c_str());
			} else{
				short currLvl = cMap2D->GetLvl();
				while(currLvl > 0){
					(void)remove(("Maps/Save_Lvl_" + to_string(currLvl) + ".csv").c_str());
					--currLvl;
				}
				CScene2D::showMenu = true;
			}
		}

		///Ice
		if(i32vec2Index.y != 0 && cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x) == 102){
			const short&& leftRight = short((cKeyboardController->IsKeyDown(VK_LEFT) || cKeyboardController->IsKeyDown('A')) - (cKeyboardController->IsKeyDown(VK_RIGHT) || cKeyboardController->IsKeyDown('D')));
			if(leftRight){
				moveDelay = .01f;
			} else{
				moveDelay = .05f;
				if(currDir == DIR::LEFT){
					if(moveBT <= aliveTime){
						if(animationBT <= aliveTime){
							currTexID = cAnimatedSprites.Get(CAnimatedSprites::TYPE::WALK_LEFT);
							animationBT = aliveTime + .4f;
						}
						const int iOldIndex = i32vec2Index.x;
						--i32vec2NumMicroSteps.x;
						if(i32vec2NumMicroSteps.x < 0){ //Constraint the player's pos within the screen boundary
							i32vec2NumMicroSteps.x = (int)cSettings->NUM_STEPS_PER_TILE_XAXIS - 1;
							--i32vec2Index.x;
						}
						Constraint(this);
						FindFeasiblePos(this, iOldIndex);
						if(IsMidAir(this) && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP){
							cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						}
						moveBT = aliveTime + moveDelay;
					}
				} else{
					if(moveBT <= aliveTime){
						if(animationBT <= aliveTime){
							currTexID = cAnimatedSprites.Get(CAnimatedSprites::TYPE::WALK_RIGHT);
							animationBT = aliveTime + .4f;
						}
						const int iOldIndex = i32vec2Index.x;
						++i32vec2NumMicroSteps.x;
						if(i32vec2NumMicroSteps.x > (int)cSettings->NUM_STEPS_PER_TILE_XAXIS){ //Constraint...
							i32vec2NumMicroSteps.x = 0;
							++i32vec2Index.x;
						}
						Constraint(this);
						FindFeasiblePos(this, iOldIndex);
						if(IsMidAir(this) && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP){
							cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						}
						moveBT = aliveTime + moveDelay;
					}
				}
			}
		}

		if(i32vec2NumMicroSteps.x == 0){
			switch(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x)){
				case 200: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0); score += 10; break; //Coin
				case 201: //Health Heart
					if(health < maxHealth){
						cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
					}
					health += 2;
					if(health > maxHealth){
						health = maxHealth;
					}
					break;
				case 202: //Life Heart
					if(lives < maxLives){
						cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
					}
					++lives;
					if(lives > maxLives){
						lives = maxLives;
					}
					break;
				case 203: acidDmgCount = 0; break; //Acid
				case 204: //Lava
					if(lavaDmgBT <= aliveTime){
						--health;
						lavaDmgBT = aliveTime + .2f;
					}
					break;
				case 205: //Checkpoint
					touchedCheckpoint = true;
					for(short r = 0; r < (int)cSettings->NUM_TILES_YAXIS; ++r){
						for(int c = 0; c < (int)cSettings->NUM_TILES_XAXIS; ++c){
							if(cMap2D->GetMapInfo(r, c) == 206){
								cMap2D->SetMapInfo(r, c, 205);
							}
						}
					}
					cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 206);
					try{
						if(!cMap2D->SaveMap("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv")){
							throw runtime_error("Unable to save game to a file");
						}
					} catch(const runtime_error& e){
						cout << "Runtime Error: " << e.what();
						return;
					}
					break;
				case 207:
				case 208:
				case 209:
				case 210:
				case 211:
				case 212:
				case 213:
					for(auto iter = inv.begin(); iter != inv.end(); ++iter){
						if(iter->first == cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x)){
							cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
							++iter->second;
							break;
						} else if(!iter->first){
							iter->first = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x);
							++iter->second;
							cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
							break;
						}
					}
					break;
				case 214: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0); gotKey = true; break; //Key
			}
		}
	}

	if(currDir == DIR::RIGHT && i32vec2Index.x < int(cSettings->NUM_TILES_XAXIS) - 1){
		///Lock
		if(gotKey){
			if(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 103){
				cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
				gotKey = false;
			}
			if(i32vec2NumMicroSteps.y != 0 && cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 103){
				cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
				gotKey = false;
			}
		}

		///Bomb
		if(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 105){
			cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
			health = 0;
		}
		if(i32vec2NumMicroSteps.y != 0 && cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 105){
			cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
			health = 0;
		}

		///Princess
		if(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 106 || cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) == 107){
			cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
			ifstream nextLvl("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv");
			if(nextLvl.good()){
				CScene2D::ChangeMap(("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv").c_str());
			} else{
				short currLvl = cMap2D->GetLvl();
				while(currLvl > 0){
					(void)remove(("Maps/Save_Lvl_" + to_string(currLvl) + ".csv").c_str());
					--currLvl;
				}
				CScene2D::showMenu = true;
			}
		}
		if(i32vec2NumMicroSteps.y != 0 && (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 106 || cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 107)){
			cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
			ifstream nextLvl("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv");
			if(nextLvl.good()){
				CScene2D::ChangeMap(("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv").c_str());
			} else{
				short currLvl = cMap2D->GetLvl();
				while(currLvl > 0){
					(void)remove(("Maps/Save_Lvl_" + to_string(currLvl) + ".csv").c_str());
					--currLvl;
				}
				CScene2D::showMenu = true;
			}
		}

		///Ice
		if(i32vec2Index.y != 0 && cMap2D->GetMapInfo(i32vec2Index.y - 1, i32vec2Index.x + 1) == 102){
			const short&& leftRight = short((cKeyboardController->IsKeyDown(VK_LEFT) || cKeyboardController->IsKeyDown('A')) - (cKeyboardController->IsKeyDown(VK_RIGHT) || cKeyboardController->IsKeyDown('D')));
			if(leftRight){
				moveDelay = .01f;
			} else{
				moveDelay = .05f;
				if(currDir == DIR::LEFT){
					if(moveBT <= aliveTime){
						if(animationBT <= aliveTime){
							currTexID = cAnimatedSprites.Get(CAnimatedSprites::TYPE::WALK_LEFT);
							animationBT = aliveTime + .4f;
						}
						const int iOldIndex = i32vec2Index.x;
						--i32vec2NumMicroSteps.x;
						if(i32vec2NumMicroSteps.x < 0){ //Constraint the player's pos within the screen boundary
							i32vec2NumMicroSteps.x = (int)cSettings->NUM_STEPS_PER_TILE_XAXIS - 1;
							--i32vec2Index.x;
						}
						Constraint(this);
						FindFeasiblePos(this, iOldIndex);
						if(IsMidAir(this) && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP){
							cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						}
						moveBT = aliveTime + moveDelay;
					}
				} else{
					if(moveBT <= aliveTime){
						if(animationBT <= aliveTime){
							currTexID = cAnimatedSprites.Get(CAnimatedSprites::TYPE::WALK_RIGHT);
							animationBT = aliveTime + .4f;
						}
						const int iOldIndex = i32vec2Index.x;
						++i32vec2NumMicroSteps.x;
						if(i32vec2NumMicroSteps.x > (int)cSettings->NUM_STEPS_PER_TILE_XAXIS){ //Constraint...
							i32vec2NumMicroSteps.x = 0;
							++i32vec2Index.x;
						}
						Constraint(this);
						FindFeasiblePos(this, iOldIndex);
						if(IsMidAir(this) && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP){
							cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						}
						moveBT = aliveTime + moveDelay;
					}
				}
			}
		}
		if(i32vec2NumMicroSteps.y != 0 && cMap2D->GetMapInfo(i32vec2Index.y + 1 - 1, i32vec2Index.x + 1) == 102){
			const short&& leftRight = short((cKeyboardController->IsKeyDown(VK_LEFT) || cKeyboardController->IsKeyDown('A')) - (cKeyboardController->IsKeyDown(VK_RIGHT) || cKeyboardController->IsKeyDown('D')));
			if(leftRight){
				moveDelay = .01f;
			} else{
				moveDelay = .05f;
				if(currDir == DIR::LEFT){
					if(moveBT <= aliveTime){
						if(animationBT <= aliveTime){
							currTexID = cAnimatedSprites.Get(CAnimatedSprites::TYPE::WALK_LEFT);
							animationBT = aliveTime + .4f;
						}
						const int iOldIndex = i32vec2Index.x;
						--i32vec2NumMicroSteps.x;
						if(i32vec2NumMicroSteps.x < 0){ //Constraint the player's pos within the screen boundary
							i32vec2NumMicroSteps.x = (int)cSettings->NUM_STEPS_PER_TILE_XAXIS - 1;
							--i32vec2Index.x;
						}
						Constraint(this);
						FindFeasiblePos(this, iOldIndex);
						if(IsMidAir(this) && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP){
							cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						}
						moveBT = aliveTime + moveDelay;
					}
				} else{
					if(moveBT <= aliveTime){
						if(animationBT <= aliveTime){
							currTexID = cAnimatedSprites.Get(CAnimatedSprites::TYPE::WALK_RIGHT);
							animationBT = aliveTime + .4f;
						}
						const int iOldIndex = i32vec2Index.x;
						++i32vec2NumMicroSteps.x;
						if(i32vec2NumMicroSteps.x > (int)cSettings->NUM_STEPS_PER_TILE_XAXIS){ //Constraint...
							i32vec2NumMicroSteps.x = 0;
							++i32vec2Index.x;
						}
						Constraint(this);
						FindFeasiblePos(this, iOldIndex);
						if(IsMidAir(this) && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP){
							cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						}
						moveBT = aliveTime + moveDelay;
					}
				}
			}
		}

		if(i32vec2NumMicroSteps.x != 0){
			switch(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1)){
				case 200: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0); score += 10; break; //Coin
				case 201: //Health Heart
					if(health < maxHealth){
						cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
					}
					health += 2;
					if(health > maxHealth){
						health = maxHealth;
					}
					break;
				case 202: //Life Heart
					if(lives < maxLives){
						cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
					}
					++lives;
					if(lives > maxLives){
						lives = maxLives;
					}
					break;
				case 203: acidDmgCount = 0; break; //Acid
				case 204: //Lava
					if(lavaDmgBT <= aliveTime){
						--health;
						lavaDmgBT = aliveTime + .2f;
					}
					break;
				case 205: //Checkpoint
					touchedCheckpoint = true;
					for(short r = 0; r < (int)cSettings->NUM_TILES_YAXIS; ++r){
						for(int c = 0; c < (int)cSettings->NUM_TILES_XAXIS; ++c){
							if(cMap2D->GetMapInfo(r, c) == 206){
								cMap2D->SetMapInfo(r, c, 205);
							}
						}
					}
					cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 206);
					try{
						if(!cMap2D->SaveMap("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv")){
							throw runtime_error("Unable to save game to a file");
						}
					} catch(const runtime_error& e){
						cout << "Runtime Error: " << e.what();
						return;
					}
					break;
				case 207:
				case 208:
				case 209:
				case 210:
				case 211:
				case 212:
				case 213:
					for(auto iter = inv.begin(); iter != inv.end(); ++iter){
						if(iter->first == cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1)){
							cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
							++iter->second;
							break;
						} else if(!iter->first){
							iter->first = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1);
							++iter->second;
							cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
							break;
						}
					}
					break;
				case 214: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0); gotKey = true; break; //Key
			}

			if(i32vec2NumMicroSteps.y != 0){
				switch(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1)){
					case 200: cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0); score += 10; break; //Coin
					case 201: //Health Heart
						if(health < maxHealth){
							cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
						}
						health += 2;
						if(health > maxHealth){
							health = maxHealth;
						}
						break;
					case 202: //Life Heart
						if(lives < maxLives){
							cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
						}
						++lives;
						if(lives > maxLives){
							lives = maxLives;
						}
						break;
					case 203: acidDmgCount = 0; break; //Acid
					case 204: //Lava
						if(lavaDmgBT <= aliveTime){
							--health;
							lavaDmgBT = aliveTime + .2f;
						}
						break;
					case 205: //Checkpoint
						touchedCheckpoint = true;
						for(short r = 0; r < (int)cSettings->NUM_TILES_YAXIS; ++r){
							for(int c = 0; c < (int)cSettings->NUM_TILES_XAXIS; ++c){
								if(cMap2D->GetMapInfo(r, c) == 206){
									cMap2D->SetMapInfo(r, c, 205);
								}
							}
						}
						cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 206);
						try{
							if(!cMap2D->SaveMap("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv")){
								throw runtime_error("Unable to save game to a file");
							}
						} catch(const runtime_error& e){
							cout << "Runtime Error: " << e.what();
							return;
						}
						break;
					case 207:
					case 208:
					case 209:
					case 210:
					case 211:
					case 212:
					case 213:
						for(auto iter = inv.begin(); iter != inv.end(); ++iter){
							if(iter->first == cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1)){
								cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
								++iter->second;
								break;
							} else if(!iter->first){
								iter->first = cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1);
								++iter->second;
								cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
								break;
							}
						}
						break;
					case 214: cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0); gotKey = true; break; //Key
				}
			}
		}
	}

	if(cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP && i32vec2Index.y < int(cSettings->NUM_TILES_YAXIS) - 1){
		///Mystery
		if(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) == 104){
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, rand() & 1 ? 200 : 201);
		}
		if(i32vec2NumMicroSteps.x != 0 && cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 104){
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, rand() & 1 ? 200 : 201);
		}

		///Lock
		if(gotKey){
			if(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) == 103){
				cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 0);
				gotKey = false;
			}
			if(i32vec2NumMicroSteps.x != 0 && cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 103){
				cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
				gotKey = false;
			}
		}

		///Bomb
		if(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) == 105){
			cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 0);
			health = 0;
		}
		if(i32vec2NumMicroSteps.x != 0 && cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 105){
			cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
			health = 0;
		}

		///Princess
		if(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) == 106 || cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) == 107){
			cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 0);
			ifstream nextLvl("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv");
			if(nextLvl.good()){
				CScene2D::ChangeMap(("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv").c_str());
			} else{
				short currLvl = cMap2D->GetLvl();
				while(currLvl > 0){
					(void)remove(("Maps/Save_Lvl_" + to_string(currLvl) + ".csv").c_str());
					--currLvl;
				}
				CScene2D::showMenu = true;
			}
		}
		if(i32vec2NumMicroSteps.y != 0 && (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 106 || cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) == 107)){
			cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
			ifstream nextLvl("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv");
			if(nextLvl.good()){
				CScene2D::ChangeMap(("Maps/Lvl_" + to_string(cMap2D->GetLvl() + 1) + ".csv").c_str());
			} else{
				short currLvl = cMap2D->GetLvl();
				while(currLvl > 0){
					(void)remove(("Maps/Save_Lvl_" + to_string(currLvl) + ".csv").c_str());
					--currLvl;
				}
				CScene2D::showMenu = true;
			}
		}

		if(i32vec2NumMicroSteps.y != 0){
			switch(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x)){
				case 200: cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 0); score += 10; break; //Coin
				case 201: //Health Heart
					if(health < maxHealth){
						cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 0);
					}
					health += 2;
					if(health > maxHealth){
						health = maxHealth;
					}
					break;
				case 202: //Life Heart
					if(lives < maxLives){
						cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 0);
					}
					++lives;
					if(lives > maxLives){
						lives = maxLives;
					}
					break;
				case 203: acidDmgCount = 0; break; //Acid
				case 204: //Lava
					if(lavaDmgBT <= aliveTime){
						--health;
						lavaDmgBT = aliveTime + .2f;
					}
					break;
				case 205: //Checkpoint
					touchedCheckpoint = true;
					for(short r = 0; r < (int)cSettings->NUM_TILES_YAXIS; ++r){
						for(int c = 0; c < (int)cSettings->NUM_TILES_XAXIS; ++c){
							if(cMap2D->GetMapInfo(r, c) == 206){
								cMap2D->SetMapInfo(r, c, 205);
							}
						}
					}
					cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 206);
					try{
						if(!cMap2D->SaveMap("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv")){
							throw runtime_error("Unable to save game to a file");
						}
					} catch(const runtime_error& e){
						cout << "Runtime Error: " << e.what();
						return;
					}
					break;
				case 207:
				case 208:
				case 209:
				case 210:
				case 211:
				case 212:
				case 213:
					for(auto iter = inv.begin(); iter != inv.end(); ++iter){
						if(iter->first == cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x)){
							cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 0);
							++iter->second;
							break;
						} else if(!iter->first){
							iter->first = cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x);
							++iter->second;
							cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 0);
							break;
						}
					}
					break;
				case 214: cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x, 0); gotKey = true; break; //Key
			}

			if(i32vec2NumMicroSteps.x != 0){
				switch(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1)){
					case 200: cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0); score += 10; break; //Coin
					case 201: //Health Heart
						if(health < maxHealth){
							cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
						}
						health += 2;
						if(health > maxHealth){
							health = maxHealth;
						}
						break;
					case 202: //Life Heart
						if(lives < maxLives){
							cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
						}
						++lives;
						if(lives > maxLives){
							lives = maxLives;
						}
						break;
					case 203: acidDmgCount = 0; break; //Acid
					case 204: //Lava
						if(lavaDmgBT <= aliveTime){
							--health;
							lavaDmgBT = aliveTime + .2f;
						}
						break;
					case 205: //Checkpoint
						touchedCheckpoint = true;
						for(short r = 0; r < (int)cSettings->NUM_TILES_YAXIS; ++r){
							for(int c = 0; c < (int)cSettings->NUM_TILES_XAXIS; ++c){
								if(cMap2D->GetMapInfo(r, c) == 206){
									cMap2D->SetMapInfo(r, c, 205);
								}
							}
						}
						cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 206);
						try{
							if(!cMap2D->SaveMap("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv")){
								throw runtime_error("Unable to save game to a file");
							}
						} catch(const runtime_error& e){
							cout << "Runtime Error: " << e.what();
							return;
						}
						break;
					case 207:
					case 208:
					case 209:
					case 210:
					case 211:
					case 212:
					case 213:
						for(auto iter = inv.begin(); iter != inv.end(); ++iter){
							if(iter->first == cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1)){
								cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
								++iter->second;
								break;
							} else if(!iter->first){
								iter->first = cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1);
								++iter->second;
								cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0);
								break;
							}
						}
						break;
					case 214: cMap2D->SetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1, 0); gotKey = true; break; //Key
				}
			}
		}
	}

	if(cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL && i32vec2Index.y > 0){
		if(i32vec2NumMicroSteps.y != 0){
			switch(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x)){
				case 200: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0); score += 10; break; //Coin
				case 201: //Health Heart
					if(health < maxHealth){
						cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
					}
					health += 2;
					if(health > maxHealth){
						health = maxHealth;
					}
					break;
				case 202: //Life Heart
					if(lives < maxLives){
						cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
					}
					++lives;
					if(lives > maxLives){
						lives = maxLives;
					}
					break;
				case 203: acidDmgCount = 0; break; //Acid
				case 204: //Lava
					if(lavaDmgBT <= aliveTime){
						--health;
						lavaDmgBT = aliveTime + .2f;
					}
					break;
				case 205: //Checkpoint
					touchedCheckpoint = true;
					for(short r = 0; r < (int)cSettings->NUM_TILES_YAXIS; ++r){
						for(int c = 0; c < (int)cSettings->NUM_TILES_XAXIS; ++c){
							if(cMap2D->GetMapInfo(r, c) == 206){
								cMap2D->SetMapInfo(r, c, 205);
							}
						}
					}
					cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 206);
					try{
						if(!cMap2D->SaveMap("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv")){
							throw runtime_error("Unable to save game to a file");
						}
					} catch(const runtime_error& e){
						cout << "Runtime Error: " << e.what();
						return;
					}
					break;
				case 207:
				case 208:
				case 209:
				case 210:
				case 211:
				case 212:
				case 213:
					for(auto iter = inv.begin(); iter != inv.end(); ++iter){
						if(iter->first == cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x)){
							cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
							++iter->second;
							break;
						} else if(!iter->first){
							iter->first = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x);
							++iter->second;
							cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
							break;
						}
					}
					break;
				case 214: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0); gotKey = true; break; //Key
			}

			if(i32vec2NumMicroSteps.x != 0){
				switch(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1)){
					case 200: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0); score += 10; break; //Coin
					case 201: //Health Heart
						if(health < maxHealth){
							cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
						}
						health += 2;
						if(health > maxHealth){
							health = maxHealth;
						}
						break;
					case 202: //Life Heart
						if(lives < maxLives){
							cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
						}
						++lives;
						if(lives > maxLives){
							lives = maxLives;
						}
						break;
					case 203: acidDmgCount = 0; break; //Acid
					case 204: //Lava
						if(lavaDmgBT <= aliveTime){
							--health;
							lavaDmgBT = aliveTime + .2f;
						}
						break;
					case 205: //Checkpoint
						touchedCheckpoint = true;
						for(short r = 0; r < (int)cSettings->NUM_TILES_YAXIS; ++r){
							for(int c = 0; c < (int)cSettings->NUM_TILES_XAXIS; ++c){
								if(cMap2D->GetMapInfo(r, c) == 206){
									cMap2D->SetMapInfo(r, c, 205);
								}
							}
						}
						cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 206);
						try{
							if(!cMap2D->SaveMap("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv")){
								throw runtime_error("Unable to save game to a file");
							}
						} catch(const runtime_error& e){
							cout << "Runtime Error: " << e.what();
							return;
						}
						break;
					case 207:
					case 208:
					case 209:
					case 210:
					case 211:
					case 212:
					case 213:
						for(auto iter = inv.begin(); iter != inv.end(); ++iter){
							if(iter->first == cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1)){
								cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
								++iter->second;
								break;
							} else if(!iter->first){
								iter->first = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1);
								++iter->second;
								cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0);
								break;
							}
						}
						break;
					case 214: cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x + 1, 0); gotKey = true; break; //Key
				}
			}
		}
	}
}