#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

// 前向声明
class StockMarket;

// ==========================================
// 1. 抽象观察者 (Observer)
// ==========================================
class Observer {
public:
    virtual ~Observer() = default;
    // 更新接口：通常传入主题指针或具体数据
    // 这里传入主题指针，让观察者决定是"拉取"数据还是直接使用推送的数据
    virtual void update(StockMarket* market) = 0;
    
    // 用于调试，显示观察者名称
    virtual std::string getName() const = 0;
};

// ==========================================
// 2. 抽象主题 (Subject)
// ==========================================
class Subject {
public:
    virtual ~Subject() = default;
    virtual void attach(std::shared_ptr<Observer> observer) = 0;
    virtual void detach(std::shared_ptr<Observer> observer) = 0;
    virtual void notify() = 0;
};

// ==========================================
// 3. 具体主题：股票市场 (ConcreteSubject)
// ==========================================
class StockMarket : public Subject {
private:
    std::vector<std::shared_ptr<Observer>> observers;
    std::string stockSymbol;
    double currentPrice;
    double previousPrice; // 用于计算变化幅度

public:
    StockMarket(const std::string& symbol, double initialPrice) 
        : stockSymbol(symbol), currentPrice(initialPrice), previousPrice(initialPrice) {}

    // 注册观察者
    void attach(std::shared_ptr<Observer> observer) override {
        observers.push_back(observer);
        std::cout << "[Market] " << observer->getName() << " attached to " << stockSymbol << "." << std::endl;
    }

    // 移除观察者
    void detach(std::shared_ptr<Observer> observer) override {
        auto it = std::find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
            std::cout << "[Market] " << observer->getName() << " detached from " << stockSymbol << "." << std::endl;
        }
    }

    // 通知所有观察者
    void notify() override {
        std::cout << "\n[Market] Notifying " << observers.size() << " observers about price change..." << std::endl;
        for (const auto& obs : observers) {
            obs->update(this);
        }
    }

    // 业务方法：设置新价格
    void setPrice(double newPrice) {
        previousPrice = currentPrice;
        currentPrice = newPrice;
        std::cout << "\n[Market] Price updated for " << stockSymbol 
                  << ": $" << previousPrice << " -> $" << currentPrice << std::endl;
        
        // 只有价格真正变动时才通知
        if (std::abs(currentPrice - previousPrice) > 0.001) {
            notify();
        }
    }

    // 提供给观察者的“拉取”数据方法
    double getPrice() const { return currentPrice; }
    double getPreviousPrice() const { return previousPrice; }
    std::string getSymbol() const { return stockSymbol; }
    
    // 计算变化百分比
    double getChangePercent() const {
        if (previousPrice == 0) return 0;
        return ((currentPrice - previousPrice) / previousPrice) * 100.0;
    }
};

// ==========================================
// 4. 具体观察者 A：手机 APP (ConcreteObserver)
// ==========================================
class MobileApp : public Observer {
private:
    std::string appName;

public:
    explicit MobileApp(const std::string& name) : appName(name) {}

    std::string getName() const override { return appName; }

    void update(StockMarket* market) override {
        // 简单的推送显示
        std::cout << ">> [" << appName << "] Push Notification: " 
                  << market->getSymbol() << " is now $" << market->getPrice() << std::endl;
    }
};

// ==========================================
// 5. 具体观察者 B：网页大屏 (ConcreteObserver)
// ==========================================
class WebDashboard : public Observer {
private:
    std::string dashboardName;

public:
    explicit WebDashboard(const std::string& name) : dashboardName(name) {}

    std::string getName() const override { return dashboardName; }

    void update(StockMarket* market) override {
        // 显示更详细的数据（拉取模式）
        double change = market->getPrice() - market->getPreviousPrice();
        char sign = (change >= 0) ? '+' : '-';
        
        std::cout << ">> [" << dashboardName << "] Update Chart: " 
                  << market->getSymbol() 
                  << " | Price: $" << market->getPrice()
                  << " | Change: " << sign << std::abs(change) 
                  << " (" << sign << std::abs(market->getChangePercent()) << "%)" << std::endl;
    }
};

// ==========================================
// 6. 具体观察者 C：邮件警报 (条件观察者)
// ==========================================
class EmailAlert : public Observer {
private:
    std::string emailAddr;
    double threshold; // 触发警报的阈值百分比

public:
    EmailAlert(const std::string& addr, double thresh) 
        : emailAddr(addr), threshold(thresh) {}

    std::string getName() const override { return "EmailAlert(" + emailAddr + ")"; }

    void update(StockMarket* market) override {
        double percent = std::abs(market->getChangePercent());
        
        // 只有波动超过阈值才发送邮件
        if (percent >= threshold) {
            std::cout << ">> [EMAIL SENT to " << emailAddr << "] ALERT: " 
                      << market->getSymbol() << " has volatile movement! Changed by " 
                      << market->getChangePercent() << "%" << std::endl;
        } else {
            // 静默忽略，不打印任何内容，模拟不发送邮件
            // std::cout << ">> [Email] No alert needed for small change." << std::endl; 
        }
    }
};

// ==========================================
// 7. 客户端测试 (Client)
// ==========================================
int main() {
    std::cout << "=== C++ Observer Pattern Demo ===" << std::endl;

    // 1. 创建被观察者 (股票：AAPL, 初始价 150.0)
    auto appleStock = std::make_shared<StockMarket>("AAPL", 150.0);

    // 2. 创建观察者
    auto app = std::make_shared<MobileApp>("iOS Stocks App");
    auto web = std::make_shared<WebDashboard>("Bloomberg Terminal");
    // 邮件警报：只有波动超过 2% 才发送
    auto email = std::make_shared<EmailAlert>("trader@example.com", 2.0);

    // 3. 注册观察者
    appleStock->attach(app);
    appleStock->attach(web);
    appleStock->attach(email);

    std::cout << "\n--- Event 1: Small Price Increase (0.5%) ---" << std::endl;
    appleStock->setPrice(150.75); // 变化约 0.5%，邮件不应触发

    std::cout << "\n--- Event 2: Large Price Drop (-3%) ---" << std::endl;
    appleStock->setPrice(146.25); // 变化约 -3%，邮件应触发

    std::cout << "\n--- Event 3: Unsubscribe Mobile App ---" << std::endl;
    appleStock->detach(app);

    std::cout << "\n--- Event 4: Another Price Change ---" << std::endl;
    appleStock->setPrice(148.00); // 只有 Web 和 Email 会收到通知

    return 0;
}
