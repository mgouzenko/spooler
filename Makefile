CC=clang++
SRCEXT=cpp
SRCS=$(shell find . -type f -name *.$(SRCEXT))
OBJS=$(subst .$(SRCEXT),.o,$(SRCS))
CFLAGS := -g -Wall -std=c++11

build:	spooler

# "spooler" requires a set of object files
# $? is the
spooler: $(OBJS)

%.o: %.$(SRCEXT)
	$(CC) $(CFLAGS) -c -o $@ $<

# Before testing, we must compile.
# Lines preceeded by @ aren't echoed before executing
# Execution will stop if a program has a non-zero return code;
# precede the line with a - to override that
test:	build
	./spooler a b "def" "hello world" <test1
	@echo "------------"
	./spooler <test2
	@echo "------------"
	./spooler " leading" "trailing " "blanks"  <test3

exec: build
	./spooler $(ARG)

clean:
	rm -f spooler *.core *.o
