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
		sceneData.setBackgroundColor1(0,0,1);
		sceneData.setBackgroundColor2(0, 0, 0);

		//Create and modify EntitySDFs here.
		EntitySDF wall = EntitySDF(renderer);
		wall.setFuncID(FuncSDF::BOX);
		wall.setScale(7, 7, 1);
		wall.setCombineID(CombineSDF::SUBTRACT);
		wall.setColor(0.7, 0, 0);

		EntitySDF bubble1 = EntitySDF(renderer, &wall);
		bubble1.x = 1.5;
		bubble1.setFuncID(FuncSDF::SPHERE);
		bubble1.setColor(0.7, 0, 0);

		EntitySDF bubble2 = EntitySDF(renderer, &wall);
		bubble2.x = -1.5;
		bubble2.scaleY = 2;
		bubble2.setColor(0.7, 0, 0);
		bubble2.setFuncID(FuncSDF::SPHERE);

		while (!renderer->shouldClose())
		{
			time = clock();

			//modify EntitySDFs here
			wall.y = sin(float(time) / 1000);

			//Set Shaders and Render
			renderer->setUniforms(sceneData);
			renderer->render();
		}
	}

	delete renderer;
}