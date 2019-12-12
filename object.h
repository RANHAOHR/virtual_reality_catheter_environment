#pragma once

class point
{
public:
	point();
	float x, y, z, h;
};

typedef struct _faceStruct {
	int v1,v2,v3;
	int t1,t2,t3;
	int n1,n2,n3;
} faceStruct;


class Object
{
public:
	Object(char* file, int sign, float rx, float ry, float rz, float tx, float ty, float tz, float s_);
	~Object();
	void load(char* file, int sign);
	void WorldTranslate(float tx, float ty, float tz);
	void WorldRotate(float rx, float ry, float rz);
	void LocalRotate(float rx, float ry, float rz);
	void LocalScale(float s);

	int verts, faces, norms, textures;    // Number of vertices, faces and normals in the system
	point *vertList, *normList, *textList; // Vertex and Normal Lists
	faceStruct *faceList;	    // Face List
	float ModelMatrix[16];

	float max_x, min_x, max_y, min_y, max_z, min_z; // parametrizion of the texture mapping function
	point tex_origin;
};

class Catheter
{
public:
	Catheter(char* file, int sign, float rx, float ry, float rz, float tx, float ty, float tz, float s_);
	~Catheter();
	void load(char* file, int sign);
	void readParam(char* file);
	void WorldTranslate(float tx, float ty, float tz);
	void WorldRotate(float rx, float ry, float rz);
	void LocalRotate(float rx, float ry, float rz);
	void LocalScale(float s);

	int Nlength;  //total number of segements
	int NdiskPts; // number of the sampled points for each circle 

	int verts, faces, norms;    // Number of vertices, faces and normals in the system
	point *vertList, *normList; // Vertex and Normal Lists, 
	faceStruct *faceList;	    // Face List
	float ModelMatrix[16];
};

point Transform(float* matrix, point& point);
float* inverseTransp(float* matrix);