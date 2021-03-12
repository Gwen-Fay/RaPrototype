#pragma once

#include "NodeSDF.h"
#include "RendererGL.h"
#include <list> 

	enum class FuncSDF
	{
		SPHERE,
		CAPSULE,
		BOX,
		BOUNDING_BOX,
		TORUS,
		CYLINDER,
		CONE,
		PLANE,
		MANDELBULB
	};

	enum class CombineSDF
	{
		UNION,
		SUBTRACT,
		INTERSECTION,
		SMOOTH_UNION,
		SMOOTH_SUBTRACT,
		SMOOTH_INTERSECTION
	};

	class EntitySDF
	{
	public:
		static std::list<EntitySDF*> entities;
		//rotation
		float pitch = 0;
		float yaw = 0;
		float roll = 0;
		//position
		float x = 0;
		float y = 0;
		float z = 0;
		//scale
		float scaleX = 1;
		float scaleY = 1;
		float scaleZ = 1;

		NodeSDF* node;

		EntitySDF* parent = nullptr;

		EntitySDF(RendererGL* renderer);
		EntitySDF(RendererGL* renderer, EntitySDF* parent);
		~EntitySDF();
		void setRotation(float pitchValue, float yawValue, float rollValue);
		void setPosition(float xPos, float yPos, float zPos);
		void setScale(float scale_x, float scale_y, float scale_z);
		void setColor(float red, float green, float blue);
		void setDataSDF(float data1, float data2 = 0, float data3 = 0, float data4 = 0);
		void setElong(float elongX, float elongY, float elongZ);
		void setRepeat(float repeatX, float repeatY, float repeatZ, float boxRadius = 0);
		void setRoundRadius(float roundRadius);
		void setOnionRadius(float onionRadius);
		void setCombineRadius(float combineRadius);
		void setFuncID(FuncSDF funcID);
		void setCombineID(CombineSDF combineID);

		void setDistortRotation(float x1, float y1, float z1, float x2, float y2, float z2,
			float startX, float startY, float startZ, float endX, float endY, float endZ);
		void setDistortScale(float x1, float y1, float z1, float x2, float y2, float z2,
			float startX, float startY, float startZ, float endX, float endY, float endZ);
		void setDistortSinWave(float ampX, float ampY, float ampZ, float freqX, float freqY, float freqZ,
			float phaseX, float phaseY, float phaseZ);

		void calcTransMatrix();
	private:
		TreeSDF* tree;
	};

