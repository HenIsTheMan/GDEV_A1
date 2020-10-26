#include "ImageLoader.h"
#include <iostream>
using namespace std;
#include <GL/glew.h>
#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
#endif
#include <includes/stb_image.h>
#include "filesystem.h"

bool CImageLoader::Init(){
	return false;
}

unsigned char* CImageLoader::Load(const char* filename, int& width, int& height, int& nrChannels, const bool bInvert){
	stbi_set_flip_vertically_on_load(bInvert);
	unsigned char *data = stbi_load(FileSystem::getPath(filename).c_str(), &width, &height, &nrChannels, 0);
	return data;
}