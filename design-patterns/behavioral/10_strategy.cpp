#include <iostream>
#include <string>
#include <memory>

// 1. 抽象策略 (Strategy)
class RouteStrategy {
public:
    virtual ~RouteStrategy() = default;
    virtual void calculateRoute(const std::string& start, const std::string& end) = 0;
    virtual int estimateTime() = 0;
};

// 2. 具体策略 A: 驾车
class DrivingStrategy : public RouteStrategy {
public:
    void calculateRoute(const std::string& start, const std::string& end) override {
        std::cout << "[Driving] Calculating highway route from " << start << " to " << end << "." << std::endl;
    }
    int estimateTime() override { return 30; } // 分钟
};

// 3. 具体策略 B: 步行
class WalkingStrategy : public RouteStrategy {
public:
    void calculateRoute(const std::string& start, const std::string& end) override {
        std::cout << "[Walking] Calculating shortest path from " << start << " to " << end << "." << std::endl;
    }
    int estimateTime() override { return 120; }
};

// 4. 具体策略 C: 骑行
class BikingStrategy : public RouteStrategy {
public:
    void calculateRoute(const std::string& start, const std::string& end) override {
        std::cout << "[Biking] Calculating bike-lane route from " << start << " to " << end << "." << std::endl;
    }
    int estimateTime() override { return 60; }
};

// 5. 上下文 (Context)
class Navigator {
private:
    std::shared_ptr<RouteStrategy> strategy;

public:
    void setStrategy(std::shared_ptr<RouteStrategy> strat) {
        strategy = strat;
        std::cout << "[Navigator] Strategy changed." << std::endl;
    }

    void executeNavigation(const std::string& start, const std::string& end) {
        if (!strategy) {
            std::cout << "[Navigator] No strategy set!" << std::endl;
            return;
        }
        strategy->calculateRoute(start, end);
        std::cout << "[Navigator] Estimated time: " << strategy->estimateTime() << " mins." << std::endl;
    }
};

// --- 测试 ---
void runStrategyDemo() {
    std::cout << "\n=== Strategy Pattern Demo ===" << std::endl;
    Navigator nav;
    auto start = "Home", end = "Office";

    // 默认驾车
    nav.setStrategy(std::make_shared<DrivingStrategy>());
    nav.executeNavigation(start, end);

    // 动态切换为步行
    nav.setStrategy(std::make_shared<WalkingStrategy>());
    nav.executeNavigation(start, end);

    // 动态切换为骑行
    nav.setStrategy(std::make_shared<BikingStrategy>());
    nav.executeNavigation(start, end);
}
