#include <iostream>
#include <string>
#include <memory>
#include <vector>

// 请求类：封装具体的业务数据
class PurchaseRequest {
private:
    int amount;
    std::string purpose;
    std::string requester;

public:
    PurchaseRequest(int amt, const std::string& purp, const std::string& req)
        : amount(amt), purpose(purp), requester(req) {}

    int getAmount() const { return amount; }
    std::string getPurpose() const { return purpose; }
    std::string getRequester() const { return requester; }
};

// 抽象处理者
class Approver {
protected:
    std::string name;
    std::shared_ptr<Approver> nextApprover; // 指向链中的下一个节点

public:
    explicit Approver(const std::string& n) : name(n), nextApprover(nullptr) {}

    // 设置下一个处理者 (构建链条的关键)
    void setNext(std::shared_ptr<Approver> next) {
        nextApprover = next;
    }

    // 处理请求的模板方法
    // 如果当前能处理，就处理；否则传给下一个
    virtual void handleRequest(const PurchaseRequest& request) {
        if (canHandle(request)) {
            process(request);
        } else {
            if (nextApprover) {
                std::cout << "[" << name << "] Forwarding request to [" 
                          << nextApprover->name << "]..." << std::endl;
                nextApprover->handleRequest(request);
            } else {
                std::cout << "[" << name << "] Request denied! No one in the chain can handle this." << std::endl;
            }
        }
    }

    // 子类实现：判断是否有权限处理
    virtual bool canHandle(const PurchaseRequest& request) = 0;

    // 子类实现：具体的处理逻辑
    virtual void process(const PurchaseRequest& request) = 0;
    
    virtual ~Approver() = default;
};

// 具体处理者 1: 组长 (上限 500)
class TeamLead : public Approver {
public:
    TeamLead() : Approver("Team Lead") {}

    bool canHandle(const PurchaseRequest& request) override {
        return request.getAmount() <= 500;
    }

    void process(const PurchaseRequest& request) override {
        std::cout << ">>> [Team Lead] Approved purchase of $" << request.getAmount() 
                  << " for: " << request.getPurpose() << std::endl;
    }
};

// 具体处理者 2: 经理 (上限 2000)
class Manager : public Approver {
public:
    Manager() : Approver("Manager") {}

    bool canHandle(const PurchaseRequest& request) override {
        return request.getAmount() <= 2000;
    }

    void process(const PurchaseRequest& request) override {
        std::cout << ">>> [Manager] Approved purchase of $" << request.getAmount() 
                  << " for: " << request.getPurpose() << std::endl;
    }
};

// 具体处理者 3: 总监 (上限 10000)
class Director : public Approver {
public:
    Director() : Approver("Director") {}

    bool canHandle(const PurchaseRequest& request) override {
        return request.getAmount() <= 10000;
    }

    void process(const PurchaseRequest& request) override {
        std::cout << ">>> [Director] Approved purchase of $" << request.getAmount() 
                  << " for: " << request.getPurpose() << std::endl;
    }
};

// 具体处理者 4: CEO (无上限，或者处理剩下的所有)
class CEO : public Approver {
public:
    CEO() : Approver("CEO") {}

    bool canHandle(const PurchaseRequest& request) override {
        // CEO 处理所有剩下的请求 (或者设定一个极高的上限)
        return true; 
    }

    void process(const PurchaseRequest& request) override {
        std::cout << ">>> [CEO] Personally approved huge purchase of $" << request.getAmount() 
                  << " for: " << request.getPurpose() << std::endl;
    }
};

int main() {
    // 1. 创建所有处理者对象
    auto teamLead = std::make_shared<TeamLead>();
    auto manager = std::make_shared<Manager>();
    auto director = std::make_shared<Director>();
    auto ceo = std::make_shared<CEO>();

    // 2. 组装链条 (设置责任顺序)
    // 组长 -> 经理 -> 总监 -> CEO
    teamLead->setNext(manager);
    manager->setNext(director);
    director->setNext(ceo);
    // CEO 是最后一个，next 默认为 nullptr

    std::cout << "=== Chain of Responsibility Demo ===\n" << std::endl;

    // --- 测试案例 1: 小额申请 (300) ---
    // 预期：组长直接处理
    std::cout << "Test 1: Request for $300 (Office Supplies)" << std::endl;
    PurchaseRequest req1(300, "Office Supplies", "Alice");
    teamLead->handleRequest(req1); // 客户端只需把头节点传进去
    std::cout << "----------------------------------------\n" << std::endl;

    // --- 测试案例 2: 中等申请 (1500) ---
    // 预期：组长转给经理，经理处理
    std::cout << "Test 2: Request for $1500 (New Monitor)" << std::endl;
    PurchaseRequest req2(1500, "New Monitor", "Bob");
    teamLead->handleRequest(req2);
    std::cout << "----------------------------------------\n" << std::endl;

    // --- 测试案例 3: 大额申请 (8000) ---
    // 预期：组长->经理->总监，总监处理
    std::cout << "Test 3: Request for $8000 (Server Upgrade)" << std::endl;
    PurchaseRequest req3(8000, "Server Upgrade", "Charlie");
    teamLead->handleRequest(req3);
    std::cout << "----------------------------------------\n" << std::endl;

    // --- 测试案例 4: 巨额申请 (50000) ---
    // 预期：一直传递到 CEO
    std::cout << "Test 4: Request for $50000 (Acquisition)" << std::endl;
    PurchaseRequest req4(50000, "Acquisition", "Dave");
    teamLead->handleRequest(req4);
    std::cout << "----------------------------------------\n" << std::endl;
    
    // --- 测试案例 5: 动态修改链条 ---
    // 假设经理休假了，我们临时让组长直接转给总监
    std::cout << "\n=== Dynamic Change: Manager is on leave ===" << std::endl;
    teamLead->setNext(director); // 跳过经理
    
    std::cout << "Test 5: Request for $1500 (New Monitor) - Manager skipped" << std::endl;
    PurchaseRequest req5(1500, "New Monitor", "Eve");
    teamLead->handleRequest(req5);

    return 0;
}
