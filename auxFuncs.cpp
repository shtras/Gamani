#include "StdAfx.h"
#include "auxFuncs.h"

double RadToDeg(double rad)
{
  return rad * 180.0 / PI;
}

double DegToRad(double deg)
{
  return deg * PI / 180.0;
}

bool isNumberChar(int key)
{
  if (key >= 'a' && key <= 'z') {
    return true;
  }
  if (key >= 'A' && key <= 'Z') {
    return true;
  }
  return false;
}

bool isLetterChar(int key)
{
  if (key >= 0x30 && key <= 0x39) {
    return true;
  }
  return false;
}

void checkErrorDebug(CString errorMsg/* = ""*/)
{
  GLenum err = glGetError();
  if (err != 0) {
    Logger::getInstance().log(ERROR_LOG_NAME, errorMsg + CString(" OpenGL error: ") + CString((int)err));
  }
  assert(err == 0);
}

void checkReleaseError(CString errorMsg)
{
  checkErrorDebug(errorMsg);
}

char *textFileRead(const char *fn) 
{
  FILE *fp;
  char *content = NULL;
  int count=0;
  if (fn != NULL) {
    fp = fopen(fn,"rt");
    if (fp != NULL) {
      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);
      if (count > 0) {
        content = (char *)malloc(sizeof(char) * (count+1));
        count = fread(content,sizeof(char),count,fp);
        content[count] = '\0';
      }
      fclose(fp);
    }
  }
  return content;
}

void printShaderInfoLog(GLuint obj)
{
  int infologLength = 0;
  int charsWritten  = 0;
  char *infoLog;

  glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

  if (infologLength > 0)
  {
    infoLog = (char *)malloc(infologLength);
    glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
    printf("%s\n",infoLog);
    free(infoLog);
  }
}

void deleteShaders(GLuint shader)
{
  glDeleteProgram(shader);
}

void setShaders(const char* vertex, const char* frag, GLuint* pn)
{
  char *vs = NULL,*fs = NULL,*fs2 = NULL;

  GLuint v = glCreateShader(GL_VERTEX_SHADER);
  GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

  vs = textFileRead(vertex);
  fs = textFileRead(frag);

  const char * ff = fs;
  const char * vv = vs;

  glShaderSource(v, 1, &vv,NULL);
  glShaderSource(f, 1, &ff,NULL);

  free(vs);
  free(fs);

  glCompileShader(v);
  checkError("");
  glCompileShader(f);
  checkError("");

  GLuint p = glCreateProgram();
  checkError("");
  glAttachShader(p,f);
  checkError("");
  glAttachShader(p,v);
  checkError("");

  cout << "v" << endl;
  printShaderInfoLog(v);
  cout << "f" << endl;
  printShaderInfoLog(f);

  int res;
  glGetObjectParameterivARB(v, GL_OBJECT_COMPILE_STATUS_ARB, &res);
  assert(res == 1);
  glGetObjectParameterivARB(f, GL_OBJECT_COMPILE_STATUS_ARB, &res);
  assert(res == 1);

  glLinkProgram(p);
  glDeleteShader(v);
  glDeleteShader(f);
  checkError("");
  //glUseProgram(p);
  checkError("");
  if (pn) {
    *pn = p;
  } else {
    assert(0);
  }
}

char setBit(char byte, int bit)
{
  assert(bit >= 0 && bit < 8);
  char mask = 0x80 >> bit;
  return byte | mask;
}

char unsetBit(char byte, int bit)
{
  assert(bit >= 0 && bit < 8);
  char mask = (0x80 >> bit)^0xff;
  return byte & mask;
}

int getBit(int byte, int bit)
{
  assert(bit >= 0 && bit < 32);
  int mask = 0x80 >> bit;
  return ((byte & mask)>0)?1:0;
}

void dumpNumber(char* mem, int& offset, int number, int len)
{
  assert(len <= 32);
  int start = 0;
  if (len <= 8) {
    start = 8-len;
  }
  if (len > 8) {
    assert (len % 8 == 0);
  }
  int currBit = start;
  for (int i=0; i<len; ++i) {
    int charNum = offset/8;
    int bitNum = offset%8;
    int bit = getBit(number, currBit++);
    if (currBit >= 8) {
      currBit = 0;
      number >>= 8;
    }
    if (bit) {
      mem[charNum] = setBit(mem[charNum], bitNum);
    } else {
      mem[charNum] = unsetBit(mem[charNum], bitNum);
    }
    ++offset;
  }
}

int getBits(char* mem, int& offset, int len)
{
  assert(len <= 32);
  int res = 0;
  for (int i=0; i<len; ++i) {
    int charNum = offset/8;
    int bitNum = offset%8;
    int bit = getBit(mem[charNum], bitNum);
    res += bit;
    if (i != len-1) {
      res <<= 1;
    }
    ++offset;
  }
  return res;
}

int get8bit(char* mem, int& offset)
{
  return getBits(mem, offset, 8);
}

int get16bit(char* mem, int& offset)
{
  int i1 = get8bit(mem, offset);
  int i2 = get8bit(mem, offset);
  return (i2 << 8) + i1;
}

int get32bit(char* mem, int& offset)
{
  int i1 = get16bit(mem, offset);
  int i2 = get16bit(mem, offset);
  return (i2 << 16) + i1;
}
