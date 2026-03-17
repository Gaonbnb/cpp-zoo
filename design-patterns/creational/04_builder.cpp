

#include <iostream>
#include <string>
#include <memory>

// 产品类：电脑
class Computer {
private:
    std::string cpu;
    std::string gpu;
    std::string ram;
    std::string storage;
    std::string powerSupply;
    std::string motherboard;
    bool hasWifi;
    bool hasBluetooth;
    
public:
    // 只提供getter，不提供setter（通过Builder设置）
    void setCPU(const std::string& c) { cpu = c; }
    void setGPU(const std::string& g) { gpu = g; }
    void setRAM(const std::string& r) { ram = r; }
    void setStorage(const std::string& s) { storage = s; }
    void setPowerSupply(const std::string& p) { powerSupply = p; }
    void setMotherboard(const std::string& m) { motherboard = m; }
    void setWifi(bool w) { hasWifi = w; }
    void setBluetooth(bool b) { hasBluetooth = b; }
    
    void showSpecs() const {
        std::cout << "\n=== 电脑配置 ===\n"
                  << "CPU: " << cpu << "\n"
                  << "GPU: " << gpu << "\n"
                  << "内存: " << ram << "\n"
                  << "硬盘: " << storage << "\n"
                  << "电源: " << powerSupply << "\n"
                  << "主板: " << motherboard << "\n"
                  << "WiFi: " << (hasWifi ? "有" : "无") << "\n"
                  << "蓝牙: " << (hasBluetooth ? "有" : "无") << "\n";
    }
};

// 抽象建造者
class ComputerBuilder {
protected:
    std::unique_ptr<Computer> computer;
    
public:
    ComputerBuilder() : computer(std::make_unique<Computer>()) {}
    virtual ~ComputerBuilder() = default;
    
    // 构建步骤的抽象方法
    virtual void buildCPU() = 0;
    virtual void buildGPU() = 0;
    virtual void buildRAM() = 0;
    virtual void buildStorage() = 0;
    virtual void buildPowerSupply() = 0;
    virtual void buildMotherboard() = 0;
    virtual void buildExtras() = 0;
    
    // 获取最终产品
    std::unique_ptr<Computer> getResult() {
        return std::move(computer);
    }
};

// 具体建造者：游戏电脑
class GamingComputerBuilder : public ComputerBuilder {
public:
    void buildCPU() override {
        computer->setCPU("Intel i9-13900K");
    }
    
    void buildGPU() override {
        computer->setGPU("NVIDIA RTX 4090");
    }
    
    void buildRAM() override {
        computer->setRAM("32GB DDR5 6000MHz");
    }
    
    void buildStorage() override {
        computer->setStorage("2TB NVMe SSD");
    }
    
    void buildPowerSupply() override {
        computer->setPowerSupply("1000W 金牌电源");
    }
    
    void buildMotherboard() override {
        computer->setMotherboard("Z790 高端主板");
    }
    
    void buildExtras() override {
        computer->setWifi(true);
        computer->setBluetooth(true);
    }
};

// 具体建造者：办公电脑
class OfficeComputerBuilder : public ComputerBuilder {
public:
    void buildCPU() override {
        computer->setCPU("Intel i5-13400");
    }
    
    void buildGPU() override {
        computer->setGPU("集成显卡");
    }
    
    void buildRAM() override {
        computer->setRAM("16GB DDR4 3200MHz");
    }
    
    void buildStorage() override {
        computer->setStorage("512GB SSD");
    }
    
    void buildPowerSupply() override {
        computer->setPowerSupply("500W 铜牌电源");
    }
    
    void buildMotherboard() override {
        computer->setMotherboard("B760 主流主板");
    }
    
    void buildExtras() override {
        computer->setWifi(true);
        computer->setBluetooth(false);
    }
};

// 具体建造者：服务器
class ServerComputerBuilder : public ComputerBuilder {
public:
    void buildCPU() override {
        computer->setCPU("Intel Xeon Platinum 8380");
    }
    
    void buildGPU() override {
        computer->setGPU("无（服务器不需要显卡）");
    }
    
    void buildRAM() override {
        computer->setRAM("128GB ECC DDR5");
    }
    
    void buildStorage() override {
        computer->setStorage("4TB NVMe SSD RAID");
    }
    
    void buildPowerSupply() override {
        computer->setPowerSupply("1600W 冗余电源");
    }
    
    void buildMotherboard() override {
        computer->setMotherboard("服务器级主板");
    }
    
    void buildExtras() override {
        computer->setWifi(false);
        computer->setBluetooth(false);
    }
};

// 导演类：控制构建过程
class ComputerEngineer {
public:
    std::unique_ptr<Computer> assembleComputer(ComputerBuilder& builder) {
        // 定义构建步骤的顺序
        builder.buildMotherboard();
        builder.buildCPU();
        builder.buildRAM();
        builder.buildGPU();
        builder.buildStorage();
        builder.buildPowerSupply();
        builder.buildExtras();
        
        return builder.getResult();
    }
};

// 测试
int main_classic() {
    ComputerEngineer engineer;
    
    // 组装游戏电脑
    GamingComputerBuilder gamingBuilder;
    auto gamingPC = engineer.assembleComputer(gamingBuilder);
    gamingPC->showSpecs();
    
    // 组装办公电脑
    OfficeComputerBuilder officeBuilder;
    auto officePC = engineer.assembleComputer(officeBuilder);
    officePC->showSpecs();
    
    // 组装服务器
    ServerComputerBuilder serverBuilder;
    auto server = engineer.assembleComputer(serverBuilder);
    server->showSpecs();
    
    return 0;
}


#include <iostream>
#include <string>
#include <vector>

class Pizza {
private:
    std::string size;
    std::string crust;
    std::vector<std::string> toppings;
    bool extraCheese;
    bool extraSauce;
    std::string bakeLevel;
    
    // 私有构造函数，只能通过Builder创建
    Pizza() = default;
    
    friend class PizzaBuilder;  // 允许Builder访问私有成员
    
public:
    void show() const {
        std::cout << "\n=== 披萨订单 ===\n"
                  << "尺寸: " << size << "\n"
                  << "饼底: " << crust << "\n"
                  << "配料: ";
        for (const auto& t : toppings) std::cout << t << " ";
        std::cout << "\n额外奶酪: " << (extraCheese ? "是" : "否")
                  << "\n额外酱料: " << (extraSauce ? "是" : "否")
                  << "\n烘烤程度: " << bakeLevel << "\n";
    }
    
    // 建造者类
    class PizzaBuilder {
    private:
        Pizza pizza;
        
    public:
        // 每个方法返回自身引用，支持链式调用
        PizzaBuilder& setSize(const std::string& s) {
            pizza.size = s;
            return *this;
        }
        
        PizzaBuilder& setCrust(const std::string& c) {
            pizza.crust = c;
            return *this;
        }
        
        PizzaBuilder& addTopping(const std::string& t) {
            pizza.toppings.push_back(t);
            return *this;
        }
        
        PizzaBuilder& addExtraCheese() {
            pizza.extraCheese = true;
            return *this;
        }
        
        PizzaBuilder& addExtraSauce() {
            pizza.extraSauce = true;
            return *this;
        }
        
        PizzaBuilder& setBakeLevel(const std::string& l) {
            pizza.bakeLevel = l;
            return *this;
        }
        
        // 最后调用build()获取产品
        Pizza build() {
            // 可以在这里添加验证逻辑
            if (pizza.size.empty()) throw std::runtime_error("必须指定披萨尺寸");
            if (pizza.crust.empty()) pizza.crust = "普通";
            if (pizza.bakeLevel.empty()) pizza.bakeLevel = "中等";
            
            return pizza;
        }
    };
};

// 使用示例
int main_fluent_builder() {
    // 流式接口：一行代码完成所有设置
    Pizza myPizza = Pizza::PizzaBuilder()
                        .setSize("大号")
                        .setCrust("薄脆")
                        .addTopping("蘑菇")
                        .addTopping("青椒")
                        .addTopping("意大利辣香肠")
                        .addExtraCheese()
                        .setBakeLevel("酥脆")
                        .build();
    
    myPizza.show();
    
    // 另一个不同组合
    Pizza veggiePizza = Pizza::PizzaBuilder()
                            .setSize("中号")
                            .addTopping("蘑菇")
                            .addTopping("青椒")
                            .addTopping("橄榄")
                            .addExtraSauce()
                            .build();
    
    veggiePizza.show();
    
    return 0;
}
