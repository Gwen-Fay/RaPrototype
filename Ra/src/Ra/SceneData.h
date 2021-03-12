#pragma once
class SceneData
{
public:
	float mandelData[6] = { 2.0, 5.0, 1.0,
							   -0.5, -0.5, 1.1 };
	
	float sun[4] = { 10.0, 5.0, -15.0, 20 };

	float camPos[3] = { 0.0, 1.5, -10.0 };

	float camRot[3][3] =
	{ {1.0, 0.0, 0.0},
	  {0.0, 1.0, 0.0},
	  {0.0, 0.0, 1.0} };

	float res[2] = { 1280,720 };

	float pitch = 0;
	float yaw = 0; 
	float roll = 0;

	float backColor1[3] = { 0.0, 0.0, 0.0 };
	float backColor2[3] = { 0.0, 0.0, 0.0 };

	void setMandelData(float pow, float loop, float isJulia = 0, float jx = 0, float jy = 0, float jz = 0);
	void setSun(float sx, float sy, float sz, float penumbra);
	void setCamPos(float cx, float cy, float cz);
	void setCamRot(float p, float y, float r);
	void setRes(float x, float y);
	void setBackgroundColor1(float r, float g, float b);
	void setBackgroundColor2(float r, float g, float b);

	SceneData();
	~SceneData();
};

