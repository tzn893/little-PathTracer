#include "../PNG/PNGImage.h"
#include "../Math/geometry.h"
#include "../Math/Ray.h"
#include "..//Math/Rand.h"
#include "..//Math/BVH.h"
//#include "../Math/Shape.h"
#include <iostream>
#include <vector>
#include <string>
#include <ppl.h>
#include <map>
using namespace std;
using namespace AABB;

Vec3f ray_cast(Ray r,vector<Shape*>& shapes,int depth);
Vec3f ray_cast(Ray r,BVHScene* scene,int depth);
Color color_space(Vec3f col);

vector<Shape*> make_scence();
map<string, Material*> mats;

Vec3f back_ground = Vec3f(.5,.6,1.);

int width = 1000, height = 800;

int MAX_DEPTH = 5;

int main() {

	PNGImage image(width, height);

	int ns = 5;

	Camera cam(3., width * ns, height * ns, Vec3f(0., 6., -6.),
		Vec3f(0.,0.,0.),Vec3f(0.,1.,0.),8.,0.);

	vector<Shape*> shapes = make_scence();

	BVHScene scene;
	scene.Create(shapes, cam.w);

	clock_t start = clock();
	//Concurrency::parallel_for(0, width, [&](int x) {
	for (int x = 0; x != width; x++){
		for (int y = 0; y != height; y++) {
			Vec3f col = { 0.,0.,0. };
			int totalSample = ns * ns;
			for (int s = 0; s != totalSample; s++) {
				float sx = x * ns + s % ns;
				float sy = y * ns + (s / ns);
				col = col + ray_cast(cam.get_ray(sx, sy), shapes, 0) / totalSample;
			}
			image.set(x, y, color_space(col));
		}
		if (x % 100 == 0) cout << x;
	//});
	}
	float time = float(clock() - start) / CLOCKS_PER_SEC;
	printf("\n总共用时%f",time);

	image.flip_vertically();
	image.save("a_bvh_test.png");
}

Color color_space(Vec3f col) {
	Vec3f norcol = sqrt(clamp3f(col,0.,1.));
	return Color(norcol.x * 255.,
		norcol.y * 255.,
		norcol.z * 255.,
		255.);
}

Vec3f ray_cast(Ray r,vector<Shape*>& shapes,int depth) {
	Vec3f col = {0.,0.,0.};
	bool flag = false;
	float last = 1e10;

	Insection Tinsec;
	for (int i = 0; i != shapes.size(); i++) {
		Insection insec = shapes[i]->insect(r);
		if (insec.isInsected && last > insec.t) {
			flag = true;
			Tinsec = insec;
			last = insec.t;
		}
	}
	if (!flag) {
		//float t = r.d.y * .5 + .5;
		return back_ground;
	}

	Material* mat = Tinsec.mat;
	Scatter scat = mat->scatter(r, Tinsec);
	if (depth != MAX_DEPTH) {
		if (scat.isScattred) {

			Vec3f attua = scat.attenuat;
			col = attua * ray_cast(scat.ray_out, shapes, depth + 1);
		}
		if (scat.isEmitting) {
			col = col + scat.emit;
		}
	}

	return col;
}

Vec3f ray_cast(Ray r, BVHScene* scene, int depth) {
	Vec3f col = {0.,0.,0.};

	Insection insec = scene->insect(r);
	if (!insec.isInsected) {
		return back_ground;
	}

	Material* mat = insec.mat;
	Scatter scat = mat->scatter(r, insec);
	if (depth != MAX_DEPTH) {

		if (scat.isScattred) {
			Vec3f attua = scat.attenuat;
			col = col + attua * ray_cast(scat.ray_out, scene, depth + 1);
		}
		if (scat.isEmitting) {
			col = col + scat.emit;
		}
	}

	return col;
}



vector<Shape*> make_scence() {
	mats["default"] = new Lambertian(Vec3f(1, .25, .25));
	mats["ground"] = new Lambertian(Vec3f(.4,.2,.3));
	mats["mirror"] = new Reflection();
	mats["metal_1"] = new Metal(.9,Vec3f(.8,.8,0));
	mats["metal_2"] = new Metal(.02, Vec3f(.3, .3, .3));
	mats["refract"] = new Dieletrics(2.4);
	mats["emit"] = new Emit(Vec3f(10.,10.,10.));

	Material* progMats[] = { new Lambertian(Vec3f(1.,.75,.4)),new Lambertian({.114,1.,.514}),
	new Lambertian({.55,.25,1.}) };


	Sphere* s = new Sphere(1., Vec3f(0., 0., 5.),mats["default"]);
	Sphere *s2 = new Sphere(100., Vec3f(0., -101., 5.),mats["ground"]);
	Sphere* s3 = new Sphere(1., Vec3f(-2., 0., 5.), mats["metal_1"]);
	Sphere* s4 = new Sphere(1., Vec3f(2.,0., 5.), mats["metal_2"]);
	Sphere* s5 = new Sphere(.4,Vec3f(0.,-.5,3.),mats["emit"]);
	Sphere* s6 = new Sphere(.4, Vec3f(-1.5, -0.5, 3.), mats["metal_2"]);
	Sphere* s7 = new Sphere(.4, Vec3f(1.5, -0.5, 3.), mats["default"]);

	vector<Shape*> shapes;
	shapes.push_back(s);
	shapes.push_back(s2);
	shapes.push_back(s3);
	shapes.push_back(s4);
	shapes.push_back(s5);
	shapes.push_back(s6);
	shapes.push_back(s7);

	return shapes;
}