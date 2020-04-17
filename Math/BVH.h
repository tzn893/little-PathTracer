#pragma once
#define BVH_DEBUG

#include "geometry.h"
#include "Ray.h"
#include <vector>

namespace AABB{
	
	class Shape;

	struct BoundBox {
		Vec3f min;
		Vec3f max;
		static BoundBox Bind(BoundBox a, BoundBox b);
		BoundBox():min(MAX,MAX,MAX),max(MIN,MIN,MIN) {}
		BoundBox(Vec3f min, Vec3f max) :min(min), max(max) {}
		bool insect(Ray r);
	};

	class Hitable {
	public:
		virtual bool get_bound_box(BoundBox&) = 0;
		virtual Insection insect(Ray r) = 0;
#ifdef BVH_DEBUG

		void set_name(const char* name) { this->name = name; }
	protected:
		std::string name;
#endif
	};

	class BVHNode : public  Hitable{
		friend class BVHScene;
	public:
		virtual bool get_bound_box(BoundBox& box) override;
		virtual Insection insect(Ray r) override;
		BVHNode():right(nullptr),left(nullptr) { }
		void SetLeft(Hitable* a) {
			BoundBox abox;
			a->get_bound_box(abox);
			box = BoundBox::Bind(abox, box);
			right = a;
		}
		void SetRight(Hitable* a) {
			BoundBox abox;
			a->get_bound_box(abox);
			box = BoundBox::Bind(abox, box);
			left = a;
		}
	private:
		Hitable* right;
		Hitable* left;
		BoundBox box;
	};

	class BVHScene : public Hitable {
	public:
		BVHScene():root(nullptr) {}

		virtual bool get_bound_box(BoundBox& box) override { return false; }//scene不能从外界获得BoundBox
		virtual Insection insect(Ray r) override;

		void CreateFromFile(const char* filename) {/*TODO*/}
		void Create(std::vector<Shape*>& items, Vec3f sort_dir = {1,0,0});//默认会是二叉树的形式创建场景,对场景物体排序默认是

	private:
		BVHNode* root;
		//BoundBox box;
	};

	class Shape : public Hitable {
		friend class BVHScene;
	public:
		Shape(Vec3f pos,Material* mat) :Pos(pos),mat(mat) {}
		Vec3f Pos;
		Material* mat;
	};

	class Sphere : public Shape {
		float radius;
		BoundBox box;
	public:
		Sphere(float radius, Vec3f Pos, Material* mat):Shape(Pos,mat),
		radius(radius){
			box.min = Pos - Vec3f(radius, radius, radius);
			box.max = Pos + Vec3f(radius, radius, radius);
		}

		virtual bool get_bound_box(BoundBox& box) override { box = this->box; return true; }
		virtual Insection insect(Ray r) override;
	};
}