
CXXFLAGS =  --std=c++11
CXXFLAGS += -Ihttp/include

all: service


OBJECTS=service.o manager.o
LIBS=-lboost_system -lboost_coroutine

service: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o service ${LIBS}


