#include "KeyboardController.h"

void CKeyboardController::PreUpdate(){}

void CKeyboardController::Update(){
	for(int i = 0; i < CKeyboardController::MAX_KEYS; ++i){ //Update kbInput
		prevStatus.set(i, currStatus[i]);
		currStatus.set(i, CheckKeyStatus(i));
	}
}

void CKeyboardController::PostUpdate(){
	prevStatus = currStatus;
}

bool CKeyboardController::IsKeyDown(const unsigned char _slot){
	return currStatus.test(_slot);
}

bool CKeyboardController::IsKeyUp(const unsigned char _slot){
	return !currStatus.test(_slot);
}

bool CKeyboardController::IsKeyPressed(const unsigned char _slot){
	return IsKeyDown(_slot) && !prevStatus.test(_slot);
}

bool CKeyboardController::IsKeyReleased(const unsigned char _slot){
	return IsKeyUp(_slot) && prevStatus.test(_slot);
}

bool CKeyboardController::CheckKeyStatus(const unsigned short key){
	return GetAsyncKeyState(key) & 0x8000;
}