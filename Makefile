#CC = g++
CC = mpic++
SRCS = $(wildcard ./src/*.cpp)
INC = ./src/
#INC-MPI = /usr/local/mpich-install/include/
OPTS = -std=c++17 -Wall -Werror -lGLEW -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
DOPTS = -fdiagnostics-color=always -g

EXEC = bin/nbody
DEXEC = debug/nbody

all: clean compile

dall: dclean dcompile

compile:
#	$(CC) $(SRCS) $(OPTS) -I $(INC) -I $(INC-MPI) -o $(EXEC)
	$(CC) $(SRCS) $(OPTS) -I $(INC) -o $(EXEC)

clean:
	rm -f $(EXEC)

dclean:
	rm -f $(DEXEC)

dcompile:
	$(CC) $(DOPTS) $(SRCS) $(OPTS) -I $(INC) -o $(DEXEC)