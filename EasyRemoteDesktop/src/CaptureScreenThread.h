#pragma once
#include <QThread>
class CaptureScreenThread :
    public QThread
{
    Q_OBJECT
signals:
    
public:
    CaptureScreenThread();
    ~CaptureScreenThread();

private:
    void StartCaptureScreen(const int fps);
    void StopCaptureScreen();
};

