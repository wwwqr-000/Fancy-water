#pragma once
#include <iostream>
class point2 {
	public :
		double x, y;
		int x_i, y_i;
		std::string face;
		point2() : x(0.0), y(0.0) {}//Minimal init
		point2(double x, double y) : x(x), y(y) {}//Normal init
		point2(int x, int y) : x_i(x), y_i(y) {}//Normal init21
		point2(double x, double y, std::string face) : x(x), y(y), face(face) {}//Fully init
		point2(int x, int y, std::string face) : x_i(x), y_i(y), face(face) {}//Fully init

		bool operator==(const point2& p) const {
			return x_i == p.x_i && y_i == p.y_i;
		}
};