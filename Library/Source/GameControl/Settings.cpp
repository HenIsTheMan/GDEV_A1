#include "Settings.h"
#include <iostream>
using namespace std;

CSettings::CSettings(): pWindow(NULL), logl_root(NULL),
	NUM_TILES_XAXIS(32), NUM_TILES_YAXIS(24),
	TILE_WIDTH(0.0625f), TILE_HEIGHT(0.08333f),
	NUM_STEPS_PER_TILE_XAXIS(4.f), NUM_STEPS_PER_TILE_YAXIS(4.f),
	MICRO_STEP_XAXIS(0.015625f), MICRO_STEP_YAXIS(0.0208325f){}

float CSettings::ConvertIndexToUVSpace(const AXIS sAxis, const int iIndex, const bool bInvert, const float fOffset){
	float fResult = 0.f;
	if(sAxis == AXIS::x){
		fResult = -1.f + (float)iIndex*TILE_WIDTH + TILE_WIDTH / 2.f + fOffset;
	} else if(sAxis == AXIS::y){
		if(bInvert){
			fResult = 1.f - (float)(iIndex + 1) * TILE_HEIGHT + TILE_HEIGHT / 2.f + fOffset;
		} else{
			fResult = -1.f + (float)iIndex * TILE_HEIGHT + TILE_HEIGHT / 2.f + fOffset;
		}
	} else if (sAxis == AXIS::z){
		//Not used in here
	} else{
		cout << "Unknown axis" << endl;
	}
	return fResult;
}

void CSettings::UpdateMapSpecifications(){
	TILE_WIDTH = 2.0f / NUM_TILES_XAXIS;
	TILE_HEIGHT = 2.0f / NUM_TILES_YAXIS;
	MICRO_STEP_XAXIS = TILE_WIDTH / NUM_STEPS_PER_TILE_XAXIS;
	MICRO_STEP_YAXIS = TILE_HEIGHT / NUM_STEPS_PER_TILE_YAXIS;
}