#include <iostream>
#include <vector>
#include <string>
#include <memory>

// ==========================================
// 1. 抽象迭代器接口 (Abstract Iterator)
// ==========================================
template <typename T>
class Iterator {
public:
    virtual ~Iterator() = default;
    
    // 判断是否还有下一个元素
    virtual bool hasNext() const = 0;
    
    // 返回下一个元素并移动游标
    virtual T next() = 0;
    
    // 重置游标到开始位置
    virtual void reset() = 0;
};

// ==========================================
// 2. 抽象聚合接口 (Abstract Aggregate)
// ==========================================
template <typename T>
class Container {
public:
    virtual ~Container() = default;
    
    // 工厂方法：创建迭代器
    // 返回智能指针以自动管理内存
    virtual std::unique_ptr<Iterator<T>> createIterator() = 0;
};

// ==========================================
// 3. 具体聚合类：频道集合 (Concrete Aggregate)
// ==========================================
// 前向声明具体迭代器，以便在集合类中友元访问或传递 this
template <typename T>
class ChannelIterator;

template <typename T>
class ChannelCollection : public Container<T> {
private:
    std::vector<T> channels;

public:
    void addChannel(const T& channel) {
        channels.push_back(channel);
    }

    // 实现工厂方法
    std::unique_ptr<Iterator<T>> createIterator() override {
        return std::make_unique<ChannelIterator<T>>(this);
    }

    // 提供给迭代器访问内部数据的接口 (封装在类内部，外部不可见)
    const std::vector<T>& getChannels() const {
        return channels;
    }
    
    size_t size() const {
        return channels.size();
    }
};

// ==========================================
// 4. 具体迭代器类：频道迭代器 (Concrete Iterator)
// ==========================================
template <typename T>
class ChannelIterator : public Iterator<T> {
private:
    const ChannelCollection<T>* collection;
    size_t currentIndex;

public:
    explicit ChannelIterator(const ChannelCollection<T>* coll) 
        : collection(coll), currentIndex(0) {}

    bool hasNext() const override {
        return currentIndex < collection->getChannels().size();
    }

    T next() override {
        if (!hasNext()) {
            throw std::out_of_range("No more channels available");
        }
        return collection->getChannels()[currentIndex++];
    }

    void reset() override {
        currentIndex = 0;
    }
};

// ==========================================
// 5. 客户端测试 (Client)
// ==========================================
int main() {
    std::cout << "=== C++ Iterator Pattern Demo ===" << std::endl;

    // 1. 创建聚合对象并填充数据
    ChannelCollection<std::string> tvChannels;
    tvChannels.addChannel("CCTV-1 News");
    tvChannels.addChannel("HBO Movies");
    tvChannels.addChannel("ESPN Sports");
    tvChannels.addChannel("Discovery Channel");
    tvChannels.addChannel("Cartoon Network");

    std::cout << "Total channels: " << tvChannels.size() << std::endl;
    std::cout << "---------------------------------" << std::endl;

    // 2. 获取迭代器
    // 客户端不需要知道内部是 vector 还是 list
    auto iterator = tvChannels.createIterator();

    // 3. 第一次遍历
    std::cout << "--- First Pass (All Channels) ---" << std::endl;
    while (iterator->hasNext()) {
        std::cout << "Watching: " << iterator->next() << std::endl;
    }

    std::cout << "---------------------------------" << std::endl;

    // 4. 重置并再次遍历 (带过滤逻辑)
    iterator->reset();
    std::cout << "--- Second Pass (Only 'C' Channels) ---" << std::endl;
    
    while (iterator->hasNext()) {
        std::string channel = iterator->next();
        // 业务逻辑：只处理包含 "C" 的频道
        if (channel.find('C') != std::string::npos) {
            std::cout << "Selected: " << channel << std::endl;
        }
    }

    // 5. 异常处理演示 (可选)
    std::cout << "---------------------------------" << std::endl;
    try {
        if (!iterator->hasNext()) {
            std::cout << "End of list reached. Calling next() will throw." << std::endl;
            iterator->next(); // 这将抛出异常
        }
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // 内存管理：
    // unique_ptr 会在 main 函数结束时自动销毁 iterator 对象
    // ChannelCollection 是栈对象，也会自动析构
    
    return 0;
}
