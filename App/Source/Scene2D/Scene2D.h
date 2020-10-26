#pragma once
#include "RenderControl\shader.h"
#include "Inputs\KeyboardController.h"
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>
#include "DesignPatterns/SingletonTemplate.h"
#include "RenderControl\TextRenderer.h"
#include "Map2D.h"

class CScene2D: public CSingletonTemplate<CScene2D>{
	friend CSingletonTemplate<CScene2D>;
protected:
	CScene2D();
	virtual ~CScene2D();
	static CMap2D* cMap2D;
public:
	CTextRenderer* titleTextRenderer;
	CTextRenderer* otherTextRenderer;
	Shader* textShader;
	POINT cursorPos;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int texID;
	static void ChangeMap(const char* fPath);
	void LoadBG(const char* fPath);
	glm::mat4 model;
	static bool showMenu;
	bool Init();
	void Update(const float& dt);
	void PreRender();
	void Render();
	void PostRender();

	CKeyboardController* cKeyboardController;
	Shader* cShader;
	glm::vec3 startColour;
	glm::vec3 continueColour;
};