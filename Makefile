all: parser

OBJS = parser.o  \
       codegen.o \
       main.o    \
       tokens.o  \
       corefn.o  \

CPPFLAGS = `llvm-config --cppflags` -std=c++11
LDFLAGS = `llvm-config --ldflags`
LIBS = `llvm-config --libs`

.PHONY : test test1 test_correct clean
test:
	./parser test/new.w

test1:
	./parser test/new1.w

test_correct:
	./parser test/correct_new.w

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