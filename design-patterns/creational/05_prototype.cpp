

// 拷贝构造函数：C++中使用拷贝构造函数实现对象的复制
// 虚clone方法：返回Shape*，实现多态克隆
// 原型管理器：ShapeCache类管理原型对象
// 深拷贝：每个克隆对象都是独立的，修改互不影响
// 内存管理：需要手动管理内存，或使用智能指针


#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

using namespace std;

// 1. 原型抽象类
class Shape {
protected:
    string id;
    string type;
    int x;
    int y;
    string color;
    
public:
    Shape() : x(0), y(0) {}
    
    // 拷贝构造函数 - 实现克隆的基础
    Shape(const Shape& source) {
        this->id = source.id;
        this->type = source.type;
        this->x = source.x;
        this->y = source.y;
        this->color = source.color;
    }
    
    virtual ~Shape() {}
    
    // 纯虚函数 - 克隆方法
    virtual Shape* clone() const = 0;
    
    // 虚函数 - 绘制图形
    virtual void draw() const {
        cout << "Drawing a " << color << " " << type 
             << " at position (" << x << ", " << y << ")";
    }
    
    // Setter方法
    void setId(const string& id) { this->id = id; }
    void setPosition(int x, int y) { this->x = x; this->y = y; }
    void setColor(const string& color) { this->color = color; }
    
    string getId() const { return id; }
    string getType() const { return type; }
};

// 2. 具体原型类 - 圆形
class Circle : public Shape {
private:
    int radius;
    
public:
    Circle() : Shape(), radius(10) {
        type = "Circle";
    }
    
    // 拷贝构造函数
    Circle(const Circle& source) : Shape(source) {
        this->radius = source.radius;
    }
    
    Shape* clone() const override {
        return new Circle(*this);
    }
    
    void setRadius(int radius) { this->radius = radius; }
    
    void draw() const override {
        Shape::draw();
        cout << ", Radius: " << radius << endl;
    }
};

// 3. 具体原型类 - 矩形
class Rectangle : public Shape {
private:
    int width;
    int height;
    
public:
    Rectangle() : Shape(), width(20), height(10) {
        type = "Rectangle";
    }
    
    // 拷贝构造函数
    Rectangle(const Rectangle& source) : Shape(source) {
        this->width = source.width;
        this->height = source.height;
    }
    
    Shape* clone() const override {
        return new Rectangle(*this);
    }
    
    void setDimensions(int width, int height) {
        this->width = width;
        this->height = height;
    }
    
    void draw() const override {
        Shape::draw();
        cout << ", Dimensions: " << width << " x " << height << endl;
    }
};

// 4. 原型管理器 - 用于存储和管理原型
class ShapeCache {
private:
    unordered_map<string, Shape*> shapes;
    
public:
    ~ShapeCache() {
        // 清理所有原型对象
        for (auto& pair : shapes) {
            delete pair.second;
        }
        shapes.clear();
    }
    
    // 加载原型到缓存
    void loadCache() {
        // 创建圆形原型
        Circle* circle = new Circle();
        circle->setId("1");
        circle->setColor("Red");
        circle->setPosition(100, 100);
        circle->setRadius(15);
        shapes["circle"] = circle;
        
        // 创建矩形原型
        Rectangle* rectangle = new Rectangle();
        rectangle->setId("2");
        rectangle->setColor("Blue");
        rectangle->setPosition(200, 200);
        rectangle->setDimensions(30, 20);
        shapes["rectangle"] = rectangle;
        
        // 创建另一个圆形原型
        Circle* circle2 = new Circle();
        circle2->setId("3");
        circle2->setColor("Green");
        circle2->setPosition(300, 300);
        circle2->setRadius(25);
        shapes["big_circle"] = circle2;
    }
    
    // 获取克隆对象
    Shape* getShape(const string& type) {
        auto it = shapes.find(type);
        if (it != shapes.end()) {
            return it->second->clone();
        }
        return nullptr;
    }
    
    // 注册新原型
    void registerShape(const string& key, Shape* shape) {
        shapes[key] = shape;
    }
};

// 5. 客户端代码
int main() {
    cout << "=== 原型模式演示 ===" << endl;
    
    // 创建并加载原型缓存
    ShapeCache shapeCache;
    shapeCache.loadCache();
    
    // 使用原型创建对象
    cout << "\n--- 从原型创建对象 ---" << endl;
    
    // 克隆圆形
    Shape* clonedCircle = shapeCache.getShape("circle");
    if (clonedCircle) {
        cout << "克隆的圆形 (原始): ";
        clonedCircle->draw();
        
        // 修改克隆对象的属性
        clonedCircle->setColor("Purple");
        clonedCircle->setPosition(150, 150);
        cout << "克隆的圆形 (修改后): ";
        clonedCircle->draw();
    }
    
    // 克隆矩形
    Shape* clonedRectangle = shapeCache.getShape("rectangle");
    if (clonedRectangle) {
        cout << "\n克隆的矩形: ";
        clonedRectangle->draw();
    }
    
    // 克隆大圆形
    Shape* clonedBigCircle = shapeCache.getShape("big_circle");
    if (clonedBigCircle) {
        cout << "\n克隆的大圆形: ";
        clonedBigCircle->draw();
    }
    
    // 6. 演示原型注册表动态添加原型
    cout << "\n--- 动态添加新原型 ---" << endl;
    
    // 创建新的原型
    Circle* customCircle = new Circle();
    customCircle->setId("4");
    customCircle->setColor("Yellow");
    customCircle->setPosition(400, 400);
    customCircle->setRadius(30);
    
    // 注册到缓存
    shapeCache.registerShape("custom_circle", customCircle);
    
    // 使用新原型
    Shape* clonedCustom = shapeCache.getShape("custom_circle");
    if (clonedCustom) {
        cout << "从动态原型克隆: ";
        clonedCustom->draw();
    }
    
    // 7. 演示深拷贝的重要性
    cout << "\n--- 验证深拷贝 ---" << endl;
    
    // 创建两个克隆，修改一个不影响另一个
    Shape* circle1 = shapeCache.getShape("circle");
    Shape* circle2 = shapeCache.getShape("circle");
    
    if (circle1 && circle2) {
        // 修改第一个
        circle1->setColor("Black");
        circle1->setPosition(500, 500);
        
        cout << "Circle1 (修改后): ";
        circle1->draw();
        
        cout << "Circle2 (未修改): ";
        circle2->draw();
    }
    
    // 清理内存
    delete clonedCircle;
    delete clonedRectangle;
    delete clonedBigCircle;
    delete clonedCustom;
    delete circle1;
    delete circle2;
    
    // shapeCache的析构函数会自动清理原型对象
    
    cout << "\n=== 演示结束 ===" << endl;
    
    return 0;
}

// 8. 使用智能指针的改进版本（可选）
#include <memory>

class ModernShapeCache {
private:
    unordered_map<string, shared_ptr<Shape>> shapes;
    
public:
    void loadCache() {
        shapes["circle"] = make_shared<Circle>();
        shapes["rectangle"] = make_shared<Rectangle>();
    }
    
    shared_ptr<Shape> getShape(const string& type) {
        auto it = shapes.find(type);
        if (it != shapes.end()) {
            return shared_ptr<Shape>(it->second->clone());
        }
        return nullptr;
    }
};
