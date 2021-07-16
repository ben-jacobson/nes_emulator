#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#

PROJECT_NAME = nes_emulator

# define the Cpp compiler to use
CXX = g++

# define any compile-time flags
CXXFLAGS	:= -std=c++17 -Wall -Wextra -g

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -Llib/SDL2 -lSDL2 -lSDL2_ttf 

# define output directory
OUTPUT	:= output

# define source directory
SRC		:= src

# define the tests directory
TESTS	:= tests

# define include directory
INCLUDE	:= include include/SDL2

# define lib directory
LIB		:= lib

ifeq ($(OS),Windows_NT)
MAIN	:= $(PROJECT_NAME).exe
TEST_OUT := tests.exe
SOURCEDIRS	:= $(SRC)
TEST_SOURCE_DIRS := $(TESTS)
INCLUDEDIRS	:= $(INCLUDE)
LIBDIRS		:= $(LIB)
FIXPATH = $(subst /,\,$1)
# RM			:= del /q /f
RM			:= rm -f
MD	:= mkdir
else
MAIN	:= $(PROJECT_NAME)
TEST_OUT := tests
SOURCEDIRS	:= $(shell find $(SRC) -type d)
TEST_SOURCE_DIRS := $(shell find $(TESTS) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d)
FIXPATH = $1
RM = rm -f
MD	:= mkdir -p
endif

# define any directories containing header files other than /usr/include
INCLUDES	:= $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))

# define the C libs
LIBS		:= $(patsubst %,-L%, $(LIBDIRS:%/=%))

# define the C source files
SOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
TEST_SOURCES := SOURCES
TEST_SOURCES := $(filter-out src/main.cpp, $(SOURCES))						# create a separate set of objects that exclude main.cpp for compiling tests
TEST_SOURCES += $(wildcard $(patsubst %,%/*.cpp, $(TEST_SOURCE_DIRS)))

# define the C object files 
OBJECTS		:= $(SOURCES:.cpp=.o)
TEST_OBJECTS := $(TEST_SOURCES:.cpp=.o) 

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))
OUTPUT_TESTS := $(call FIXPATH,$(OUTPUT)/$(MAIN))

all: $(OUTPUT) $(MAIN)
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(OBJECTS) 
	$(CXX) $(OBJECTS) $(CXXFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS) -o $(OUTPUTMAIN) 

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN)
	$(RM) $(TEST_SOURCE_DIRS)/$(TEST_OUT)
	$(RM) $(call FIXPATH,$(OBJECTS))
	$(RM) $(call FIXPATH,$(TEST_OBJECTS))
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN)
	@echo Executing 'run: all' complete!

$(TESTS): $(TEST_OBJECTS)
	make
	$(CXX) $(TEST_OBJECTS) $(CXXFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS) -o $(OUTPUT)/$(TEST_OUT) 
