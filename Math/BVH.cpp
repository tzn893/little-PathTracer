#include "BVH.h"
using namespace AABB;
using namespace std;

inline Vec3f mmax(Vec3f a, Vec3f b) {
	return TwoHandOperator<Vec3f, 3, float>(a, b, mmax);
}
inline Vec3f mmin(Vec3f a, Vec3f b) {
	return TwoHandOperator<Vec3f, 3, float>(a, b, mmin);
}

BoundBox BoundBox::Bind(BoundBox a,BoundBox b) {
	return BoundBox(mmin(a.min, b.min),
		mmax(a.max, b.max));
}

bool BoundBox::insect(Ray r) {
	for (int i = 0; i != 3; i++) {
		float t0 = (min.raw[i] - r.p.raw[i]) / r.d.raw[i];
		float t1 = (max.raw[i] - r.p.raw[i]) / r.d.raw[i];
		if (t0 > t1) std::swap(t0, t1);
		if (mmax(t0, 0) > mmin(t1, 1e10))
			return false;
	}
	return true;
}

bool BVHNode::get_bound_box(BoundBox& box) {
	if (!left && !right) return false;
	box = this->box;
	return true;
}

Insection BVHNode::insect(Ray r) {
	BoundBox current;
	Insection insec;
	insec.isInsected = false;
	insec.t = 1e10;
	if (left != nullptr && left->get_bound_box(current) && current.insect(r)) {
		Insection linsec = left->insect(r);
		if (linsec.isInsected)
			insec = linsec;
	}
	if (right != nullptr && right->get_bound_box(current) && current.insect(r)) {
		Insection rinsec = right->insect(r);
		if (rinsec.isInsected) {
			insec = insec.t < rinsec.t ? insec : rinsec;
		}
	}

	return insec;
}

void do_dir_sort(vector<Shape*>& target, int start, int end, Vec3f dir) {
	if (end - start <= 1) return;
	int mp = start + 1, cp = mp;
	while (cp < end) {
		if (dot(dir, target[cp]->Pos) < dot(dir, target[start]->Pos)) {
			swap(target[cp], target[mp++]);
		}
		cp++;
	}
	swap(target[start], target[mp - 1]);
	do_dir_sort(target, start, mp, dir);
	do_dir_sort(target, mp, end, dir);
}

void shape_sort(vector<Shape*>& shapes,Vec3f dir) {
	do_dir_sort(shapes, 0, shapes.size(), dir);
}

Hitable* CreateFromSubArray(vector<Shape*>& shapes,int start,int end) {
	BVHNode* node = new BVHNode();

	switch (end - start) {
	case 1:
		return shapes[start];
	case 2:
		node->SetLeft(shapes[start]);
		node->SetRight(shapes[start + 1]);
		return node;
	default:
		int mid = (end + start) / 2;
		node->SetLeft(CreateFromSubArray(shapes,start,mid));
		node->SetRight(CreateFromSubArray(shapes, mid, end));
		return node;
	}

}


void BVHScene::Create(vector<Shape*>& shapes,Vec3f dir) {
	shape_sort(shapes, dir);
	root = new BVHNode();

	switch (shapes.size()) {
	case 1:
		root->SetLeft(shapes[0]);
	case 2:
		root->SetLeft(shapes[0]);
		root->SetRight(shapes[1]);
	default:
		int mid = shapes.size() / 2;
		root->SetLeft(CreateFromSubArray(shapes, 0, mid));
		root->SetRight(CreateFromSubArray(shapes, mid, shapes.size()));
	}
}

Insection BVHScene::insect(Ray r) {
	BoundBox box;
	Insection res;
	res.isInsected = false;
	if (root->get_bound_box(box) && box.insect(r)) {
		res = root->insect(r);
	}
	return res;
}

Insection Sphere::insect(Ray r) {
	Vec3f pc = r.p - Pos;
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

	if (t < 0) {
		t = (-b + sqrt(delta)) / (2 * a);
		if (t < 0) {
			insec.isInsected = false;
			return insec;
		}
	}

	Vec3f p = r.Pos(t);
	insec.normal = (p - Pos).normalize();
	insec.pos = p + insec.normal * 1e-3;
	insec.t = t;
	insec.mat = mat;
	return insec;
}