# little-PathTracer

Recently, I'm working on a tutorial about ray tracing and here is my progress.

![spheres](/Debug/a_emit.png)

At present, the path tracer's effiency is still pretty low.The picture you see above takes about 10 mintues to render by my laptop.Maybe I can use GPU to 
speed up the algorithm sometime.

I choose to save the result into a png file.The functions about output are all in the PNG directory.Then every thing about math and ray tracing are all 
in the Math directory.The file geometry.h packed some geometry objects such as vectors,matrixs in it.Meanwhile,Ray.h defines struct Ray ,Intersection,
Scatter,Material in it,which are the core of the path tracing algorithm.They describe the information about ray and the surface the rays intersect with.
To describe the objects to render in the scene,there are two classes : Shape and AABB::Shape.Shape is defined in the file Shape.h, rays can intersect the
shapes by interface Shape::insect.Compare to the Shape class,the AABB::Shape defined in BVH.h have AABB bounding box.The BVHScene can accelerate the 
intersections by organizing shapes in to binary tree and testing bounding boxes(This part seems still have some problem,I hope I can fix it sometime).
Camera is the object casting ray into the scene.By giving out the pixel cooridnate,it can cast a corresponding ray.It can also perfrom depth of
field by paramter foucs and aperture like the picture below.

![depth of field](/Debug/a_b.png)

To Do:

1.Fix the BVH

2.Add more types of shapes

3.Support rendering to video like .avi

4.Accelerate the program by gpu(Consider using DX's compute shader?)

I'm the type of person listing a bunch of plans and do nothing,so I'm not sure how far can I go in this project.
