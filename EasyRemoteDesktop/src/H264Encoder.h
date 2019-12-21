#pragma once

#include "EasySingleton.h"
#include "x264/x264.h"

#include <mutex>

class H264Encoder
{
public:
    H264Encoder();
    ~H264Encoder();

    bool OpenEncoder();
    void CloseEncoder();

    /**
    * 编码一帧图像
    * pRawData：原始图像数据
    * pEncodeData: 编码后的数据
    * width: 原始图片宽度
    * height： 原始图片高度
    */
    void Encode(uint8_t* pRawData, uint8_t* pEncodeData, const int width, const int height);

    void SetMaxBandWidth(const int bps = 0);
private:
    int m_maxBandWidthBps = 1024;  //最大带宽
    int m_frameWidth = 1280;     //默认720p
    int m_frameHeight = 720;
    int m_imageWidth = 0;
    int m_imageHeight = 0;
    std::mutex m_mutex;
    x264_t *m_x264Encoder = nullptr;
    x264_picture_t m_picIn;
    x264_picture_t m_picOut;
};

typedef EasySingleton<H264Encoder> H264EncoderSingleton;

