#include "Rand.h"

bool Rand::isSeeded = false;
int Rand::seed = 1;

float Rand::rand() {
	if (!isSeeded) {
		seed = time(nullptr);
		isSeeded = true;
	}
	return fract(sin(seed++) * 1145.14);
}

int Rand::rand(int min,int max) {
	return (max - min) * rand() + min;
}

Vec2f Rand::rand2D() {
	return Vec2f(rand(),rand());
}

Vec3f Rand::rand3D() {
	return Vec3f(rand(),rand(),rand());
}

Vec4f Rand::rand4D() {
	return Vec4f(rand(), rand(), rand(), rand());
}