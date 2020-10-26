#include "AnimatedSprites.h"
#include <iostream>
using namespace std;

#include <GL/glew.h>
#include "System\ImageLoader.h"

CAnimatedSprites::CAnimatedSprites(){
	indexWalkLeft = indexWalkRight = indexCoinSpin = indexAttackLeft = indexAttackRight = 0;
}

void CAnimatedSprites::Add(const TYPE& type, const GLuint& texID){
	switch(type){
		case TYPE::WALK_LEFT: walkLeftTexIDs.insert(pair<int, int>(walkLeftTexIDs.size(), texID)); break; //Use curr size as index
		case TYPE::WALK_RIGHT: walkRightTexIDs.insert(pair<int, int>(walkRightTexIDs.size(), texID)); break; //...
		case TYPE::COIN_SPIN: coinSpinTexIDs.insert(pair<int, int>(coinSpinTexIDs.size(), texID)); break; //...
		case TYPE::ATTACK_LEFT: attackLeftTexIDs.insert(pair<int, int>(attackLeftTexIDs.size(), texID)); break; //...
		case TYPE::ATTACK_RIGHT: attackRightTexIDs.insert(pair<int, int>(attackRightTexIDs.size(), texID)); break; //...
		default: printf("Trying to add unknown tex type!\n"); exit(1);
	}
}

void CAnimatedSprites::Remove(const TYPE& type, const GLuint& texID){
	switch(type){
		case TYPE::WALK_LEFT:
			for(std::map<int, int>::iterator iter = walkLeftTexIDs.begin(); iter != walkLeftTexIDs.end(); ++iter){
				if(iter->second == texID){
					iter = walkLeftTexIDs.erase(iter);
					break;
				}
			}
			break;
		case TYPE::WALK_RIGHT:
			for(std::map<int, int>::iterator iter = walkRightTexIDs.begin(); iter != walkRightTexIDs.end(); ++iter){
				if(iter->second == texID){
					iter = walkRightTexIDs.erase(iter);
					break;
				}
			}
			break;
		case TYPE::COIN_SPIN:
			for(std::map<int, int>::iterator iter = coinSpinTexIDs.begin(); iter != coinSpinTexIDs.end(); ++iter){
				if(iter->second == texID){
					iter = coinSpinTexIDs.erase(iter);
					break;
				}
			}
			break;
		case TYPE::ATTACK_LEFT:
			for(std::map<int, int>::iterator iter = attackLeftTexIDs.begin(); iter != attackLeftTexIDs.end(); ++iter){
				if(iter->second == texID){
					iter = attackLeftTexIDs.erase(iter);
					break;
				}
			}
			break;
		case TYPE::ATTACK_RIGHT:
			for(std::map<int, int>::iterator iter = attackRightTexIDs.begin(); iter != attackRightTexIDs.end(); ++iter){
				if(iter->second == texID){
					iter = attackRightTexIDs.erase(iter);
					break;
				}
			}
			break;
		default: printf("Trying to remove unknown tex type!\n"); exit(1);
	}
}

GLuint CAnimatedSprites::Get(const TYPE& type){
	switch(type){
		case TYPE::WALK_LEFT:
			if(!walkLeftTexIDs.size()){
				printf("No WALK_LEFT tex to get!"); exit(1);
			}
			++indexWalkLeft;
			if(indexWalkLeft == (int)walkLeftTexIDs.size()){
				indexWalkLeft = 0;
			}
			return walkLeftTexIDs.at(indexWalkLeft);
		case TYPE::WALK_RIGHT:
			if(!walkRightTexIDs.size()){
				printf("No WALK_RIGHT tex to get!"); exit(1);
			}
			++indexWalkRight;
			if(indexWalkRight == (int)walkRightTexIDs.size()){
				indexWalkRight = 0;
			}
			return walkRightTexIDs.at(indexWalkRight);
		case TYPE::COIN_SPIN:
			if(!coinSpinTexIDs.size()){
				printf("No COIN_SPIN tex to get!"); exit(1);
			}
			++indexCoinSpin;
			if(indexCoinSpin == (int)coinSpinTexIDs.size()){
				indexCoinSpin = 0;
			}
			return coinSpinTexIDs.at(indexCoinSpin);
		case TYPE::ATTACK_LEFT:
			if(!attackLeftTexIDs.size()){
				printf("No ATTACK_LEFT tex to get!"); exit(1);
			}
			++indexAttackLeft;
			if(indexAttackLeft == (int)attackLeftTexIDs.size()){
				indexAttackLeft = 0;
			}
			return attackLeftTexIDs.at(indexAttackLeft);
		case TYPE::ATTACK_RIGHT:
			if(!attackRightTexIDs.size()){
				printf("No ATTACK_RIGHT tex to get!"); exit(1);
			}
			++indexAttackRight;
			if(indexAttackRight == (int)attackRightTexIDs.size()){
				indexAttackRight = 0;
			}
			return attackRightTexIDs.at(indexAttackRight);
		default: printf("Trying to get unknown tex type!\n"); exit(1);
	}
}