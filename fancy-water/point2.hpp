#pragma once
#include <iostream>
class point2 {
	public :
		double x, y;
		std::string face;
		point2() : x(0.0), y(0.0) {}//Minimal init
		point2(double x, double y) : x(x), y(y) {}//Normal init
		point2(double x, double y, std::string face) : x(x), y(y), face(face) {}//Fully init
};