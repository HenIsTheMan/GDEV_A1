#include "Entity.h"
#include <iostream>
#include <GL/glew.h>
#include "System\ImageLoader.h"
using namespace std;

Entity::Entity(const ENTITY_TYPE& type): cShader(nullptr), cSettings(nullptr), transform(glm::mat4(1.f)){
	aliveTime = 0.f;
	currDir = DIR::RIGHT;
	VAO = VBO = EBO = 0;
	currTexID = idleLeftTexID = idleRightTexID = jumpLeftTexID = jumpRightTexID = fallLeftTexID = fallRightTexID = 0;
	i32vec2Index = i32vec2NumMicroSteps = glm::i32vec2(0);
	vec2UVCoords = glm::vec2(0.f);
	this->type = type;
}

Entity::~Entity(){
	cShader = nullptr; //Don't delete as...
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

bool Entity::Init(){
	cSettings = CSettings::GetInstance();
	float vertices[]{
		(cSettings->TILE_WIDTH / 2.f), (cSettings->TILE_HEIGHT / 2.f), 0.f, 1.f, 1.f, //Top right
		(cSettings->TILE_WIDTH / 2.f), -(cSettings->TILE_HEIGHT / 2.f), 0.f, 1.f, 0.f, //Bottom right
		-(cSettings->TILE_WIDTH / 2.f), -(cSettings->TILE_HEIGHT / 2.f), 0.f, 0.f, 0.f, //Bottom left
		-(cSettings->TILE_WIDTH / 2.f), (cSettings->TILE_HEIGHT / 2.f), 0.f, 0.f, 1.f //Top left 
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

	return true;
}

void Entity::PreRender(){
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	cShader->use();
}

void Entity::Render(){
	transform = glm::mat4(1.f);
	transform = glm::translate(transform, glm::vec3(vec2UVCoords.x, vec2UVCoords.y, 0.f));
	glUniformMatrix4fv(glGetUniformLocation(cShader->ID, "model"), 1, GL_FALSE, glm::value_ptr(transform));
	glBindTexture(GL_TEXTURE_2D, currTexID);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Entity::PostRender(){
	glDisable(GL_BLEND);
}

void Entity::SetShader(Shader* cShader){
	this->cShader = cShader;
}

void Entity::Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis){
	this->i32vec2Index.x = iIndex_XAxis;
	this->i32vec2Index.y = iIndex_YAxis;
}

void Entity::Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis){
	this->i32vec2NumMicroSteps.x = iNumMicroSteps_XAxis;
	this->i32vec2NumMicroSteps.y = iNumMicroSteps_YAxis;
}

void Entity::Setvec2UVCoords(const float fUVCoords_XAxis, const float fUVCoords_YAxis){
	this->vec2UVCoords.x = fUVCoords_XAxis;
	this->vec2UVCoords.y = fUVCoords_YAxis;
}

glm::i32vec2 Entity::Geti32vec2Index() const{
	return i32vec2Index;
}

glm::i32vec2 Entity::Geti32vec2NumMicroSteps() const{
	return i32vec2NumMicroSteps;
}

glm::vec2 Entity::Getvec2UVCoords() const{
	return vec2UVCoords;
}

bool Entity::LoadTexture(const char* fPath, GLuint& iTextureID){
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