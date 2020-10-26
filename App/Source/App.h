#pragma once
#include "DesignPatterns\SingletonTemplate.h"
#include "TimeControl\StopWatch.h"
#include "Scene2D\Scene2D.h"

struct GLFWwindow;
class CSettings;

class App final: public CSingletonTemplate<App>{
	friend CSingletonTemplate<App>;
	App();
	CStopWatch cStopWatch;
	CScene2D* cScene2D;
	CSettings* cSettings;
	static const GLFWvidmode* mode;
	void UpdateInputDevices();
	void PostUpdateInputDevices();
public:
	bool Init();
	void Run();
	void Destroy();
	int GetWindowHeight() const;
	int GetWindowWidth() const;
};