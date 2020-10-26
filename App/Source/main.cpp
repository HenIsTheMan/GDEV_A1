#include "App.h"
//#define _DEBUGMODE 1; //Can use debug macro in VS proj properties instead

BOOL PlayMidi(char* fileName){
	wchar_t buffer[256];
	swprintf_s(buffer, L"open %S type sequencer alias MUSIC", fileName);
	if(mciSendStringW(L"close all", NULL, 0, NULL) != 0){
		return(FALSE);
	}
	if(mciSendStringW(buffer, NULL, 0, NULL) != 0){
		return(FALSE);
	}
	if(mciSendStringW(L"play MUSIC from 0", NULL, 0, NULL) != 0){
		return(FALSE);
	}
	return TRUE;
}

BOOL WINAPI ConsoleEventHandler(DWORD event){
    LPCWSTR msg;
    switch(event){
        case CTRL_C_EVENT: msg = L"Ctrl + C"; break;
        case CTRL_BREAK_EVENT: msg = L"Ctrl + BREAK"; break;
        case CTRL_CLOSE_EVENT: msg = L"Closing prog..."; break;
        case CTRL_LOGOFF_EVENT: case CTRL_SHUTDOWN_EVENT: msg = L"User is logging off..."; break;
        default: msg = L"???";
    }
    MessageBoxW(NULL, msg, L"Msg from 193541T", MB_OK);
    return TRUE;
}

int main(){
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
	if(!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleEventHandler, TRUE)){
		printf("Failed to install console event handler!\n");
		return -1;
	}
	PlayMidi("Audio/Music.mid");

	App* pApp = App::GetInstance();
	if(!pApp->Init()){
		return false;
	}
	pApp->Run();
	pApp->Destroy();
	pApp = nullptr;
}