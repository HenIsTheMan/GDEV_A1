#pragma once

#ifndef GLEW_STATIC
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <map>
using namespace std;

class CAnimatedSprites{
public:
	enum class TYPE{
		WALK_LEFT,
		WALK_RIGHT,
		COIN_SPIN,
		ATTACK_LEFT,
		ATTACK_RIGHT,
		AMT
	};
	CAnimatedSprites();
	virtual ~CAnimatedSprites() = default;
	virtual void Add(const TYPE& type, const GLuint& texID);
	virtual void Remove(const TYPE& type, const GLuint& texID);
	virtual GLuint Get(const TYPE& type);
protected:
	map<int, int> walkLeftTexIDs, walkRightTexIDs, coinSpinTexIDs, attackLeftTexIDs, attackRightTexIDs;
	unsigned short indexWalkLeft, indexWalkRight, indexCoinSpin, indexAttackLeft, indexAttackRight; //Curr index for each map
};