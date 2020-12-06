CXX= mpicxx
LINK = $(CXX)
EXE = nbody_simulation
CXXFLAGS = -O3 -std=c++11
CXXFLAGSPARALLEL = -fopenmp -lgomp -O3 -std=c++11 -lpthread

#CXXFLAGS += -g -DVERBOSE

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

OBJ_ROOT = nbody.o

mpi: $(EXE)

$(EXE): $(OBJ) $(OBJ_ROOT)
	$(LINK) $(OBJ) $(CXXFLAGSPARALLEL) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGSPARALLEL) -c $<

clean:
	rm -rf *.o $(EXE)

clean-o:
	rm -rf *.o
