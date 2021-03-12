/*
 * Matrix Math Functions
 */

#pragma once

#include "EntitySDF.h"


	void getRotMat3(float(&mat)[3][3], float pitch, float yaw, float roll);

	void mat4Mult(float(&leftMat)[4][4], float(&rightMat)[4][4], float(&retMat)[4][4]);

	void genTrans(float(&mat)[4][4], const EntitySDF* const entity);

	void genPosRot(float(&mat)[4][4], const EntitySDF* const entity);
