/**
* 参数统计类
*/
#pragma once
#include "EasySingleton.h"
#include <thread>
#include <mutex>

class MetricsClient
{
public:
    MetricsClient();
    ~MetricsClient();

    void OnCaptureScreen();

private:
    class Counter {
    public:
        Counter() {}
        ~Counter() {}
        void Add();
        int64_t GetPeriodCnt();
        int64_t GetTotalCnt();
        void ResetPeriodCnt();
        void ResetTotalCnt();
    private:
        int64_t m_cntPeriod = 0;
        int64_t m_cntTotal = 0;
    };

    void ResetCounterThread();

    Counter m_fpsCounter;
    int m_period = 10;  //统计周期10s
    std::mutex m_mutex;
};

typedef EasySingleton<MetricsClient> MetricsClientSingleton;

