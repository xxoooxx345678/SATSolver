# A template C++ Makefile for your SAT solver.

# Debugging flags
#FLAGS=-Wall -Wold-style-cast -Wformat=2 -ansi -pedantic -ggdb3 -DDEBUG

# Optimizing flags
FLAGS:=-Wall -g -Wold-style-cast -Wformat=2 -ansi -pedantic -pthread -O3 -std=c++17

# List all the .o files you need to build here
OBJS =  src/parser.o \
	src/main.o \
	src/LiteralandClause.o \
	src/SATSolver.o \
	src/Heap.o

# This is the name of the executable file that gets built.  Please
# don't change it.
EXENAME=sat

RESTART=
NIVER=

ifneq ("$(RESTART)", "")
	FLAGS += -DRESTART
endif
ifneq ("$(NIVER)", "")
	FLAGS += -DNIVER
endif

# Compile targets

all: $(OBJS)
	g++ $(FLAGS) $(OBJS) -lz -o $(EXENAME)
%.o: src/%.cpp
	g++ $(FLAGS) -c src/$<

#all: $(OBJS)
#	g++ $(FLAGS) $(OBJS) -lz -o $(EXENAME)
#parser.o: parser.cpp parser.h
#	g++ $(FLAGS) -c parser.cpp
#main.o: main.cpp parser.h LiteralandClause.h SATSolver.h
#	g++ $(FLAGS) -c main.cpp
#LiteralandClause.o: LiteralandClause.h LiteralandClause.cpp
#	g++ $(FLAGS) -c LiteralandClause.cpp
#SATSolver.o: SATSolver.h SATSolver.cpp
#	g++ $(FLAGS) -c SATSolver.cpp

# Add more compilation targets here



# The "phony" `clean' compilation target.  Type `make clean' to remove
# your object files and your executable.
.PHONY: clean clean_res
clean:
	rm -rf $(OBJS) $(EXENAME)

clean_res:
	rm -rf results/*.sat