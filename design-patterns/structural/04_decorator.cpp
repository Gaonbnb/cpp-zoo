

#include <iostream>
#include <string>
#include <memory>

// 抽象组件：咖啡
class Coffee {
public:
    virtual ~Coffee() = default;
    
    // 获取描述
    virtual std::string getDescription() const = 0;
    
    // 获取价格
    virtual double getCost() const = 0;
};

// 具体组件：美式咖啡
class Americano : public Coffee {
public:
    std::string getDescription() const override {
        return "Americano";
    }

    double getCost() const override {
        return 5.0;
    }
};

// 抽象装饰器
class CoffeeDecorator : public Coffee {
protected:
    std::shared_ptr<Coffee> wrappedCoffee; // 持有被装饰的对象

public:
    explicit CoffeeDecorator(std::shared_ptr<Coffee> coffee) 
        : wrappedCoffee(coffee) {}

    // 默认行为：直接转发给被装饰对象
    std::string getDescription() const override {
        return wrappedCoffee->getDescription();
    }

    double getCost() const override {
        return wrappedCoffee->getCost();
    }
};

// 具体装饰器：加牛奶
class MilkDecorator : public CoffeeDecorator {
public:
    MilkDecorator(std::shared_ptr<Coffee> coffee) 
        : CoffeeDecorator(coffee) {}

    std::string getDescription() const override {
        // 在被装饰对象的描述后追加 ", Milk"
        return wrappedCoffee->getDescription() + ", Milk";
    }

    double getCost() const override {
        // 在被装饰对象的价格上 + 2.0
        return wrappedCoffee->getCost() + 2.0;
    }
};

// 具体装饰器：加摩卡
class MochaDecorator : public CoffeeDecorator {
public:
    MochaDecorator(std::shared_ptr<Coffee> coffee) 
        : CoffeeDecorator(coffee) {}

    std::string getDescription() const override {
        return wrappedCoffee->getDescription() + ", Mocha";
    }

    double getCost() const override {
        return wrappedCoffee->getCost() + 3.0;
    }
};

// 具体装饰器：加奶油
class WhipDecorator : public CoffeeDecorator {
public:
    WhipDecorator(std::shared_ptr<Coffee> coffee) 
        : CoffeeDecorator(coffee) {}

    std::string getDescription() const override {
        return wrappedCoffee->getDescription() + ", Whip";
    }

    double getCost() const override {
        return wrappedCoffee->getCost() + 1.5;
    }
};



void printOrder(const std::shared_ptr<Coffee>& coffee) {
    std::cout << "Order: " << coffee->getDescription() << std::endl;
    std::cout << "Total Cost: $" << coffee->getCost() << std::endl;
    std::cout << "--------------------------" << std::endl;
}

int main() {
    // 1. 点一杯普通美式
    auto order1 = std::make_shared<Americano>();
    printOrder(order1);
    // 输出: Americano, $5.0

    // 2. 点一杯美式加牛奶
    // 将 Americano 包裹在 MilkDecorator 中
    auto order2 = std::make_shared<MilkDecorator>(std::make_shared<Americano>());
    printOrder(order2);
    // 输出: Americano, Milk, $7.0

    // 3. 点一杯超级豪华咖啡：美式 + 牛奶 + 摩卡 + 奶油
    // 层层包裹：Americano -> Milk -> Mocha -> Whip
    std::shared_ptr<Coffee> order3 = std::make_shared<Americano>();
    order3 = std::make_shared<MilkDecorator>(order3);      // 加奶
    order3 = std::make_shared<MochaDecorator>(order3);     // 加摩卡
    order3 = std::make_shared<WhipDecorator>(order3);      // 加奶油
    
    printOrder(order3);
    // 输出: Americano, Milk, Mocha, Whip, $11.5

    // 4. 换个顺序：摩卡 + 美式 + 奶油 (逻辑上一样，只是描述顺序不同)
    auto order4 = std::make_shared<WhipDecorator>(
                      std::make_shared<MochaDecorator>(
                          std::make_shared<Americano>()
                      )
                  );
    printOrder(order4);
    // 输出: Americano, Mocha, Whip, $9.5

    return 0;
}
