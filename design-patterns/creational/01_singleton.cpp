
#include <iostream>
class Singleton {
public:
    static Singleton& getInstance() {
        //静态局部变量的初始化是线程安全的
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

#include <thread>
#include <vector>

void worker() {
    std::cout << "Thread " << std::this_thread::get_id() 
              << " getting instance: " << &Singleton::getInstance() << "\n";
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    return 0;
}
