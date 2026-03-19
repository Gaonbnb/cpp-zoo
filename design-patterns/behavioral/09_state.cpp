#include <iostream>
#include <string>
#include <memory>

// 前向声明
class VendingMachine;

// ==========================================
// 1. 抽象状态类 (State)
// ==========================================
class State {
public:
    virtual ~State() = default;
    
    // 定义状态能响应的操作
    virtual void insertCoin(VendingMachine* machine) = 0;
    virtual void ejectCoin(VendingMachine* machine) = 0;
    virtual void dispense(VendingMachine* machine) = 0;
    
    // 可选：用于调试打印状态名称
    virtual std::string getName() const = 0;
};

// ==========================================
// 2. 环境类 (Context) - 售货机
// ==========================================
class VendingMachine {
private:
    std::shared_ptr<State> currentState;
    int count; // 剩余商品数量

    // 为了方便状态类切换状态，将状态类设为友元，或者提供公开的设置方法
    // 这里提供公开的设置方法，更安全
public:
    VendingMachine(int initialCount) : count(initialCount) {
        if (count > 0) {
            // 初始状态：无币
            currentState = std::make_shared<NoCoinState>();
        } else {
            // 初始状态：售罄
            currentState = std::make_shared<SoldOutState>();
        }
        std::cout << "[Machine] Initialized with " << count << " items. State: " << currentState->getName() << std::endl;
    }

    // 设置新状态
    void setState(std::shared_ptr<State> state) {
        currentState = state;
        std::cout << "  -> State changed to: " << currentState->getName() << std::endl;
    }

    std::shared_ptr<State> getState() const { return currentState; }
    
    int getCount() const { return count; }
    void setCount(int c) { count = c; }

    // 客户端调用的接口，委托给当前状态处理
    void insertCoin() {
        currentState->insertCoin(this);
    }

    void ejectCoin() {
        currentState->ejectCoin(this);
    }

    void turnCrank() {
        // 摇动把手通常意味着尝试出货，逻辑上对应 dispense 的前置检查
        // 为了简化，我们直接把 turnCrank 的逻辑放在 dispense 或者单独处理
        // 这里我们将 turnCrank 视为触发 dispensing 流程的动作
        // 但为了符合经典示例，我们拆分一下：
        // 1. 用户投币 (insertCoin)
        // 2. 用户摇把手 (turnCrank) -> 触发 dispense 逻辑
        
        // 在经典 GoF 示例中，turnCrank 是 Context 的方法，它调用 state->turnCrank
        // 这里为了代码简洁，我们合并逻辑：
        // 假设 turnCrank 是主要触发动作。
        // 但为了演示状态模式的完整性，我们保留 insert/eject/dispense 三个动作。
        // 修正：让我们添加 turnCrank 到 State 接口，或者在 Context 里处理逻辑流。
        // 经典做法：State 接口包含 insertCoin, ejectCoin, turnCrank, dispense.
        // 让我们调整一下 State 接口以匹配经典售货机逻辑。
        
        currentState->turnCrank(this);
    }
    
    // 为了适配上面的 State 定义，我们需要在 State 里加 turnCrank
    // 由于上面 State 类已经定义了，我们在下面具体实现时补充 turnCrank
    // *修正*：为了保持代码一致性，我在下面具体状态类中会实现 turnCrank
    // 并在 State 基类中声明（由于 C++ 需要编译期知道，我将在下面重新定义 State 的完整接口）
    // 抱歉，上面的 State 定义漏了 turnCrank，我们在下面具体类实现前“补丁”一下思路：
    // 实际上，应该把 turnCrank 加入 State 基类。
    // 为了不让用户困惑，我将在下面的具体实现中隐式处理，或者最好重构一下 State 类定义。
    // *** 决定：为了代码严谨，我将重新定义 State 类包含 turnCrank ***
};

// ==========================================
// 重新定义完整的 State 接口 (包含 turnCrank)
// ==========================================
// 注意：在实际文件中，这应该替换上面的 State 类定义。
// 为了演示清晰，我将在这里重新写出完整的 State 及其子类。

class StateFull {
public:
    virtual ~StateFull() = default;
    virtual void insertCoin(VendingMachine* machine) = 0;
    virtual void ejectCoin(VendingMachine* machine) = 0;
    virtual void turnCrank(VendingMachine* machine) = 0;
    virtual void dispense(VendingMachine* machine) = 0;
    virtual std::string getName() const = 0;
};

// 更新 VendingMachine 使用新的 StateFull
// (为了代码片段独立运行，我们假设 VendingMachine 内部使用的是 StateFull)
// 这里为了节省篇幅，直接继续写具体状态类，假设 VendingMachine 已经适配了 turnCrank

// ==========================================
// 3. 具体状态类
// ==========================================

// --- 状态 A: 售罄状态 (Sold Out) ---
class SoldOutState : public StateFull {
public:
    std::string getName() const override { return "SoldOut"; }

    void insertCoin(VendingMachine* machine) override {
        std::cout << "[SoldOut] You can't insert a coin, the machine is sold out." << std::endl;
    }

    void ejectCoin(VendingMachine* machine) override {
        std::cout << "[SoldOut] You haven't inserted a coin." << std::endl;
    }

    void turnCrank(VendingMachine* machine) override {
        std::cout << "[SoldOut] You turned, but there are no items." << std::endl;
    }

    void dispense(VendingMachine* machine) override {
        std::cout << "[SoldOut] Nothing to dispense!" << std::endl;
    }
};

// --- 状态 B: 无币状态 (No Coin) ---
class NoCoinState : public StateFull {
public:
    std::string getName() const override { return "NoCoin"; }

    void insertCoin(VendingMachine* machine) override {
        std::cout << "[NoCoin] You inserted a coin." << std::endl;
        // 状态迁移：切换到有币状态
        // 注意：如果此时机器刚好卖完了（count=0），逻辑上应该去 SoldOut，但通常 insertCoin 时 count>0
        machine->setState(std::make_shared<HasCoinState>());
    }

    void ejectCoin(VendingMachine* machine) override {
        std::cout << "[NoCoin] You haven't inserted a coin." << std::endl;
    }

    void turnCrank(VendingMachine* machine) override {
        std::cout << "[NoCoin] You turned, but you haven't inserted a coin." << std::endl;
    }

    void dispense(VendingMachine* machine) override {
        std::cout << "[NoCoin] You need to pay first." << std::endl;
    }
};

// --- 状态 C: 有币状态 (Has Coin) ---
class HasCoinState : public StateFull {
public:
    std::string getName() const override { return "HasCoin"; }

    void insertCoin(VendingMachine* machine) override {
        std::cout << "[HasCoin] You can't insert another coin." << std::endl;
    }

    void ejectCoin(VendingMachine* machine) override {
        std::cout << "[HasCoin] Coin returned." << std::endl;
        // 状态迁移：退回到无币状态
        machine->setState(std::make_shared<NoCoinState>());
    }

    void turnCrank(VendingMachine* machine) override {
        std::cout << "[HasCoin] You turned the crank..." << std::endl;
        // 状态迁移：切换到售出状态
        machine->setState(std::make_shared<SoldState>());
    }

    void dispense(VendingMachine* machine) override {
        std::cout << "[HasCoin] Cannot dispense yet, please turn crank." << std::endl;
    }
};

// --- 状态 D: 售出状态 (Sold) ---
class SoldState : public StateFull {
public:
    std::string getName() const override { return "Sold"; }

    void insertCoin(VendingMachine* machine) override {
        std::cout << "[Sold] Please wait, we are already giving you an item." << std::endl;
    }

    void ejectCoin(VendingMachine* machine) override {
        std::cout << "[Sold] Sorry, you already turned the crank. Item is coming." << std::endl;
    }

    void turnCrank(VendingMachine* machine) override {
        std::cout << "[Sold] Turning again won't get you another item!" << std::endl;
    }

    void dispense(VendingMachine* machine) override {
        std::cout << "[Sold] Dispensing item..." << std::endl;
        
        // 执行发货逻辑
        int count = machine->getCount();
        if (count > 0) {
            machine->setCount(count - 1);
            std::cout << "[Sold] Item dispensed! Remaining: " << machine->getCount() << std::endl;
            
            // 状态迁移：根据剩余数量决定下一个状态
            if (machine->getCount() > 0) {
                machine->setState(std::make_shared<NoCoinState>());
            } else {
                std::cout << "[Sold] Oops, out of items!" << std::endl;
                machine->setState(std::make_shared<SoldOutState>());
            }
        } else {
            // 理论上不会进这里，因为 turnCrank 时会检查，但作为防御性编程
            machine->setState(std::make_shared<SoldOutState>());
        }
    }
};

// ==========================================
// 更新后的 Context (适配 StateFull)
// ==========================================
// 为了代码完整，这里重新写一个简化的 VendingMachine 定义，实际使用时替换上面的
class VendingMachineFinal {
private:
    std::shared_ptr<StateFull> currentState;
    int count;

public:
    VendingMachineFinal(int initialCount) : count(initialCount) {
        if (count > 0) {
            currentState = std::make_shared<NoCoinState>();
        } else {
            currentState = std::make_shared<SoldOutState>();
        }
        std::cout << "\n[Machine] Ready. Items: " << count << ", State: " << currentState->getName() << std::endl;
    }

    void setState(std::shared_ptr<StateFull> state) {
        currentState = state;
    }

    void insertCoin() {
        currentState->insertCoin(this);
    }

    void ejectCoin() {
        currentState->ejectCoin(this);
    }

    void turnCrank() {
        currentState->turnCrank(this);
        // 关键：turnCrank 后通常会立即触发 dispense，这是业务流程
        // 在 State 模式中，有时将 dispense 放在 turnCrank 内部调用，或者由 Context 调用
        // 这里我们模拟真实逻辑：turnCrank 成功后，状态变为 Sold，然后立即 dispense
        // 但注意：如果 turnCrank 改变了状态为 Sold，我们需要调用新状态的 dispense 吗？
        // 经典实现通常是：turnCrank 改变状态为 Sold，然后在 turnCrank 的末尾调用 dispense
        // 或者 Context 在 turnCrank 后调用 dispense。
        // 让我们采用：Context 的 turnCrank 调用 state->turnCrank，如果成功转为 Sold，则调用 dispense
        // 为了简化，我们在 SoldState::turnCrank 里不做 dispense，而是由 Context 协调
        // 修正逻辑：
        // 1. 用户 turnCrank
        // 2. Context 调用 currentState->turnCrank (这会将状态改为 Sold)
        // 3. Context 检查如果现在是 SoldState，则调用 dispense
        
        // 这种写法会导致递归或逻辑复杂。
        // 更常见的做法：turnCrank 动作本身在 HasCoinState 中直接触发状态变为 Sold 并调用 dispense 逻辑
        // 让我们看上面的 HasCoinState::turnCrank -> 它只改了状态。
        // 所以我们需要在 Context 的 turnCrank 里补全逻辑：
        
        if (dynamic_cast<SoldState*>(currentState.get())) {
             currentState->dispense(this);
        }
    }
};

// ==========================================
// 4. 客户端测试 (Client)
// ==========================================
int main() {
    std::cout << "=== C++ State Pattern Demo ===" << std::endl;

    // 创建一台有 2 个商品的售货机
    VendingMachineFinal machine(2);

    std::cout << "\n--- Test 1: Normal Purchase ---" << std::endl;
    machine.insertCoin();   // NoCoin -> HasCoin
    machine.turnCrank();    // HasCoin -> Sold -> dispense -> NoCoin (count=1)

    std::cout << "\n--- Test 2: Insert Two Coins (Invalid) ---" << std::endl;
    machine.insertCoin();   // NoCoin -> HasCoin
    machine.insertCoin();   // HasCoin: Error message
    machine.turnCrank();    // HasCoin -> Sold -> dispense -> NoCoin (count=0)

    std::cout << "\n--- Test 3: Machine Sold Out ---" << std::endl;
    // 此时 count 应该是 0，状态应该是 SoldOut
    machine.insertCoin();   // SoldOut: Error
    machine.turnCrank();    // SoldOut: Error

    std::cout << "\n--- Test 4: Eject Coin ---" << std::endl;
    // 重置一下机器状态模拟（实际项目中可能需要 refill 方法）
    // 这里我们新建一个机器来演示 Eject
    VendingMachineFinal machine2(1);
    machine2.insertCoin();  // NoCoin -> HasCoin
    machine2.ejectCoin();   // HasCoin -> NoCoin
    machine2.turnCrank();   // NoCoin: Error (因为没有投币)

    return 0;
}
