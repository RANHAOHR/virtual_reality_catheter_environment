#ifndef __assign8_temp_h__
#define __assign8_temp_h__


#include <stdio.h>
#include <GL/glut.h>


// Vertex and Face data structure sued in the mesh reader
// Feel free to change them
int getUniformVariable(GLuint program,char *name);
void update_Light_Position();
void setHeartParameters(GLuint program);
void setHumanParameters(GLuint program);
void setCatheterParameters(GLuint program);

void error_exit(int status, char *text);
int PrintOGLError(char *file, int line);
void setHeartShaders();
void setHumanShaders();
void setCatheterShaders();

void readCatheterParam(char* file);

#endif 