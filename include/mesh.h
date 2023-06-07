#pragma once

#include "vecmat.h"


struct Triangle{

Vector4 _vert[3];

Vector4 normal();
double minZ() const;

};