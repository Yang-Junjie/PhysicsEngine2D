#include "forcegenerator.h"

ForceGenerator::ForceGenerator()
{
}

ForceGenerator::~ForceGenerator()
{
}

void ForceGenerator::GenGravity(const oeVec2 gravity_acc,const float body_mass_)
{
    force_ += gravity_acc * body_mass_;
}

oeVec2 ForceGenerator::GenGravityAcc(const oeVec2 gravity_acc)
{
    return gravity_acc;
}

