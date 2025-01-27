# Makefile - Single runner, cleaning up object files immediately after build

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

OBJS = main.o cache_manager.o lru.o nhit.o arc.o larc.o

all: cache_runner ocp
	@rm -f $(OBJS)

ocp: ocp.cpp
	$(CXX) $(CXXFLAGS) -o oracle ocp.cpp

cache_runner: $(OBJS)
	$(CXX) $(CXXFLAGS) -o cache_runner $(OBJS)

main.o: main.cpp cache_manager.h lru.h nhit.h arc.h larc.h
	$(CXX) $(CXXFLAGS) -c main.cpp

cache_manager.o: cache_manager.cpp cache_manager.h
	$(CXX) $(CXXFLAGS) -c cache_manager.cpp

lru.o: lru.cpp lru.h cache_manager.h
	$(CXX) $(CXXFLAGS) -c lru.cpp

nhit.o: nhit.cpp nhit.h cache_manager.h
	$(CXX) $(CXXFLAGS) -c nhit.cpp

arc.o: arc.cpp arc.h cache_manager.h
	$(CXX) $(CXXFLAGS) -c arc.cpp

larc.o: larc.cpp larc.h cache_manager.h
	$(CXX) $(CXXFLAGS) -c larc.cpp

clean:
	rm -f cache_runner $(OBJS)