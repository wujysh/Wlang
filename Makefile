all: parser

OBJS = parser.o  \
       codegen.o \
       main.o    \
       tokens.o  \
       corefn.o  \

CPPFLAGS = `llvm-config --cppflags
LDFLAGS = `llvm-config --ldflags`
LIBS = `llvm-config --libs`

clean:
	$(RM) -rf parser.cpp parser.hpp parser tokens.cpp $(OBJS)

parser.cpp: wlang.y
	win_bison -d -o $@ $^

parser.hpp: parser.cpp

tokens.cpp: wlang.l parser.hpp
	win_flex -o $@ $^

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<


parser: $(OBJS)
	g++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)