all: parser

OBJS = parser.o  \
       codegen.o \
       main.o    \
       tokens.o  \
#       corefn.o  \

CPPFLAGS = `llvm-config --cppflags`
LDFLAGS = `llvm-config --ldflags`
LIBS = `llvm-config --libs`

test:
	parser test/wlang.w

clean:
	$(RM) -rf parser.cpp parser.hpp parser tokens.cpp $(OBJS)

parser.cpp: wlang.y
	bison -d -o $@ $^

parser.hpp: parser.cpp

tokens.cpp: wlang.l parser.hpp
	flex -o $@ $^

%.o: %.cpp
	c++ -c $(CPPFLAGS) -o $@ $<


parser: $(OBJS)
	c++ -o $@ $(OBJS) $(LIBS) $(LDFLAGS)