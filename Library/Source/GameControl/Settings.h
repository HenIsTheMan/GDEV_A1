#pragma once
#include "../DesignPatterns/SingletonTemplate.h"
#include <Windows.h>
#ifndef GLEW_STATIC
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

class CSettings final: public CSingletonTemplate<CSettings>{
	friend CSingletonTemplate<CSettings>;
	CSettings();
public:
	enum class AXIS{
		x,
		y,
		z,
		NUM
	};
	char* logl_root; //Root directory for loading digital assets
	GLFWwindow* pWindow;
	bool bUse4XAntiliasing = true;

	unsigned int iWindowWidth = 0;
	unsigned int iWindowHeight = 0;
	unsigned int iWindowPosX = 0;
	unsigned int iWindowPosY = 0;
	bool bShowMousePointer = true;
	const unsigned char FPS = 60;
	const unsigned int frameTime = 1000 / FPS;

	///Map specifications
	unsigned int NUM_TILES_XAXIS;
	unsigned int NUM_TILES_YAXIS;
	float TILE_WIDTH;
	float TILE_HEIGHT;
	float NUM_STEPS_PER_TILE_XAXIS;
	float NUM_STEPS_PER_TILE_YAXIS;
	float MICRO_STEP_XAXIS;
	float MICRO_STEP_YAXIS;

	void UpdateMapSpecifications();
	float ConvertIndexToUVSpace(const AXIS sAxis, const int iIndex, const bool bInvert, const float fOffset = 0.f); //Convert index of a tile to a coord in UV Space
};