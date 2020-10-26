#pragma once
#include "../DesignPatterns/SingletonTemplate.h"

class CImageLoader: public CSingletonTemplate<CImageLoader>{
	friend CSingletonTemplate<CImageLoader>;
public:
	bool Init();
	unsigned char* Load(const char* filename, int& width, int& height, int& nrChannels, const bool bInvert = false); //Load img and return as unsigned char*
protected:
	CImageLoader() = default;
	virtual ~CImageLoader() = default;
};