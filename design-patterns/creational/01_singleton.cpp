// Meyers' Singleton
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <memory>
#include <cassert>
class Singleton {
public:
    static Singleton& getInstance() {
        //静态局部变量的初始化是线程安全的
        //在多次调用时只会构造一次。这是 C++ 静态局部变量的核心特性
        static Singleton instance;
        return instance;
    }
private:
    
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    // 私有构造函数
    Singleton() {
        std::cout << "Singleton created\n";
    }
    ~Singleton() {
        std::cout << "Singleton destroyed\n";
    }
};

void worker() {
    std::cout << "Thread " << std::this_thread::get_id() 
              << " getting instance: " << &Singleton::getInstance() << "\n";
}

int main_mayer() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}

// version 1:Eager Initialization
class EagerSingleton {
public:
    // 全局访问点
    static EagerSingleton* getInstance() {
        return instance;
    }
    
    void doSomething() const {
        std::cout << "Doing something in thread " 
                  << std::this_thread::get_id() << "\n";
    }
    
    // 删除拷贝构造和赋值
    EagerSingleton(const EagerSingleton&) = delete;
    EagerSingleton& operator=(const EagerSingleton&) = delete;

private:
    // 私有构造函数
    EagerSingleton() {
        std::cout << "EagerSingleton 创建 (程序启动时)\n";
    }
    
    ~EagerSingleton() {
        std::cout << "EagerSingleton 销毁\n";
    }
    
    // 静态成员指针 - 程序启动时就初始化
    static EagerSingleton* instance;
};

// 在类外初始化静态成员 - 程序启动时就创建
EagerSingleton* EagerSingleton::instance = new EagerSingleton();

// 测试代码
void worker_eager() {
    EagerSingleton* s = EagerSingleton::getInstance();
    std::cout << "线程 " << std::this_thread::get_id() 
              << " 获取实例地址: " << s << "\n";
    s->doSomething();
}

int main_eager() {
    std::cout << "main 函数开始\n";
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(worker_eager);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "main 函数结束\n";
    // 问题：析构函数永远不会被调用！内存泄漏
    return 0;
}

// version 2: Lazy Initialization

class LazySingleton {
public:
    static LazySingleton* getInstance() {
        // 双重检查锁的简化版 - 非线程安全！
        if (instance == nullptr) {
            instance = new LazySingleton();  // 第一次使用时才创建
        }
        return instance;
    }
    
    void doSomething() const {
        std::cout << "Doing something in thread " 
                  << std::this_thread::get_id() << "\n";
    }
    
    // 手动释放（需要用户调用）
    static void destroyInstance() {
        delete instance;
        instance = nullptr;
    }
    
    LazySingleton(const LazySingleton&) = delete;
    LazySingleton& operator=(const LazySingleton&) = delete;

private:
    LazySingleton() {
        std::cout << "LazySingleton 创建 (首次使用时)\n";
    }
    
    ~LazySingleton() {
        std::cout << "LazySingleton 销毁\n";
    }
    
    static LazySingleton* instance;
};

// 初始化为 nullptr
LazySingleton* LazySingleton::instance = nullptr;

void worker_lazy() {
    LazySingleton* s = LazySingleton::getInstance();
    std::cout << "线程 " << std::this_thread::get_id() 
              << " 获取实例地址: " << s << "\n";
    s->doSomething();
}

int main_lazy() {
    std::cout << "main 函数开始\n";
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(worker_lazy);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "main 函数结束\n";
    
    // 必须手动释放
    LazySingleton::destroyInstance();  // 容易忘记调用
    return 0;
}


// version 2.2 safe Lazy Initialization
class ThreadSafeLazySingleton {
public:
    static ThreadSafeLazySingleton* getInstance() {
        // 双重检查锁 (Double-Checked Locking)
        if (instance == nullptr) {  // 第一次检查（无锁）
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr) {  // 第二次检查（有锁）
                instance = new ThreadSafeLazySingleton();
            }
        }
        return instance;
    }
    
    void doSomething() const {
        std::cout << "Doing something in thread " 
                  << std::this_thread::get_id() << "\n";
    }
    
    static void destroyInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        delete instance;
        instance = nullptr;
    }
    
    ThreadSafeLazySingleton(const ThreadSafeLazySingleton&) = delete;
    ThreadSafeLazySingleton& operator=(const ThreadSafeLazySingleton&) = delete;

private:
    ThreadSafeLazySingleton() {
        std::cout << "ThreadSafeLazySingleton 创建\n";
    }
    
    ~ThreadSafeLazySingleton() {
        std::cout << "ThreadSafeLazySingleton 销毁\n";
    }
    
    static ThreadSafeLazySingleton* instance;
    static std::mutex mutex;
};

ThreadSafeLazySingleton* ThreadSafeLazySingleton::instance = nullptr;
std::mutex ThreadSafeLazySingleton::mutex;

void worker_threadsafe() {
    ThreadSafeLazySingleton* s = ThreadSafeLazySingleton::getInstance();
    std::cout << "线程 " << std::this_thread::get_id() 
              << " 获取实例地址: " << s << "\n";
    s->doSomething();
}

int main_threadsafe_lazy() {
    std::cout << "main 函数开始\n";
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker_threadsafe);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "main 函数结束\n";
    
    ThreadSafeLazySingleton::destroyInstance();  // 必须手动释放
    return 0;
}


int main() {
    main_mayer();
    main_eager();
    main_lazy();
    main_threadsafe_lazy();
    return 0;
}
// mayers
// 有饿汉模式的简单性
// 有懒汉模式的延迟加载
// 线程安全
// 自动内存管理
