#pragma once

#include <vector>
using std::vector;

#include "bundle.h"

class Shape {
	public:
		virtual vector<Scanline> Rasterize() = 0;

		// Call delete on this
		virtual Shape* Copy() = 0;
		virtual void Mutate() = 0;
		virtual char* BORST(char* attrs) = 0;
};

enum ShapeType {
	// TODO: Create a circle line class that could be used to speed up the drawing process
	ShapeTypeCircle,

	// [Exprimental]
	ShapeTypeCircleLine,
};