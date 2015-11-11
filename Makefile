CC=g++
SRCEXT=cpp
SRCS=$(shell find . -type f -name '*.$(SRCEXT)')
OBJS=$(subst .$(SRCEXT),.o,$(SRCS))
TESTS=$(shell find . -type f -name '*_test.sh')
CFLAGS := -g -Wall -std=c++0x

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
exec: build
	./spooler $(ARG)

clean:
	rm -f spooler *.core *.o

test:  set_up_tests run_tests tear_down_tests

set_up_tests: spooler ./tests/test_dir

./tests/test_dir:
	@mkdir tests/test_dir
	@echo bar > tests/test_dir/foo
	@echo meow > tests/test_dir/cat
	@echo bark > tests/test_dir/dog
	@echo hello > tests/test_dir/world
	@chmod 000 tests/test_dir/foo
	@chmod 400 tests/test_dir/cat
	@chmod 200 tests/test_dir/dog

.PHONY: run_tests
run_tests: $(shell echo "$(TESTS)" | tr " " "\n" | sort -d)

.FORCE:
%_test.sh: .FORCE
	@sudo rm -rf /var/spool/printer
	@sudo mkdir -m 707 /var/spool/printer
	@sudo chown spooler /var/spool/printer
	@echo "Running test: $@"
	@$(SHELL) $@ | diff - $(subst .sh,.out,$@) || true
	@echo "----------------------------"

tear_down_tests: make_spool_dir
	@rm -rf ./tests/test_dir

install: spooler make_spooler_user set_executable_perms make_spool_dir cp_files

make_spool_dir:
	sudo rm -rf /var/spool/printer
	sudo mkdir -m 700 /var/spool/printer
	sudo chown spooler /var/spool/printer

make_spooler_user:
	sudo userdel spooler
	sudo useradd spooler
	echo "qrnMtlBsXI\nqrnMtlBsXI\n" | sudo passwd spooler

set_executable_perms: spooler
	chmod 755 ./addqueue
	chmod 755 ./showqueue
	chmod 755 ./rmqueue
	chmod 755 ./spooler
	sudo chown spooler ./spooler
	sudo -u spooler chmod u+s ./spooler

cp_files:
	sudo cp -p addqueue showqueue rmqueue spooler /usr/bin/
