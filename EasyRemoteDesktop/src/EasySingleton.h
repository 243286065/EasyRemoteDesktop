/**
* µ¥ÀýÄ£°åÀà
*/

#pragma once

#include <memory>
#include <mutex>

template <typename T>
class EasySingleton
{
public:
    ~EasySingleton() {}
    static std::shared_ptr<T> GetInstance()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_pInstance)
        {
            m_pInstance.reset(new T());
        }

        return m_pInstance;
    }

private:
    EasySingleton() {}

    static std::shared_ptr<T> m_pInstance;
    static std::mutex m_mutex;
};

template <typename T>
std::shared_ptr<T> EasySingleton<T>::m_pInstance = nullptr;

template <typename T>
std::mutex EasySingleton<T>::m_mutex;

