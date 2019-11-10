
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
#define M_PI 3.14159265359

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
	// cout << lightSource->lightPosition[0] << endl;
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

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  0,0,0,
			  0,0,1);
	// draw worlc coordinate frames
	if(ShowAxes)
	{
		glLineWidth(3.0);
		glBegin(GL_LINES);

		temp = Transform(pDisplayCamera->ViewingMatrix,orig);
		temp1 = Transform(pDisplayCamera->ProjectionMatrix,temp);
		temp = Transform(pDisplayCamera->ViewingMatrix,xunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(1, 0, 0);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);

		temp = Transform(pDisplayCamera->ViewingMatrix,yunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(0, 1, 0);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);

		temp = Transform(pDisplayCamera->ViewingMatrix,zunit);
		temp2= Transform(pDisplayCamera->ProjectionMatrix,temp);
		glColor3f(0, 0, 1);
		glVertex2f(temp1.x/temp1.h, temp1.y/temp1.h);
		glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
		glEnd();
		glLineWidth(1.0);
	}
	// glEnable(GL_DEPTH_TEST);
	// 		      glm::vec3 v1(0.0,1.6,0.0);
 //      glm::vec3 v2(0.8,-0.4,0.8);
 //      glm::vec3 v3(-0.8,-0.4,0.8);
 //      glm::vec3 v4(0.0,-0.4,-0.8);

 //        glColor3f(0,0,1);
 //        glBegin(GL_TRIANGLES);
 //          glVertex3f(v1.x,v1.y,v1.z);
 //          glVertex3f(v2.x,v2.y,v2.z);
 //          glVertex3f(v3.x,v3.y,v3.z); 

 //          glVertex3f(v1.x,v1.y,v1.z);
 //          glVertex3f(v2.x,v2.y,v2.z);
 //          glVertex3f(v4.x,v4.y,v4.z);       

 //          glVertex3f(v1.x,v1.y,v1.z);
 //          glVertex3f(v4.x,v4.y,v4.z);
 //          glVertex3f(v3.x,v3.y,v3.z);   

 //          glVertex3f(v3.x,v3.y,v3.z);
 //          glVertex3f(v2.x,v2.y,v2.z);
 //          glVertex3f(v4.x,v4.y,v4.z);   
 //        glEnd();
 //        glFlush();

	// draw objects
	for(int i = 0; i < pDisplayScene->ObjectCount; i++)
	{
		// Color the selected object yellow and others blue
		glColor3f(0, 0, 1);
		//  draw object faces
		for(int j = 0; j < pDisplayScene->pObjectList[i].FaceCount; j++)
		{
			input = new Vertex[3];
			input[0] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v1];
			input[1] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v2];
			input[2] = pDisplayScene->pObjectList[i].pVertexList[pDisplayScene->pObjectList[i].pFaceList[j].v3];

			for (int k=0; k<3; k++){
				temp	= Transform(pDisplayScene->pObjectList[i].ModelMatrix,input[k]);
				temp2	= Transform(pDisplayCamera->ViewingMatrix,temp);
				input[k]= Transform(pDisplayCamera->ProjectionMatrix,temp2);
			}

			output = ClipPolygon(input, &length);


			glBegin(GL_POLYGON);
			for(int k = 0; k < length; k++)
				glVertex2f(output[k].x/output[k].h, output[k].y/output[k].h);
			glEnd();

			delete [] input;
			input = NULL;
			delete [] output;
			output = NULL;
		
		}

		// Draw object coordinate frames
		if(ShowAxes)
		{
			glLineWidth(3.0);
			glBegin(GL_LINES);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,orig);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp2 = Transform(pDisplayCamera->ProjectionMatrix,temp1);
			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,xunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,yunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);

			temp = Transform(pDisplayScene->pObjectList[i].ModelMatrix,zunit);
			temp1= Transform(pDisplayCamera->ViewingMatrix,temp);
			temp3= Transform(pDisplayCamera->ProjectionMatrix,temp1);
			glColor3f(1, 0, 0);
			glVertex2f(temp2.x/temp2.h, temp2.y/temp2.h);
			glVertex2f(temp3.x/temp3.h, temp3.y/temp3.h);
			glEnd();
			glLineWidth(1.0);
		}

		if(ShowBoundingBoxes)
		{
			// Color the selected object's bounding box red and others' blue
			if(i == SelectedObject)
				glColor3f(1, 0, 0);
			else
				glColor3f(0, 0, 1);
			// This code uses the object faces, not bounding boxes for ray casting,
			//  therefore bounding box drawing is skipped.
		} 
	}

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

	lightSource.lightPosition[0] = pDisplayCamera->Position.x; lightSource.lightPosition[1] = pDisplayCamera->Position.y;
	lightSource.lightPosition[2] = pDisplayCamera->Position.z;

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

	lightSource.lightPosition[0] = pDisplayCamera->Position.x; lightSource.lightPosition[1] = pDisplayCamera->Position.y;
	lightSource.lightPosition[2] = pDisplayCamera->Position.z;

	glutPostRedisplay();
}

void lightSpecification(){

	printf("Using Primary light source........\n");
	GLfloat lightPosition[] = {pDisplayCamera->Position.x,
			  pDisplayCamera->Position.y,pDisplayCamera->Position.z,1.0}; //same with camera

	GLfloat ambientColor[] = {1.0,1.0,1.0,1.0};
	GLfloat dissuseColor[] = {1.0,1.0,1.0,1.0};
	GLfloat specularColor[] = {1.0,1.0,1.0,1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);


		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dissuseColor);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


int main(int argc, char* argv[])
{
	glGetString(GL_VERSION);

    pDisplayScene = new Scene;


	pDisplayScene->Load("scene1.dat");
	pDisplayCamera = new Camera;
	pDisplayCamera->ViewWidth = (float)WindowWidth/32;
	pDisplayCamera->ViewHeight = (float)WindowHeight/32;

	// Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitWindowPosition(100,100);
    glutCreateWindow("Scene");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

 //    // Initialize GL
 //    glMatrixMode(GL_PROJECTION);
 //    glLoadIdentity();
 //    glMatrixMode(GL_MODELVIEW);
 //    glLoadIdentity();
	// glEnable(GL_DEPTH_TEST);

	lightSource.lightPosition[0] = pDisplayCamera->Position.x; lightSource.lightPosition[1] = pDisplayCamera->Position.y;
	lightSource.lightPosition[2] = pDisplayCamera->Position.z;

lightSpecification();
	// lightSource.lightSpecification();


	// heartShader.setShaders();

    // Switch to main loop
    glutMainLoop();

	return 0;
}