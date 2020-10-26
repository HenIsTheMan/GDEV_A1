#include "App.h"
#include <Windows.h>

#ifndef GLEW_STATIC
	#define GLEW_STATIC
	#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "GameControl\Settings.h"
#include "Inputs\KeyboardController.h"
#include <iostream>
using namespace std;

const GLFWvidmode* App::mode = nullptr;

static void error_callback(int error, const char* description){
	fputs(description, stderr);
	_fgetchar();
}

void repos_callback(GLFWwindow*, int xPos, int yPos){
	CSettings::GetInstance()->iWindowPosX = xPos;
	CSettings::GetInstance()->iWindowPosY = yPos;
	glViewport(0, 0, CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight);
}

void resize_callback(GLFWwindow*, int width, int height){
	CSettings::GetInstance()->iWindowWidth = width;
	CSettings::GetInstance()->iWindowHeight = height;
	glViewport(0, 0, width, height);
}

App::App(): cSettings(nullptr), cScene2D(nullptr){}

bool App::Init(){
	system("color 0A");
	if(!glfwInit()){
		return false;
	}
	cSettings = CSettings::GetInstance();
	cSettings->logl_root = "D:/My Documents/2020_2021_SEM1/DM2231 Game Development Techniques/Teaching Materials/Week 01/Practical/NYP_Framework_Week01"; //Set fPath for digital assets //Is a backup for when filesystem cannot find the curr directory

	///Set GLFW window creation hints (optional)
	if(cSettings->bUse4XAntiliasing){
		glfwWindowHint(GLFW_SAMPLES, 4);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	cSettings->iWindowWidth = cSettings->iWindowHeight = 945;
	cSettings->pWindow = glfwCreateWindow(cSettings->iWindowWidth, cSettings->iWindowHeight, "193541T GDEV", NULL, NULL);
	if(!cSettings->pWindow){
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return false;
	}

	cSettings->iWindowPosX = mode->width / 4;
	cSettings->iWindowPosY = mode->height / 16;
	glfwSetWindowPos(cSettings->pWindow, cSettings->iWindowPosX, cSettings->iWindowPosY);
	glfwMakeContextCurrent(cSettings->pWindow); //Makes OpenGL context of the specified win curr on the calling thread 
	glfwSetWindowPosCallback(cSettings->pWindow, repos_callback);
	glfwSetWindowSizeCallback(cSettings->pWindow, resize_callback);
	glfwSetFramebufferSizeCallback(cSettings->pWindow, resize_callback);
	glfwSetErrorCallback(error_callback);

	if(!cSettings->bShowMousePointer){
		glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); //Hide cursor
	}

	glewExperimental = GL_TRUE; //Var alr defined by GLEW (must set to GL_TRUE before calling glewInit)
	GLenum glewInitErr = glewInit();
	if(glewInitErr != GLEW_OK){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glewInitErr));
		glfwTerminate();
		return false;
	}

	cScene2D = CScene2D::GetInstance();
	if(!cScene2D->Init()){
		printf("Failed to load cScene2D\n");
		return false;
	}
	return true;
}

void App::Run(){
	cStopWatch.StartTimer();
	while(!glfwWindowShouldClose(cSettings->pWindow) && (!CKeyboardController::GetInstance()->IsKeyReleased(VK_ESCAPE))){
		cScene2D->Update((float)cStopWatch.GetElapsedTime());
		cScene2D->PreRender();
		cScene2D->Render();
		cScene2D->PostRender();
		glfwSwapBuffers(cSettings->pWindow);
		glfwPollEvents();
		UpdateInputDevices();
		cStopWatch.WaitUntil(cSettings->frameTime);
	}
}

void App::Destroy(){
	glfwDestroyWindow(cSettings->pWindow); //Close OpenGL win and terminate GLFW
	glfwTerminate(); //Finalise and clean up GLFW
	CKeyboardController::GetInstance()->Destroy();
	if(cSettings){
		cSettings->Destroy();
		cSettings = nullptr;
	}
	cScene2D->Destroy();
	CSingletonTemplate::Destroy();
}

int App::GetWindowHeight() const{
	return cSettings->iWindowHeight;
}

int App::GetWindowWidth() const{
	return cSettings->iWindowWidth;
}

void App::UpdateInputDevices(){
	CKeyboardController::GetInstance()->Update();
}

void App::PostUpdateInputDevices(){
	CKeyboardController::GetInstance()->PostUpdate();
}