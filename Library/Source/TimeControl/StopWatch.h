#pragma once
#include <windows.h>

class CStopWatch final{
	LARGE_INTEGER frequency;
	LARGE_INTEGER prevTime, currTime;
	UINT wTimerRes;
	double LIToSecs(LARGE_INTEGER& L); //Convert from Large Int to seconds in double
public:
	CStopWatch();
	~CStopWatch();
	void StartTimer();
	void StopTimer();
	double GetElapsedTime(); //Get elapsed time in seconds since the last call to this function
	void WaitUntil(const long long llTime);
};