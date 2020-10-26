#pragma once
#include "../DesignPatterns/SingletonTemplate.h"
#include <bitset>
#include <windows.h>

class CKeyboardController: public CSingletonTemplate<CKeyboardController>{
	friend CSingletonTemplate<CKeyboardController>;
public:
	const static int MAX_KEYS = 256; //How many keys will be processed

	///System interface
	void PreUpdate();
	void Update();
	void PostUpdate();
	
	///User interface
	bool IsKeyDown(const unsigned char _slot);
	bool IsKeyUp(const unsigned char _slot);
	bool IsKeyPressed(const unsigned char _slot);
	bool IsKeyReleased(const unsigned char _slot);
protected:
	CKeyboardController() = default;
	std::bitset<MAX_KEYS> currStatus, prevStatus; //Store info abt curr and prev keypress statuses
	bool CheckKeyStatus(const unsigned short key);
};