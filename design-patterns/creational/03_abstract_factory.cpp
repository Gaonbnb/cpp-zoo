

#include <iostream>
#include <memory>

// 抽象产品：按钮
class Button {
public:
    virtual void render() = 0;
    virtual void onClick() = 0;
    virtual ~Button() = default;
};

// 抽象产品：文本框
class TextBox {
public:
    virtual void render() = 0;
    virtual void setText(const std::string& text) = 0;
    virtual ~TextBox() = default;
};

// 具体产品：Windows 按钮
class WindowsButton : public Button {
public:
    void render() override {
        std::cout << "渲染 Windows 风格的按钮" << std::endl;
    }
    void onClick() override {
        std::cout << "Windows 按钮被点击" << std::endl;
    }
};

// 具体产品：Windows 文本框
class WindowsTextBox : public TextBox {
private:
    std::string text;
public:
    void render() override {
        std::cout << "渲染 Windows 风格的文本框，内容: " << text << std::endl;
    }
    void setText(const std::string& text) override {
        this->text = text;
    }
};

// 具体产品：Mac 按钮
class MacButton : public Button {
public:
    void render() override {
        std::cout << "渲染 Mac 风格的按钮" << std::endl;
    }
    void onClick() override {
        std::cout << "Mac 按钮被点击" << std::endl;
    }
};

// 具体产品：Mac 文本框
class MacTextBox : public TextBox {
private:
    std::string text;
public:
    void render() override {
        std::cout << "渲染 Mac 风格的文本框，内容: " << text << std::endl;
    }
    void setText(const std::string& text) override {
        this->text = text;
    }
};

// 抽象工厂
class UIFactory {
public:
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<TextBox> createTextBox() = 0;
    virtual ~UIFactory() = default;
};

// 具体工厂：Windows 风格
class WindowsFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
    std::unique_ptr<TextBox> createTextBox() override {
        return std::make_unique<WindowsTextBox>();
    }
};

// 具体工厂：Mac 风格
class MacFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();
    }
    std::unique_ptr<TextBox> createTextBox() override {
        return std::make_unique<MacTextBox>();
    }
};

// 客户端代码
class Application {
private:
    std::unique_ptr<UIFactory> factory;
    std::unique_ptr<Button> button;
    std::unique_ptr<TextBox> textBox;
    
public:
    Application(std::unique_ptr<UIFactory> f) : factory(std::move(f)) {}
    
    void createUI() {
        button = factory->createButton();
        textBox = factory->createTextBox();
        
        textBox->setText("Hello World");
    }
    
    void render() {
        button->render();
        textBox->render();
    }
    
    void simulateClick() {
        button->onClick();
    }
};

int main() {
    std::cout << "=== Windows 应用 ===" << std::endl;
    Application winApp(std::make_unique<WindowsFactory>());
    winApp.createUI();
    winApp.render();
    winApp.simulateClick();
    
    std::cout << "\n=== Mac 应用 ===" << std::endl;
    Application macApp(std::make_unique<MacFactory>());
    macApp.createUI();
    macApp.render();
    macApp.simulateClick();
    
    return 0;
}
// simple factory,修改工厂
// factory method 新增子类
// abstract factory 新增产品，新增工厂
