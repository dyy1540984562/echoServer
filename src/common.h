#pragma once

#include <glog/logging.h>
#include <thread>
#include <sstream>
#include <iomanip>


// 自定义日志格式
class CompactLogMessage : public google::LogMessage {
public:
    CompactLogMessage(const char* file, int line, google::LogSeverity severity)
        : google::LogMessage(file, line, severity) {}

    // 重写 stream() 方法，返回 google::LogMessage 的 stream
    std::ostream& stream() { return google::LogMessage::stream(); }
};

#define LOG_INFO(message) COMPACT_GOOGLE_LOG_INFO.stream() \
    << "[T:" << std::this_thread::get_id() << "] " \
    << "[" << __FILE__ << ":" << __LINE__ << "] " << message

#define LOG_WARNING(message) COMPACT_GOOGLE_LOG_WARNING.stream() \
    << "[T:" << std::this_thread::get_id() << "] " \
    << "[" << __FILE__ << ":" << __LINE__ << "] " << message

#define LOG_ERROR(message) COMPACT_GOOGLE_LOG_ERROR.stream() \
    << "[T:" << std::this_thread::get_id() << "] " \
    << "[" << __FILE__ << ":" << __LINE__ << "] " << message

#define COMPACT_GOOGLE_LOG_INFO CompactLogMessage(__FILE__, __LINE__, google::INFO)
#define COMPACT_GOOGLE_LOG_WARNING CompactLogMessage(__FILE__, __LINE__, google::WARNING)
#define COMPACT_GOOGLE_LOG_ERROR CompactLogMessage(__FILE__, __LINE__, google::ERROR)