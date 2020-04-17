#include "Ray.h"
#include "Rand.h"

Vec3f rand_sphere_dir() {
	Vec3f d = Rand::rand3D() * 2. - Vec3f(1., 1., 1.);
	return d.normalize(Rand::rand());
}


Ray Camera::get_ray(float x,float y) {
	x = (x / width * 2. - 1.) * width / height;
	y = y / height * 2. - 1.;
	Ray r;

	if (defoucs) {

		Vec3f dir = (x * w + y * u + v * proj).normalize();
		Vec3f offset = rand_sphere_dir() * len_radius;
		dir = dir * foucs_dis - offset;
		
		r = Ray(dir, pos + offset);
	} else{
		Vec3f dir = (x * w + y * u + v * proj);
		r = Ray(dir, pos);
	}
	return r;
}


Vec3f reflect(Vec3f l,Vec3f n) {
	return l - 2 * n * dot(n, l);
}

bool refract(Vec3f l,Vec3f n,float ni,Vec3f& out_ray) {
	float cos_l = dot(l, n);
	float cos_refrac = 1 - ni * ni * (1 - cos_l * cos_l);
	if (cos_refrac > 0) {
		out_ray = n * sqrt(cos_refrac) + (l - cos_l * n) * ni;
		return true;
	}
	else
		return false;
}

Scatter Lambertian::scatter(Ray in,Insection insec) {
	Scatter scat;
	if (!insec.isInsected) {
		scat.isScattred = false;
		return scat;
	}
	Vec3f dir = insec.normal + rand_sphere_dir();
	Vec3f pos = insec.pos;
	Ray r(dir, pos);
	scat.isScattred = true;
	scat.attenuat = albedo;
	scat.ray_out = r;

	return scat;
}

Scatter Reflection::scatter(Ray in,Insection insec) {
	Scatter scat;
	if (!insec.isInsected) {
		scat.isScattred = false;
		return scat;
	}
	Vec3f dir = reflect(in.d,insec.normal);
	Vec3f pos = insec.pos;
	Ray r(dir, pos);
	scat.ray_out = r;
	scat.isScattred = true;
	scat.attenuat = albedo;
	return scat;
}

Scatter Metal::scatter(Ray in,Insection insec) {
	Scatter scat;
	if (!insec.isInsected) {
		scat.isScattred = false;
		return scat;
	}
	Vec3f dir = reflect(in.d,insec.normal) + (1 - Roughness) * rand_sphere_dir();
	Vec3f pos = insec.pos;
	Ray r(dir, pos);
	scat.ray_out = r;
	scat.isScattred = true;
	scat.attenuat = albedo;
	return scat;
}

Scatter Dieletrics::scatter(Ray r,Insection insec) {
	Scatter scat;
	if (!insec.isInsected) {
		scat.isScattred = false;
		return scat;
	}
	Vec3f reflec = reflect(r.d, insec.normal);
	Vec3f forward_normal;
	float forward_ni;
	if (dot(r.d, insec.normal) < 0) {
		forward_normal = insec.normal;
		forward_ni = ni;
	}else {
		forward_normal = Vec3f() -insec.normal;
		forward_ni = 1 / ni;
	}
	Vec3f dir;
	Vec3f pos = insec.pos;
	Vec3f albedo = Vec3f(1., 1., 1.);

	Ray out_r;
	if (!refract(r.d, forward_normal, forward_ni, dir)) {
		out_r = Ray(reflec, pos);
	}else {
		out_r = Ray(dir, pos);
	}

	scat.attenuat = albedo;
	scat.isScattred = true;
	scat.ray_out = out_r;
	return scat;
}

Scatter Emit::scatter(Ray /*r*/, Insection /*insec*/) {
	Scatter scat;
	scat.isScattred = false;
	scat.isEmitting = true;
	scat.emit = albedo;

	return scat;
}