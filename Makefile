CXX= mpicxx
LINK = $(CXX)
EXE = nbody_simulation
EXE_SEQ = nbody_simulation_seq
CXXFLAGS = -O3 -std=c++11 -fpermissive
CXXFLAGSPARALLEL = -fopenmp -lgomp -O3 -std=c++11 -lpthread -fpermissive

#CXXFLAGS += -g -DVERBOSE

SRC = $(wildcard main.cpp nbody.cpp)
OBJ = $(SRC:.cpp=.o)

SRC_SEQ = $(wildcard main_seq.cpp nbody.cpp)
OBJ_SEQ = $(SRC_SEQ:.cpp=.o)

OBJ_ROOT = nbody.o

mpi: $(EXE)
$(EXE): $(OBJ) $(OBJ_ROOT)
	$(LINK) $(OBJ) $(CXXFLAGSPARALLEL) -o $@

seq: $(EXE_SEQ)
$(EXE_SEQ): $(OBJ_SEQ) $(OBJ_ROOT)
	$(LINK) $(OBJ_SEQ) $(CXXFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGSPARALLEL) -c $<

clean:
	rm -rf *.o $(EXE) $(EXE_SEQ)
	rm -rf nbody_simulation.out
