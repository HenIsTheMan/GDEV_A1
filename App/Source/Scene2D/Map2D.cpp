#include "Map2D.h"
#ifndef GLEW_STATIC
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif
#include "System\filesystem.h"
#include "System\ImageLoader.h"
#include "../App/Source/CSVReader/CSVReader.h"
#include "../CSVWriter/CSVWriter.h"
#include <iostream>
using namespace std;

CSettings* CMap2D::cSettings = nullptr;

CMap2D::CMap2D():
	showCoinSpin(true), showDebugInfo(true), lvl(0), VAO(0), VBO(0), EBO(0), currCoinTexID(0), princessR(-1), princessC(-1), mapElapsedTime(0.f), coinSpinBT(0.f), switchDirBT(0.f),
	cKeyboardController(nullptr), cShader(nullptr), cPlayer2D(nullptr), entityFactory(nullptr),
	infoTextRenderer(nullptr), invCountTextRenderer(nullptr), textShader(nullptr), transform(glm::mat4(1.f)){}

CMap2D::~CMap2D(){
	cKeyboardController = nullptr; //...
	cSettings = nullptr; //...
	cShader = nullptr; //...
	if(entityFactory){
		entityFactory->Destroy();
		entityFactory = nullptr;
	}
	if(textShader){
		delete textShader;
		textShader = nullptr;
	}
	if(infoTextRenderer){
		delete infoTextRenderer;
		infoTextRenderer = nullptr;
	}
	if(invCountTextRenderer){
		delete invCountTextRenderer;
		invCountTextRenderer = nullptr;
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void CMap2D::SetShader(Shader* cShader){
	this->cShader = cShader;
}

bool CMap2D::Init(const string& fPath){
	if(!cShader){
		cout << "CMap2D::Init(): Shader not found" << endl;
		return 0;
	}
	cKeyboardController = CKeyboardController::GetInstance();
	cSettings = CSettings::GetInstance();

	textShader = new Shader("Shader/text.vs", "Shader/text.fs");
	textShader->use();
	glActiveTexture(GL_TEXTURE0);
	cShader->setInt("text", 0);

	infoTextRenderer = new CTextRenderer;
	infoTextRenderer->SetShader(textShader);
	if(!infoTextRenderer->Init("Fonts/Notes.ttf")){
		cout << "Failed to load infoTextRenderer" << endl;
		return false;
	}

	invCountTextRenderer = new CTextRenderer;
	invCountTextRenderer->SetShader(textShader);
	if(!invCountTextRenderer->Init("Fonts/Bold.ttf")){
		cout << "Failed to load invCountTextRenderer" << endl;
		return false;
	}

	SetNumSteps(CSettings::AXIS::x, 7);
	SetNumSteps(CSettings::AXIS::y, 7);

	float vertices[] = {
		(cSettings->TILE_WIDTH / 2.f), (cSettings->TILE_HEIGHT / 2.f), 0.f, 1.f, 1.f,
		(cSettings->TILE_WIDTH / 2.f), -(cSettings->TILE_HEIGHT / 2.f), 0.f, 1.f, 0.f,
		-(cSettings->TILE_WIDTH / 2.f), -(cSettings->TILE_HEIGHT / 2.f), 0.f, 0.f, 0.f,
		-(cSettings->TILE_WIDTH / 2.f), (cSettings->TILE_HEIGHT / 2.f), 0.f, 0.f, 1.f
	};
	unsigned int indices[] = {0, 1, 3, 1, 2, 3};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	bool dir;
	int xMicroSteps, yMicroSteps;
	if(!LoadMap(fPath, xMicroSteps, yMicroSteps, dir)){ //Load map into an arr to init it and if fail...
		return false;
	}
	if(!LoadTex("Images/EmptyHeart.tga", -3)){
		std::cout << "Failed to load EmptyHeart.tga" << std::endl;
		return false;
	}
	if(!LoadTex("Images/HalfHealthHeart.tga", -2)){
		std::cout << "Failed to load HalfHealthHeart.tga" << std::endl;
		return false;
	}
	if(!LoadTex("Images/BG.tga", -1)){
		std::cout << "Failed to load BG.tga" << std::endl;
		return false;
	}
	if(!LoadTex("Images/InvSlot.tga", 0)){
		std::cout << "Failed to load InvSlot.tga" << std::endl;
		return false;
	}

	const char* fPathsSolid[]{
		"Images/Dirt0.tga", //100
		"Images/Dirt1.tga", //101
		"Images/Ice.tga", //102
		"Images/Lock.tga", //103
		"Images/Mystery.tga", //104
		"Images/Bomb.tga", //105
		"Images/PrincessLeft.tga", //106
		"Images/PrincessRight.tga", //107
	};
	const char* fPathsNotSolid[]{
		"Images/Coin0.tga", //200
		"Images/HealthHeart.tga", //201
		"Images/LifeHeart.tga", //202
		"Images/Acid.tga", //203
		"Images/Lava.tga", //204
		"Images/Checkpoint1.tga", //205
		"Images/Checkpoint2.tga", //206
		"Images/Medkit.tga", //207
		"Images/BlackPotion.tga", //208
		"Images/BluePotion.tga", //209
		"Images/GreenPotion.tga", //210
		"Images/OrangePotion.tga", //211
		"Images/PurplePotion.tga", //212
		"Images/RedPotion.tga", //213
		"Images/Key.tga", //214
	};
	for(short i = 0; i < sizeof(fPathsSolid) / sizeof(fPathsSolid[0]); ++i){
		if(!LoadTex(fPathsSolid[i], 100 + i)){
			std::cout << "Failed to load " << fPathsSolid[i] << std::endl;
			return false;
		}
	}
	for(short i = 0; i < sizeof(fPathsNotSolid) / sizeof(fPathsNotSolid[0]); ++i){
		if(!LoadTex(fPathsNotSolid[i], 200 + i)){
			std::cout << "Failed to load " << fPathsNotSolid[i] << std::endl;
			return false;
		}
	}

	animatedSprites.Add(CAnimatedSprites::TYPE::COIN_SPIN, currCoinTexID);
	if(!LoadAnimationTex("Images/Coin1.tga", currCoinTexID)){
		std::cout << "Failed to load Coin1.tga" << std::endl;
		return false;
	}
	animatedSprites.Add(CAnimatedSprites::TYPE::COIN_SPIN, currCoinTexID);
	if(!LoadAnimationTex("Images/Coin2.tga", currCoinTexID)){
		std::cout << "Failed to load Coin2.tga" << std::endl;
		return false;
	}
	animatedSprites.Add(CAnimatedSprites::TYPE::COIN_SPIN, currCoinTexID);
	if(!LoadAnimationTex("Images/Coin3.tga", currCoinTexID)){
		std::cout << "Failed to load Coin3.tga" << std::endl;
		return false;
	}
	animatedSprites.Add(CAnimatedSprites::TYPE::COIN_SPIN, currCoinTexID);
	if(!LoadAnimationTex("Images/Coin4.tga", currCoinTexID)){
		std::cout << "Failed to load Coin4.tga" << std::endl;
		return false;
	}
	animatedSprites.Add(CAnimatedSprites::TYPE::COIN_SPIN, currCoinTexID);
	if(!LoadAnimationTex("Images/Coin5.tga", currCoinTexID)){
		std::cout << "Failed to load Coin5.tga" << std::endl;
		return false;
	}
	animatedSprites.Add(CAnimatedSprites::TYPE::COIN_SPIN, currCoinTexID);
	if(!LoadAnimationTex("Images/Coin6.tga", currCoinTexID)){
		std::cout << "Failed to load Coin6.tga" << std::endl;
		return false;
	}
	animatedSprites.Add(CAnimatedSprites::TYPE::COIN_SPIN, currCoinTexID);
	if(!LoadAnimationTex("Images/Coin7.tga", currCoinTexID)){
		std::cout << "Failed to load Coin7.tga" << std::endl;
		return false;
	}
	animatedSprites.Add(CAnimatedSprites::TYPE::COIN_SPIN, currCoinTexID);

	entityFactory = EntityFactory::GetInstance();
	cPlayer2D = (CPlayer2D*)entityFactory->Create(Entity::ENTITY_TYPE::PLAYER);

	cPlayer2D->SetShader(cShader);
	if(!cPlayer2D->Init()){
		cout << "Failed to load CPlayer2D" << endl;
		return false;
	}
	if(!FindValue(1000, princessR, princessC)){
		cout << "Unable to find player's start pos" << endl;
		return false;
	}
	SetMapInfo(princessR, princessC, GetMapInfo(princessR, princessC) - 1000);
	cPlayer2D->Seti32vec2Index(princessC, princessR); //Set start pos of cPlayer2D
	cPlayer2D->Seti32vec2NumMicroSteps(xMicroSteps, yMicroSteps);
	cPlayer2D->currDir = dir ? Entity::DIR::RIGHT : Entity::DIR::LEFT;

	while(FindValue(500, princessR, princessC)){
		CEnemy2D* const& cEnemy2D = (CEnemy2D*)entityFactory->Create(Entity::ENTITY_TYPE::ENEMY);
		cEnemy2D->SetShader(cShader);
		if(!cEnemy2D->Init()){
			cout << "Failed to load cEnemy2D\n";
			return 0;
		}
		SetMapInfo(princessR, princessC, GetMapInfo(princessR, princessC) - 500);
		cEnemy2D->Seti32vec2Index(princessC, princessR); //Set start pos of cEnemy2D
		cEnemy2D->SetSpawnIndex(princessC, princessR);
		cEnemy2D->Seti32vec2NumMicroSteps(0, 0); //Microsteps are 0 by default
		cEnemy2D->SetPlayer2D(cPlayer2D);
	}

	///Princess
	if(!FindValue(106, princessR, princessC)){
		if(!FindValue(107, princessR, princessC)){
			cout << "Unable to find princess's pos" << endl;
			return false;
		}
	}
	return 1;
}

void CMap2D::Update(const float& dt){
	mapElapsedTime += dt;
	if(switchDirBT <= mapElapsedTime){
		SetMapInfo(princessR, princessC, GetMapInfo(princessR, princessC) == 106 ? 107 : 106);
		switchDirBT = mapElapsedTime + (float((rand() % 6) + 1) / 10.f + .4f);
	}
	if(coinSpinBT <= mapElapsedTime){
		currCoinTexID = animatedSprites.Get(CAnimatedSprites::TYPE::COIN_SPIN);
		coinSpinBT = mapElapsedTime + .1f;
	}
	entityFactory->Update(dt);
	if(cKeyboardController->IsKeyPressed('Y')){
		showDebugInfo = !showDebugInfo;
	}
	if(cKeyboardController->IsKeyPressed('U')){
		showCoinSpin = !showCoinSpin;
	}
}

void CMap2D::PreRender(){
	cShader->use();
	infoTextRenderer->PreRender();
	invCountTextRenderer->PreRender();
	entityFactory->PreRender();
}

void CMap2D::Render(){
	///Render BG
	transform = glm::scale(glm::mat4(1.f), glm::vec3(cSettings->NUM_TILES_XAXIS, cSettings->NUM_TILES_YAXIS, 1.f));
	glUniformMatrix4fv(glGetUniformLocation(cShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(transform));
	glBindTexture(GL_TEXTURE_2D, MapOfTexIDs.at(-1));
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	for(int iRow = 0; iRow < (int)cSettings->NUM_TILES_YAXIS; ++iRow){
		for(int iCol = 0; iCol < (int)cSettings->NUM_TILES_XAXIS; ++iCol){
			transform = glm::mat4(1.f);
			transform = glm::translate(transform, glm::vec3(cSettings->ConvertIndexToUVSpace(CSettings::AXIS::x, iCol, false, 0), cSettings->ConvertIndexToUVSpace(CSettings::AXIS::y, iRow, true, 0), 0.f));
			glUniformMatrix4fv(glGetUniformLocation(cShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(transform));
			if(vMapInfo[iRow][iCol]){
				if(vMapInfo[iRow][iCol] == 200 && showCoinSpin){
					glBindTexture(GL_TEXTURE_2D, currCoinTexID);
				} else{
					glBindTexture(GL_TEXTURE_2D, MapOfTexIDs.at(vMapInfo[iRow][iCol]));
				}
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
			}
		}
	}
	
	playerInv = cPlayer2D->GetInv();
	for(size_t i = 0; i < playerInv.size(); ++i){
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3((i & 1 ? .9f : .7f) - .02f, i < 2 ? .9f : .75f, 0.f));
		transform = glm::scale(transform, glm::vec3(3.f, 3.f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(cShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(transform));
		glBindTexture(GL_TEXTURE_2D, MapOfTexIDs.at(playerInv[i].first));
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3((i & 1 ? .9f : .7f) - .02f, i < 2 ? .9f : .75f, 0.f));
		transform = glm::scale(transform, glm::vec3(3.4f, 3.4f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(cShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(transform));
		glBindTexture(GL_TEXTURE_2D, MapOfTexIDs.at(0));
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	entityFactory->Render();
	for(short i = 0; i < cPlayer2D->maxLives; ++i){ //Render player lives
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(-.6f + .08f * i, .96f, 0.f));
		transform = glm::scale(transform, glm::vec3(1.5f, 1.5f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(cShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(transform));
		glBindTexture(GL_TEXTURE_2D, MapOfTexIDs.at(cPlayer2D->lives >= i + 1 ? 202 : -3));
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	for(short i = 0; i < cPlayer2D->maxHealth / 2; ++i){ //Render player health
		transform = glm::mat4(1.f);
		transform = glm::translate(transform, glm::vec3(-.55f + .08f * i, .89f, 0.f));
		transform = glm::scale(transform, glm::vec3(1.5f, 1.5f, 1.f));
		glUniformMatrix4fv(glGetUniformLocation(cShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(transform));
		if(cPlayer2D->health <= i * 2){
			glBindTexture(GL_TEXTURE_2D, MapOfTexIDs.at(-3));
		} else if(cPlayer2D->health == i * 2 + 1){
			glBindTexture(GL_TEXTURE_2D, MapOfTexIDs.at(-2));
		} else{
			glBindTexture(GL_TEXTURE_2D, MapOfTexIDs.at(201));
		}
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	stringstream ss;
	for(size_t i = 0; i < playerInv.size(); ++i){
		ss << i + 1;
		invCountTextRenderer->Render(ss.str(), CSettings::GetInstance()->iWindowWidth - (cSettings->NUM_TILES_XAXIS * .7f) * (~i & 1 ? 3.15f : .5f),
			CSettings::GetInstance()->iWindowHeight - (cSettings->NUM_TILES_YAXIS * .7f) * (i > 1 ? 3.f : 1.f), .7f, glm::vec3(.7f));
		ss.str("");

		if(playerInv[i].first && playerInv[i].second > 1){
			ss << playerInv[i].second;
			invCountTextRenderer->Render(ss.str(), CSettings::GetInstance()->iWindowWidth - (cSettings->NUM_TILES_XAXIS * .7f) * (~i & 1 ? 5.1f : 2.4f),
				CSettings::GetInstance()->iWindowHeight - (cSettings->NUM_TILES_YAXIS * .7f) * (i > 1 ? 4.f : 2.f), .7f, glm::vec3(1.f));
			ss.str("");
		}
	}

	ss << "Lives: ";
	infoTextRenderer->Render(ss.str(), 10.f, CSettings::GetInstance()->iWindowHeight - (cSettings->NUM_TILES_YAXIS * 1.3f) * .5f, 1.3f, glm::vec3(1.f));
	ss.str("");

	ss << "Health: ";
	infoTextRenderer->Render(ss.str(), 10.f, CSettings::GetInstance()->iWindowHeight - (cSettings->NUM_TILES_YAXIS * 1.3f) * 1.f, 1.3f, glm::vec3(1.f));
	ss.str("");

	ss << "Score: " << cPlayer2D->GetScore();
	infoTextRenderer->Render(ss.str(), 10.f, CSettings::GetInstance()->iWindowHeight - (cSettings->NUM_TILES_YAXIS * 1.3f) * 1.5f, 1.3f, glm::vec3(1.f));
	ss.str("");

	ss << "Got Key: " << string(cPlayer2D->GetGotKey() ? "Yes" : "No");
	infoTextRenderer->Render(ss.str(), 10.f, CSettings::GetInstance()->iWindowHeight - (cSettings->NUM_TILES_YAXIS * 1.3f) * 2.f, 1.3f, glm::vec3(1.f));
	ss.str("");

	if(showDebugInfo){
		ss << "FPS: " << 1.f / (float)cStopWatch.GetElapsedTime();
		infoTextRenderer->Render(ss.str(), 10.f, CSettings::GetInstance()->iWindowHeight - (cSettings->NUM_TILES_YAXIS * 1.3f) * 2.5f, 1.3f, glm::vec3(0.f, 1.f, 0.f));
		ss.str("");

		ss << "Level: " << lvl;
		infoTextRenderer->Render(ss.str(), 10.f, CSettings::GetInstance()->iWindowHeight - (cSettings->NUM_TILES_YAXIS * 1.3f) * 3.f, 1.3f, glm::vec3(0.f, 1.f, 0.f));
		ss.str("");
	}
}

void CMap2D::PostRender(){
	glDisable(GL_BLEND);
	infoTextRenderer->PostRender();
	invCountTextRenderer->PostRender();
	entityFactory->PostRender();
}

const float& CMap2D::GetNumSteps(const CSettings::AXIS& sAxis){
	if(sAxis == CSettings::AXIS::x){
		return cSettings->NUM_STEPS_PER_TILE_XAXIS;
	} else if(sAxis == CSettings::AXIS::y){
		return cSettings->NUM_STEPS_PER_TILE_YAXIS;
	} else{
		cout << "Unknown axis" << endl;
		exit(1);
	}
}

void CMap2D::SetNumSteps(const CSettings::AXIS& sAxis, const unsigned int& val){
	if(val <= 0){
		cout << "CMap2D::SetNumSteps(): value must be more than 0" << endl;
		return;
	}
	if(sAxis == CSettings::AXIS::x){
		cSettings->NUM_STEPS_PER_TILE_XAXIS = (float)val;
		cSettings->UpdateMapSpecifications();
	} else if(sAxis == CSettings::AXIS::y){
		cSettings->NUM_STEPS_PER_TILE_YAXIS = (float)val;
		cSettings->UpdateMapSpecifications();
	} else if(sAxis == CSettings::AXIS::z){
		//Not used here
	} else{
		cout << "Unknown axis" << endl;
	}
}

void CMap2D::SetNumTiles(const CSettings::AXIS sAxis, const unsigned int uiValue){
	if(uiValue <= 0){
		cout << "CMap2D::SetNumTiles(): value must be > 0" << endl;
		return;
	}
	if(sAxis == CSettings::AXIS::x){
		cSettings->NUM_TILES_XAXIS = uiValue;
		cSettings->UpdateMapSpecifications();
	} else if(sAxis == CSettings::AXIS::y){
		cSettings->NUM_TILES_YAXIS = uiValue;
		cSettings->UpdateMapSpecifications();
	} else if(sAxis == CSettings::AXIS::z){
		//Not used here
	} else{
		cout << "Unknown axis" << endl;
	}
}

void CMap2D::SetMapInfo(const int iRow, const int iCol, const int iValue, const bool bInvert){
	vMapInfo[bInvert ? cSettings->NUM_TILES_YAXIS - iRow - 1 : iRow][iCol] = iValue;
}

int CMap2D::GetMapInfo(const int iRow, const int iCol, const bool bInvert) const{
	return vMapInfo[bInvert ? cSettings->NUM_TILES_YAXIS - iRow - 1 : iRow][iCol];
}

bool CMap2D::LoadMap(const string& fPath, int& xMicroSteps, int& yMicroSteps, bool& dir){
	std::ifstream extraData("Maps/ExtraData.lmao");
	std::string line;
	short count = 3; //No. of lines to read from
	try{
		if(!extraData.good() || !extraData.is_open()){
			throw runtime_error("");
		}
		while(count && getline(extraData, line)){
			switch(count){
				case 3: xMicroSteps = stoi(line); break;
				case 2:	yMicroSteps = 0; break;
				case 1: dir = line == "right";
			}
			--count;
		}
		extraData.close();
	} catch(const runtime_error&){
		xMicroSteps = yMicroSteps = 0;
		dir = true;
	}

	CSVReader readerCSV;
	vMapInfo = readerCSV.read_CSV(FileSystem::getPath(fPath).c_str(), cSettings->NUM_TILES_XAXIS, cSettings->NUM_TILES_YAXIS); //Load CSV file into vMapInfo
	lvl = fPath.at(fPath.find('.') - 1) - 48;
	return vMapInfo.size() ? true : false;
}

bool CMap2D::SaveMap(const string& fPath, bool&& saveExtraData){
	vector<vector<int>> vMapForSaving(vMapInfo);

	const size_t&& entityMapSize = entityFactory->GetEntityMapSize();
	for(size_t i = 1; i < entityMapSize; ++i){
		Entity* const entity = entityFactory->GetEntity(short(i));
		if(entity){
			const glm::i32vec2&& enemyPos = entity->Geti32vec2Index();
			vMapForSaving[(int)cSettings->NUM_TILES_YAXIS - enemyPos.y - 1][enemyPos.x] += 500; ///////////////////
		}
	}
	glm::i32vec2 playerPos = cPlayer2D->Geti32vec2Index();
	vMapForSaving[(int)cSettings->NUM_TILES_YAXIS - playerPos.y - 1][playerPos.x] += 1000; //////////////////////

	if(saveExtraData){
		try{
			std::fstream extraData("Maps/ExtraData.lmao", std::ios::out);
			if(extraData.is_open()){
				extraData << cPlayer2D->Geti32vec2NumMicroSteps().x << endl;
				extraData << cPlayer2D->Geti32vec2NumMicroSteps().y << endl;
				extraData << (cPlayer2D->currDir == Entity::DIR::LEFT ? "left" : "right");
				extraData.close();
			} else{
				throw("Unable to store extra data.\n");
			}
		} catch(const char* errorMsg){
			printf(errorMsg);
		}
	}

	CCSVWriter CSVWriter;
	CSVWriter.write_CSV(fPath, (int)cSettings->NUM_TILES_XAXIS, (int)cSettings->NUM_TILES_YAXIS, vMapForSaving);
	return true;
}

const short& CMap2D::GetLvl() const{
	return lvl;
}

bool CMap2D::FindValue(const int iValue, int& iRow, int& iCol, const bool bInvert){
	for(int irow = 0; irow < (int)cSettings->NUM_TILES_YAXIS; ++irow){
		for(int icol = 0; icol < (int)cSettings->NUM_TILES_XAXIS; ++icol){
			if(iValue == 1000){
				if(vMapInfo[irow][icol] >= iValue){
					iRow = bInvert ? cSettings->NUM_TILES_YAXIS - irow - 1 : irow;
					iCol = icol;
					return true; //Return true if val was found
				}
			}
			if(iValue == 500){
				if(vMapInfo[irow][icol] >= iValue){
					iRow = bInvert ? cSettings->NUM_TILES_YAXIS - irow - 1 : irow;
					iCol = icol;
					return true; //Return true if val was found
				}
			}
			if(iValue != 500 && iValue != 1000){
				if(vMapInfo[irow][icol] == iValue){
					iRow = bInvert ? cSettings->NUM_TILES_YAXIS - irow - 1 : irow;
					iCol = icol;
					return true; //Return true if val was found
				}
			}
		}
	}
	return false;
}

bool CMap2D::LoadTex(const char* filename, const int iTextureCode){
	int width, height, nrChannels;
	unsigned int texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(iTextureCode == 200){
		currCoinTexID = texID;
	}

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char *data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if(data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB + nrChannels - 3, width, height, 0, GL_RGB + nrChannels - 3, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		MapOfTexIDs.insert(pair<int, int>(iTextureCode, texID)); //Store texID in MapOfTexIDs
		free(data); //Free mem of file data is read in
		cImageLoader->Destroy();
		return 1;
	}
	cImageLoader->Destroy();
	return 0;
}

bool CMap2D::LoadAnimationTex(const char* fPath, GLuint& iTextureID){
	int width, height, nrChannels;
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char *data = cImageLoader->Load(fPath, width, height, nrChannels, true);
	if(data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB + nrChannels - 3, width, height, 0, GL_RGB + nrChannels - 3, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		free(data); //Free mem of file data read in
		cImageLoader->Destroy();
		return true;
	}
	cImageLoader->Destroy();
	return false;
}