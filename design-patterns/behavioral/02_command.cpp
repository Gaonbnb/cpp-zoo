#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <stack>

// ==========================================
// 1. Receiver (接收者)
// 真正执行具体业务逻辑的角色
// ==========================================

class Light {
private:
    std::string location;
    bool isOn;

public:
    explicit Light(const std::string& loc) : location(loc), isOn(false) {}

    void on() {
        isOn = true;
        std::cout << "  [Light] " << location << " is turned ON." << std::endl;
    }

    void off() {
        isOn = false;
        std::cout << "  [Light] " << location << " is turned OFF." << std::endl;
    }

    bool getState() const { return isOn; }
};

class Fan {
private:
    std::string location;
    int speed; // 0: off, 1: low, 2: high

public:
    explicit Fan(const std::string& loc) : location(loc), speed(0) {}

    void setSpeed(int s) {
        speed = s;
        if (s == 0) std::cout << "  [Fan] " << location << " is stopped." << std::endl;
        else std::cout << "  [Fan] " << location << " speed set to " << s << "." << std::endl;
    }

    int getSpeed() const { return speed; }
};

// ==========================================
// 2. Command (命令接口)
// 声明执行和撤销操作
// ==========================================

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

// ==========================================
// 3. ConcreteCommand (具体命令)
// 绑定接收者和动作，并记录状态用于撤销
// ==========================================

// --- 开灯命令 ---
class LightOnCommand : public Command {
private:
    std::shared_ptr<Light> light;
    bool previousState; // 记录执行前的状态，用于撤销

public:
    explicit LightOnCommand(std::shared_ptr<Light> l) 
        : light(l), previousState(false) {}

    void execute() override {
        // 1. 保存当前状态
        previousState = light->getState();
        // 2. 执行动作
        light->on();
    }

    void undo() override {
        // 恢复到之前的状态
        if (previousState) {
            light->on();
        } else {
            light->off();
        }
        std::cout << "  [Undo] Light state restored to: " 
                  << (previousState ? "ON" : "OFF") << std::endl;
    }
};

// --- 关灯命令 ---
class LightOffCommand : public Command {
private:
    std::shared_ptr<Light> light;
    bool previousState;

public:
    explicit LightOffCommand(std::shared_ptr<Light> l) 
        : light(l), previousState(true) {}

    void execute() override {
        previousState = light->getState();
        light->off();
    }

    void undo() override {
        if (previousState) {
            light->on();
        } else {
            light->off();
        }
        std::cout << "  [Undo] Light state restored to: " 
                  << (previousState ? "ON" : "OFF") << std::endl;
    }
};

// --- 风扇调速命令 ---
class FanSpeedCommand : public Command {
private:
    std::shared_ptr<Fan> fan;
    int newSpeed;
    int oldSpeed; // 记录旧速度

public:
    FanSpeedCommand(std::shared_ptr<Fan> f, int speed) 
        : fan(f), newSpeed(speed), oldSpeed(0) {}

    void execute() override {
        oldSpeed = fan->getSpeed(); // 保存旧状态
        fan->setSpeed(newSpeed);
    }

    void undo() override {
        fan->setSpeed(oldSpeed); // 恢复旧状态
        std::cout << "  [Undo] Fan speed restored to: " << oldSpeed << std::endl;
    }
};

// ==========================================
// 4. Invoker (调用者)
// 负责触发命令，并维护历史栈以支持撤销
// ==========================================

class RemoteControl {
private:
    std::shared_ptr<Command> currentCommand;
    std::stack<std::shared_ptr<Command>> history;

public:
    // 设置当前要执行的命令（比如按下某个按钮时）
    void setCommand(std::shared_ptr<Command> cmd) {
        currentCommand = cmd;
    }

    // 按下按钮：执行命令
    void pressButton() {
        if (currentCommand) {
            std::cout << "\n[Remote] Executing command..." << std::endl;
            currentCommand->execute();
            // 将执行过的命令压入历史栈，以便后续撤销
            history.push(currentCommand);
        } else {
            std::cout << "[Remote] No command assigned!" << std::endl;
        }
    }

    // 按下撤销按钮
    void pressUndo() {
        if (!history.empty()) {
            std::cout << "\n[Remote] Undoing last action..." << std::endl;
            auto cmd = history.top();
            history.pop();
            cmd->undo();
        } else {
            std::cout << "[Remote] Nothing to undo." << std::endl;
        }
    }
};

// ==========================================
// 5. Client (客户端)
// 组装所有对象并模拟操作
// ==========================================

int main() {
    // 1. 创建接收者 (家电)
    auto livingRoomLight = std::make_shared<Light>("Living Room");
    auto bedroomFan = std::make_shared<Fan>("Bedroom");

    // 2. 创建调用者 (遥控器)
    RemoteControl remote;

    std::cout << "=== Smart Home Command Pattern Demo ===" << std::endl;

    // --- 场景 1: 开灯 ---
    std::cout << "\n--- Step 1: Turn On Light ---" << std::endl;
    auto lightOnCmd = std::make_shared<LightOnCommand>(livingRoomLight);
    remote.setCommand(lightOnCmd);
    remote.pressButton();

    // --- 场景 2: 调高风扇 ---
    std::cout << "\n--- Step 2: Set Fan to High (2) ---" << std::endl;
    auto fanCmd = std::make_shared<FanSpeedCommand>(bedroomFan, 2);
    remote.setCommand(fanCmd);
    remote.pressButton();

    // --- 场景 3: 关灯 ---
    std::cout << "\n--- Step 3: Turn Off Light ---" << std::endl;
    auto lightOffCmd = std::make_shared<LightOffCommand>(livingRoomLight);
    remote.setCommand(lightOffCmd);
    remote.pressButton();

    // --- 场景 4: 连续撤销 ---
    
    // 撤销 Step 3 (关灯) -> 应该变回开灯
    std::cout << "\n--- Step 4: UNDO (Should turn Light ON) ---" << std::endl;
    remote.pressUndo();

    // 撤销 Step 2 (风扇调速) -> 应该变回停止
    std::cout << "\n--- Step 5: UNDO (Should stop Fan) ---" << std::endl;
    remote.pressUndo();

    // 撤销 Step 1 (开灯) -> 应该变回关灯
    std::cout << "\n--- Step 6: UNDO (Should turn Light OFF) ---" << std::endl;
    remote.pressUndo();

    // 再撤销一次 (空)
    std::cout << "\n--- Step 7: UNDO (Nothing left) ---" << std::endl;
    remote.pressUndo();

    return 0;
}


// 关键点解析
// 解耦 (Decoupling)：
// RemoteControl 类完全不知道 Light 或 Fan 的存在。它只依赖 Command 接口。
// 如果你明天想添加一个“窗帘”设备，你只需要写 Curtain 类和 OpenCurtainCommand 类，不需要修改 RemoteControl 的任何代码。
// 撤销机制 (Undo)：
// 这是命令模式的杀手锏。
// 在 execute() 执行前，具体命令类（如 LightOnCommand）会先快照当前的状态（previousState）。
// 当调用 undo() 时，利用这个快照将接收者还原。
// RemoteControl 使用 std::stack 来存储执行过的命令，实现了“后进先出”的撤销逻辑。
// 宏命令 (Macro Command) 扩展性：
// 虽然代码中没写，但你可以轻松创建一个 MacroCommand 类，它内部持有一个 vector<Command>。
// 它的 execute() 会遍历列表执行所有命令，undo() 会反向遍历执行所有撤销。
// 应用场景：一键“晚安模式”（关灯 + 关风扇 + 锁门）。
// 日志与事务：
// 因为命令是对象，你可以把它们序列化存入数据库。如果系统崩溃，重启后读取日志重新 execute()，就能恢复现场。
// 什么时候使用命令模式？
// 你需要支持 Undo/Redo 功能（如文本编辑器、绘图软件）。
// 你需要将操作排队执行（如打印任务队列、线程池任务）。
// 你需要记录操作日志，以便在系统崩溃后重放操作。
// 你想解耦请求发送者和接收者，或者想在运行时动态组合请求（如 GUI 菜单项绑定不同的动作）。
