#ifndef __assign8_temp_h__
#define __assign8_temp_h__


#include <stdio.h>
#include <GL/glut.h>

int getUniformVariable(GLuint program,char *name);
void update_Light_Position();
void setParameters(GLuint program);

void error_exit(int status, char *text);
int PrintOGLError(char *file, int line);
void setHeartShaders();
void setHumanShaders();
void meshReader (char *filename,int sign);

#endif 