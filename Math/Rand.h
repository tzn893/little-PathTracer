#include "geometry.h"
#include <ctime>


class Rand {
	static int seed;
	static bool isSeeded;
public:
	static float rand();
	static Vec2f rand2D();
	static Vec3f rand3D();
	static Vec4f rand4D();

	static int rand(int min, int max);

	Rand() = delete;

};