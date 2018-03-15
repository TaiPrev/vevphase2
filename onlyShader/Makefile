# The source file where the main() function is

SOURCEMAIN = drawCube.cc

# Library files

SRC = shaderUtils.cc

# Don't change anything below
DEBUG = 1

LIBS = -lm -lglut -lGLU -lGL -lGLEW

ifdef DEBUG
OPTFLAGS = -g
else
OPTFLAGS = -O2
endif

CCOPTIONS = -Wall -Wno-unused-function -Wno-unused-variable $(OPTFLAGS)
MEMBERS = $(SRC:.c=.o)
EXEC  = $(basename $(notdir $(SOURCEMAIN)))

all: $(EXEC)

%.o : %.cc
	g++ -c -o $@ $(CCOPTIONS) $(INCLUDE_DIR) $<

$(EXEC): $(TARGET) $(MEMBERS) $(SOURCEMAIN)
	g++ $(CCOPTIONS) -o $@ $@.cc $(MEMBERS) $(INCLUDE_DIR) $(LIBDIR) $(LIBS)

.PHONY : all clean

clean:
	find . -type f -name '*.o' | xargs rm -f
	rm -f $(EXEC)
