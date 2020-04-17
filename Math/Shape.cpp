#include "Shape.h"

Insection Sphere::insect(Ray r) {
	Vec3f pc = r.p - C;
	Vec3f d = r.d;

	float a = dot(d, d);
	float b = dot(d, pc) * 2;
	float c = dot(pc, pc) - radius * radius;

	Insection insec;

	float delta = b * b - 4 * a * c;
	if (delta < 0) {
		insec.isInsected = false;
		return insec;
	}

	float t = (-b - sqrt(delta)) / (2 * a);
	/*if (insec.t < 0 || equalZero(insec.t)) {
		insec.isInsected = false;
		return insec;
	}*/

	if (t < 0) {
		t = (-b + sqrt(delta)) / (2 * a);
		if (t < 0) {
			insec.isInsected = false;
			return insec;
		}
	}

	Vec3f p = r.Pos(t);
	insec.normal = (p - C).normalize();
	insec.pos = p + insec.normal * 1e-3;
	insec.t = t;
	insec.mat = mat;
	return insec;
}