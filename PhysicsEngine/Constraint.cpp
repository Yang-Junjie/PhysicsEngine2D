#include "Constraint.h"
#include "body.h"
#include "world.h"
#include "collision.h"




Arbiter::Arbiter()
{
}

Arbiter::Arbiter(oeBody* b1, oeBody* b2):body1(b1),body2(b2)
{

}
