#include "Scene2D.h"
#include <iostream>
using namespace std;
#define GLEW_STATIC
#include <GL/glew.h>
#include "System\ImageLoader.h"

bool CScene2D::showMenu = true;
CMap2D* CScene2D::cMap2D = nullptr;

CScene2D::CScene2D(): cursorPos({0, 0}), cKeyboardController(nullptr), cShader(nullptr), otherTextRenderer(nullptr), titleTextRenderer(nullptr),
	textShader(nullptr), model(glm::mat4(1.f)){
	texID = VAO = VBO = EBO = 0;
	startColour = continueColour = glm::vec3(1.f);
}

CScene2D::~CScene2D(){
	if(cShader){
		delete cShader;
		cShader = nullptr;
	}
	if(cMap2D){
		cMap2D->Destroy();
		cMap2D = nullptr;
	}
	if(textShader){
		delete textShader;
		textShader = nullptr;
	}
	if(otherTextRenderer){
		delete otherTextRenderer;
		otherTextRenderer = nullptr;
	}
	if(titleTextRenderer){
		delete titleTextRenderer;
		titleTextRenderer = nullptr;
	}
}

bool CScene2D::Init(){
	cKeyboardController = CKeyboardController::GetInstance();
	cShader = new Shader("Shader/main.vs", "Shader/main.fs");
	cShader->use();
	glActiveTexture(GL_TEXTURE0);
	cShader->setInt("texture1", 0);

	textShader = new Shader("Shader/text.vs", "Shader/text.fs");
	textShader->use();
	glActiveTexture(GL_TEXTURE0);
	cShader->setInt("text", 0);

	titleTextRenderer = new CTextRenderer;
	titleTextRenderer->SetShader(textShader);
	if(!titleTextRenderer->Init("Fonts/Bold.ttf")){
		cout << "Failed to load titleTextRendererer" << endl;
		return false;
	}

	otherTextRenderer = new CTextRenderer;
	otherTextRenderer->SetShader(textShader);
	if(!otherTextRenderer->Init("Fonts/Notes.ttf")){
		cout << "Failed to load otherTextRenderer" << endl;
		return false;
	}

	LoadBG("Images/BG.tga");
	float vertices[]{
		(CSettings::GetInstance()->TILE_WIDTH / 2.f), (CSettings::GetInstance()->TILE_HEIGHT / 2.f), 0.f, 1.f, 1.f, //Top right
		(CSettings::GetInstance()->TILE_WIDTH / 2.f), -(CSettings::GetInstance()->TILE_HEIGHT / 2.f), 0.f, 1.f, 0.f, //Bottom right
		-(CSettings::GetInstance()->TILE_WIDTH / 2.f), -(CSettings::GetInstance()->TILE_HEIGHT / 2.f), 0.f, 0.f, 0.f, //Bottom left
		-(CSettings::GetInstance()->TILE_WIDTH / 2.f), (CSettings::GetInstance()->TILE_HEIGHT / 2.f), 0.f, 0.f, 1.f //Top left 
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
	glBindVertexArray(0);

	return true;
}

void CScene2D::Update(const float& dt){
	if(showMenu){
		if(CSettings::GetInstance()->NUM_TILES_XAXIS != 32){
			CSettings::GetInstance()->NUM_TILES_XAXIS = 32;
		}
		if(CSettings::GetInstance()->NUM_TILES_YAXIS != 24){
			CSettings::GetInstance()->NUM_TILES_YAXIS = 24;
		}
		if(GetCursorPos(&cursorPos) && ScreenToClient(::GetActiveWindow(), &cursorPos)){
			if(cursorPos.x >= (CSettings::GetInstance()->NUM_TILES_XAXIS * 2.f) * .2f && cursorPos.x <= (CSettings::GetInstance()->NUM_TILES_XAXIS * 2.f) * .2f + 20.8f * 2.f * 5.f &&
				CSettings::GetInstance()->iWindowHeight - cursorPos.y >= (CSettings::GetInstance()->NUM_TILES_YAXIS * 2.f) * .2f &&
				CSettings::GetInstance()->iWindowHeight - cursorPos.y <= (CSettings::GetInstance()->NUM_TILES_YAXIS * 2.f) * .2f + 20.8f * 2.f){
				startColour = glm::vec3(1.f, 1.f, 0.f);
				continueColour = glm::vec3(1.f);

				if(GetKeyState(VK_LBUTTON) & 0x80){
					showMenu = false;
					if(CSettings::GetInstance()->NUM_TILES_XAXIS != 50){
						CSettings::GetInstance()->NUM_TILES_XAXIS = 50;
					}
					if(CSettings::GetInstance()->NUM_TILES_YAXIS != 50){
						CSettings::GetInstance()->NUM_TILES_YAXIS = 50;
					}
					if(!cMap2D){
						cMap2D = CMap2D::GetInstance();
						cMap2D->SetShader(cShader);
						if(!cMap2D->Init("Maps/Lvl_1.csv")){
							cout << "Failed to load CMap2D" << endl;
							exit(1);
						}
						cMap2D->cPlayer2D->lives = cMap2D->cPlayer2D->maxLives;
					} else{
						ChangeMap("Maps/Lvl_1.csv");
						cMap2D->cPlayer2D->lives = cMap2D->cPlayer2D->maxLives;
					}
				}
			} else if(cursorPos.x >= (CSettings::GetInstance()->NUM_TILES_XAXIS * 2.f) * .2f && cursorPos.x <= (CSettings::GetInstance()->NUM_TILES_XAXIS * 2.f) * .2f + 20.8f * 2.f * 8.f &&
				CSettings::GetInstance()->iWindowHeight - cursorPos.y >= (CSettings::GetInstance()->NUM_TILES_YAXIS * 2.f) * 1.2f &&
				CSettings::GetInstance()->iWindowHeight - cursorPos.y <= (CSettings::GetInstance()->NUM_TILES_YAXIS * 2.f) * 1.2f + 20.8f * 2.f){
				continueColour = glm::vec3(1.f, 1.f, 0.f);
				startColour = glm::vec3(1.f);

				ifstream lvl1("Maps/Save_Lvl_1.csv");
				ifstream lvl2("Maps/Save_Lvl_2.csv");
				string gdFile = "";
				if(lvl2.good()){
					gdFile = "Maps/Save_Lvl_2.csv";
				} else if(lvl1.good()){
					gdFile = "Maps/Save_Lvl_1.csv";
				}
				if((GetKeyState(VK_LBUTTON) & 0x80) && gdFile.length()){
					showMenu = false;
					if(CSettings::GetInstance()->NUM_TILES_XAXIS != 50){
						CSettings::GetInstance()->NUM_TILES_XAXIS = 50;
					}
					if(CSettings::GetInstance()->NUM_TILES_YAXIS != 50){
						CSettings::GetInstance()->NUM_TILES_YAXIS = 50;
					}
					if(!cMap2D){
						cMap2D = CMap2D::GetInstance();
						cMap2D->SetShader(cShader);
						if(!cMap2D->Init(gdFile)){
							cout << "Failed to load CMap2D" << endl;
							exit(1);
						}
						cMap2D->cPlayer2D->lives = cMap2D->cPlayer2D->maxLives;
					}
				}
			} else{
				startColour = continueColour = glm::vec3(1.f);
			}
		}
	} else{
		cMap2D->Update(dt);
		if(cKeyboardController->IsKeyDown('P')){
			try{
				if(!cMap2D->SaveMap("Maps/Save_Lvl_" + to_string(cMap2D->GetLvl()) + ".csv", true)){
					throw runtime_error("Unable to save game to a file");
				}
			} catch(const runtime_error& e){
				cout << "Runtime Error: " << e.what();
				return;
			}
		}
	}
}

void CScene2D::PreRender(){
	glClearColor(1.f, .3f, .3f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D); //Enable 2D tex rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	otherTextRenderer->PreRender();
	titleTextRenderer->PreRender();
	if(cMap2D){
		cMap2D->PreRender();
	}
}

void CScene2D::Render(){
	if(showMenu){
		cShader->use();
		model = glm::mat4(1.f);
		model = glm::scale(model, glm::vec3(float(CSettings::GetInstance()->NUM_TILES_XAXIS), float(CSettings::GetInstance()->NUM_TILES_YAXIS), 0.f));
		glUniformMatrix4fv(glGetUniformLocation(cShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		stringstream ss;

		ss << "Save";
		titleTextRenderer->Render(ss.str(), float(CSettings::GetInstance()->iWindowWidth) / 4.f, CSettings::GetInstance()->iWindowHeight - (CSettings::GetInstance()->NUM_TILES_YAXIS * 4.f) * 2.f, 4.f, glm::vec3(0.f, 1.f, 1.f));
		ss.str("");

		ss << "The";
		titleTextRenderer->Render(ss.str(), float(CSettings::GetInstance()->iWindowWidth) / 4.f, CSettings::GetInstance()->iWindowHeight - (CSettings::GetInstance()->NUM_TILES_YAXIS * 4.f) * 3.f, 4.f, glm::vec3(0.f, 1.f, 1.f));
		ss.str("");

		ss << "Princess!";
		titleTextRenderer->Render(ss.str(), float(CSettings::GetInstance()->iWindowWidth) / 4.f, CSettings::GetInstance()->iWindowHeight - (CSettings::GetInstance()->NUM_TILES_YAXIS * 4.f) * 4.f, 4.f, glm::vec3(0.f, 1.f, 1.f));
		ss.str("");

		ss << "Start";
		otherTextRenderer->Render(ss.str(), (CSettings::GetInstance()->NUM_TILES_XAXIS * 2.f) * .2f, (CSettings::GetInstance()->NUM_TILES_YAXIS * 2.f) * .2f, 2.f, startColour);
		ss.str("");

		ss << "Continue";
		otherTextRenderer->Render(ss.str(), (CSettings::GetInstance()->NUM_TILES_XAXIS * 2.f) * .2f, (CSettings::GetInstance()->NUM_TILES_YAXIS * 2.f) * 1.2f, 2.f, continueColour);
		ss.str("");
	} else{
		cMap2D->Render();
	}
}

void CScene2D::PostRender(){
	if(cMap2D){
		cMap2D->PostRender();
	}
	otherTextRenderer->PostRender();
	titleTextRenderer->PostRender();
}

void CScene2D::ChangeMap(const char* fPath){
	cMap2D->cPlayer2D->currDir = Entity::DIR::RIGHT;
	cMap2D->cPlayer2D->gotKey = false;
	cMap2D->cPlayer2D->health = cMap2D->cPlayer2D->maxHealth;

	///Ensure power-ups don't carry forward
	cMap2D->cPlayer2D->acidDmgCount = 999;
	cMap2D->cPlayer2D->cannotDieTime = -1.f;
	cMap2D->cPlayer2D->infiniteJumpsTime = -1.f;
	cMap2D->cPlayer2D->higherJumpsTime = -1.f;
	cMap2D->cPlayer2D->healFromDmg = 999;
	cMap2D->cPlayer2D->periodicHealTime = -1.f;

	const short prevLvl = cMap2D->lvl;
	bool dir;
	int xMicroSteps, yMicroSteps;
	if(!cMap2D->LoadMap(fPath, xMicroSteps, yMicroSteps, dir)){ //Load map into an arr to init it and if fail...
		cout << "Failed to load new map" << endl;
		exit(EXIT_FAILURE);
	}

	int iRow, iCol;
	const size_t&& entityMapSize = cMap2D->entityFactory->GetEntityMapSize();
	if(!cMap2D->FindValue(1000, iRow, iCol)){
		cout << "Unable to find player's start pos" << endl;
	}
	cMap2D->SetMapInfo(iRow, iCol, cMap2D->GetMapInfo(iRow, iCol) - 1000);
	cMap2D->cPlayer2D->Seti32vec2Index(iCol, iRow);
	cMap2D->cPlayer2D->Seti32vec2NumMicroSteps(xMicroSteps, yMicroSteps);
	cMap2D->cPlayer2D->currDir = dir ? Entity::DIR::RIGHT : Entity::DIR::LEFT;

	if(cMap2D->cPlayer2D->lives >= 0){ //If not changing lvls...
		cMap2D->cPlayer2D->score = 0;
		--cMap2D->cPlayer2D->lives;
		for(size_t i = 1; i < entityMapSize; ++i){
			Entity* entity = cMap2D->entityFactory->GetEntity(short(i));
			if(entity){
				const glm::i32vec2& enemySpawnIndex = dynamic_cast<const CEnemy2D* const>(entity)->spawnIndex;
				entity->Seti32vec2Index(enemySpawnIndex.x, enemySpawnIndex.y);
			}
		}
		while(cMap2D->FindValue(500, iRow, iCol)){
			cMap2D->SetMapInfo(iRow, iCol, cMap2D->GetMapInfo(iRow, iCol) - 500);
		}
	} else{
		cMap2D->cPlayer2D->touchedCheckpoint = false;
		cMap2D->cPlayer2D->lives = cMap2D->cPlayer2D->maxLives;
		for(size_t i = 1; i < entityMapSize; ++i){
			cMap2D->entityFactory->RemoveEntity(short(i));
		}
		while(cMap2D->FindValue(500, iRow, iCol)){
			CEnemy2D* const& cEnemy2D = (CEnemy2D*)cMap2D->entityFactory->Create(Entity::ENTITY_TYPE::ENEMY);
			cEnemy2D->SetShader(cMap2D->cShader);
			if(!cEnemy2D->Init()){
				cout << "Failed to load cEnemy2D\n";
			}
			cMap2D->SetMapInfo(iRow, iCol, cMap2D->GetMapInfo(iRow, iCol) - 500);
			cEnemy2D->Seti32vec2Index(iCol, iRow); //Set start pos of cEnemy2D
			cEnemy2D->SetSpawnIndex(iCol, iRow);
			cEnemy2D->Seti32vec2NumMicroSteps(0, 0); //Microsteps are 0 by default
			cEnemy2D->SetPlayer2D(cMap2D->cPlayer2D);
		}
	}
	if(cMap2D->lvl != prevLvl){
		cMap2D->cPlayer2D->lives = cMap2D->cPlayer2D->maxLives;
	}

	if(!cMap2D->FindValue(106, cMap2D->princessR, cMap2D->princessC)){
		if(!cMap2D->FindValue(107, cMap2D->princessR, cMap2D->princessC)){
			cout << "Unable to find princess's pos" << endl;
		}
	}
}

void CScene2D::LoadBG(const char* fPath){
	int width, height, nrChannels;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char *data = cImageLoader->Load(fPath, width, height, nrChannels, true);
	if(data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB + nrChannels - 3, width, height, 0, GL_RGB + nrChannels - 3, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		free(data); //...
		cImageLoader->Destroy();
		return;
	}
	cImageLoader->Destroy();
	exit(1);
}