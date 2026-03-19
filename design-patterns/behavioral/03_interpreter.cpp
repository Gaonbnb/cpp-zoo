#include <iostream>
#include <memory>
#include <string>

// 前向声明
class Context;

// 抽象表达式接口
class Expression {
public:
    virtual ~Expression() = default;
    // 解释操作，返回计算结果
    virtual int interpret() const = 0;
    // 用于打印表达式结构
    virtual std::string toString() const = 0;
};

// 上下文类 (本例中主要用于扩展变量功能，基础算术中暂未被深度使用)
class Context {
public:
    void setValue(const std::string& key, int value) {
        data[key] = value;
    }
    
    int getValue(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return it->second;
        }
        return 0; // 或者抛出异常
    }

private:
    std::map<std::string, int> data;
};

// 继承自 Expression
class NumberExpression : public Expression {
private:
    int number;

public:
    explicit NumberExpression(int num) : number(num) {}

    int interpret() const override {
        return number;
    }

    std::string toString() const override {
        return std::to_string(number);
    }
};
class AddExpression : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    // 接收所有权
    AddExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}

    int interpret() const override {
        return left->interpret() + right->interpret();
    }

    std::string toString() const override {
        return "(" + left->toString() + " + " + right->toString() + ")";
    }
};
class SubtractExpression : public Expression {
private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

public:
    SubtractExpression(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left(std::move(l)), right(std::move(r)) {}

    int interpret() const override {
        return left->interpret() - right->interpret();
    }

    std::string toString() const override {
        return "(" + left->toString() + " - " + right->toString() + ")";
    }
};
#include <iostream>
#include <memory>
// 假设上面的类定义都在同一个文件中，或者已包含 header

int main() {
    std::cout << "=== C++ Interpreter Pattern Demo ===" << std::endl;

    // 目标表达式: 10 + 5 - 2
    // 逻辑结构: (10 + 5) - 2

    // 1. 创建叶子节点 (数字)
    // 使用 make_unique 创建智能指针
    auto num10 = std::make_unique<NumberExpression>(10);
    auto num5 = std::make_unique<NumberExpression>(5);
    auto num2 = std::make_unique<NumberExpression>(2);

    // 2. 构建加法节点: 10 + 5
    // 注意：num10 和 num5 的所有权被转移给 AddExpression
    auto addExpr = std::make_unique<AddExpression>(std::move(num10), std::move(num5));

    // 3. 构建减法节点: (10 + 5) - 2
    // addExpr 的所有权被转移给 SubtractExpression
    auto finalExpr = std::make_unique<SubtractExpression>(std::move(addExpr), std::move(num2));

    // 4. 执行解释
    int result = finalExpr->interpret();
    std::string structure = finalExpr->toString();

    std::cout << "表达式结构: " << structure << std::endl;
    std::cout << "计算结果: " << result << std::endl;

    // 5. 内存清理
    // 不需要手动 delete！
    // 当 finalExpr 离开作用域时，unique_ptr 会自动递归调用析构函数，
    // 释放 SubtractExpression -> AddExpression -> NumberExpression 所有对象。
    
    return 0;
}
