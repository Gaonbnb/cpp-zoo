
#include <iostream>
#include <string>
#include <memory>

// --- 子系统 1: 灯光 ---
class Lights {
public:
    void dim(int level) {
        std::cout << "[Lights] Dimming to " << level << "%" << std::endl;
    }
    void on() {
        std::cout << "[Lights] Turning on" << std::endl;
    }
};

// --- 子系统 2: 投影仪 ---
class Projector {
public:
    void on() { std::cout << "[Projector] Turning on" << std::endl; }
    void off() { std::cout << "[Projector] Turning off" << std::endl; }
    void setInput(std::string source) { 
        std::cout << "[Projector] Setting input to " << source << std::endl; 
    }
    void focus() { std::cout << "[Projector] Focusing lens" << std::endl; }
};

// --- 子系统 3: 音响系统 ---
class SoundSystem {
public:
    void on() { std::cout << "[SoundSystem] Turning on" << std::endl; }
    void off() { std::cout << "[SoundSystem] Turning off" << std::endl; }
    void setVolume(int level) { 
        std::cout << "[SoundSystem] Setting volume to " << level << std::endl; 
    }
    void setSurroundMode() { 
        std::cout << "[SoundSystem] Activating Surround Sound" << std::endl; 
    }
};

// --- 子系统 4: 爆米花机 ---
class PopcornMachine {
public:
    void heatUp() { std::cout << "[PopcornMachine] Heating up..." << std::endl; }
    void pop() { std::cout << "[PopcornMachine] Popping corn!" << std::endl; }
    void stop() { std::cout << "[PopcornMachine] Stopping" << std::endl; }
};

// --- 子系统 5: 屏幕 ---
class Screen {
public:
    void down() { std::cout << "[Screen] Lowering screen" << std::endl; }
    void up() { std::cout << "[Screen] Raising screen" << std::endl; }
};

class HomeTheaterFacade {
private:
    // 外观类持有所有子系统的实例
    std::shared_ptr<Lights> lights;
    std::shared_ptr<Projector> projector;
    std::shared_ptr<SoundSystem> sound;
    std::shared_ptr<PopcornMachine> popcorn;
    std::shared_ptr<Screen> screen;

public:
    // 构造函数初始化所有子系统
    HomeTheaterFacade(std::shared_ptr<Lights> l, 
                      std::shared_ptr<Projector> p, 
                      std::shared_ptr<SoundSystem> s, 
                      std::shared_ptr<PopcornMachine> pm, 
                      std::shared_ptr<Screen> sc)
        : lights(l), projector(p), sound(s), popcorn(pm), screen(sc) {}

    // --- 高级方法：一键看电影 ---
    // 封装了复杂的操作流程，客户端只需调用这一个函数
    void watchMovie(const std::string& movieName) {
        std::cout << "\n=== Getting ready to watch: " << movieName << " ===" << std::endl;
        
        // 按照正确的顺序操作所有设备
        popcorn->heatUp();
        popcorn->pop();
        
        lights->dim(10); // 调暗灯光
        
        screen->down();  // 降下屏幕
        
        projector->on();
        projector->setInput("Blu-Ray Player");
        projector->focus();
        
        sound->on();
        sound->setVolume(20);
        sound->setSurroundMode();
        
        std::cout << "=== Movie Started! Enjoy! ===\n" << std::endl;
    }

    // --- 高级方法：一键结束电影 ---
    void endMovie() {
        std::cout << "\n=== Shutting down theater... ===" << std::endl;
        
        popcorn->stop();
        
        lights->on(); // 打开灯光
        
        screen->up(); // 升起屏幕
        
        projector->off();
        
        sound->off();
        
        std::cout << "=== Theater closed. See you next time! ===\n" << std::endl;
    }
    
    // 也可以提供一些中间状态的方法，比如只调音量
    void setVolume(int level) {
        sound->setVolume(level);
    }
};

int main() {
    // 1. 创建所有子系统对象
    auto lights = std::make_shared<Lights>();
    auto projector = std::make_shared<Projector>();
    auto sound = std::make_shared<SoundSystem>();
    auto popcorn = std::make_shared<PopcornMachine>();
    auto screen = std::make_shared<Screen>();

    // 2. 创建外观对象，传入所有子系统
    HomeTheaterFacade homeTheater(lights, projector, sound, popcorn, screen);

    // 3. 客户端调用：只需一行代码！
    // 以前可能需要写20行代码来操作各个设备，现在只需一行
    homeTheater.watchMovie("Inception");

    // 吃点爆米花，看会儿电影...
    std::cout << "(Watching the movie...)" << std::endl;

    // 4. 结束电影：也是一行代码
    homeTheater.endMovie();

    // 5. 如果用户有特殊需求，依然可以直接操作子系统（外观模式不限制直接访问）
    std::cout << "\nUser wants to change volume manually:" << std::endl;
    sound->setVolume(5); // 直接调用子系统

    return 0;
}
