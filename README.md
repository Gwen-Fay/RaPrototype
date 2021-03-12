# Ra
a Ray Marching Renderer

## Table of contents
* [About Ra](#About-Ra)
* [How does Ray Marching Work?](#How-does-Ray-Marching-Work)
* [Basic Ra Tutorial](#Basic-Ra-Tutorial)

## About Ra
Ra is a Ray Marching render engine. Currently Ra supports light and penumbra shadows.
Planned features include Ambient Occlusion, transparency, reflections, and Textures.

Here are some screenshots of the unique Ray Marched rendering in action:  
![blob](https://imgur.com/sBvJpzy.gif)

## How does Ray Marching Work?

## Basic Ra Tutorial
![alt text](https://imgur.com/ODeeMJh.png)
In the Main function of this default scene:
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
