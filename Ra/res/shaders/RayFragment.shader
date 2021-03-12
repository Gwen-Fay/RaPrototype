#version 410 core

const int NODE_MAX = 200;
const int CHILD_MAX = 20;
const int PARENT_MAX = 100;

//struct used to pass SDF primitive data between cpu and gpu
struct NodeSDF
{
	mat4 trans;
	int funcID;
	int childArrayID;
	int padding;
	int parentID;
	vec4 color;
	vec4 sdfData;

	//Modify
	vec4 elong;
	vec4 repeat;
	float roundRad;
	float onionRad;

	//Combine
	int combineID;
	float combineRad;

	//Distort
	vec4 distortRot1;
	vec4 distortRot2;
	vec4 distortRotStart;
	vec4 distortRotEnd;
	vec4 distortScale1;
	vec4 distortScale2;
	vec4 distortScaleStart;
	vec4 distortScaleEnd;
	vec4 distortSinAmp;
	vec4 distortSinFreq;
	vec4 distortSinPhase;

};

//UBO for nodes
layout (std140, binding = 0) uniform NodeBlock{
	NodeSDF uNodes[NODE_MAX];
};


out vec4 color;

uniform vec2 uRes;
uniform vec4 uSun;
uniform vec3 uCamPos;
uniform mat3 uCamRot;

uniform int[NODE_MAX] uRoots;
uniform int[PARENT_MAX * CHILD_MAX] uChildren;


const int MAX_STEPS = 150;
const float MAX_DIST = 200.0;
const float SURF_DIST = 0.001;

//mandelbulb data
uniform float uMandelPower;
uniform float uMandelLoop;
uniform float uIsJulia;
uniform vec3 uJulia;

uniform vec3 uBackColor1;
uniform vec3 uBackColor2;

const int FOV = 90;

//used for rotations that changes based on position
mat2 Rot(float r)
{
	float s = sin(r);
	float c = cos(r);
	return mat2(c, -s, s, c);
}

/*
 * Primitive SDF Shapes
 */

float sphere(vec3 p)
{
	return length(p) - 1.0;
}

//height h, radius r
float capsule(vec3 p, float h, float r)
{
	p.y -= clamp(p.y, 0.0, h);
	return length(p) - r;
}


float box(vec3 p)
{
	vec3 q = abs(p) - vec3(0.5);
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

//box of size b, outlined with thickness e
float boundingBox(vec3 p, vec3 b, float e)
{
	p = abs(p) - b;
	vec3 q = abs(p + e) - e;
	return min(min(
		length(max(vec3(p.x, q.y, q.z), 0.0)) + min(max(p.x, max(q.y, q.z)), 0.0),
		length(max(vec3(q.x, p.y, q.z), 0.0)) + min(max(q.x, max(p.y, q.z)), 0.0)),
		length(max(vec3(q.x, q.y, p.z), 0.0)) + min(max(q.x, max(q.y, p.z)), 0.0));
}
 
float torus(vec3 p, float r1, float r2)
{
	vec2 q = vec2(length(p.xz) - r1, p.y);
	return length(q) - r2;
}

float cylinder(vec3 p)
{
	vec2 d = abs(vec2(length(p.xz), p.y)) - vec2(0.5);
	return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float cone(in vec3 p)
{
	vec2 q =  vec2(1.0, -1.0);

	vec2 w = vec2(length(p.xz), p.y);
	vec2 a = w - q * clamp(dot(w, q) / dot(q, q), 0.0, 1.0);
	vec2 b = w - q * vec2(clamp(w.x / q.x, 0.0, 1.0), 1.0);
	float k = sign(q.y);
	float d = min(dot(a, a), dot(b, b));
	float s = max(k * (w.x * q.y - w.y * q.x), k * (w.y - q.y));
	return sqrt(d) * sign(s);
}

float plane(vec3 p)
{
	return p.y;
}

float mandelbulb(vec3 position) 
{
	vec3 z = position;
	float dr = 1.0;
	float r = 0.0;
	int iterations = 0;

	for (int i = 0; i < floor(uMandelLoop); i++)
	{
		iterations = i;
		r = length(z);

		if (r > 2) 
		{
			break;
		}

		// convert to polar coordinates
		float theta = acos(z.z / r);
		float phi = atan(z.y, z.x);
		dr = pow(r, uMandelPower - 1.0) * uMandelPower * dr + 1.0;

		// scale and rotate the point
		float zr = pow(r, uMandelPower);
		theta = theta * uMandelPower;
		phi = phi * uMandelPower;

		// convert back to cartesian coordinates
		z = zr * vec3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
		if (uIsJulia < 0.5) 
		{
			
			z += position;
		}
		else
		{
			z += uJulia;
		}
		
	}
	float dst = 0.5 * log(r) * r / dr;
	return dst;
}

/*
 * Primitive SDF Alterations
 */

// use like this: sphere(elongate(p,h))
// Slice shape along xy, yz, xz plane,
// moves parts by h, and connect the pieces.
// This is how a capsule is generated from a sphere
vec3  elongate(vec3 p, vec3 h)
{
	return p - clamp(p, -h, h);
}

//use like this: round(box(p),r)
//rounds an SDF, and grows and SDF by radius r
float round(float dist, float r)
{
	return dist - r;
}

//use like this: onion(box(p),r)
// makes SDF hollow, with a thickness of t.
float onion(float dist, float t)
{
	return abs(dist) - t;
}

/*
 * Primitive SDF Set Opperations
 */

float or(float dist1, float dist2) { return min(dist1, dist2); } //union

float sub(float dist1, float dist2) { return max(-dist1, dist2); } //d1 but not d2

float and(float dist1, float dist2) { return max(dist1, dist2); } //intersection

//k is the smooth radius.
float smoothOr(float d1, float d2, float k) {
	float h = clamp(0.5 + 0.5 * (d2 - d1) / k, 0.0, 1.0);
	return mix(d2, d1, h) - k * h * (1.0 - h);
}

float smoothSub(float d1, float d2, float k) {
	float h = clamp(0.5 - 0.5 * (d2 + d1) / k, 0.0, 1.0);
	return mix(d2, -d1, h) + k * h * (1.0 - h);
}

float smoothAnd(float d1, float d2, float k) {
	float h = clamp(0.5 - 0.5 * (d2 - d1) / k, 0.0, 1.0);
	return mix(d2, d1, h) + k * h * (1.0 - h);
}

//repeats the SDF inside of c and infinite amount of times
vec3 infinite(vec3 p, float c)
{
	vec3 C = vec3(c);
	return mod(p + 0.5 * C, C) - 0.5 * C;
}

//repeats SDF in box c a finine l amount of times
vec3 repeat(vec3 p, float c, vec3 l)
{
	return p - c * clamp(round(p / c), -l, l);
	
}

/*
 *  RayMarching
 */

//returns the SDF of a single node
float processNode(vec3 p, int id)
{
	vec4 modPos = vec4(p, 1.0);

	float scale = 1.0;
	

	if (length(uNodes[id].distortScale1) != 0 || length(uNodes[id].distortScale2) != 0)
	{
		vec4 ds1 = uNodes[id].distortScale1;
		vec4 ds2 = uNodes[id].distortScale2;
		vec4 dsS = uNodes[id].distortScaleStart;
		vec4 dsE = uNodes[id].distortScaleEnd;

		vec3 scaleVec = vec3(1.0);

		scaleVec.x = mix(ds1.x, ds2.x, smoothstep(dsS.x, dsE.x, modPos.x));
		scaleVec.y = mix(ds1.y, ds2.y, smoothstep(dsS.y, dsE.y, modPos.y));
		scaleVec.z = mix(ds1.z, ds2.z, smoothstep(dsS.z, dsE.z, modPos.z));

		modPos.yz /= scaleVec.x;
		modPos.xz /= scaleVec.y;
		modPos.xy /= scaleVec.z;

		scale = length(scaleVec);
	}

	if (length(uNodes[id].distortRot1) != 0 || length(uNodes[id].distortRot2) != 0)
	{
		vec4 dr1 = uNodes[id].distortRot1;
		vec4 dr2 = uNodes[id].distortRot2;
		vec4 drS = uNodes[id].distortRotStart;
		vec4 drE = uNodes[id].distortRotEnd;

		vec3 rotVec = vec3(1.0);

		rotVec.x = mix(dr1.x, dr2.x, smoothstep(drS.x, drE.x, modPos.x));
		rotVec.y = mix(dr1.y, dr2.y, smoothstep(drS.y, drE.y, modPos.y));
		rotVec.z = mix(dr1.z, dr2.z, smoothstep(drS.z, drE.z, modPos.z));

		modPos.yz *= Rot(rotVec.x);
		modPos.xz *= Rot(rotVec.y);
		modPos.xy *= Rot(rotVec.z);

	}

	float transScaleVec = length(vec3(uNodes[id].trans[0].x, uNodes[id].trans[1].y, uNodes[id].trans[2].z));
	scale *= transScaleVec;

	vec4 newPos = uNodes[id].trans * vec4(modPos);

	if (length(uNodes[id].elong) > 0.0)
	{
		newPos = vec4(elongate(newPos.xyz, uNodes[id].elong.xyz), 1.0);
	}

	if (length(uNodes[id].repeat) > 0.0)
	{
		if (length(uNodes[id].repeat.xyz) > 0.0)
		{
			newPos = vec4(repeat(newPos.xyz, uNodes[id].repeat.w, uNodes[id].repeat.xyz), 1.0);
		}
		else if(abs(uNodes[id].repeat.w) > 0)
		{
			newPos = vec4(infinite(newPos.xyz, uNodes[id].repeat.w), 1.0);
		}
	}

	float dist = 0;

	switch (uNodes[id].funcID)
	{
	case -1:
		//Node was not defined, therefore ignore this node
		dist = MAX_DIST;
		break;
	case 0:
		dist = sphere(newPos.xyz);
		break;
	case 1:
		dist = capsule(newPos.xyz, uNodes[id].sdfData.x, uNodes[id].sdfData.y);
		break;
	case 2:
		dist = box(newPos.xyz);
		break;
	case 3:
		dist = boundingBox(newPos.xyz, vec3(uNodes[id].sdfData.x, uNodes[id].sdfData.y, uNodes[id].sdfData.z), uNodes[id].sdfData.w);
		break;
	case 4:
		dist = torus(newPos.xyz, uNodes[id].sdfData.x, uNodes[id].sdfData.y);
		break;
	case 5:
		dist = cylinder(newPos.xyz);
		break;
	case 6:
		dist = cone(newPos.xyz);
		break;
	case 7:
		dist = plane(newPos.xyz);
		break;
	case 8:
		dist = mandelbulb(newPos.xyz);
		break;
	}

	if (uNodes[id].roundRad > 0.0) {
		dist = round(dist, uNodes[id].roundRad);
	}


	if (uNodes[id].onionRad > 0.0)
	{
		dist = onion(dist, uNodes[id].onionRad);
	}

	if (length(uNodes[id].distortSinAmp) != 0)
	{
		vec4 amp = uNodes[id].distortSinAmp;
		vec4 freq = uNodes[id].distortSinFreq;
		vec4 phase = uNodes[id].distortSinPhase;
		vec4 sinVec = amp * sin(freq * modPos + phase);

		dist += sinVec.x + sinVec.y + sinVec.z;
	}


	return dist/(scale * (1.0- length(uNodes[id].distortSinAmp)));

}

//returns the distance to closest object in the scene
vec2 getDist(vec3 p)
{
	float dist = MAX_DIST;
	float index = -1;
	for (int i = 0; i < NODE_MAX; i++) {
		if(uRoots[i] == -1)
		{
			break;
		}
		int id = uRoots[i];
		
		float distCurrent = processNode(p, id);
		

		if (dist > distCurrent) {
			index = id;
		}

		if (uNodes[id].childArrayID != -1)
		{
			for (int i = 0; i < CHILD_MAX; i++)
			{
				int childNode = uChildren[uNodes[id].childArrayID * CHILD_MAX + i];
				if (childNode  != -1)
				{
					float distChild = processNode(p, childNode);
					if (distCurrent > distChild)
					{
						index = childNode;
					}
					switch (uNodes[id].combineID)
					{
						case 0:
							distCurrent = or(distCurrent, distChild);
							
							break;
						case 1:
							//parent, subtracted by child
							distCurrent = sub(distChild, distCurrent);
							break;
						case 2:
							distCurrent = and(distCurrent, distChild);
							break;
						case 3:
							distCurrent = smoothOr(distCurrent, distChild, uNodes[id].combineRad);
							break;
						case 4:
							distCurrent = smoothSub(distChild, distCurrent, uNodes[id].combineRad);
							break;
						case 5:
							distCurrent = smoothAnd(distCurrent, distChild, uNodes[id].combineRad);
							break;
					}
				}
			}
		}

		dist = or(dist, distCurrent);
	}
	return vec2(dist,index);
	
}

//returns normal vector, asuming p is close to a surface
vec3 getNormal(vec3 p)
{
	vec2 e = vec2(SURF_DIST, 0.0);
	float d = getDist(p).x;
	vec3 n = vec3(
		d - getDist(p - e.xyy).x,
		d - getDist(p - e.yxy).x,
		d - getDist(p - e.yyx).x
	);
	return normalize(n);
}

//March a ray to find out the color of this pixel
//returns disttance, number of loops, index of closest object
//ro = ray origion, rd = ray direction
vec3 rayMarch(vec3 ro, vec3 rd)
{
	float dist = 0;
	int i;
	float index = -1;
	for (i = 0; i < MAX_STEPS; i++)
	{
		vec3 p = ro + (dist * rd);
		vec2 distData = getDist(p);
		float ds = distData.x;
		index = distData.y;
		dist += ds;
		if (ds < SURF_DIST)
		{
			break;
		}
		if (dist >  MAX_DIST)
		{
			index = -1;
			break;
		}
	}
	return vec3(dist,i, index);
}


// Calculates if pixel is in shadow, and how much to darken if it is
// assumes ro is close to surface of object
float shadow(in vec3 ro, in vec3 rd, float mint, float maxt, float k)
{
	float res = 1.0;
	float ph = 1e20;
	for (float t = mint; t < maxt; )
	{
		float h = getDist(ro + rd * t).x;
		if (h < 0.001)
			return 0.0;
		float y = h * h / (2.0 * ph);
		float d = sqrt(h * h - y * y);
		res = min(res, k * d / max(0.0, t - y));
		ph = h;
		t += h;
	}
	return res;
}

//calculates the light value, assuming p is close to surface
float getLight(vec3 p)
{
	vec3 l = normalize(uSun.xyz - p);
	vec3 n = getNormal(p);
	float dif = clamp(dot(n, l), 0.25, 1.0) ;
	float shad = shadow(p, l, 0.01, length(uSun.xyz - p), uSun.w);
	shad = clamp(shad, 0.25, 1.0);
	
	return dif * shad;
}

void main()
{
	//camera
	vec2 uv = ((gl_FragCoord.xy - 0.5 * uRes) / uRes.y); //-0.5 to 0.5
	vec4 col = vec4(0.0);
	float view = tan(radians(FOV / 2.0));
	vec3 rd = uCamRot * normalize(vec3(uv.x * view, uv.y * view, 1));

	vec3 rm = rayMarch(uCamPos, rd);
	if (rm.z >= 0) {
		vec3 p = uCamPos + rd * rm.x;

		float l = getLight(p);
		col += vec4(l) * uNodes[int(rm.z)].color;

		color = vec4(col);
	}
	else
	{
		color = vec4(smoothstep(1- uBackColor1, 1 -uBackColor2, vec3(uv.y + 0.5)), 1.0);
	}
	//color = vec4(1, 0, 0, 1);
}

