CXX=g++
CXXFLAGS=-Wall -g -D_POSIX_C_SOURCE=200809L -pedantic -std=c++17
LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto
TARGET=isabot
OBJECTS=DiscordSocket.o DiscordBot.o HttpResponse.o JsonParser.o JsonValue.o
HEADERS=DiscordSocket.hpp DiscordBot.hpp JsonParser.hpp HttpResponse.hpp JsonValue.hpp

all: ${TARGET}

${TARGET}: ${OBJECTS} ${TARGET}.o ${HEADERS}
	${CXX} -o ${TARGET} ${TARGET}.o ${OBJECTS} ${LDFLAGS} ${LDLIBS}

clean:
	rm -rf ${TARGET}.o ${TARGET} ${OBJECTS}

test: ${TARGET}
	./${TARGET} -v -t NzY5NjcwODgyNTY5NDg2Mzc2.X5SZ3g.Pw3JEddBOMCyp8VHd10ZnowZ8UY