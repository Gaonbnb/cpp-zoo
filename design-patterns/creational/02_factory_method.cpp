

#include <iostream>
#include <memory>
#include <vector>
// simple factory
// 产品基类
class Pizza {
public:
    virtual void prepare() = 0;
    virtual void bake() = 0;
    virtual void cut() = 0;
    virtual void box() = 0;
    virtual ~Pizza() = default;
};

// 具体产品：奶酪披萨
class CheesePizza : public Pizza {
public:
    void prepare() override {
        std::cout << "准备奶酪披萨：加入奶酪、面粉、番茄酱" << std::endl;
    }
    void bake() override {
        std::cout << "烘烤奶酪披萨：200度，15分钟" << std::endl;
    }
    void cut() override {
        std::cout << "切奶酪披萨：8块" << std::endl;
    }
    void box() override {
        std::cout << "包装奶酪披萨：用红色盒子" << std::endl;
    }
};

// 具体产品：意式腊肠披萨
class PepperoniPizza : public Pizza {
public:
    void prepare() override {
        std::cout << "准备腊肠披萨：加入腊肠、奶酪、面粉" << std::endl;
    }
    void bake() override {
        std::cout << "烘烤腊肠披萨：220度，18分钟" << std::endl;
    }
    void cut() override {
        std::cout << "切腊肠披萨：6块" << std::endl;
    }
    void box() override {
        std::cout << "包装腊肠披萨：用蓝色盒子" << std::endl;
    }
};

// 具体产品：海鲜披萨
class SeafoodPizza : public Pizza {
public:
    void prepare() override {
        std::cout << "准备海鲜披萨：加入虾仁、鱿鱼、奶酪" << std::endl;
    }
    void bake() override {
        std::cout << "烘烤海鲜披萨：180度，12分钟" << std::endl;
    }
    void cut() override {
        std::cout << "切海鲜披萨：8块" << std::endl;
    }
    void box() override {
        std::cout << "包装海鲜披萨：用银色盒子" << std::endl;
    }
};

// 简单工厂
class SimplePizzaFactory {
public:
    static std::unique_ptr<Pizza> createPizza(const std::string& type) {
        if (type == "cheese") {
            return std::make_unique<CheesePizza>();
        } else if (type == "pepperoni") {
            return std::make_unique<PepperoniPizza>();
        } else if (type == "seafood") {
            return std::make_unique<SeafoodPizza>();
        }
        return nullptr;
    }
};

// 披萨店
class PizzaStore {
public:
    void orderPizza(const std::string& type) {
        std::cout << "\n=== 顾客点了一份" << type << "披萨 ===" << std::endl;
        
        // 使用工厂创建披萨
        auto pizza = SimplePizzaFactory::createPizza(type);
        
        if (pizza) {
            pizza->prepare();
            pizza->bake();
            pizza->cut();
            pizza->box();
            std::cout << "披萨制作完成，交给顾客" << std::endl;
        } else {
            std::cout << "抱歉，我们不卖这种披萨" << std::endl;
        }
    }
};

// 测试
int main_simple_factory() {
    PizzaStore store;
    
    store.orderPizza("cheese");
    store.orderPizza("pepperoni");
    store.orderPizza("seafood");
    store.orderPizza("fruit");  // 不支持的披萨类型
    
    return 0;
}

//factory method

#include <iostream>
#include <memory>

// 产品基类
class Transport {
public:
    virtual void deliver() = 0;
    virtual ~Transport() = default;
};

// 具体产品：卡车
class Truck : public Transport {
public:
    void deliver() override {
        std::cout << "用卡车在陆地上运输货物" << std::endl;
    }
};

// 具体产品：轮船
class Ship : public Transport {
public:
    void deliver() override {
        std::cout << "用轮船在海面上运输货物" << std::endl;
    }
};

// 具体产品：飞机
class Plane : public Transport {
public:
    void deliver() override {
        std::cout << "用飞机在空中运输货物" << std::endl;
    }
};

// 抽象创建者
class Logistics {
public:
    // 工厂方法 - 子类实现
    virtual std::unique_ptr<Transport> createTransport() = 0;
    
    // 公共业务逻辑
    void planDelivery() {
        std::cout << "\n=== 开始规划运输 ===" << std::endl;
        auto transport = createTransport();
        transport->deliver();
        std::cout << "=== 运输完成 ===\n" << std::endl;
    }
    
    virtual ~Logistics() = default;
};

// 具体创建者：陆路物流
class RoadLogistics : public Logistics {
public:
    std::unique_ptr<Transport> createTransport() override {
        return std::make_unique<Truck>();
    }
};

// 具体创建者：海路物流
class SeaLogistics : public Logistics {
public:
    std::unique_ptr<Transport> createTransport() override {
        return std::make_unique<Ship>();
    }
};

// 具体创建者：航空物流
class AirLogistics : public Logistics {
public:
    std::unique_ptr<Transport> createTransport() override {
        return std::make_unique<Plane>();
    }
};

// 测试
void testLogistics(Logistics& logistics) {
    logistics.planDelivery();
}

int main_factory_method() {
    RoadLogistics roadLogistics;
    SeaLogistics seaLogistics;
    AirLogistics airLogistics;
    
    testLogistics(roadLogistics);
    testLogistics(seaLogistics);
    testLogistics(airLogistics);
    
    return 0;
}
