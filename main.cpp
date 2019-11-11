
// Allow use of M_PI constant
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include "shaders/shaders.h"
#include "camera_geometry/camera_geometry.h"

#include <fstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "glm/gtc/matrix_transform.hpp"

using namespace std;
// #define M_PI 3.14159265359

// User Interface Variables
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseMiddle = false;
bool MouseRight = false;
bool PerspectiveMode = true;
bool ShowAxes = true;
bool SelectionMode = false;
int SelectedObject = 0;
bool ShowBoundingBoxes = true;
int WindowWidth = 300, WindowHeight = 300;

float CameraRadius = 10;
float CameraTheta = M_PI / 2;
float CameraPhi = M_PI / 2;

// Scene Content Variables
Scene* pDisplayScene;
Camera* pDisplayCamera;

LightSpec lightSource;
// 	// cout << lightSource->lightPosition[0] << endl;
ShaderSpec heartShader("heart_cross_section.mtl", 0);

void DisplayFunc()
{
		int length;
	Vertex* input;
	Vertex* output;
	Vertex	temp,temp1,temp2,temp3;
	Vertex  orig,xunit,yunit,zunit;
	
	// vertices to be used in drawing the coordiante axes
	orig.x=orig.y=orig.z=xunit.y=xunit.z=yunit.x=yunit.z=zunit.x=zunit.y=0.0;
	orig.h=xunit.x=xunit.h=yunit.y=yunit.h=zunit.z=zunit.h=1.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	pDisplayCamera->Perspective();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	glutSolidTeapot(1);
 
	// glColor3f(0, 0, 1);
	// int i = 0;
	// for(int j = 0; j < pDisplayScene->pObjectList[i].FaceCount; j++)
	// {
	// 	input = new Vertex[3];
	// 	input[0] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v1];
	// 	input[1] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v2];
	// 	input[2] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v3];

	// 	for (int k=0; k<3; k++){
	// 		temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,input[k]);
	// 		temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
	// 		input[k]= Transform(pDisplayCamera->ProjectionMatrix,temp2);
	// 	}

	// 	output = ClipPolygon(input, &length);

	// 	glBegin(GL_POLYGON);
	// 	for(int k = 0; k < length; k++)
	// 		glVertex2f(output[k].x/output[k].h, output[k].y/output[k].h);
	// 	glEnd();

	// 	delete [] input;
	// 	input = NULL;
	// 	delete [] output;
	// 	output = NULL;
	// }


	glutSwapBuffers();

}

void ReshapeFunc(int x,int y)
{
	// Get the correct view proportionality for the new window size
	pDisplayCamera->ViewWidth = pDisplayCamera->ViewWidth*((float)x/WindowWidth);
	pDisplayCamera->ViewHeight = pDisplayCamera->ViewHeight*((float)y/WindowHeight);
	glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}

void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !state;
	if(button == GLUT_MIDDLE_BUTTON)
		MouseMiddle = !state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !state;

	if(MouseLeft && SelectionMode)
	{
		// Select a new object with (x,y) mapped back to the (-1.1,1.1)x(-1.1,1.1) view volume
		SelectedObject = Select(SelectedObject, pDisplayScene, pDisplayCamera,
			2.2*(float)x/WindowWidth - 1.1, 1.1 - 2.2*(float)y/WindowHeight);
		glutPostRedisplay();
	}
}

void MotionFunc(int x, int y)
{
	if(MouseLeft && !SelectionMode)
		pDisplayCamera->Pan(((float) x - MouseX)/128, ((float) y - MouseY)/128);
	if(MouseMiddle && !SelectionMode)
		pDisplayCamera->Move(((float) MouseX - x)/32, ((float) y - MouseY)/32, 0);
	if(MouseRight && !SelectionMode)
		pDisplayCamera->Move(0, 0, ((float) y - MouseY)/32);

    float alpha = 0.01;
	if(MouseMiddle && SelectionMode)
	{
		// Move the Near Plane
		// ADD CODE HERE
		if( y > MouseY ){
            pDisplayCamera->NearPlane =  pDisplayCamera->NearPlane + alpha * ((float) y - MouseY);
            if(pDisplayCamera->NearPlane >= pDisplayCamera->FarPlane){
                pDisplayCamera->NearPlane = pDisplayCamera->FarPlane - 2.0;
            }

            if(pDisplayCamera->NearPlane <= pDisplayCamera->Position.z){
                pDisplayCamera->NearPlane = pDisplayCamera->Position.z + 1.0;
            }
        }

        if( y < MouseY ){
            pDisplayCamera->NearPlane =  pDisplayCamera->NearPlane - alpha * ((float) y - MouseY);
            if(pDisplayCamera->NearPlane >= pDisplayCamera->FarPlane){
                pDisplayCamera->NearPlane = pDisplayCamera->FarPlane - 2.0;
            }

            if(pDisplayCamera->NearPlane <= pDisplayCamera->Position.z){
                pDisplayCamera->NearPlane = pDisplayCamera->Position.z + 1.0;
            }
        }
	}

	if(MouseRight && SelectionMode)
	{
		// Move the Far Plane
		// ADD CODE HERE
        if( (y > MouseY) ){
            pDisplayCamera->FarPlane =  pDisplayCamera->FarPlane + alpha * ((float) y - MouseY);
            if(pDisplayCamera->FarPlane <= pDisplayCamera->NearPlane){
                pDisplayCamera->FarPlane = pDisplayCamera->NearPlane + 2.0;
            }
        }

        if( (y < MouseY) ){
            pDisplayCamera->FarPlane =  pDisplayCamera->FarPlane - alpha * ((float) y - MouseY);
            if(pDisplayCamera->FarPlane <= pDisplayCamera->NearPlane){
                pDisplayCamera->FarPlane = pDisplayCamera->NearPlane + 2.0;
            }
        }
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}

void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'O':
	case 'o':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(M_PI/32, 0, 0);
		break;
	case 'I':
	case 'i':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(-M_PI/32, 0, 0);
		break;
	case 'L':
	case 'l':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, M_PI/32, 0);
		break;
	case 'K':
	case 'k':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, -M_PI/32, 0);
		break;
	case ',':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, 0, M_PI/32);
		break;
	case 'M':
	case 'm':
		pDisplayScene->pObjectList[SelectedObject].LocalRotate(0, 0, -M_PI/32);
		break;
	case '6':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(M_PI/32, 0, 0);
		break;
	case '4':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(-M_PI/32, 0, 0);
		break;
	case '8':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, M_PI/32, 0);
		break;
	case '2':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, -M_PI/32, 0);
		break;
	case '9':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, 0, M_PI/32);
		break;
	case '1':
		pDisplayScene->pObjectList[SelectedObject].WorldTranslate(0, 0, -M_PI/32);
		break;
	case ']':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(M_PI/32, 0, 0);
		break;
	case '[':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(-M_PI/32, 0, 0);
		break;
	case 39:	// Apostrophe
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, M_PI/32, 0);
		break;
	case 59:	// Semicolon
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, -M_PI/32, 0);
		break;
	case '/':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, 0, M_PI/32);
		break;
	case '.':
		pDisplayScene->pObjectList[SelectedObject].WorldRotate(0, 0, -M_PI/32);
		break;
	case '=':
		pDisplayScene->pObjectList[SelectedObject].LocalScale(1.05);
		break;
	case '-':
		pDisplayScene->pObjectList[SelectedObject].LocalScale(0.95);
		break;
	case 'A':
	case 'a':
		ShowAxes = !ShowAxes;
		break;
	case 'B':
	case 'b':
		ShowBoundingBoxes = !ShowBoundingBoxes;
		break;
	case 'N':
	case 'n':
		SelectionMode = !SelectionMode;
		if(SelectionMode){
			cout << "Selection Mode" << endl;
		}
		else{
			cout << "Camera Mode" << endl;
		}
		break;
	case 'P':
	case 'p':
		PerspectiveMode = !PerspectiveMode;
		if(PerspectiveMode) {
            glutSetWindowTitle("Assignment 5 (Perspective)");
        }else{
            glutSetWindowTitle("Assignment 5 (Orthogonal)");
		}
		break;
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'Y':
	case 'y':
		pDisplayCamera->MoveView(0.5);
		break;
	case 'U':
	case 'u':
		pDisplayCamera->MoveView(-0.5);
		break;
	case 'H':
	case 'h':
		pDisplayCamera->ScaleView(0.95);
		break;
	case 'J':
	case 'j':
		pDisplayCamera->ScaleView(1.05);
		break;
    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char **argv) 
{			  
    pDisplayScene = new Scene;


	pDisplayScene->Load("scene1.dat");
	pDisplayCamera = new Camera;
	pDisplayCamera->ViewWidth = (float)WindowWidth/32;
	pDisplayCamera->ViewHeight = (float)WindowHeight/32;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow("Scene");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

	lightSource.lightSpecification();
	heartShader.setShaders();

	glutMainLoop();

	return 0;
}
