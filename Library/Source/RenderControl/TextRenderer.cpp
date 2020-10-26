#include "TextRenderer.h"
#include <iostream>
using namespace std;
#include <GL/glew.h>
#include "../System/filesystem.h"
#include <ft2build.h>
#include FT_FREETYPE_H

CTextRenderer::CTextRenderer(): cShader(nullptr), cSettings(nullptr), VAO(0), VBO(0), EBO(0){}

CTextRenderer::~CTextRenderer(){
	cShader = nullptr; //Don't delete as it was created elsewhere
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void CTextRenderer::SetShader(Shader* cShader){
	this->cShader = cShader;
}

bool CTextRenderer::Init(const char* fPath){
	if(!cShader){
		cout << "CTextRenderer::Init: Initialise shader before initialising this" << endl;
		return false;
	}
	cSettings = CSettings::GetInstance();
	glm::mat4 projection = glm::ortho(0.f, GLfloat(cSettings->iWindowWidth), 0.f, GLfloat(cSettings->iWindowHeight));
	cShader->use();
	cShader->setMat4("projection", projection);
	cShader->setInt("text", 0);

	FT_Library ft;
	if(FT_Init_FreeType(&ft)){ //Return non-0 when an error occurs
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
	}

	FT_Face face;
	if(FT_New_Face(ft, fPath, 0, &face)){ //Load font as face
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}
	FT_Set_Pixel_Sizes(face, 24, 24); //Set size to load glyphs as
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable byte-alignment restriction

	for(GLubyte c = 0; c < 128; c++){ //Load 1st 128 ASCII chars 
		if(FT_Load_Char(face, c, FT_LOAD_RENDER)){ //Load character glyph
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Character character = {texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), GLuint(face->glyph->advance.x)}; //Store char for later use
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	///Destroy FreeType
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return true;
}

void CTextRenderer::PreRender(){
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	cShader->use();
}

void CTextRenderer::Render(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 colour){
	cShader->use();
	glUniform3f(glGetUniformLocation(cShader->ID, "textColour"), colour.x, colour.y, colour.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	for(std::string::const_iterator c = text.begin(); c != text.end(); ++c){ //Iterate through all chars
		Character ch = Characters[*c];
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		///Update VBO for each character
		GLfloat vertices[6][4] = {
			{xpos, ypos + h, 0.f, 0.f},
			{xpos, ypos, 0.f, 1.f},
			{xpos + w, ypos, 1.f, 1.f},
			{xpos, ypos + h, 0.f, 0.f},
			{xpos + w, ypos, 1.f, 1.f},
			{xpos + w, ypos + h, 1.f, 0.f}
		};
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6); //Render quad
		x += (ch.Advance >> 6) * scale; //Advance cursors for next glyph (advance is the amt of 1/64 pixels) //Bitshift by 6 to get value in pixels (2 ^ 6 = 64 [divide amt of 1/64th pixels by 64 to get amount of pixels])
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CTextRenderer::PostRender(){
	glDisable(GL_BLEND);
}