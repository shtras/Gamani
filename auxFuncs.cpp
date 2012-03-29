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
