#include "StopWatch.h"
#define TARGET_RESOLUTION 1 // 1-millisecond target resolution

CStopWatch::CStopWatch(): currTime({0}), prevTime({0}){
	QueryPerformanceFrequency(&frequency);
	TIMECAPS tc;
	if(timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR){
		//Error, app can't continue
	}
	wTimerRes = min(max(tc.wPeriodMin, TARGET_RESOLUTION), tc.wPeriodMax);
	timeBeginPeriod(wTimerRes);
}

CStopWatch::~CStopWatch(){
	StopTimer();
}

void CStopWatch::StartTimer(){
	QueryPerformanceCounter(&prevTime);
}

void CStopWatch::StopTimer(){
	timeEndPeriod(wTimerRes);
}

double CStopWatch::GetElapsedTime(){
	LARGE_INTEGER time;
	QueryPerformanceCounter(&currTime);
	time.QuadPart = currTime.QuadPart - prevTime.QuadPart;
	prevTime = currTime;
	return LIToSecs(time);
}

void CStopWatch::WaitUntil(const long long llTime){
	LARGE_INTEGER nowTime;
	LONGLONG timeElapsed;
	while(true){
		QueryPerformanceCounter(&nowTime);
		timeElapsed = ((nowTime.QuadPart - prevTime.QuadPart) * 1000 / frequency.QuadPart);
		if(timeElapsed > llTime){
			return;
		} else if(llTime - timeElapsed > 1){
			Sleep(1);
		}
	}
}

double CStopWatch::LIToSecs(LARGE_INTEGER& L){ //@param L A LARGE_INTEGER & containing the time value
	return ((double)L.QuadPart / (double)frequency.QuadPart);
}