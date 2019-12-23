#pragma once
#include <QObject>
#include <atomic>
#include <thread>
#include <mutex>

#define DEFAULT_CAPTURE_FPS 15

namespace media {
struct CaptureBmpData;
}

class CaptureScreenThread : public QObject
{
    Q_OBJECT
signals:
    void onGetNextScreenFrame();

public:
    CaptureScreenThread();
    ~CaptureScreenThread();

    void StartCaptureScreen(const size_t fps);
    void StopCaptureScreen();

    void SetFps(const size_t fps);

    bool IsRunning() {
        return m_bRunning;
    }

    std::shared_ptr<media::CaptureBmpData> GetFrame();
private:
    void DoCaptureScreen();

    std::atomic_bool m_bRunning = false;
    int m_sampleInterval = 1000/ DEFAULT_CAPTURE_FPS;
    size_t m_fps = DEFAULT_CAPTURE_FPS;
    std::mutex m_mutex;
    std::unique_ptr<std::thread> m_thread;

    std::shared_ptr<media::CaptureBmpData> m_pLastFrame;
};

