#pragma once
#include"math.h"



class ForceGenerator {
private:
	oeVec2 force_ = { 0.0f,0.0f };
public:
	ForceGenerator();
	~ForceGenerator();
	oeVec2 GenGravity(const oeVec2 gravity_acc);
	oeVec2 GenGravityAcc(const oeVec2 gravity_acc);
	/*oeVec2 UniversalGravitation();*/
};