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

char setBit(char byte, int bit);

char unsetBit(char byte, int bit);

int getBit(int byte, int bit);

void dumpNumber(char* mem, int& offset, int number, int len);

int getBits(char* mem, int& offset, int len);

int get8bit(char* mem, int& offset);

int get16bit(char* mem, int& offset);

int get32bit(char* mem, int& offset);
