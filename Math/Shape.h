#pragma once
#include "Ray.h"

class Shape {
public:
	virtual Insection insect(Ray r) = 0;
	virtual ~Shape() {}
};

class Sphere : public Shape {
	float radius;
	Vec3f C;
	Material* mat;
public:
	Sphere(float r, Vec3f pos, Material* mat) :radius(r), C(pos), mat(mat) {}
	virtual Insection insect(Ray r) override;
};