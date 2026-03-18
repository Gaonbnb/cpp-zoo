
// Pimpl 是头文件分离的“终极进化版”，专门用来解决普通分离方式解决不了的编译耦合和二进制兼容问题

// Logger.h


#pragma once
#include <string>
#include <memory>

// 前向声明实现类，避免包含其定义
// 这样即使 Impl 类的定义变了，这个头文件也不需要重新编译
class LoggerImpl; 

class Logger {
public:
    // 构造函数
    Logger();
    
    // 析构函数
    // 【重要】必须在 .cpp 中定义，因为此时需要知道 Impl 的完整类型来销毁 unique_ptr
    ~Logger();

    // 拷贝构造和赋值操作
    // 由于使用了 unique_ptr，默认生成的拷贝构造函数会报错，需要手动处理
    // 这里选择删除拷贝操作，仅允许移动（常见做法），或者实现深拷贝
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    Logger(Logger&&) noexcept;
    Logger& operator=(Logger&&) noexcept;

    // 公共接口 (抽象部分)
    void log_info(const std::string& message);
    void log_error(const std::string& message);
    void set_log_level(int level);

private:
    // 桥接指针：指向实现部分
    // 使用 unique_ptr 自动管理内存，无需在析构函数中手动 delete
    std::unique_ptr<LoggerImpl> pImpl;
};


// Logger.cpp

#include "Logger.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

// 1. 定义实现类 (Implementor)
// 这个类完全对头文件的使用者隐藏
// 这里可以包含任何沉重的头文件，比如 <windows.h>, <mysql/mysql.h> 等
class LoggerImpl {
public:
    int log_level;
    std::string log_file_path;
    std::ofstream file_stream;

    LoggerImpl() : log_level(1), log_file_path("app.log") {
        file_stream.open(log_file_path, std::ios::app);
        std::cout << "[Impl] Logger initialized, writing to: " << log_file_path << std::endl;
    }

    ~LoggerImpl() {
        if (file_stream.is_open()) {
            file_stream.close();
        }
        std::cout << "[Impl] Logger destroyed." << std::endl;
    }

    void write_log(const std::string& level_tag, const std::string& msg) {
        // 模拟获取时间戳
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << "[" << level_tag << "] " 
           << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %X") 
           << " - " << msg;
        
        std::string full_msg = ss.str();

        // 输出到控制台
        std::cout << full_msg << std::endl;
        
        // 输出到文件
        if (file_stream.is_open()) {
            file_stream << full_msg << std::endl;
            file_stream.flush();
        }
    }
};

// 2. 实现 Logger 类的成员函数

// 构造函数：初始化 pImpl
Logger::Logger() : pImpl(std::make_unique<LoggerImpl>()) {}

// 析构函数：默认即可，unique_ptr 会自动调用 LoggerImpl 的析构
// 但必须在 .cpp 中写出来，因为此时编译器看到了 LoggerImpl 的完整定义
Logger::~Logger() = default;

// 移动构造函数
Logger::Logger(Logger&& other) noexcept = default;

// 移动赋值运算符
Logger& Logger::operator=(Logger&& other) noexcept = default;

// 桥接方法：将调用转发给 pImpl
void Logger::log_info(const std::string& message) {
    if (pImpl) {
        pImpl->write_log("INFO", message);
    }
}

void Logger::log_error(const std::string& message) {
    if (pImpl) {
        pImpl->write_log("ERROR", message);
    }
}

void Logger::set_log_level(int level) {
    if (pImpl) {
        pImpl->log_level = level;
        std::cout << "[Adapter] Log level set to: " << level << std::endl;
    }
}


// main.cpp
#include "Logger.h"
#include <thread>
#include <chrono>

int main() {
    std::cout << "=== Starting Application ===" << std::endl;

    {
        // 创建 Logger 对象
        Logger logger;

        logger.log_info("Application started.");
        
        // 模拟一些工作
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        logger.log_error("Something went wrong! Connection failed.");
        
        logger.set_log_level(3);
        logger.log_info("Log level updated, continuing process...");
        
        // 当 logger 离开作用域时，unique_ptr 会自动删除 LoggerImpl 实例
    }

    std::cout << "=== Application Ended ===" << std::endl;
    return 0;
}
