CC=mingw32-g++.exe
cc=gcc
SOURCES=Camera.cpp \
		Gamani.cpp \
		Generic/CString.cpp \
		GUI/Display.cpp \
		GUI/Forms/NavDisplay.cpp \
		GUI/GUIRenderer.cpp \
		GUI/LayoutManager.cpp \
		GUI/WButton.cpp \
		GUI/Widget.cpp \
		GUI/WLayout.cpp \
		GUI/WMenu.cpp \
		GUI/WText.cpp \
		HUD.cpp \
		Math/Matrix.cpp \
		Math/Vector.cpp \
		Models/GLTexture.cpp \
		Models/Model.cpp \
		Models/Wrapper3DS.cpp \
		Objects/AstralBody.cpp \
		Objects/DynamicBody.cpp \
		Objects/ModelRenderable.cpp \
		Objects/Planet.cpp \
		Objects/Renderable.cpp \
		Objects/Ship.cpp \
		Objects/Star.cpp \
		Objects/StarSystem.cpp \
		Objects/Station.cpp \
		pngLib/PngWrapper.cpp \
		Renderer.cpp \
		SkyBox.cpp \
		StdAfx.cpp \
		World.cpp \

OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=gamani.exe

INCLUDE=-IInclude/ -IInclude/Generic -IInclude/GUI -IInclude/GUI/Forms -IInclude/Models -IInclude/Objects -IInclude/Math -IInclude/pngLib

FLAGS=${INCLUDE} -DGLUT_DISABLE_ATEXIT_HACK -DGL_GLEXT_PROTOTYPES -O5

LDFLAGS=-LLibs -lglut32 -lglu32 -lopengl32 -lwinmm -lgdi32 -lglew32

GLLIBS=Libs/* pngLib/libpng.lib pngLib/zlib.lib

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) ${GLLIBS} -o $@

.cpp.o:
	$(CC) $(FLAGS) -c $< -o $@
clean:
	rm -f `find ./ -name "*.o"`
	rm -f gamani.exe
