#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// 前向声明
class User;
class ChatRoom;

// ==========================================
// 1. 抽象中介者 (Mediator)
// ==========================================
class Mediator {
public:
    virtual ~Mediator() = default;
    // 发送消息的方法，通常包含发送者、接收者（可选）和消息内容
    virtual void sendMessage(const std::string& message, User* sender, User* receiver = nullptr) = 0;
};

// ==========================================
// 2. 抽象同事 (Colleague)
// ==========================================
class User {
protected:
    std::string name;
    std::shared_ptr<Mediator> mediator; // 持有中介者的智能指针

public:
    User(const std::string& n, std::shared_ptr<Mediator> med) 
        : name(n), mediator(med) {}

    virtual ~User() = default;

    // 发送消息：委托给中介者处理
    void send(const std::string& message) {
        std::cout << "[" << name << "]: Sending -> \"" << message << "\"" << std::endl;
        mediator->sendMessage(message, this);
    }

    // 接收消息：由中介者调用
    virtual void receive(const std::string& message, const std::string& from) = 0;

    std::string getName() const { return name; }
};

// ==========================================
// 3. 具体同事 (ConcreteColleague)
// ==========================================
class ChatUser : public User {
public:
    ChatUser(const std::string& n, std::shared_ptr<Mediator> med) : User(n, med) {}

    void receive(const std::string& message, const std::string& from) override {
        if (from == name) return; // 不接收自己发的消息（防止广播时回环）
        std::cout << "[" << name << "]: Received from [" << from << "] -> \"" << message << "\"" << std::endl;
    }
};

// ==========================================
// 4. 具体中介者 (ConcreteMediator)
// ==========================================
class ChatRoom : public Mediator {
private:
    std::vector<std::shared_ptr<User>> users;

public:
    // 注册用户
    void addUser(std::shared_ptr<User> user) {
        users.push_back(user);
    }

    // 实现消息分发逻辑
    void sendMessage(const std::string& message, User* sender, User* receiver = nullptr) override {
        if (receiver) {
            // 点对点消息：只发给特定用户
            // 这里需要找到对应的用户对象，简单起见假设传入的就是有效指针
            // 实际项目中可能需要通过 ID 查找
             for (auto& u : users) {
                if (u.get() == receiver) {
                    u->receive(message, sender->getName());
                    break;
                }
            }
        } else {
            // 广播消息：发给除了发送者以外的所有人
            for (auto& u : users) {
                // 避免发给自己
                if (u.get() != sender) {
                    u->receive(message, sender->getName());
                }
            }
        }
    }
};

// ==========================================
// 5. 客户端测试 (Client)
// ==========================================
int main() {
    std::cout << "=== C++ Mediator Pattern Demo ===" << std::endl;

    // 1. 创建中介者 (聊天室)
    auto chatRoom = std::make_shared<ChatRoom>();

    // 2. 创建同事 (用户)，并注入中介者
    // 注意：用户之间没有互相持有引用，它们只认识 chatRoom
    auto alice = std::make_shared<ChatUser>("Alice", chatRoom);
    auto bob = std::make_shared<ChatUser>("Bob", chatRoom);
    auto charlie = std::make_shared<ChatUser>("Charlie", chatRoom);

    // 3. 将用户注册到聊天室
    chatRoom->addUser(alice);
    chatRoom->addUser(bob);
    chatRoom->addUser(charlie);

    std::cout << "\n--- Scenario 1: Broadcast Message ---" << std::endl;
    // Alice 发送广播消息，Bob 和 Charlie 都会收到
    alice->send("Hello everyone! Welcome to the chat.");

    std::cout << "\n--- Scenario 2: Another Broadcast ---" << std::endl;
    // Bob 发送广播
    bob->send("Hi Alice, Hi Charlie!");

    std::cout << "\n--- Scenario 3: Direct Message (Extension) ---" << std::endl;
    // 如果中介者支持点对点，可以这样调用（需要修改 User::send 或增加新方法）
    // 为了演示中介者的灵活性，我们直接调用中介者接口模拟点对点
    std::cout << "[System]: Charlie sending private message to Alice..." << std::endl;
    chatRoom->sendMessage("Can we talk privately?", charlie.get(), alice.get());
    // Bob 不会收到这条消息

    return 0;
}
