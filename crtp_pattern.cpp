#include <iostream>


template <typename T>
class Base {
public:
    // 普通成员函数
    void func(int x) {
        static_cast<T*>(this)->derived_func(x);
    }
};

class Derived : public Base<Derived> {
public:
    // 定义同名函数（参数不同）
    void derived_func(int x) {
        // 调用基类的 func(int) 
        std::cout << "Derived::func(int): " << x << std::endl;
    }
};

class Derived2 : public Base<Derived2> {
public:
    // 定义同名函数（参数不同）
    inline void derived_func(int x) {
        // 调用基类的 func(int) 
        std::cout << "Derived2::func(int): " << x << std::endl;
    }
};

int main() {
    Derived d;
    Derived2 d2;
    d.func(14);  // 输出: Derived::func(int): 14   
    d2.func(15);  // 输出: Derived2::func(int): 15     
    return 0;
}
