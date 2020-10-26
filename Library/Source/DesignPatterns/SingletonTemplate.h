#pragma once

template <typename T>
class CSingletonTemplate{
public:
	static T* GetInstance(){
		if(!instance){
			instance = new T;
		}
		return instance;
	}
	static void Destroy(){
		if(instance){
			delete instance;
			instance = nullptr;
		}
	}
protected:
	CSingletonTemplate() = default;
private:
	static T* instance;
};

template <typename T>
T* CSingletonTemplate<T>::instance = nullptr;