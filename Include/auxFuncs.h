#pragma once

#include <gl/glew.h>
#include <gl/glut.h>

double RadToDeg(double rad);
double DegToRad(double deg);

bool isNumberChar(int key);
bool isLetterChar(int key);

void checkErrorDebug(CString errorMsg = "");

void checkReleaseError(CString errorMsg);

char *textFileRead(const char *fn);

void deleteShaders(GLuint shader);

void setShaders(const char* vertex, const char* frag, GLuint* pn);

