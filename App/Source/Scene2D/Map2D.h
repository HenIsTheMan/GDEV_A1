#pragma once
#include "DesignPatterns\SingletonTemplate.h"
#include "RenderControl\shader.h"
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>
#include "../CSVReader/CSVReader.h"
#include "RenderControl\TextRenderer.h"
#include <map>
#include "GameControl\Settings.h"
#include "Player2D.h"
#include "Enemy2D.h"
#include "EntityFactory.h"
#include "TimeControl\StopWatch.h"
#include "Inputs\KeyboardController.h"

class CMap2D: public CSingletonTemplate<CMap2D>{
	friend CSingletonTemplate<CMap2D>;
public:
	bool Init(const string& fPath);
	void Update(const float& dt);
	void PreRender();
	void Render();
	void PostRender();
	CPlayer2D* cPlayer2D;
	EntityFactory* entityFactory;
	Shader* cShader;
	int princessR, princessC;
	short lvl;
	
	bool FindValue(const int iValue, int& iRow, int& iCol, const bool bInvert = true); //Find indices of a certain val in vMapInfo
	static const float& GetNumSteps(const CSettings::AXIS& sAxis);
	static void SetNumSteps(const CSettings::AXIS& sAxis, const unsigned int& val);
	void SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue);
	void SetMapInfo(const int iRow, const int iCol, const int iValue, const bool bInvert = true); //Set val at certain indices in vMapInfo
	void SetShader(Shader* cShader);
	int GetMapInfo(const int iRow, const int iCol, const bool bInvert = true) const; //Get val at certain indices in vMapInfo
	bool LoadMap(const string& fPath, int& xMicroSteps, int& yMicroSteps, bool& dir);
	bool SaveMap(const string& fPath, bool&& saveExtraData = false);
	const short& GetLvl() const;
protected:
	static CSettings* cSettings;
	vector<vector<int>> vMapInfo;

	bool showCoinSpin;
	bool showDebugInfo;
	float mapElapsedTime;
	float coinSpinBT;
	float switchDirBT;
	unsigned int VBO, VAO, EBO;
	unsigned int currCoinTexID;
	glm::mat4 transform;
	map<int, int> MapOfTexIDs;
	CKeyboardController* cKeyboardController;
	CTextRenderer* infoTextRenderer;
	CTextRenderer* invCountTextRenderer;
	Shader* textShader;
	CStopWatch cStopWatch;
	CAnimatedSprites animatedSprites;
	vector<pair<int, int>> playerInv;

	CMap2D();
	virtual ~CMap2D();
	bool LoadTex(const char* filename, const int iTextureCode);
	bool LoadAnimationTex(const char* fPath, GLuint& iTextureID);
};