
#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <thread>

// 抽象主题接口
class Image {
public:
    virtual ~Image() = default;
    virtual void display() = 0;
    virtual std::string getFileName() const = 0;
};

class RealImage : public Image {
private:
    std::string fileName;
    
    // 模拟耗时的加载过程
    void loadImageFromDisk() {
        std::cout << "  [RealImage] Loading heavy data for '" << fileName << "'..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2)); // 模拟 2 秒延迟
        std::cout << "  [RealImage] Loading finished!" << std::endl;
    }

public:
    explicit RealImage(const std::string& file) : fileName(file) {
        // 在构造函数中立即加载数据（这是开销大的地方）
        loadImageFromDisk();
    }

    void display() override {
        std::cout << "  [RealImage] Displaying image: " << fileName << std::endl;
    }

    std::string getFileName() const override {
        return fileName;
    }
};

class ImageProxy : public Image {
private:
    std::string fileName;
    std::string userRole;             // 用于权限检查
    std::shared_ptr<RealImage> realImage; // 指向真实对象的指针，初始为空
    int accessCount;                  // 访问计数

    // 检查权限
    bool hasAccess() const {
        if (userRole == "admin" || userRole == "user") {
            return true;
        }
        return false;
    }

    // 确保真实对象已加载 (懒加载核心逻辑)
    void ensureLoaded() {
        if (!realImage) {
            std::cout << "  [Proxy] RealImage not loaded yet. Creating now..." << std::endl;
            realImage = std::make_shared<RealImage>(fileName);
        }
    }

public:
    ImageProxy(const std::string& file, const std::string& role) 
        : fileName(file), userRole(role), realImage(nullptr), accessCount(0) {}

    void display() override {
        // 1. 前置处理：权限检查
        if (!hasAccess()) {
            std::cout << "  [Proxy] Access Denied! User '" << userRole 
                      << "' cannot view '" << fileName << "'" << std::endl;
            return;
        }

        // 2. 前置处理：懒加载 (如果还没加载，现在才加载)
        ensureLoaded();

        // 3. 委托给真实对象
        realImage->display();

        // 4. 后置处理：记录日志/计数
        accessCount++;
        std::cout << "  [Proxy] Access count for '" << fileName << "': " << accessCount << std::endl;
    }

    std::string getFileName() const override {
        return fileName;
    }
    
    // 代理特有的方法：获取当前是否已加载
    bool isLoaded() const {
        return realImage != nullptr;
    }
};

void clientCode(std::shared_ptr<Image> img) {
    std::cout << "\n--- Client requesting display ---" << std::endl;
    img->display();
    std::cout << "---------------------------------\n" << std::endl;
}

int main() {
    std::cout << "=== Scenario 1: Admin user (Allowed) ===" << std::endl;
    // 创建代理：文件名 + 角色
    // 注意：此时 RealImage 还没有被创建！构造函数瞬间完成。
    auto proxyAdmin = std::make_shared<ImageProxy>("secret_plan.jpg", "admin");
    
    std::cout << "Is image loaded? " << (proxyAdmin->isLoaded() ? "Yes" : "No") << std::endl;

    // 第一次调用 display() -> 触发权限检查 (通过) -> 触发懒加载 (耗时 2 秒) -> 显示
    clientCode(proxyAdmin);

    std::cout << "Is image loaded now? " << (proxyAdmin->isLoaded() ? "Yes" : "No") << std::endl;

    // 第二次调用 display() -> 触发权限检查 (通过) -> 无需加载 (直接复用) -> 显示
    // 这次会非常快，因为真实对象已经在内存中了
    clientCode(proxyAdmin);


    std::cout << "\n=== Scenario 2: Guest user (Denied) ===" << std::endl;
    auto proxyGuest = std::make_shared<ImageProxy>("top_secret.jpg", "guest");

    // 调用 display() -> 触发权限检查 (失败) -> 直接返回，根本不会加载图片
    // 即使图片很大，也没浪费资源加载
    clientCode(proxyGuest);
    
    std::cout << "Is image loaded? " << (proxyGuest->isLoaded() ? "Yes" : "No") 
              << " (Should be No because access was denied)" << std::endl;

    return 0;
}
