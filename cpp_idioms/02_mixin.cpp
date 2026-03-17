#include <iostream>
#include <string>
// --- Mixin 1: 计数功能 (CountingMixin) ---
// 它不继承任何特定类，它是一个模板，等待派生类传入自己
template <class T>
class CountingMixin {
public:
    CountingMixin() {
        ++count;
        std::cout << "[Count] Object created. Total: " << count << std::endl;
    }
    
    ~CountingMixin() {
        --count;
    }

    static int getCount() {
        return count;
    }
private:
    static int count;
};
// 静态成员初始化
// 类内的 static 成员变量（如你代码中的 count）： 必须在类外进行显式初始化（除非是 C++17 及以后的 inline static 或某些特定常量）。
// 函数内的 static 局部变量： 不需要显式初始化，编译器会自动初始化为 0。
// 全局 static 变量： 不需要显式初始化，编译器会自动初始化为 0。
template <class T>
int CountingMixin<T>::count = 0;

template <typename Derived>
class LoggingMixin {
public:
    LoggingMixin() {
        std::cout << "[Log] Constructing a " << typeid(Derived).name() << " object." << std::endl;
    }
    
    void log(const std::string& msg) {
        std::cout << "[Log] " << msg << std::endl;
    }
};
// 业务类：Person
// 它“混入”了计数和日志功能
class Person : public CountingMixin<Person>, 
               public LoggingMixin<Person> {
public:
    Person(const std::string& name) : name_(name) {
        // 可以直接调用 Mixin 提供的 log 函数
        this->log("Person named " + name_ + " is being initialized.");
    }

    void introduce() {
        std::cout << "Hello, I am " << name_ << "." << std::endl;
        // 也可以调用静态计数函数
        std::cout << "Total people alive: " << this->getCount() << std::endl;
    }

private:
    std::string name_;
};


int main() {
    Person p1("Alice");
    Person p2("Bob");
    
    p1.introduce();
    
    std::cout << "Current total count via Mixin: " << Person::getCount() << std::endl;
    
    return 0;
}

// 利用 C++ 的多重继承和模板机制，将“功能”像插件一样“混入”到主类中的一种高级设计技巧。它是解决 C++ 多重继承菱形问题和代码复用问题的最佳实践之一
