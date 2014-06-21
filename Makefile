all: parser

OBJS = parser.o  \
       codegen.o \
       main.o    \
       tokens.o  \

CPPFLAGS = `llvm-config --cppflags`
LDFLAGS = `llvm-config --ldflags`
LIBS = `llvm-config --libs`

.PHONY : test test_correct clean
test:
	./parser test/new.w

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