#include "CaptureScreenThread.h"

#include "glog/logging.h"
#include "EasyScreenCapturer/EasyScreenCapturer.h"
#include "EasyScreenCapturer/CaptureStatusCode.h"

#include "Utils.h"
#include "MetricsClient.h"

#include <thread>

#pragma comment(lib, "EasyScreenCapture.lib")

CaptureScreenThread::CaptureScreenThread()
{
}


CaptureScreenThread::~CaptureScreenThread()
{
    if (m_thread && m_thread->joinable()) {
        m_thread->join();
    }
}


void CaptureScreenThread::StartCaptureScreen(const size_t fps)
{
    if (m_bRunning) {
        LOG(WARNING) << "The instance is already in running!";
        return;
    }

    SetFps(fps);
    m_bRunning = true;

    if (m_thread && m_thread->joinable()) {
        m_thread->join();
    }
    m_thread.reset(new std::thread(&CaptureScreenThread::DoCaptureScreen, this));
}


void CaptureScreenThread::StopCaptureScreen()
{
    m_bRunning = false;
}

void CaptureScreenThread::SetFps(const size_t fps) {
    size_t realFps = (fps == 0) ? DEFAULT_CAPTURE_FPS : fps;
    //抓屏间隔
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sampleInterval = 1000 / realFps;
}

void CaptureScreenThread::DoCaptureScreen()
{
    while (m_bRunning)
    {
        int64_t timestamp = GetTimestampMs();
        std::shared_ptr<media::CaptureBmpData> frame = std::make_shared<media::CaptureBmpData>();
        auto res = media::EasyScreenCapturer::GetInstance()->CaptureFullScreen(*frame);

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (res == media::StatusCode::CAPTURE_OK)
            {
                if (m_pLastFrame) {
                    media::EasyScreenCapturer::GetInstance()->FreeCaptureBmpData(*m_pLastFrame);
                }
                m_pLastFrame = frame;
                emit onGetNextScreenFrame();
            }
            else if (res == media::StatusCode::CAPTURE_D3D_FRAME_NOCHANGE) {
                media::EasyScreenCapturer::GetInstance()->FreeCaptureBmpData(*frame);
                //m_pLastFrame = frame;
                emit onGetNextScreenFrame();
            }
            else {
                LOG(ERROR) << "Capture screen failed: " << res;
            }
        }
        
        MetricsClientSingleton::GetInstance()->OnCaptureScreen();
        // 调整截屏间隔，尽量满足帧率设置
        int spent = GetTimestampMs() - timestamp;
        int interval = m_sampleInterval;
        if (spent > 0 && spent < m_sampleInterval)
        {
            interval = (int)(m_sampleInterval - spent);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
}

std::shared_ptr<media::CaptureBmpData> CaptureScreenThread::GetFrame()
{
    // 拷贝一个副本出来
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_pLastFrame) return m_pLastFrame;

    std::shared_ptr<media::CaptureBmpData> frame = std::make_shared<media::CaptureBmpData>();
    frame->m_free = false;
    frame->m_headerInfo = m_pLastFrame->m_headerInfo;
    frame->m_pixels = (uint8_t*)malloc(m_pLastFrame->m_headerInfo.biSizeImage);
    memcpy(frame->m_pixels, m_pLastFrame->m_pixels, m_pLastFrame->m_headerInfo.biSizeImage);

    return frame;
}