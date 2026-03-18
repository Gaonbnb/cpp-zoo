
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>

// 1. 组件接口 (Component)
// 定义所有对象（文件和文件夹）的通用行为
class FileSystemComponent {
public:
    virtual ~FileSystemComponent() = default;
    
    // 显示层级结构
    virtual void display(int indent = 0) const = 0;
    
    // 获取总大小
    virtual long long get_size() const = 0;
    
    // 可选：添加/移除子节点的方法
    // 对于叶子节点（文件），这些方法通常抛出异常或什么都不做
    virtual void add(std::shared_ptr<FileSystemComponent> component) {
        // 默认实现：不做任何事或抛出异常
        // 这里选择静默忽略，或者你可以选择抛出一个运行时错误
        // throw std::logic_error("Cannot add to a leaf node.");
    }
    
    virtual void remove(std::shared_ptr<FileSystemComponent> component) {
        // 默认实现：不做任何事
    }
};

// 2. 叶子节点 (Leaf) - 文件
class File : public FileSystemComponent {
private:
    std::string name;
    long long size;

public:
    File(const std::string& n, long long s) : name(n), size(s) {}

    void display(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[File] " << name 
                  << " (" << size << " bytes)" << std::endl;
    }

    long long get_size() const override {
        return size;
    }
    
    // 文件不能添加子节点，使用基类的默认空实现即可
};

// 3. 容器节点 (Composite) - 文件夹
class Folder : public FileSystemComponent {
private:
    std::string name;
    std::vector<std::shared_ptr<FileSystemComponent>> children;

public:
    Folder(const std::string& n) : name(n) {}

    // 重写添加方法
    void add(std::shared_ptr<FileSystemComponent> component) override {
        children.push_back(component);
    }

    // 重写移除方法
    void remove(std::shared_ptr<FileSystemComponent> component) override {
        children.erase(
            std::remove(children.begin(), children.end(), component), 
            children.end()
        );
    }

    void display(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[Folder] " << name << std::endl;
        
        // 递归显示所有子节点
        for (const auto& child : children) {
            child->display(indent + 4); // 缩进增加
        }
    }

    long long get_size() const override {
        long long total_size = 0;
        // 递归计算所有子节点的大小之和
        for (const auto& child : children) {
            total_size += child->get_size();
        }
        return total_size;
    }
};

// 客户端代码
void client_code() {
    // 创建一些文件（叶子节点）
    auto file1 = std::make_shared<File>("report.pdf", 1024);
    auto file2 = std::make_shared<File>("image.png", 2048);
    auto file3 = std::make_shared<File>("notes.txt", 512);
    auto file4 = std::make_shared<File>("backup.zip", 5000);

    // 创建一些文件夹（容器节点）
    auto docs_folder = std::make_shared<Folder>("Documents");
    auto images_folder = std::make_shared<Folder>("Images");
    auto root_folder = std::make_shared<Folder>("Root");

    // 构建树形结构
    docs_folder->add(file1);
    docs_folder->add(file3);
    
    images_folder->add(file2);
    
    // 文件夹可以包含文件夹
    root_folder->add(docs_folder);
    root_folder->add(images_folder);
    root_folder->add(file4); // 根目录下也可以直接放文件

    std::cout << "=== File System Structure ===" << std::endl;
    // 客户端只需要调用 display，不需要关心是文件还是文件夹
    root_folder->display();

    std::cout << "\n=== Total Size Calculation ===" << std::endl;
    std::cout << "Total size of Root: " << root_folder->get_size() << " bytes" << std::endl;
    std::cout << "Size of Documents: " << docs_folder->get_size() << " bytes" << std::endl;
    std::cout << "Size of image.png: " << file2->get_size() << " bytes" << std::endl;

    // 演示移除操作
    std::cout << "\n=== Removing notes.txt from Documents ===" << std::endl;
    docs_folder->remove(file3);
    docs_folder->display();
    std::cout << "New size of Documents: " << docs_folder->get_size() << " bytes" << std::endl;
}

int main() {
    client_code();
    return 0;
}
