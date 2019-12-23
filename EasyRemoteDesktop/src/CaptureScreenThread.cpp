#include "CaptureScreenThread.h"

#include "glog/logging.h"
#include "EasyScreenCapturer/EasyScreenCapturer.h"
#include "EasyScreenCapturer/CaptureStatusCode.h"

#include "Utils.h"
#include "MetricsClient.h"
#include "H264Encoder.h"

#include <thread>

#pragma comment(lib, "EasyScreenCapture.lib")

CaptureScreenThread::CaptureScreenThread()
{
    H264EncoderSingleton::GetInstance()->OpenEncoder();
}


CaptureScreenThread::~CaptureScreenThread()
{
    if (m_thread && m_thread->joinable()) {
        m_thread->join();
    }

    H264EncoderSingleton::GetInstance()->CloseEncoder();
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
    std::lock_guard<std::mutex> lock(m_mutex);
    m_fps = (fps == 0) ? DEFAULT_CAPTURE_FPS : fps;
    //抓屏间隔
    m_sampleInterval = 1000 / m_fps;
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
                m_pLastFrame = frame;
                emit onGetNextScreenFrame();
            }
            else if (res == media::StatusCode::CAPTURE_D3D_FRAME_NOCHANGE) {
                emit onGetNextScreenFrame();
            }
            else {
                LOG(ERROR) << "Capture screen failed: " << res;
            }

            // H264编码
            if (m_pLastFrame)
            {
                H264EncoderSingleton::GetInstance()->Encode(m_pLastFrame, m_fps);
            }
        }
        
        // 帧率统计
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
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_pLastFrame;
}