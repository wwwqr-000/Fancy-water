#pragma once
class point2 {
	public :
		double x, y;
		point2() : x(0.0), y(0.0) {}//Minimal init
		point2(double x, double y) { this->x = x; this->y = y; }//Fully init
};