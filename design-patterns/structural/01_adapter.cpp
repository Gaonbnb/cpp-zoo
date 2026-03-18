

#include <iostream>
#include <string>
#include <memory>

// 1. 目标接口 (Target)
// 这是客户端期望使用的接口
class IPayment {
public:
    virtual ~IPayment() = default;
    virtual void process_payment(double amount) = 0;
};

// 2. 被适配者 (Adaptee)
// 这是已经存在的、接口不兼容的类（例如旧的第三方库或遗留代码）
class OldPaymentSystem {
public:
    void pay_with_card(double amount) {
        std::cout << "[Old System] Processing card payment of $" << amount << std::endl;
        // 模拟旧系统的复杂逻辑...
        if (amount > 10000) {
            std::cout << "[Old System] Warning: Large transaction requires manual review." << std::endl;
        }
    }
    
    void pay_with_cash(double amount) {
        std::cout << "[Old System] Processing cash payment of $" << amount << std::endl;
    }
};

// 3. 适配器 (Adapter)
// 它实现了目标接口，并持有被适配者的实例
class PaymentAdapter : public IPayment {
private:
    std::shared_ptr<OldPaymentSystem> oldSystem; // 使用智能指针管理生命周期

public:
    // 构造函数，注入被适配的对象
    explicit PaymentAdapter(std::shared_ptr<OldPaymentSystem> system) 
        : oldSystem(system) {}

    // 实现目标接口的方法
    void process_payment(double amount) override {
        std::cout << "[Adapter] Converting request to old system format..." << std::endl;
        
        // 在这里进行必要的转换逻辑，然后调用被适配者的方法
        // 假设新系统统一用卡支付，或者根据业务逻辑决定调用哪个旧方法
        if (oldSystem) {
            oldSystem->pay_with_card(amount);
        } else {
            std::cerr << "[Adapter] Error: Old payment system is not available." << std::endl;
        }
    }
    
    // 适配器也可以扩展新功能，或者暴露被适配者的其他特定方法（如果必要）
    void process_cash_payment(double amount) {
        if (oldSystem) {
            oldSystem->pay_with_cash(amount);
        }
    }
};

// 客户端代码 (Client)
// 客户端只依赖目标接口，不需要知道适配器的存在
void execute_payment(std::shared_ptr<IPayment> payment, double amount) {
    std::cout << "--- Client initiating payment ---" << std::endl;
    payment->process_payment(amount);
    std::cout << "--- Payment complete ---\n" << std::endl;
}

int main() {
    // 创建被适配者（旧系统实例）
    auto oldSystem = std::make_shared<OldPaymentSystem>();

    // 创建适配器，将旧系统包装起来
    auto adapter = std::make_shared<PaymentAdapter>(oldSystem);

    // 客户端现在可以使用统一的接口进行支付
    // 即使底层使用的是旧系统，客户端也感知不到
    execute_payment(adapter, 500.00);
    execute_payment(adapter, 15000.00);

    // 如果适配器暴露了额外方法，也可以直接调用（但这打破了多态性，视情况而定）
    // static_cast<PaymentAdapter*>(adapter.get())->process_cash_payment(200.0);

    return 0;
}
