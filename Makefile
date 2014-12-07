ASSETS_DIR=assets
INCLUDE_DIR=include
OBJ_DIR=obj
DEST_DIR=bin
SRC_DIR=src

CXX=g++
CC=gcc
DEBUG_OPTIONS=-std=c++11 -g -Wall -Werror
RELEASE_OPTIONS=-std=c++11-O2 -Wall
GLFLAGS=-lglut -lGL -lglfw -lGLEW
CFLAGS=-I./${INCLUDE_DIR} -DASSETS=\"${CURDIR}/${ASSETS_DIR}\"


_HEADERS = dbg.h assets.hpp obj_model.hpp zad1_demo.hpp
HEADERS = $(patsubst %,${INCLUDE_DIR}/%,${_HEADERS})

_OBJ = assets.o obj_model.o zad1_demo.o main.o
OBJ = $(patsubst %,${OBJ_DIR}/%,${_OBJ})


# ---------------------------

all: debug

release: OPTIONS=${RELEASE_OPTIONS}
release: CFLAGS += -DNDEBUG
release: executable

debug: OPTIONS=${DEBUG_OPTIONS}
debug: executable


executable: ${OBJ}
	${CXX} ${OPTIONS} -o ${DEST_DIR}/zad1 $^ ${CFLAGS} ${GLFLAGS}

# ------------------------------------

${OBJ}: ${OBJ_DIR}/%.o: ${SRC_DIR}/%.cpp ${HEADERS}
	${CXX} ${OPTIONS} -c -o $@ $< ${CFLAGS}


.PHONY:
	all clean

clean:
	@echo "Cleaning OBJ_DIR..."
ifdef OBJ_DIR
	-rm -rf ./${OBJ_DIR}/*
	@echo "Done."
else
	@echo "No OBJ_DIR set."
endif
	@echo "Cleaning OBJ_DIR..."
ifdef DEST_DIR
	-rm -rf ./${DEST_DIR}/*
	@echo "Done."
else
	@echo "No DEST_DIR set."
endif

# all:

# 	g++ -Werror -Wall main.cpp -lglut -lGL
