

// 内存节省：
// 如果没有享元模式，放置 100 个 "I" 方块需要创建 100 个对象，每个对象都存一份形状坐标数据。
// 使用享元模式，无论放置多少个 "I" 方块，内存中只有 1 个 ConcreteTetromino 对象。其他 99 次调用只是复用了这个指针。
// 如果有 7 种标准方块，内存中永远最多只有 7 个对象，而不是成千上万个。
// 内部状态 vs 外部状态：
// 内部状态（在 ConcreteTetromino 中）：shapeType, color, relativeCoords。这些是不变的，所以可以共享。
// 外部状态（在 draw 参数中）：boardX, boardY。这些是变化的，每次调用都不同，所以不能存在享元对象里，必须由客户端传入。
// 线程安全提示：
// 上面的 TetrominoFactory 不是线程安全的。如果在多线程环境下（如游戏主线程和资源加载线程），需要对 flyweights map 加锁，或者使用双重检查锁定（Double-Checked Locking）。
// 享元对象本身（ConcreteTetromino）必须是不可变（Immutable）的。一旦创建，其内部状态就不能被修改，否则一个线程修改了颜色，另一个线程看到的颜色也会变，导致严重 Bug

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

// 坐标结构体
struct Position {
    int x, y;
};

// 抽象享元类：方块
class Tetromino {
public:
    virtual ~Tetromino() = default;
    
    // 绘制方法：接收外部状态（位置）
    virtual void draw(int boardX, int boardY) const = 0;
    
    // 获取内部状态描述（用于调试）
    virtual std::string getType() const = 0;
};

// 具体享元：定义不同形状的方块
class ConcreteTetromino : public Tetromino {
private:
    std::string shapeType;      // 内部状态：形状类型 ("I", "O", "T", etc.)
    std::string color;          // 内部状态：颜色
    std::vector<std::pair<int, int>> relativeCoords; // 内部状态：相对坐标形状

public:
    ConcreteTetromino(std::string type, std::string col, std::vector<std::pair<int, int>> coords)
        : shapeType(type), color(col), relativeCoords(coords) {}

    // 绘制时，将内部状态（相对坐标）与外部状态（boardX, boardY）结合
    void draw(int boardX, int boardY) const override {
        std::cout << "[" << shapeType << "-" << color << "] drawn at (" 
                  << boardX << ", " << boardY << ")" << std::endl;
        
        // 模拟渲染逻辑：遍历相对坐标，加上外部偏移量
        // for (const auto& offset : relativeCoords) {
        //     renderPixel(boardX + offset.first, boardY + offset.second, color);
        // }
    }

    std::string getType() const override {
        return shapeType;
    }
};

class TetrominoFactory {
private:
    // 对象池：Key 是形状类型，Value 是共享的享元对象
    std::map<std::string, std::shared_ptr<Tetromino>> flyweights;

    // 辅助函数：生成形状的相对坐标（简化版）
    std::vector<std::pair<int, int>> getShapeCoords(const std::string& type) {
        if (type == "I") return {{0,0}, {1,0}, {2,0}, {3,0}};
        if (type == "O") return {{0,0}, {1,0}, {0,1}, {1,1}};
        if (type == "T") return {{0,0}, {1,0}, {2,0}, {1,1}};
        if (type == "L") return {{0,0}, {0,1}, {0,2}, {1,2}};
        return {}; // 默认空
    }
    
    // 辅助函数：获取对应颜色
    std::string getColor(const std::string& type) {
        if (type == "I") return "Cyan";
        if (type == "O") return "Yellow";
        if (type == "T") return "Purple";
        if (type == "L") return "Orange";
        return "White";
    }

public:
    // 获取享元对象
    std::shared_ptr<Tetromino> getTetromino(const std::string& type) {
        // 1. 检查池中是否已有
        if (flyweights.find(type) != flyweights.end()) {
            std::cout << "  [Factory] Reusing existing '" << type << "' tetromino." << std::endl;
            return flyweights[type];
        }

        // 2. 如果没有，创建新对象并放入池中
        std::cout << "  [Factory] Creating NEW '" << type << "' tetromino." << std::endl;
        auto newTetro = std::make_shared<ConcreteTetromino>(
            type, 
            getColor(type), 
            getShapeCoords(type)
        );
        flyweights[type] = newTetro;
        return newTetro;
    }

    // 统计池中对象数量
    int getCount() const {
        return flyweights.size();
    }
};

class GameBoard {
private:
    TetrominoFactory factory;

public:
    void placePiece(const std::string& type, int x, int y) {
        // 1. 从工厂获取共享的享元对象（内部状态）
        std::shared_ptr<Tetromino> piece = factory.getTetromino(type);
        
        // 2. 调用绘制方法，传入外部状态（x, y）
        piece->draw(x, y);
    }

    void printStats() {
        std::cout << "\nTotal unique tetromino types created in memory: " 
                  << factory.getCount() << std::endl;
        std::cout << "(If we placed 1000 'I' pieces, we still only have 1 object in memory!)\n";
    }
};

int main() {
    GameBoard board;

    std::cout << "=== Starting Game ===" << std::endl;

    // 放置几个方块
    // 第一次请求 "I" -> 工厂创建新对象
    board.placePiece("I", 5, 0); 
    
    // 第一次请求 "O" -> 工厂创建新对象
    board.placePiece("O", 2, 0); 
    
    // 第二次请求 "I" -> 工厂直接返回之前的对象 (复用!)
    board.placePiece("I", 5, 4); 
    
    // 第三次请求 "I" -> 继续复用
    board.placePiece("I", 8, 0); 

    // 请求 "T" -> 创建新对象
    board.placePiece("T", 3, 2);

    // 再次请求 "O" -> 复用
    board.placePiece("O", 6, 5);

    std::cout << "\n=== Placing 100 more 'I' pieces silently... ===" << std::endl;
    for(int i=0; i<100; ++i) {
        // 这里不会创建任何新对象，全部复用
        board.placePiece("I", i % 10, i / 10); 
    }

    board.printStats();

    return 0;
}
