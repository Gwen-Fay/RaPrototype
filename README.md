# Ra
a Ray Marching Renderer

## Table of contents
* [About Ra](#About-Ra)
* [How does this magic work?](#How-does-this-magic-work)
* [How do I use Ra?](#How-do-I-use-Ra?)

## About Ra
Ra is a Ray Marching render engine. Ray Marching is a unique way to render 3d shapes 
using rays and without meshes. Currently Ra supports light and penumbra shadows.
Planned features include Ambient Occlusion, transparency, reflections, and Textures.

My goals with this project was to:
1) Practice C++
2) Practice low level OpenGL stuff
3) Learn about and experiement with Ray Marching

Here are some screenshots of the unique Ray Marched rendering in action.
Each scene is rendered in real time in a glfw window, again without meshes.  

![blob](https://imgur.com/sBvJpzy.gif)  
This is 6 Spheres, smooth unioned together and each are moving.  
<br />
<br />
![Shadow](https://imgur.com/KlXvIjW.gif)  
Light is moving, and so is it's penumbra value.  
<br />
<br />
![Slime Cube](https://imgur.com/9ZLggEj.gif)  
It's just a cube, with some sin wave distortion.  
<br />
<br />
![Fractal Nonsense](https://imgur.com/cmL4STj.gif)  
This Fractal is cycling through some Julia Sets of the famous Mandelbulb.  
The Color is based on the coordinates used for that Julia set.  
<br />
<br />
![Infinite Donuts](https://imgur.com/pOOhabN.gif)  
This is a single Torus object, with a repeating function applied to it.  
<br />
<br />
![Subtracting sets](https://imgur.com/6X5e0vi.gif)  
I am showing you here how primitive shapes can be subtracted from eachother.  
The 6 spheres are subtracting from the box, and they are child nodes to the box.  
I am only moving the box with code.  
<br />
<br />
![Twisting Box](https://imgur.com/EftbKMF.gif)  
The rotation of the box is distorted based on position. 
<br />
<br />
![Snowman](https://imgur.com/AIt8QfD.gif)  
And finally, here is a cute image I made.
<br />
<br />
## How does this magic work?
![2 Triangles](https://imgur.com/mf2Gm19.gif)  
First off, Ra uses 2 triangles to encompas the entire screen.
This means to render a scene, we need some function that 
takes in a screen coordinate and returns the color for that pixel.
In the fragment shader, we need some sort of algorithm that can
shoot rays into a scene with known spacial data and see if they hit anything, and at what angle.
<br />
Ray Marching uses a Signed Distance Field, or SDF to represent the spacial data. 
We can ask, from any point in the scene, how far away are we from the surface.
These distance fields are Signed, because inside of the shape returns a negative value.
For primitive shapes, it is easy to calculate the SDF.  
![SphereSDF](https://imgur.com/kj4py8C.gif)  
Ra suports Sphere, Capsule, Box, Bounding Box, Torus, Cylinder, Cone, Plane and Mandelbulb as SDF primitives.
With Ray Marching, you take the Ray Origion, the Ray Direction, and the scene SDF and use this algorithm:
Get the SDF. "March" forward from the Ray Origion in Ray Direction by the SDF Distance Function. We don't know
the direction that a surface might be, but we can safely measure forward the SDF Distance.
Then, from this new location, repeat the process. Get the SDF, march forward. Do this untill the SDF is arbitrarily small,
say epsilon = 0.01 for example. Then stop, you got close enough. If you go too far, just stop.  
![RayMarchMiss](https://imgur.com/A3cSS7l.gif)  
![RayMarchHit](https://imgur.com/kIWqyZk.gif)  
After the Ray March, you will have a close aproxximation of the point the ray collided with.
From here, you can do some Ray Tracing like effects, like calculating soft shadows, ambient occusion,
Reflections, Transparency, Lighting, etc. You can also distort the SDF distance like I did with the green cube above, or you can modify the Ray Origion position.
In the Twisting box example, the Ray Origion is rotated around, the amound is a function of height. You can combine SDF primitives with various combine functions.
The simpliest of which is the Union function, which just takes the min of each SDF. 
And finally, because Ray Marching is a method of rendering that does not calcuate the exact surface, it just approaches the surface untill it is within some 
epsilon, you can render shapes who's surface can't be calculated exactly. This is how it is possible to render the Mandelbulb fractal.
<br />
<br />

## How do I use Ra?
![alt text](https://imgur.com/ODeeMJh.png)
In the Main function of this scene in C++:
```
  EntitySDF sphere = EntitySDF(renderer);
	EntitySDF box = EntitySDF(renderer);
	EntitySDF cone = EntitySDF(renderer);

	sphere.setColor(1, 0, 0);
	sphere.x = -2;
	sphere.setFuncID(FuncSDF::SPHERE);

  box.setColor(0, 1, 0);
	box.setFuncID(FuncSDF::BOX);
	box.setScale(1.5, 1.5, 1.5);

	cone.setColor(0, 0, 1);
	cone.setPosition(2,0.5,0);
	cone.setFuncID(FuncSDF::CONE);
```
TODO Add more examples;
