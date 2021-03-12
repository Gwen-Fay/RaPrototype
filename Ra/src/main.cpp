#include "Math.h"
#include "Ra.h"
#include <iostream>



const int res[2] = { 1280, 720 }; //screen resolution
const char* vertShader = "res/shaders/RayVertex.shader";
const char* fragShader = "res/shaders/RayFragment.shader";

int main()
{
	RendererGL* renderer = new RendererGL(res, vertShader, fragShader);
	SceneData sceneData = SceneData();
	if (renderer->error == -1)
	{
		std::cout << "Error, Renderer did not initalize!" << std::endl;
	}

	clock_t frame;
	clock_t time = clock();
	{
		sceneData.setBackgroundColor1(0,0,0);
		sceneData.setBackgroundColor2(0.3, 0.3, 0.3);
		sceneData.setSun(2,5,-10,20);

		//Create and modify EntitySDFs here.
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

		while (!renderer->shouldClose())
		{
			time = clock();

			//modify EntitySDFs over time here 

			//Set Shaders and Render
			renderer->setUniforms(sceneData);
			renderer->render();
		}
	}

	delete renderer;
}