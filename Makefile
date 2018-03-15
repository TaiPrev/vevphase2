# The source file where the main() function is

SOURCEMAIN = Browser/browser.cc

# Library files

SRC = Math/vector3.cc Math/trfm3D.cc Math/plane.cc Math/line.cc Math/segment.cc Math/bbox.cc Math/bsphere.cc Math/intersect.cc\
	Math/bboxGL.cc Math/trfmStack.cc\
	Geometry/triangleMesh.cc Geometry/gObject.cc Geometry/gObjectManager.cc\
	Geometry/triangleMeshGL.cc\
	Shading/light.cc Shading/material.cc Shading/texture.cc Shading/texturert.cc Shading/image.cc\
	Shading/textureManager.cc Shading/materialManager.cc Shading/lightManager.cc Shading/imageManager.cc\
	Shaders/shaderUtils.cc Shaders/shaderManager.cc Shaders/shader.cc\
	Camera/camera.cc Camera/avatar.cc Camera/cameraManager.cc Camera/avatarManager.cc\
	Scene/node.cc Scene/nodeManager.cc Scene/renderState.cc Scene/scene.cc\
	Misc/constants.cc Misc/tools.cc Misc/jsoncpp.cc Misc/parse_scene.cc\
	Browser/scenes.cc Browser/skybox.cc
#   Browser/skybox.cc
#	Misc/list.cc Misc/hash.cc Misc/hashlib.cc Misc/set.cc Misc/vector.cc Misc/parse_scene.cc Misc/parse_scene_json.cc Misc/JSON_parser.cc\

CSRC = Misc/glm.c

# Don't change anything below
DEBUG = 1

JPEG_LIBDIR=./libjpeg6b-6b
JPEG_LIB=$(JPEG_LIBDIR)/libjpeg.a

INCLUDE_DIR = -I. -I./Camera -I./Geometry  -I./Math -I./Misc -I./Shading -I./Shaders -I./Scene -I$(JPEG_LIBDIR)
LIBDIR = -L/usr/lib/nvidia-367/ -L/usr/lib/nvidia-375/ -L $(JPEG_LIBDIR)
LIBS = -lm -lglut -lGLU -lGL -ljpeg -lGLEW

ifdef DEBUG
OPTFLAGS = -g
else
OPTFLAGS = -O2
endif

CCOPTIONS = -Wall -Wno-unused-function -Wno-unused-variable $(OPTFLAGS)
MEMBERS = $(SRC:.cc=.o)
CMEMBERS = $(CSRC:.c=.o)
EXEC  = $(basename $(notdir $(SOURCEMAIN)))

all: $(EXEC)

%.o : %.cc
	g++ -c -o $@ $(CCOPTIONS) $(INCLUDE_DIR) $<

%.o : %.c
	gcc -c -o $@ $(CCOPTIONS) $(INCLUDE_DIR) $<

$(EXEC): $(JPEG_LIB) $(TARGET) $(MEMBERS) $(CMEMBERS) $(SOURCEMAIN)
	g++ $(CCOPTIONS) -o $@ Browser/$@.cc $(MEMBERS) $(CMEMBERS) $(INCLUDE_DIR) $(LIBDIR) $(LIBS)

$(JPEG_LIB):
	(cd $(JPEG_LIBDIR); ./configure --enable-static --disable-shared)
	(cd $(JPEG_LIBDIR); make)
	mv $(JPEG_LIBDIR)/.libs/libjpeg.a $(JPEG_LIBDIR)

.PHONY : all clean jpeglib_clean

clean:
	find . -type f -name '*.o' | xargs rm -f
	rm -f $(EXEC)

distclean:
	(cd $(JPEG_LIBDIR); make distclean)
