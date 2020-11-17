CXX=g++
CXXFLAGS=-Wall -g -D_POSIX_C_SOURCE=200809L -pedantic -std=c++17
LDFLAGS = -L/usr/local/ssl/lib
LDLIBS = -lssl -lcrypto
TARGET=isabot
OBJECTS=DiscordSocket.o DiscordBot.o HttpResponse.o JsonParser.o JsonValue.o
HEADERS=DiscordSocket.hpp DiscordBot.hpp JsonParser.hpp HttpResponse.hpp JsonValue.hpp
SOURCES=DiscordSocket.cpp DiscordBot.cpp JsonParser.cpp HttpResponse.cpp JsonValue.cpp
PACK=xstrba05.tar
TEX = pdflatex -shell-escape -interaction=nonstopmode -file-line-error

all: ${TARGET}

${TARGET}: ${OBJECTS} ${TARGET}.o ${HEADERS}
	${CXX} -o ${TARGET} ${TARGET}.o ${OBJECTS} ${LDFLAGS} ${LDLIBS}

raw:
	make clean && make

manual: manual.pdf

manual.pdf: manual/manual.tex
	$(TEX) manual/manual.tex

pack: manual.pdf
	tar -zcvf ${PACK} ${HEADERS} ${SOURCES} ${TARGET}.cpp tests.cpp Makefile README.md manual.pdf

test: tests
	./tests

tests: tests.cpp ${TARGET}
	${CXX} ${CXXFLAGS} -c tests.cpp
	${CXX} ${CXXFLAGS} -o tests tests.o

clean:
	rm -rf ${TARGET}.o ${TARGET} ${OBJECTS} tests tests.o ${PACK} manual.pdf manual.aux manual.log manual.out manual.toc
