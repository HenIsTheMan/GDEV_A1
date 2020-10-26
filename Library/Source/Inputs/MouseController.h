#pragma once
#include "../DesignPatterns/SingletonTemplate.h"

class MouseController: public CSingletonTemplate<MouseController>{
	friend CSingletonTemplate<MouseController>;
public:
	enum class BUTTON_TYPE{
		LMB,
		RMB,
		MMB,
		NUM_MB
	};
	enum class SCROLL_TYPE{
		XOFFSET,
		YOFFSET,
		NUM
	};

	///System interface
	void UpdateMousePosition(double _x, double _y);
	void UpdateMouseButtonPressed(int _slot);
	void UpdateMouseButtonReleased(int _slot);
	void UpdateMouseScroll(const double xOffset, const double yOffset);
	void EndFrameUpdate();

	///User interface
	bool IsButtonDown(unsigned char _slot);
	bool IsButtonUp(unsigned char _slot);
	bool IsButtonPressed(unsigned char _slot);
	bool IsButtonReleased(unsigned char _slot);
	double GetMouseScrollStatus(SCROLL_TYPE _scrolltype);
	void GetMousePosition(float& _resultX, float& _resultY);
	void GetMousePosition(double& _resultX, double& _resultY);
	void GetMouseDelta(float& _resultX, float& _resultY);
	void GetMouseDelta(double& _resultX, double& _resultY);

	///Inlined funcs
	bool GetKeepMouseCentered(){
		return keepMouseCentered;
	};
	void SetKeepMouseCentered(bool _value){
		keepMouseCentered = _value;
	};
protected:
	MouseController();
	double curr_posX, curr_posY, prev_posX, prev_posY;
	unsigned char currBtnStatus, prevBtnStatus;
	double xOffset, yOffset;
	bool keepMouseCentered;
};