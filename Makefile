CXX=g++
CXXFLAGS=-Wall -g -D_POSIX_C_SOURCE=200809L -pedantic -std=c++17
LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto
TARGET=isabot
OBJECTS=DiscordSocket.o DiscordBot.o HttpResponse.o JsonParser.o JsonValue.o
HEADERS=DiscordSocket.hpp DiscordBot.hpp JsonParser.hpp HttpResponse.hpp JsonValue.hpp
SOURCES=DiscordSocket.cpp DiscordBot.cpp JsonParser.cpp HttpResponse.cpp JsonValue.cpp
PACK=xstrba05.tar

all: ${TARGET}

${TARGET}: ${OBJECTS} ${TARGET}.o ${HEADERS}
	${CXX} -o ${TARGET} ${TARGET}.o ${OBJECTS} ${LDFLAGS} ${LDLIBS}

raw:
	make clean && make

pack:
	tar -zcvf ${PACK} ${HEADERS} ${SOURCES} ${TARGET}.cpp tests.cpp Makefile README.md manual.pdf

test: tests
	./tests

tests: tests.cpp ${TARGET}
	${CXX} ${CXXFLAGS} -c tests.cpp
	${CXX} ${CXXFLAGS} -o tests tests.o

clean:
	rm -rf ${TARGET}.o ${TARGET} ${OBJECTS} tests tests.o ${PACK}

try: ${TARGET}
	./${TARGET} -v -t NzY5NjcwODgyNTY5NDg2Mzc2.X5SZ3g.Pw3JEddBOMCyp8VHd10ZnowZ8UY