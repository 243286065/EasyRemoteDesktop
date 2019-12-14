#include "MetricsClient.h"
#include "glog/logging.h"

void MetricsClient::Counter::Add()
{
    m_cntPeriod++;
    m_cntTotal++;
}

int64_t MetricsClient::Counter::GetPeriodCnt()
{
    return m_cntPeriod;
}

int64_t MetricsClient::Counter::GetTotalCnt()
{
    return m_cntTotal;
}

void MetricsClient::Counter::ResetPeriodCnt()
{
    m_cntPeriod = 0;
}

void MetricsClient::Counter::ResetTotalCnt()
{
    m_cntPeriod = 0;
    m_cntTotal = 0;
}

MetricsClient::MetricsClient()
{
    std::thread(std::bind(&MetricsClient::ResetCounterThread, this)).detach();
}


MetricsClient::~MetricsClient()
{

}

void MetricsClient::ResetCounterThread()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(m_period));
        std::lock_guard<std::mutex> lock(m_mutex);
        //获取统计数据
        LOG(INFO) << "Capture frame: " << m_fpsCounter.GetPeriodCnt() << ", total: " << m_fpsCounter.GetTotalCnt()<< ", fps: " << (double)m_fpsCounter.GetPeriodCnt() / (m_period);

        m_fpsCounter.ResetPeriodCnt();
    }
}

void MetricsClient::OnCaptureScreen()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_fpsCounter.Add();
}
