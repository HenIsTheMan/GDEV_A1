#pragma once
#include "../DesignPatterns/SingletonTemplate.h"
#include "shader.h"
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>
#include "../GameControl/Settings.h"
#include <map>

class CTextRenderer{
	struct Character final{ //Holds all state info relevant to a char as loaded using FreeType
		GLuint TextureID; //ID handle of the glyph texture
		glm::ivec2 Size; //Size of glyph
		glm::ivec2 Bearing; //Offset from baseline to left/top of glyph
		GLuint Advance; //Horizontal offset to advance to next glyph
	};
public:
	CTextRenderer();
	virtual ~CTextRenderer();
	bool Init(const char* fPath);
	void PreRender();
	void Render(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour);
	void PostRender();
	void SetShader(Shader* cShader);
protected:
	Shader* cShader; //Handler to Shader Prog instance
	unsigned int VBO, VAO, EBO;
	CSettings* cSettings;
	std::map<GLchar, Character> Characters;
};