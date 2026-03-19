#include <iostream>
#include <string>
#include <memory>
#include <vector>

// ==========================================
// 1. 抽象产品 (Abstract Product)
// ==========================================
class Logger {
public:
    virtual ~Logger() = default;
    
    // 核心业务方法
    virtual void log(const std::string& message) = 0;
    
    // 辅助方法：判断是否是空对象（可选，用于调试）
    virtual bool isNull() const { return false; }
};

// ==========================================
// 2. 具体产品：真实日志器 (Concrete Product)
// ==========================================
class FileLogger : public Logger {
private:
    std::string filename;

public:
    explicit FileLogger(const std::string& file) : filename(file) {}

    void log(const std::string& message) override {
        // 模拟写文件操作
        std::cout << "[FileLogger -> " << filename << "] LOG: " << message << std::endl;
        // 实际代码这里会打开文件并写入
    }
};

// ==========================================
// 3. 空对象 (Null Object)
// ==========================================
class NullLogger : public Logger {
public:
    // 关键点：方法体为空，什么都不做
    void log(const std::string& message) override {
        //  intentionally empty - 沉默是金
        // 不需要打印，不需要写文件，不需要抛异常
    }

    // 标记这是一个空对象
    bool isNull() const override {
        return true;
    }
};

// ==========================================
// 4. 工厂/提供者 (Factory/Provider)
// ==========================================
// 负责根据配置决定返回真实对象还是空对象
class LoggerFactory {
public:
    static std::shared_ptr<Logger> createLogger(bool enableLogging, const std::string& filename) {
        if (enableLogging) {
            return std::make_shared<FileLogger>(filename);
        } else {
            // 返回单例的空对象，避免重复创建
            // 注意：这里为了演示简单每次都 new，生产环境建议用静态单例
            return std::make_shared<NullLogger>();
        }
    }
    
    // 获取全局唯一的空对象实例 (单例模式结合)
    static std::shared_ptr<Logger> getNullLogger() {
        static auto nullLogger = std::make_shared<NullLogger>();
        return nullLogger;
    }
};

// ==========================================
// 5. 客户端业务逻辑 (Client Business Logic)
// ==========================================
class Application {
private:
    std::shared_ptr<Logger> logger;

public:
    explicit Application(std::shared_ptr<Logger> log) : logger(log) {}

    void runProcess(const std::string& step) {
        std::cout << "\n--- Processing: " << step << " ---" << std::endl;
        
        // 【关键点】这里不需要写 if (logger != nullptr)
        // 无论 logger 是 FileLogger 还是 NullLogger，代码都能安全运行
        logger->log("Starting process: " + step);
        
        // 模拟一些耗时操作
        // ...
        
        logger->log("Finished process: " + step);
        
        // 即使 logger 是 NullLogger，下面的代码也不会崩溃，只是没输出而已
        logger->log("Checkpoint reached.");
    }
    
    void checkLoggerType() {
        if (logger->isNull()) {
            std::cout << "[System] Warning: Logging is disabled (NullLogger active)." << std::endl;
        } else {
            std::cout << "[System] Info: Logging is enabled." << std::endl;
        }
    }
};

// ==========================================
// 6. 主函数测试 (Main)
// ==========================================
int main() {
    std::cout << "=== C++ Null Object Pattern Demo ===" << std::endl;

    // 场景 A: 日志功能开启
    std::cout << "\n>>> Scenario A: Logging Enabled" << std::endl;
    auto realLogger = LoggerFactory::createLogger(true, "app.log");
    Application appA(realLogger);
    appA.checkLoggerType();
    appA.runProcess("User Login");
    appA.runProcess("Data Sync");

    // 场景 B: 日志功能关闭 (传统方式需要大量 if 判断，这里不需要)
    std::cout << "\n>>> Scenario B: Logging Disabled" << std::endl;
    
    // 方式 1: 通过工厂获取空对象
    auto nullLogger = LoggerFactory::createLogger(false, "");
    
    // 方式 2: 或者直接获取全局单例空对象 (推荐)
    // auto nullLogger = LoggerFactory::getNullLogger();

    Application appB(nullLogger);
    appB.checkLoggerType();
    
    // 注意看输出：这里不会有任何 "[FileLogger...]" 的输出，也不会崩溃
    appB.runProcess("User Login");
    appB.runProcess("Data Sync");
    
    std::cout << "\n>>> Scenario C: Direct Null Check Demonstration" << std::endl;
    // 如果我们需要区分行为（比如只在非空时做额外统计），可以使用 isNull()
    if (!nullLogger->isNull()) {
        std::cout << "Calculating log size..." << std::endl;
    } else {
        std::cout << "Skipping log size calculation because logger is null." << std::endl;
    }

    return 0;
}
