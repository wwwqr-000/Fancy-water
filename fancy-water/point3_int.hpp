#pragma once
#include <iostream>
class point3_int {
public:
	int x, y, z;
	point3_int() : x(0), y(0), z(0) {}//Minimal init
	point3_int(int x, int y, int z) : x(x), y(y), z(z) {}//Fully init
};