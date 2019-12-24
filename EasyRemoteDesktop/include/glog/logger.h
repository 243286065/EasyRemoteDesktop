#ifndef GLOG_LOGGER_H_
#define GLOG_LOGGER_H_

#ifndef LOG_DLL_DECL
#if defined(_WIN32) && !defined(__CYGWIN__)
#define LOG_DLL_DECL __declspec(dllimport)
#else
#define LOG_DLL_DECL
#endif
#endif

#include "glog/logging.h"
#include "glog/log_severity.h"

#include <string>

/**
 * Init log system
 * Each process needs to be initialized once and the file name cannot be the same
 * 
 * @ logName: log file name, if empty, it is the process name
 * @ logPath: the path to save log file, it 
 */ 
LOG_DLL_DECL int InitLogger(
    const std::string& logName = "",
    const std::string& logPath = "",
    const google::LogSeverity minSeverity = google::GLOG_INFO,
    const unsigned int fileMaxSize = 10);

LOG_DLL_DECL void ReleaseLogger();
#endif // GLOG_LOGGER_H_
