#include "H264Encoder.h"

#include <stdio.h>
#include "glog/logging.h"
#include "libyuv/convert.h"
#include "EasyScreenCapturer/EasyScreenCapturer.h"
#include "Utils.h"

#pragma comment(lib, "libx264.lib")
#pragma comment(lib, "yuv.lib")

void H264Log(void *file, int level, const char *psz, va_list)
{
    LOG(ERROR) << "x264 log: " <<file << ", " << level << ", " << psz;
}

H264Encoder::H264Encoder()
{
    x264_picture_init(&m_picIn);
    memset(&m_picIn, 0, sizeof(x264_picture_t));
    memset(&m_picOut, 0, sizeof(x264_picture_t));
    std::thread(std::bind(&H264Encoder::DoEncode, this)).detach();
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
        param.rc.i_bitrate = m_maxBandWidthBps * 0.6;   //平均码率
        param.rc.i_vbv_max_bitrate = m_maxBandWidthBps * 0.8; //最大码率

        //重新打开编码器
        //x264_encoder_close(m_x264Encoder);
        //m_x264Encoder = x264_encoder_open(&param);
        x264_encoder_reconfig(m_x264Encoder, &param);
    }
}

void H264Encoder::SetFps(const int fps)
{
    if (fps <= 0)
    {
        return;
    }

    x264_param_t param;
    if (m_x264Encoder)
    {
        //更新编码器参数, 2s一个关键帧
        m_fps = fps;
        x264_encoder_parameters(m_x264Encoder, &param);
        param.i_fps_num = fps;
        param.i_fps_den = 1;
        param.i_keyint_max = fps * 2;

        //重新打开编码器
        //x264_encoder_close(m_x264Encoder);
        //m_x264Encoder = x264_encoder_open(&param);
        x264_encoder_reconfig(m_x264Encoder, &param);
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
    param.i_bframe = 3;
    param.i_frame_reference = 3;
    param.i_csp = X264_CSP_I420;

    param.i_width = m_frameWidth;
    param.i_height = m_frameHeight;

    //设置最大码率和平均码率
    param.rc.i_rc_method = X264_RC_ABR;
    param.rc.i_bitrate = m_maxBandWidthBps * 0.6;   //平均码率
    param.rc.i_vbv_max_bitrate = m_maxBandWidthBps * 0.8; //最大码率
    param.rc.i_vbv_buffer_size = kMaxBandWitdhBps;

    param.i_log_level = X264_LOG_ERROR;
    param.pf_log = H264Log;
    param.i_csp = X264_CSP_I420;

    //param.i_fps_num = 15;
    //param.i_fps_den = 1;
    x264_param_apply_profile(&param, "baseline");
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

void H264Encoder::Encode(std::shared_ptr<media::CaptureBmpData> frame, const size_t fps)
{
    std::lock_guard<std::mutex> lock(m_queueMutex);
    std::shared_ptr<FrameData> data = std::make_shared<FrameData>();
    data->fps = fps;
    data->frame = frame;
    m_encodeQueue.push(data);
    m_encodeCv.notify_one();
}

void H264Encoder::DoEncode()
{
    int cnt = 0;
    uint64_t startTs = 0, endTs = 0, totalCostMs = 0, lastIncreaseTs = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(m_queueMutex);

        if (m_encodeQueue.empty())
        {
            m_encodeCv.wait(lock);
        }

        if (m_encodeQueue.empty())
        {
            continue;
        }
        
        //最多保留10帧,防止占用内存过大
        while (m_encodeQueue.size() > 10)
        {
            m_encodeQueue.pop();
        }
        auto frameData = m_encodeQueue.front();
        m_encodeQueue.pop();
        auto queueSize = m_encodeQueue.size();
        lock.unlock();

        if (!frameData || !frameData->frame)
        {
            continue;
        }

        {
            std::lock_guard<std::mutex> syncLock(m_mutex);
            startTs = GetTimestampMs();
            if (frameData->fps != m_fps)
            {
                //更新帧率配置
                SetFps(frameData->fps);
            }
            int width = abs(frameData->frame->m_headerInfo.biWidth);
            int height = abs(frameData->frame->m_headerInfo.biHeight);
            uint8_t* pRawData = frameData->frame->m_pixels.get();
            //memset(&m_picIn, 0, sizeof(x264_picture_t));
            //memset(&m_picOut, 0, sizeof(x264_picture_t));

            //picIn.i_type = X264_TYPE_IDR;
            m_picIn.img.i_csp = X264_CSP_I420;
            m_picIn.img.i_plane = 1;
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

            //需要先将ARGB数据转换伟YUV420
            //libyuv::ARGBToI420
            //libyuv::BGRAToI420
            int res = libyuv::BGRAToI420(pRawData, width * 4, m_picIn.img.plane[0], width, m_picIn.img.plane[1], (width + 1) / 2, m_picIn.img.plane[2], (width + 1) / 2, width, height);

            //编码
            if (res == 0)
            {
                int frame_size = x264_encoder_encode(m_x264Encoder, &nal, &i_nal, &m_picIn, &m_picOut);
                //LOG(INFO) << "H264 encode: " << i_nal << ", " << frame_size;
                if (i_nal > 0)
                {

                }
            }
            else {
                LOG(INFO) << "----------------------------------------------------2--------------";
            }
            //x264_picture_clean(&picIn);
            //x264_picture_clean(&picOut);
        }

        //自动调整码率
        endTs = GetTimestampMs();
        totalCostMs += endTs - startTs;
        cnt++;
        //每50帧进行码率调整
        if (cnt == 50)
        {
            auto perCost = totalCostMs / cnt;
            auto expectCost = 1000 / m_fps;
            if (perCost > expectCost - 5)
            {
                // 应适当减小码率
                auto bitrate = m_maxBandWidthBps > 256 ? max(m_maxBandWidthBps * 0.5, 128) : max(m_maxBandWidthBps * 0.75, 128);
                SetMaxBandWidth(bitrate);
                LOG(INFO) << "Reduce bitrate in x264encoder: " << m_maxBandWidthBps << "bps";
            }
            else if((perCost < expectCost - 10) && m_maxBandWidthBps < kMaxBandWitdhBps * 0.8 && queueSize <=1 ){
                // 适当增加码率
                auto bitrate = m_maxBandWidthBps <= 128 ? m_maxBandWidthBps * 2 : m_maxBandWidthBps + (kMaxBandWitdhBps - m_maxBandWidthBps) * 0.1;
                SetMaxBandWidth(bitrate);
                LOG(INFO) << "Increase bitrate in x264encoder: " << m_maxBandWidthBps << "bps";
                lastIncreaseTs = endTs;
            }
            cnt = 0;
            totalCostMs = 0;
        }
    }
}