CXX= g++
LINK = $(CXX)
EXE = nbody_simulation
CXXFLAGS = -O3 -std=c++11
CXXFLAGSPARALLEL = -fopenmp -O3 -std=c++11 -lpthread

#CXXFLAGS += -g -DVERBOSE

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

OBJ_ROOT = nbody.o

default: $(EXE)

$(EXE): $(OBJ) $(OBJ_ROOT)
	$(LINK) $(OBJ) $(CXXFLAGS) -o $@

openmp: $(EXE)

$(EXE): $(OBJ) $(OBJ_ROOT)
	$(LINK) $(OBJ) $(CXXFLAGSPARALLEL) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGSPARALLEL) -c $<

clean:
	rm -rf *.o $(EXE)

clean-o:
	rm -rf *.o
