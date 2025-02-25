#ifndef OEIMGUIMANAGFR_HPP
#define OEIMGUIMANAGFR_HPP

#include "OEWindow.hpp"
#include <string>
#include <functional>
#include <iostream>

class OEImGuiManager{
public:
OEImGuiManager(OEWindow *window);
~OEImGuiManager();
private:
OEWindow *window_;
};


#endif