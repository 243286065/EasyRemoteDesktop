#include "CaptureScreenThread.h"



CaptureScreenThread::CaptureScreenThread()
{
}


CaptureScreenThread::~CaptureScreenThread()
{
}


void CaptureScreenThread::StartCaptureScreen(const int fps)
{
    if (fps <= 0) return;
    //抓屏间隔
    int interval = 1000 / fps;
}

void CaptureScreenThread::StopCaptureScreen()
{

}