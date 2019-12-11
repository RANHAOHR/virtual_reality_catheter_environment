/*

EECS 366/466 COMPUTER GRAPHICS
Template for Assignment 8-MAPPING
Spring 2006

*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include <SOIL/SOIL.h> 

# include "catheter_scene.h"
#include "read_tga.h"
# include "object.h"

#define PI 3.14159265359

#define PrintOpenGLError()::PrintOGLError(__FILE__, __LINE__)

using namespace std;


//Illimunation and shading related declerations
char *shaderFileRead(char *fn);

GLuint vertex_shader,fragment_shader,p;
int illimunationMode = 0;
int shadingMode = 0;
int lightSource = 0;
int program=-1;

float alpha_ = 0.0;
//Parameters for Copper (From: "Computer Graphics Using OpenGL" BY F.S. Hill, Jr.) 
GLfloat ambient_cont [] = {0.19125,0.0735,0.0225};
GLfloat diffuse_cont [] = {0.7038,0.27048,0.0828};
GLfloat specular_cont [] = {0.256777,0.137622,0.086014};
GLfloat ns_ = 5000;

int Ntex = 5;
GLuint *textures = new GLuint[Ntex];

//Projection, camera contral related declerations
int WindowWidth,WindowHeight;


float CameraRadius = 10;
float CameraTheta = PI / 2;
float CameraPhi = PI / 2;
int MouseX = 0;
int MouseY = 0;
bool MouseLeft = false;
bool MouseRight = false;

float norm(point &p_){

	float res = sqrt(p_.x * p_.x + p_.y * p_.y + p_.z * p_.z);
	return res;

}

point cross(point &p1, point &p2){

	point res;
	res.x = p1.y * p2.z - p2.y * p1.z;
	res.y = p1.z * p2.x - p2.z * p1.x;
	res.z = p1.x * p2.y - p1.y * p2.x;
	return res;

}


point textureCoordSphere(Object &mesh, point &v_){

	// change center
	point coord;
	float delt_x = v_.x - mesh.tex_origin.x;
	float delt_z = v_.z - mesh.tex_origin.z;
	float theta_ = atan2(delt_z, delt_x);

	coord.x = (theta_ + PI )/  (2 * PI);
	float delt_y = v_.y - mesh.tex_origin.y;

	float phi_ = atan2(sqrt(delt_x*delt_x + delt_z*delt_z), delt_y); // return (0, pi)
	coord.y = phi_ / PI;

	return  coord;

}

void renderFunction( Object &mesh){

	for (int i = 0; i < mesh.faces; i++)
	{

		glBegin(GL_TRIANGLES);
			point v1, v2, v3, n1, n2, n3, coord1,coord2, coord3;
			v1 = mesh.vertList[mesh.faceList[i].v1];
			v2 = mesh.vertList[mesh.faceList[i].v2];
			v3 = mesh.vertList[mesh.faceList[i].v3];
			n1 = mesh.normList[mesh.faceList[i].n1];
			n2 = mesh.normList[mesh.faceList[i].n2];
			n3 = mesh.normList[mesh.faceList[i].n3];

			glNormal3f(n1.x, n1.y, n1.z);
			// glTexCoord2f (coord1.x, coord1.y);
			glVertex3f(v1.x, v1.y, v1.z);
	
			glNormal3f(n2.x, n2.y, n2.z);
			// glTexCoord2f (coord2.x, coord2.y);
			glVertex3f(v2.x, v2.y, v2.z);
				
			glNormal3f(n3.x, n3.y, n3.z);
			// glTexCoord2f (coord3.x, coord3.y);
			glVertex3f(v3.x, v3.y, v3.z);

		glEnd();

	}	


}


void mappingFunction(Object &mesh){

	// Load image from tga file
	int width, height;
	unsigned char* blendmap = SOIL_load_image("blend.png", &width, &height, 0, SOIL_LOAD_RGB);

	glGenTextures(Ntex, textures);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, textures[0]);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Finaly build the mipmaps
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, blendmap);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGB, GL_UNSIGNED_BYTE, blendmap);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// glEnable( GL_TEXTURE_2D );

	SOIL_free_image_data(blendmap);

	unsigned char* redmap = SOIL_load_image("aortic_color.jpg", &width, &height, 0, SOIL_LOAD_RGB);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture (GL_TEXTURE_2D, textures[1]); 

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Finaly build the mipmaps
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, redmap);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGB, GL_UNSIGNED_BYTE, redmap);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	SOIL_free_image_data(redmap);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture (GL_TEXTURE_2D, textures[2]); 

	unsigned char* bluemap = SOIL_load_image("pulmonary_color.jpg", &width, &height, 0, SOIL_LOAD_RGB);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Finaly build the mipmaps
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bluemap);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGB, GL_UNSIGNED_BYTE, bluemap);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable( GL_TEXTURE_2D );



	SOIL_free_image_data(bluemap);


	glActiveTexture(GL_TEXTURE3);
	glBindTexture (GL_TEXTURE_2D, textures[3]); 

	unsigned char* muscle_map = SOIL_load_image("heart_muscle.png", &width, &height, 0, SOIL_LOAD_RGB);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Finaly build the mipmaps
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, muscle_map);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGB, GL_UNSIGNED_BYTE, muscle_map);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable( GL_TEXTURE_2D );

	SOIL_free_image_data(muscle_map);

	unsigned char* backgroundmap = SOIL_load_image("background.jpg", &width, &height, 0, SOIL_LOAD_RGB);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture (GL_TEXTURE_2D, textures[4]); 

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// Finaly build the mipmaps
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, backgroundmap);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGB, GL_UNSIGNED_BYTE, backgroundmap);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	SOIL_free_image_data(backgroundmap);

	for (int i = 0; i < mesh.faces; i++)
	{

		glBegin(GL_TRIANGLES);
			point v1, v2, v3, n1, n2, n3, coord1, coord2, coord3, t1, t2, t3;
			v1 = mesh.vertList[mesh.faceList[i].v1];
			v2 = mesh.vertList[mesh.faceList[i].v2];
			v3 = mesh.vertList[mesh.faceList[i].v3];
			n1 = mesh.normList[mesh.faceList[i].n1];
			n2 = mesh.normList[mesh.faceList[i].n2];
			n3 = mesh.normList[mesh.faceList[i].n3];

			t1 = textureCoordSphere(mesh, v1);
			t2 = textureCoordSphere(mesh, v2);
			t3 = textureCoordSphere(mesh, v3);		

			glNormal3f(n1.x, n1.y, n1.z);
			glTexCoord2f (t1.x, t1.y);
			glVertex3f(v1.x, v1.y, v1.z);
	
			glNormal3f(n2.x, n2.y, n2.z);
			glTexCoord2f (t2.x, t2.y);
			glVertex3f(v2.x, v2.y, v2.z);

			glNormal3f(n3.x, n3.y, n3.z);
			glTexCoord2f (t3.x, t3.y);
			glVertex3f(v3.x, v3.y, v3.z);

		glEnd();

	}	

}


void DisplayFunc(void) 
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//load projection and viewing transforms
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
        
	gluPerspective(60,(GLdouble) WindowWidth/WindowHeight,0.01,10000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(CameraRadius*cos(CameraTheta)*sin(CameraPhi),
			  CameraRadius*cos(CameraPhi),
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),
			  0,0,0,
			  0,1,0);

	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_TEXTURE_2D);


    setHeartShaders();
    Object heart("heart_cross_section.obj", 1, -100.0, 30.0, 0.0, 0.4, 1.0, -0.1, 0.5);
    mappingFunction(heart);


	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    setHumanShaders();

    Object human("human_large.obj", 1, 0.0, 0.0, 0.0, 0.0, -14.5, 0.0, 4.5);
    // const char *path1 = "skin.tga";
   	renderFunction(human);

	glDisable(GL_BLEND);

    // setCatheterShaders();
    // Object catheter("catheter_model.obj", 1, 90.0, 0.0, 0.0, 0.4, 0.0, 1.6, 20.0);
    // mappingFunction(catheter);

	glutSwapBuffers();
}

void ReshapeFunc(int x,int y)
{
    glViewport(0,0,x,y);
    WindowWidth = x;
    WindowHeight = y;
}


void MouseFunc(int button,int state,int x,int y)
{
	MouseX = x;
	MouseY = y;

    if(button == GLUT_LEFT_BUTTON)
		MouseLeft = !(bool) state;
	if(button == GLUT_RIGHT_BUTTON)
		MouseRight = !(bool) state;
}

void MotionFunc(int x, int y)
{
	if(MouseLeft)
	{
        CameraTheta += 0.01*PI*(MouseX - x);
		CameraPhi += 0.01*PI*(MouseY - y);
		if (CameraPhi > (PI - 0.01))
			CameraPhi = PI - 0.01;
		if (CameraPhi < 0.01)
			CameraPhi = 0.01;
	}
	if(MouseRight)
	{
        CameraRadius += 0.2*(MouseY-y);
		if(CameraRadius <= 0)
			CameraRadius = 0.2;
	}
    
	MouseX = x;
	MouseY = y;

	glutPostRedisplay();
}

//Motion and camera controls
void KeyboardFunc(unsigned char key, int x, int y)
{
    switch(key)
	{
	case 'Q':
	case 'q':
		exit(1);
		break;
	case 'w':
	case 'W':
		if (illimunationMode == 0)
		{
			illimunationMode = 1;
		}
		else
		{
			illimunationMode = 0;
		}
		break;
	case 'e':
	case 'E':
		if (shadingMode == 0)
		{
			shadingMode =1;
		}
		else
		{
			shadingMode =0;
		}
		break;
	case 'd':
	case 'D':
		alpha_ -= 0.05;
		if (alpha_ <= 0.0)
		{
			alpha_ = 0.0;
		}
		break;
	case 'f':
	case 'F':
		alpha_ += 0.05;
		if (alpha_ >= 1.0)
		{
			alpha_ = 1.0;
		}
		break;

    default:
		break;
    }

	glutPostRedisplay();
}

int main(int argc, char **argv) 
{			  

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Scene");

	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutMouseFunc(MouseFunc);
    glutMotionFunc(MotionFunc);
    glutKeyboardFunc(KeyboardFunc);

//	setShaders();
	glutMainLoop();

	return 0;
}

/*************************************************************
Shader related methods,
Setting the shader files
Setting the shader variables
*************************************************************/

void error_exit(int status, char *text)
{

	// Print error message

	fprintf(stderr,"Internal Error %i: ", status);
	fprintf(stderr,text);
	printf("\nTerminating as Result of Internal Error.\nPress Enter to exit.\n");

	// Keep the terminal open

	int anyKey = getchar();

	// Exit program

	exit(status);
}

int PrintOGLError(char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


void setHeartShaders()
{
	glewInit();
	char *vs = NULL,*fs = NULL;

	//create the empty shader objects and get their handles
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	//read the shader files and store the strings in corresponding char. arrays.
	vs = shaderFileRead("texture_shader.vert");
	fs = shaderFileRead("texture_shader.frag");

	const char * vv = vs;
	const char * ff = fs;

//	GLint vertCompiled, fragCompiled;

	//set the shader's source code by using the strings read from the shader files.
	glShaderSource(vertex_shader, 1, &vv,NULL);
	glShaderSource(fragment_shader, 1, &ff,NULL);

	free(vs);free(fs);

	//Compile the shader objects
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);

	//create an empty program object to attach the shader objects
	p = glCreateProgram();


	//attach the shader objects to the program object
	glAttachShader(p,vertex_shader);
	glAttachShader(p,fragment_shader);
	// define unifrom variables

	/*
	**************
	Programming Tip:
	***************
	Delete the attached shader objects once they are attached.
	They will be flagged for removal and will be freed when they are no more used.
	*/
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	//Link the created program.
	/*
	**************
	Programming Tip:
	***************
	You can trace the status of link operation by calling
	"glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
	*/
	glLinkProgram(p);

	//Start to use the program object, which is the part of the current rendering state
	glUseProgram(p);


	setParameters(p);

}


void setHumanShaders()
{
    glewInit();
    char *vs = NULL,*fs = NULL;

    //create the empty shader objects and get their handles
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //read the shader files and store the strings in corresponding char. arrays.
    vs = shaderFileRead("humanshader.vert");
    fs = shaderFileRead("humanshader.frag");

    const char * vv = vs;
    const char * ff = fs;

//	GLint vertCompiled, fragCompiled;

    //set the shader's source code by using the strings read from the shader files.
    glShaderSource(vertex_shader, 1, &vv,NULL);
    glShaderSource(fragment_shader, 1, &ff,NULL);

    free(vs);free(fs);

    //Compile the shader objects
    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    //create an empty program object to attach the shader objects
    p = glCreateProgram();

    //attach the shader objects to the program object
    glAttachShader(p,vertex_shader);
    glAttachShader(p,fragment_shader);
    // define unifrom variables

    /*
    **************
    Programming Tip:
    ***************
    Delete the attached shader objects once they are attached.
    They will be flagged for removal and will be freed when they are no more used.
    */
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    //Link the created program.
    /*
    **************
    Programming Tip:
    ***************
    You can trace the status of link operation by calling
    "glGetObjectParameterARB(p,GL_OBJECT_LINK_STATUS_ARB)"
    */
    glLinkProgram(p);

    //Start to use the program object, which is the part of the current rendering state
    glUseProgram(p);

    setParameters(p);

}

//Gets the location of the uniform variable given with "name" in the memory
//and tests whether the process was successfull.
//Returns the location of the queried uniform variable
int getUniformVariable(GLuint program,char *name)
{
	int location = glGetUniformLocationARB(program, name);
	
	if (location == -1)
	{
 		error_exit(1007, "No such uniform variable");
	}
	PrintOpenGLError();
	return location;
}

void update_Light_Position()
{
	
	// Create light components
	GLfloat light_position[] = { CameraRadius*cos(CameraTheta)*sin(CameraPhi),			  
			  CameraRadius*cos(CameraPhi) , 
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	GLfloat ambientColor[] = {1.0,1.0,1.0,1.0};
	GLfloat dissuseColor[] = {0.0,1.0,1.0,1.0};
	GLfloat specularColor[] = {0.0,1.0,1.0,1.0};	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dissuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat light_position2[] = { CameraRadius*cos(CameraTheta)*sin(CameraPhi)+5.1,			  
			  CameraRadius*cos(CameraPhi)+0.1 , 
			  CameraRadius*sin(CameraTheta)*sin(CameraPhi),0.0 };

	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);
	GLfloat ambientColor1[] = {0.8863,0.7255,0.5608, 1.0};
	GLfloat dissuseColor1[] = {0.8863,0.7255,0.5608,1.0};
	GLfloat specularColor1[] = {0.8863,0.7255,0.5608,1.0};	
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientColor1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, dissuseColor1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularColor1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);

}

//Sets the light positions, etc. parameters for the shaders
void setParameters(GLuint program)
{

	update_Light_Position();

	GLint locns = glGetUniformLocation(program, "ns_");
	if (locns == -1)
        std::cout << "Warning: can't find uniform variable ns_ !\n";
    glUniform1f(locns, ns_);

	GLint localpha = glGetUniformLocation(program, "alpha_");
	if (localpha == -1)
        std::cout << "Warning: can't find uniform variable alpha_ !\n";
    glUniform1f(localpha, alpha_);

	GLint locblend = glGetUniformLocation(program, "blendMap");
	if (locblend == -1)
        std::cout << "Warning: can't find uniform variable blendMap !\n";
    glUniform1i(locblend, 0);

	GLint locred = glGetUniformLocation(program, "texAort");
 	if (locred == -1)
        std::cout << "Warning: can't find uniform variable texAort !\n";
    glUniform1i(locred, 1);   


	GLint locblue = glGetUniformLocation(program, "texPulm");
 	if (locblue == -1)
        std::cout << "Warning: can't find uniform variable texPulm !\n";
    glUniform1i(locblue, 2);   

	GLint locmuscle = glGetUniformLocation(program, "texMuscle");
 	if (locmuscle == -1)
        std::cout << "Warning: can't find uniform variable texMuscle !\n";
    glUniform1i(locmuscle, 3);  

	GLint locback = glGetUniformLocation(program, "texBackground");
 	if (locback == -1)
        std::cout << "Warning: can't find uniform variable texBackground !\n";
    glUniform1i(locback, 4);  

}


/****************************************************************
Utility methods:
shader file reader
mesh reader for objectt
****************************************************************/
//Read the shader files, given as parameter.
char *shaderFileRead(char *fn) {


	FILE *fp = fopen(fn,"r");
	if(!fp)
	{
		cout<< "Failed to load " << fn << endl;
		return " ";
	}
	else
	{
		cout << "Successfully loaded " << fn << endl;
	}
	
	char *content = NULL;

	int count=0;

	if (fp != NULL) 
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);

		if (count > 0) 
		{
			content = (char *)malloc(sizeof(char) * (count+1));
			count = fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);
	}
	return content;
}