#include "H264Encoder.h"

#include <stdio.h>
#include "glog/logging.h"
#include "libyuv/convert.h"

#pragma comment(lib, "libx264.lib")
#pragma comment(lib, "yuv.lib")

const int kMaxBandWitdhBps = 1024;  // 1Mb/s

void H264Log(void *file, int level, const char *psz, va_list)
{
    //LOG(INFO) << "x264 log: " <<file << ", " << level << ", " << psz;
}

H264Encoder::H264Encoder()
{
    x264_picture_init(&m_picIn);
    memset(&m_picIn, 0, sizeof(x264_picture_t));
    memset(&m_picOut, 0, sizeof(x264_picture_t));
}


H264Encoder::~H264Encoder()
{
}

void H264Encoder::SetMaxBandWidth(const int bps)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (bps == 0)
    {
        m_maxBandWidthBps = kMaxBandWitdhBps;
    }
    else
    {
        m_maxBandWidthBps = bps;
    }

    x264_param_t param;
    if (m_x264Encoder)
    {
        //更新编码器参数
        x264_encoder_parameters(m_x264Encoder, &param);
        param.rc.i_bitrate = m_maxBandWidthBps * 0.5;   //平均码率
        param.rc.i_vbv_max_bitrate = m_maxBandWidthBps; //最大码率
        param.rc.i_vbv_buffer_size = param.rc.i_bitrate;

        //重新打开编码器
        x264_encoder_close(m_x264Encoder);
        m_x264Encoder = x264_encoder_open(&param);
    }
}


bool H264Encoder::OpenEncoder()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    x264_param_t param;
    //避免编码延迟
    x264_param_default_preset(&param, "fast", "zerolatency");
    param.b_repeat_headers = 1;  // 在关键帧前面放置SPS/PPS
    //param.b_cabac = 1;
    param.i_threads = 1;    //为了实时性必须使用单线程
    //param.i_bframe = 3;
    //param.i_frame_reference = 3;
    //param.i_csp = X264_CSP_I420;

    param.i_width = m_frameWidth;
    param.i_height = m_frameHeight;

    //设置最大码率和平均码率
    param.rc.i_rc_method = X264_RC_ABR;
    param.rc.i_bitrate = m_maxBandWidthBps * 0.5;   //平均码率
    param.rc.i_vbv_max_bitrate = m_maxBandWidthBps; //最大码率
    param.rc.i_vbv_buffer_size = param.rc.i_bitrate;

    param.i_log_level = X264_LOG_DEBUG;
    param.pf_log = H264Log;
    param.i_csp = X264_CSP_I420;

    //param.i_fps_num = 15;
    //param.i_fps_den = 1;
    x264_param_apply_profile(&param, "main");
    m_x264Encoder = x264_encoder_open(&param);

    if (m_x264Encoder)
    {
        LOG(INFO) << "Success to init h264 encoder";
    }
    else
    {
        LOG(ERROR) << "Failed to init h264 encoder";
    }

    return m_x264Encoder != NULL;
}

void H264Encoder::CloseEncoder()
{
    x264_encoder_close(m_x264Encoder);
    m_x264Encoder = NULL;
}

void H264Encoder::Encode(uint8_t* pRawData, uint8_t* pEncodeData, const int width, const int height)
{
    //LOG(INFO) << "----------------------------------------------------1--------------";
    std::unique_lock<std::mutex> lock(m_mutex);
    //memset(&m_picIn, 0, sizeof(x264_picture_t));
    //memset(&m_picOut, 0, sizeof(x264_picture_t));

    //picIn.i_type = X264_TYPE_IDR;
    m_picIn.img.i_csp = X264_CSP_I420;
    m_picIn.img.i_plane = 1;
    //picIn.img.plane[0] = pRawData;
    m_picIn.img.i_stride[0] = width; //平面步长（字节）
    m_picIn.img.i_stride[1] = ((width + 1) / 2);
    m_picIn.img.i_stride[2] = ((width + 1) / 2);

    if (m_imageWidth != width || m_imageHeight != height)
    {
        //回收空间
        for (int i = 0; i < 3; i++) {
            if (m_picIn.img.plane[i])
            {
                delete[] m_picIn.img.plane[i];
                m_picIn.img.plane[i] = nullptr;
            }

            m_picIn.img.plane[i] = new uint8_t[m_picIn.img.i_stride[i] * height];
        }

        m_imageWidth = width;
        m_imageHeight = height;
    }

    for (int i = 0; i < 3; i++) {
        memset(m_picIn.img.plane[i], 0, m_picIn.img.i_stride[i] * height);
    }

    //x264_picture_alloc(&m_picIn, X264_CSP_I420, width, height);

    //是否是i帧
    int i_frame;
    x264_nal_t *nal = NULL;
    int i_nal;
    
    //char* buf = (char*)pRawData;
    //需要先将ARGB数据转换伟YUV420
    //libyuv::ARGBToI420
    //libyuv::BGRAToI420
    int res = libyuv::BGRAToI420(pRawData, width * 4, m_picIn.img.plane[0], width, m_picIn.img.plane[1], (width + 1) / 2, m_picIn.img.plane[2], (width + 1) / 2, width, height);

    //编码
    if (res == 0)
    {
        int frame_size = x264_encoder_encode(m_x264Encoder, &nal, &i_nal, &m_picIn, &m_picOut);
        //LOG(INFO) << "H264 encode: " << i_nal << ", " << frame_size;
    }
    //LOG(INFO) << "----------------------------------------------------2--------------";
    //x264_picture_clean(&picIn);
    //x264_picture_clean(&picOut);
}