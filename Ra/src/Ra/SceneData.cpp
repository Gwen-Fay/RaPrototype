#include "SceneData.h"

void SceneData::setMandelData(float pow, float loop, float isJulia, float jx, float jy, float jz)
{
	mandelData[0] = pow;
	mandelData[1] = loop;
	mandelData[2] = isJulia;
	mandelData[3] = jx;
	mandelData[4] = jy;
	mandelData[5] = jz;
}
void SceneData::setSun(float sx, float sy, float sz, float penumbra)
{
	sun[0] = sx;
	sun[1] = sy;
	sun[2] = sz;
	sun[3] = penumbra;
}
void SceneData::setCamPos(float cx, float cy, float cz)
{
	camPos[0] = cx;
	camPos[1] = cy;
	camPos[2] = cz;
}
void SceneData::setCamRot(float p, float y, float r)
{
	pitch = p;
	yaw = y;
	roll = r;
}

void SceneData::setRes(float x, float y)
{
	res[0] = x;
	res[1] = y;
}

void SceneData::setBackgroundColor1(float r, float g, float b)
{
	backColor1[0] = r;
	backColor1[1] = g;
	backColor1[2] = b;
}

void SceneData::setBackgroundColor2(float r, float g, float b)
{
	backColor2[0] = r;
	backColor2[1] = g;
	backColor2[2] = b;
}

SceneData::SceneData()
{

}

SceneData::~SceneData()
{

}