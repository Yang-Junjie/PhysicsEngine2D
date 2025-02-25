#include "OEGraphics.hpp"

OEWindow window(800, 600, "Physics Engine");
void runCode(){
   


}

int main() {
   
    if (!window.init()) {
        std::cerr << "Failed to initialize window!" << std::endl;
        return -1;
    }

    std::function<void()> runcode = runCode;
    window.runCode(runcode);
    window.run();

    return 0;
}