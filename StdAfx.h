#pragma once

//#include "VersionInfo.h"
#include "sigslot.h"
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <list>
#include <set>
#include <map>
#include "Vector.h"
#include "Matrix.h"
#include <Windowsx.h>
#include <process.h>
//#include <string>
#include "CString.h"
#include "auxFuncs.h"
#include "Logger.h"

#ifdef DEBUG
#define checkError(n) checkErrorDebug(n)
#else
#define checkError(n)
#endif

#include <gl/glew.h>
#include <gl/glut.h>

#ifdef DEBUG
#define BUILDSTR "Debug"
#else
#define BUILDSTR "Release"
#endif

#define INFO_LOG_NAME "info.log"
#define ERROR_LOG_NAME "error.log"

#define PI (3.14159265)

using namespace std;
using namespace sigslot;

typedef unsigned int uint32_t;

