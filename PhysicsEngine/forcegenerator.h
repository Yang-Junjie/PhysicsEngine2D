#pragma once
#include"math.h"


class ForceGenerator {
public:
	ForceGenerator();
	~ForceGenerator();
	oeVec2 force_ = { 0.0f,0.0f };
	void GenGravity(const oeVec2 gravity_acc, const float body_mass_);
	oeVec2 GenGravityAcc(const oeVec2 gravity_acc);
	/*oeVec2 UniversalGravitation();*/
};