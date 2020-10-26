#pragma once
#include "DesignPatterns\SingletonTemplate.h"
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>
#include "Inputs\KeyboardController.h"
#include "GameControl\Settings.h"
#include "Entity.h"
#include <vector>

class CMap2D;

class CPlayer2D: public Entity{
	template<class T>
	friend bool IsMidAir(const T* const& entity);
	template<class T>
	friend void FindFeasiblePos(T* const& entity, const int oldIndex);
	template<class T>
	friend void UpdateJumpFall(T* const& entity, const float& dt);
	const char inputs[4]{'1', '2', '3', '4'};
public:
	CPlayer2D();
	virtual ~CPlayer2D();
	bool Init();
	void Update(const float& dt);
	bool gotKey;
	bool touchedCheckpoint;
	const short maxHealth, maxLives;
	short score, health, lives;
	float cannotDieTime;
	float higherJumpsTime;
	float infiniteJumpsTime;
	float periodicHealTime;
	short healFromDmg;
	short acidDmgCount;

	///Getters
	bool GetGotKey() const;
	glm::i32vec2 Geti32vec2Index() const; //Get indices of player
	glm::i32vec2 Geti32vec2NumMicroSteps() const; //Get no. of microsteps of player
	glm::vec2 Getvec2UVCoords() const; //Get UV coords of player
	const short& GetHealth() const;
	const short& GetScore() const;
	const vector<pair<int, int>>& GetInv();

	///Setters
	void SetHealth(const short& health);
	void SetShader(Shader* cShader);
	void Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis); //Set indices of player
	void Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis); //Set no. of microsteps of player
	void Setvec2UVCoords(const float fUVCoord_XAxis, const float fUVCoord_YAxis); //Set UV coords of player
protected:
	vector<pair<int, int>> inv;
	float animationBT;
	float acidDmgBT;
	float lavaDmgBT;
	float moveBT;
	float moveDelay;
	float acidDmgTime;
	unsigned int VBO, VAO, EBO;
	short maxJumps;
	CMap2D* cMap2D;
	CKeyboardController* cKeyboardController;
	void InteractWithMap(); //Let player interact with map
};