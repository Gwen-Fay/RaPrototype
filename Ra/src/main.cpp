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
		sceneData.setBackgroundColor2(0,0,0.5);
		sceneData.setSun(2,5,-10,20);

		//Create and modify EntitySDFs here.
		sceneData.setCamPos(0, 1, -5);

		EntitySDF plane = EntitySDF(renderer);
		plane.setFuncID(FuncSDF::PLANE);
		plane.setDistortSinWave(0.2, 0.2, 0.2, 0.5, 0.5, 0.5, 0, 0, 0);
		plane.setCombineID(CombineSDF::SMOOTH_UNION);
		plane.setCombineRadius(0.1);
		
		EntitySDF sphere1 = EntitySDF(renderer, &plane);
		sphere1.setScale(0.5, 0.5, 0.5);
		sphere1.y = 0.25;
		EntitySDF sphere2 = EntitySDF(renderer, &plane);
		sphere2.setScale(0.4, 0.4, 0.4);
		sphere2.y = 0.9;
		EntitySDF sphere3 = EntitySDF(renderer, &plane);
		sphere3.setScale(0.3, 0.3, 0.3);
		sphere3.y = 1.5;

		EntitySDF hat = EntitySDF(renderer);
		hat.setFuncID(FuncSDF::CYLINDER);
		hat.setColor(0.1, 0.1, 0.1);
		hat.setPosition(0.1, 1.95, 0);
		hat.setDistortScale(1, 1.3, 1, 1, 1, 1, -1, -0.45, -1,   1, -.35, 1);
		hat.setScale(0.4, 0.4, 0.4);
		hat.setRotation(10, 0, 0);

		while (!renderer->shouldClose())
		{
			time = clock();
			float t = float(time);
			//modify EntitySDFs over time here 
			

			//Set Shaders and Render
			renderer->setUniforms(sceneData);
			renderer->render();
		}
	}

	delete renderer;
}