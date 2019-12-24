#pragma once

#include "EasySingleton.h"
#include "x264/x264.h"

#include <mutex>
#include <queue>

const int kMaxBandWitdhBps = 1024;

namespace media {
    struct CaptureBmpData;
}

class H264Encoder
{
public:
    H264Encoder();
    ~H264Encoder();

    bool OpenEncoder();
    void CloseEncoder();

    void Encode(std::shared_ptr<media::CaptureBmpData> frame, const size_t fps);
    void SetMaxBandWidth(const int bps = 0);
private:
    struct FrameData {
        size_t fps;
        std::shared_ptr<media::CaptureBmpData> frame;
    };
    void DoEncode();
    void SetFps(const int fps);

    int m_maxBandWidthBps = kMaxBandWitdhBps / 2;  //最大带宽
    int m_frameWidth = 640;     //默认480p
    int m_frameHeight = 480;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    std::mutex m_mutex;         //编码器同步锁
    std::mutex m_queueMutex;    //数据队列锁
    std::condition_variable m_encodeCv;
    x264_t *m_x264Encoder = nullptr;
    x264_picture_t m_picIn;
    x264_picture_t m_picOut;
    size_t m_fps = 0;
    uint64_t m_lastModifyBitRate = 0;

    std::queue<std::shared_ptr<FrameData>> m_encodeQueue;
};

typedef EasySingleton<H264Encoder> H264EncoderSingleton;

