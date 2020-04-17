#pragma once
#include "geometry.h"

struct Insection;

struct Ray {
	Vec3f p, d;
	Ray(Vec3f dir, Vec3f pos = Vec3f(0, 0, 0)) : d(dir.normalize()) , p(pos){}
	Ray() {}
	Vec3f Pos(float t) { return  t * d  + p; }
};

struct Scatter {
	bool isScattred = false;
	Ray ray_out;
	Vec3f attenuat;
	bool isEmitting = false;
	Vec3f emit;
};

struct Camera {
	Vec3f pos;
	Vec3f u, v, w;
	float proj;
	float width,height;
	float len_radius;
	float foucs_dis;
	bool  defoucs;

	Ray get_ray(float u, float v);
	Ray get_ray(Vec2f uv) { return get_ray(uv.u,uv.v); }

	Camera(float proj, int width, int height, Vec3f pos,
		Vec3f look_at,Vec3f up,float foucs,float aperture = 0.) :
		proj(proj), width(width), height(height), pos(pos)
	{ 
		Vec3f look_from = pos;
		v = (look_at - look_from).normalize();
		w = cross(up, v).normalize();
		u = cross(v, w).normalize();
		defoucs = !equalZero(aperture);
		foucs_dis = foucs;
		len_radius = aperture / 2.;
	}
};

class Material {
public:
	virtual Scatter scatter(Ray in, Insection insec) = 0;
	virtual ~Material() {}
};

class Lambertian : public Material {
	Vec3f albedo;
public:
	Lambertian(Vec3f albedo):albedo(albedo) {}
	virtual Scatter scatter(Ray in, Insection insec) override;
};

class Reflection : public Material {
	Vec3f albedo;
public:
	Reflection(Vec3f albedo = Vec3f(1.,1.,1.)) :albedo(albedo) {}
	virtual Scatter scatter(Ray in, Insection insec) override;
};

class Metal : public Material {
	Vec3f albedo;
	float Roughness;
public:
	Metal(float Roughness, Vec3f albedo = Vec3f(.5, .5, .5)) :
		Roughness(Roughness), albedo(albedo) {}

	virtual Scatter scatter(Ray in, Insection insec) override;
};

class Dieletrics : public Material{
	float ni;
public:
	Dieletrics(float ni) : ni(ni) {}
	virtual Scatter scatter(Ray in, Insection insec) override;
};

class Emit : public Material {
	Vec3f albedo;
	float  far;
public:
	Emit(Vec3f albedo,float far = 5.):albedo(albedo),far(far) {}
	virtual Scatter scatter(Ray in, Insection insec) override;
};

struct Insection {
	bool  isInsected;
	Vec3f pos;
	float t;
	Vec3f normal;
	Material* mat;
};

