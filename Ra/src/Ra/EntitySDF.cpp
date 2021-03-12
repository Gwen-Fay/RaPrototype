#include "EntitySDF.h"
#include "Math.h"
#include <iostream>

	std::list<EntitySDF*> EntitySDF::entities;

	EntitySDF::EntitySDF(RendererGL* renderer)
	{
		tree = &renderer->tree;
		node = tree->newNode();
		entities.push_front(this);
	}


	EntitySDF::EntitySDF(RendererGL* renderer, EntitySDF* theParent)
	{
		tree = &renderer->tree;
		node = tree->newChild(theParent->node);
		parent = theParent;
		entities.push_front(this);
	}

	EntitySDF::~EntitySDF()
	{
		tree->delNode(node);
		entities.remove(this);
	}

	void EntitySDF::setRotation(float pitchValue, float yawValue, float rollValue)
	{
		pitch = pitchValue;
		yaw = yawValue;
		roll = rollValue;
	}

	void EntitySDF::setPosition(float xPos, float yPos, float zPos)
	{
		x = xPos;
		y = yPos;
		z = zPos;
	}

	void EntitySDF::setScale(float scale_x, float scale_y, float scale_z)
	{
		scaleX = scale_x;
		scaleY = scale_y;
		scaleZ = scale_z;
	}

	void EntitySDF::setColor(float red, float green, float blue)
	{
		node->color[0] = red;
		node->color[1] = green;
		node->color[2] = blue;
		node->color[3] = 1.0;
	}

	void EntitySDF::setDataSDF(float data1, float data2, float data3, float data4)
	{
		node->sdfData[0] = data1;
		node->sdfData[1] = data2;
		node->sdfData[2] = data3;
		node->sdfData[3] = data4;
	}

	void EntitySDF::setElong(float elongX, float elongY, float elongZ)
	{
		node->elong[0] = elongX;
		node->elong[1] = elongY;
		node->elong[2] = elongZ;
	}

	void EntitySDF::setRepeat(float repeatX, float repeatY, float repeatZ, float boxRadius)
	{
		node->repeat[0] = repeatX;
		node->repeat[1] = repeatY;
		node->repeat[2] = repeatZ;
		node->repeat[3] = boxRadius;
	}

	void EntitySDF::setRoundRadius(float roundRadius)
	{
		node->roundRad = roundRadius;
	}

	void EntitySDF::setOnionRadius(float onionRadius)
	{
		node->onionRad = onionRadius;
	}

	void EntitySDF::setCombineRadius(float combineRadius)
	{
		node->combineRad = combineRadius;
	}

	void EntitySDF::setDistortRotation(float x1, float y1, float z1, float x2, float y2, float z2,
		float startX, float startY, float startZ, float endX, float endY, float endZ)
	{
		node->distortRot1[0] = x1;
		node->distortRot1[1] = y1;
		node->distortRot1[2] = z1;

		node->distortRot2[0] = x2;
		node->distortRot2[1] = y2;
		node->distortRot2[2] = z2;

		node->distortRotStart[0] = startX;
		node->distortRotStart[1] = startY;
		node->distortRotStart[2] = startZ;

		node->distortRotEnd[0] = endX;
		node->distortRotEnd[1] = endY;
		node->distortRotEnd[2] = endZ;
	}

	void EntitySDF::setDistortScale(float x1, float y1, float z1, float x2, float y2, float z2,
		float startX, float startY, float startZ, float endX, float endY, float endZ)
	{
		node->distortScale1[0] = x1;
		node->distortScale1[1] = y1;
		node->distortScale1[2] = z1;

		node->distortScale2[0] = x2;
		node->distortScale2[1] = y2;
		node->distortScale2[2] = z2;

		node->distortScaleStart[0] = startX;
		node->distortScaleStart[1] = startY;
		node->distortScaleStart[2] = startZ;

		node->distortScaleEnd[0] = endX;
		node->distortScaleEnd[1] = endY;
		node->distortScaleEnd[2] = endZ;
	}

	void EntitySDF::setDistortSinWave(float ampX, float ampY, float ampZ, float freqX, float freqY,
		float freqZ, float phaseX, float phaseY, float phaseZ)
	{
		node->distortSinAmp[0] = ampX;
		node->distortSinAmp[1] = ampY;
		node->distortSinAmp[2] = ampZ;

		node->distortSinFreq[0] = freqX;
		node->distortSinFreq[1] = freqY;
		node->distortSinFreq[2] = freqZ;

		node->distortSinPhase[0] = phaseX;
		node->distortSinPhase[1] = phaseY;
		node->distortSinPhase[2] = phaseZ;
	}

	void EntitySDF::calcTransMatrix()
	{
		genTrans(node->trans, this);

		EntitySDF* currentParent = nullptr;
		bool hasParent = parent == nullptr ? false : true;
		if (hasParent)
		{
			currentParent = parent;
			float mat[4][4] =
			{
				{1.0, 0.0, 0.0, 0.0},
				{0.0, 1.0, 0.0, 0.0},
				{0.0, 0.0, 1.0, 0.0},
				{0.0, 0.0, 0.0, 1.0}
			};

			float matNew[4][4] =
			{
				{1.0, 0.0, 0.0, 0.0},
				{0.0, 1.0, 0.0, 0.0},
				{0.0, 0.0, 1.0, 0.0},
				{0.0, 0.0, 0.0, 1.0}
			};
			while (hasParent)
			{
				genPosRot(matNew, currentParent);
				mat4Mult(mat, matNew, mat);
				hasParent = currentParent->parent == nullptr ? false : true;
			}

			mat[3][0] /= scaleX;
			mat[3][1] /= scaleY;
			mat[3][2] /= scaleZ;

			mat4Mult(node->trans, mat, node->trans);
		}
	}

	void EntitySDF::setFuncID(FuncSDF funcID)
	{
		node->funcID = int(funcID);
	}

	void EntitySDF::setCombineID(CombineSDF combineID)
	{
		node->combineID = int(combineID);
	}
