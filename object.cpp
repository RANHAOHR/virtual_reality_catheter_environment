// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <string.h>
#include "object.h"

using namespace std;

point::point()
{
	x = y = z = 0;
	h = 1;
}

// Transform a point with an arbitrary matrix
point Transform(float* matrix, point& p_)
{
	point temp;
	temp.x = matrix[0]*p_.x + matrix[4]*p_.y + matrix[8]*p_.z + matrix[12]*p_.h;
	temp.y = matrix[1]*p_.x + matrix[5]*p_.y + matrix[9]*p_.z + matrix[13]*p_.h;
	temp.z = matrix[2]*p_.x + matrix[6]*p_.y + matrix[10]*p_.z + matrix[14]*p_.h;
	temp.h = matrix[3]*p_.x + matrix[7]*p_.y + matrix[11]*p_.z + matrix[15]*p_.h;
	return temp;
}

float* inverseTransp(float* matrix){
	static float output[16];  // so it's accessible to all functions here.

    output[0] = matrix[0]; output[1] = matrix[1]; output[2] = matrix[2];
    output[4] = matrix[4]; output[5] = matrix[5]; output[6] = matrix[6];
    output[8] = matrix[8]; output[9] = matrix[9]; output[10] = matrix[10];
    output[12] = output[13] = output[14] = 0;

    output[3] = -1*(matrix[0] * matrix[12] + matrix[4] * matrix[13] + matrix[8] * matrix[14]);
    output[7] = -1*(matrix[1] * matrix[12] + matrix[5] * matrix[13] + matrix[9] * matrix[14]);
    output[11] = -1*(matrix[2] * matrix[12] + matrix[6] * matrix[13] + matrix[10] * matrix[14]);

    output[15]= 1;
	return output;
}

Object::Object(char* file, int sign, float rx, float ry, float rz, float tx, float ty, float tz, float s_)
{
	// Load the identity for the initial modeling matrix
	ModelMatrix[0] = ModelMatrix[5] = ModelMatrix[10] = ModelMatrix[15] = 1;
	ModelMatrix[1] = ModelMatrix[2] = ModelMatrix[3] = ModelMatrix[4] =
		ModelMatrix[6] = ModelMatrix[7] = ModelMatrix[8] = ModelMatrix[9] =
		ModelMatrix[11] = ModelMatrix[12] = ModelMatrix[13] = ModelMatrix[14]= 0;

	// Apply the initial transformations in order
	LocalScale(s_);
	WorldRotate((float)(M_PI*rx/180.0), (float)(M_PI*ry/180.0), (float)(M_PI*rz/180.0));
	WorldTranslate(tx, ty, tz);	

	load(file, sign);

	for (int i = 0; i < verts; ++i)
	{
		point vert_ = vertList[i];
		vertList[i] =  Transform(ModelMatrix, vert_);
	}

	point temp = Transform(ModelMatrix, tex_origin);
	tex_origin = temp;

	float *inverseTrans_ = inverseTransp(ModelMatrix); // if normal needs to be transformed

	for (int i = 0; i < norms; ++i)
	{
		point norm_ = normList[i];
		normList[i] =  Transform(inverseTrans_, norm_);
	}

}

Object::~Object()
{
	delete [] vertList;
	delete [] normList;
	delete [] textList;
	delete [] faceList;
}


// Load an object (.obj) file
void Object::load(char* file, int sign)
{
	FILE* pObjectFile = fopen(file, "r");
	if(!pObjectFile)
		cout << "Failed to load " << file << "." << endl;
	else
		cout << "Successfully loaded " << file << "." << endl;

	char DataType[128];
	float x, y, z;
	unsigned int v1, v2, v3, t1, t2, t3, n1, n2, n3;
	// Scan the file and count the faces and vertices
	verts = faces = norms = textures = 0;
	while(!feof(pObjectFile))
	{
		fscanf(pObjectFile, "%s %f %f %f\n", &DataType, &x, &y, &z);
		if(strcmp( DataType, "v" ) == 0)
            verts++;
		else if(strcmp( DataType, "vt" ) == 0)
			textures++;
		else if(strcmp( DataType, "vn" ) == 0)
			norms++;
		else if(strcmp( DataType, "f" ) == 0)
			faces++;
	}
	faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
	vertList = (point *)malloc(sizeof(point)*verts);
	normList = (point *)malloc(sizeof(point)*norms);
	textList = (point *)malloc(sizeof(point)*textures);

	fseek(pObjectFile, 0L, SEEK_SET);

//	cout << "Number of vertices: " << verts << endl;
//	cout << "Number of faces: " << faces << endl;
//	cout << "Number of VN: " << norms << endl;

	max_x = max_y = max_z = -1000;
	min_x = min_y = min_z = 1000;
	// Load and create the faces and vertices
	int CurrentVertex = 0, CurrentNormal = 0, CurrentTexture = 0, CurrentFace = 0;
	while(!feof(pObjectFile))
	{
        int res = fscanf(pObjectFile, "%s", DataType);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

		if(strcmp( DataType, "v" ) == 0)
		{
			fscanf(pObjectFile, "%f %f %f\n", &x, &y, &z);
			vertList[CurrentVertex].x = x;
			vertList[CurrentVertex].y = y;//+1;
			vertList[CurrentVertex].z = z;//-3;
			vertList[CurrentVertex].h = 1;

			if (min_x > x)
			{
				min_x = x;
			}

		    if (max_x < x)
			{
				max_x = x;
			}


		    if (min_y > y)
			{
				min_y = y;
			}


		    if (max_y < y)
			{
				max_y = y;
			}

		    if (min_z > z)
			{
				min_z = z;
			}

		    if (max_z < z)
			{
				max_z = z;
			}


	    tex_origin.x = (max_x + min_x) /2;
	    tex_origin.z = (max_z + min_z) /2;
		tex_origin.y = (max_y + min_y) / 2;
	    if( (max_x - min_x) > (max_z - min_z) ){
	    	if ( (max_x - min_x) > (max_y - min_y) )
	    	{
	    		obj_r = (max_x - min_x);
	    	}else{obj_r = (max_y - min_y);}
	    }else{
	    	if ( (max_z - min_z) > (max_y - min_y) )
	    	{
	    		obj_r = (max_z - min_z);
	    	}else{obj_r = (max_y - min_y);}

	    }

		CurrentVertex++;
		}
		else if(strcmp( DataType, "vt" ) == 0){
			fscanf(pObjectFile, "%f %f\n", &x, &y);
			textList[CurrentTexture].x = x;
			textList[CurrentTexture].y = y;
			textList[CurrentTexture].z = 0;
			CurrentTexture++;
		}
		else if(strcmp( DataType, "vn" ) == 0){
			fscanf(pObjectFile, "%f %f %f\n", &x, &y, &z);
			normList[CurrentNormal].x = sign * x;
			normList[CurrentNormal].y = sign * y;
			normList[CurrentNormal].z = sign * z;
			normList[CurrentNormal].h = 0;
			CurrentNormal++;
		}
		else if(strcmp( DataType, "f" ) == 0)
		{
			fscanf(pObjectFile, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3 );

			// Convert to a zero-based index for convenience
			faceList[CurrentFace].v1 = v1 - 1;
			faceList[CurrentFace].v2 = v2 - 1;
			faceList[CurrentFace].v3 = v3 - 1;

			faceList[CurrentFace].t1 = t1 - 1;
			faceList[CurrentFace].t2 = t2 - 1;
			faceList[CurrentFace].t3 = t3 - 1;

			faceList[CurrentFace].n1 = n1 - 1;
			faceList[CurrentFace].n2 = n2 - 1;
			faceList[CurrentFace].n3 = n3 - 1;

			CurrentFace++;
		}
	}

}


// Do world-based translation
void Object::WorldTranslate(float tx, float ty, float tz)
{
	ModelMatrix[12] += tx;
	ModelMatrix[13] += ty;
	ModelMatrix[14] += tz;
}

// Perform world-based rotations in x,y,z order (intended for one-at-a-time use)
void Object::WorldRotate(float rx, float ry, float rz)
{
	float temp[16];

	if(rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[1] = temp[1]*cosx - temp[2]*sinx;
		ModelMatrix[2] = temp[2]*cosx + temp[1]*sinx;
		ModelMatrix[5] = temp[5]*cosx - temp[6]*sinx;
		ModelMatrix[6] = temp[6]*cosx + temp[5]*sinx;
		ModelMatrix[9] = temp[9]*cosx - temp[10]*sinx;
		ModelMatrix[10] = temp[10]*cosx + temp[9]*sinx;
		ModelMatrix[13] = temp[13]*cosx - temp[14]*sinx;
		ModelMatrix[14] = temp[14]*cosx + temp[13]*sinx;
	}

	if(ry != 0)
	{
		float cosy = cos(ry), siny = sin(ry);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosy + temp[2]*siny;
		ModelMatrix[2] = temp[2]*cosy - temp[0]*siny;
		ModelMatrix[4] = temp[4]*cosy + temp[6]*siny;
		ModelMatrix[6] = temp[6]*cosy - temp[4]*siny;
		ModelMatrix[8] = temp[8]*cosy + temp[10]*siny;
		ModelMatrix[10] = temp[10]*cosy - temp[8]*siny;
		ModelMatrix[12] = temp[12]*cosy + temp[14]*siny;
		ModelMatrix[14] = temp[14]*cosy - temp[12]*siny;
	}

	if(rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosz - temp[1]*sinz;
		ModelMatrix[1] = temp[1]*cosz + temp[0]*sinz;
		ModelMatrix[4] = temp[4]*cosz - temp[5]*sinz;
		ModelMatrix[5] = temp[5]*cosz + temp[4]*sinz;
		ModelMatrix[8] = temp[8]*cosz - temp[9]*sinz;
		ModelMatrix[9] = temp[9]*cosz + temp[8]*sinz;
		ModelMatrix[12] = temp[12]*cosz - temp[13]*sinz;
		ModelMatrix[13] = temp[13]*cosz + temp[12]*sinz;
	}
}

void Object::LocalScale(float s)
{
	ModelMatrix[0] = s*ModelMatrix[0];
	ModelMatrix[5] = s*ModelMatrix[5];
	ModelMatrix[10] = s*ModelMatrix[10];
}

// Perform locally-based rotations in x,y,z order (intended for one-at-a-time use)
void Object::LocalRotate(float rx, float ry, float rz)
{
	float temp[16];

	if(rx != 0)
	{
		float cosx = cos(rx), sinx = sin(rx);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[4] = temp[4]*cosx + temp[8]*sinx;
		ModelMatrix[5] = temp[5]*cosx + temp[9]*sinx;
		ModelMatrix[6] = temp[6]*cosx + temp[10]*sinx;
		ModelMatrix[7] = temp[7]*cosx + temp[11]*sinx;
		ModelMatrix[8] = temp[8]*cosx - temp[4]*sinx;
		ModelMatrix[9] = temp[9]*cosx - temp[5]*sinx;
		ModelMatrix[10] = temp[10]*cosx - temp[6]*sinx;
		ModelMatrix[11] = temp[11]*cosx - temp[7]*sinx;
	}

	if(ry != 0)
	{
        float cosy = cos(ry), siny = sin(ry);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosy - temp[8]*siny;
		ModelMatrix[1] = temp[1]*cosy - temp[9]*siny;
		ModelMatrix[2] = temp[2]*cosy - temp[10]*siny;
		ModelMatrix[3] = temp[3]*cosy - temp[11]*siny;
		ModelMatrix[8] = temp[8]*cosy + temp[0]*siny;
		ModelMatrix[9] = temp[9]*cosy + temp[1]*siny;
		ModelMatrix[10] = temp[10]*cosy + temp[2]*siny;
		ModelMatrix[11] = temp[11]*cosy + temp[3]*siny;
	}

	if(rz != 0)
	{
		float cosz = cos(rz), sinz = sin(rz);
		for(int i = 0; i < 16; i++)
			temp[i] = ModelMatrix[i];
		ModelMatrix[0] = temp[0]*cosz + temp[4]*sinz;
		ModelMatrix[1] = temp[1]*cosz + temp[5]*sinz;
		ModelMatrix[2] = temp[2]*cosz + temp[6]*sinz;
		ModelMatrix[3] = temp[3]*cosz + temp[7]*sinz;
		ModelMatrix[4] = temp[4]*cosz - temp[0]*sinz;
		ModelMatrix[5] = temp[5]*cosz - temp[1]*sinz;
		ModelMatrix[6] = temp[6]*cosz - temp[2]*sinz;
		ModelMatrix[7] = temp[7]*cosz - temp[3]*sinz;
	}
}
