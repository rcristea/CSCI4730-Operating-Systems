CFLAGS=-g

all: vm input_gen list_test

vm: vm.c vm.h pagetable.c disk.c list.c clock_list.c pt_core.c
		gcc $(CFLAGS) vm.c pagetable.c disk.c list.c clock_list.c pt_core.c -g -o vm

input_gen: input_gen.c vm.h
		gcc input_gen.c $(CFLAGS) -o input_gen

list_test: list_test.c list.c list.h
		gcc list_test.c list.c $(CFLAGS) -o list_test

clean:
		rm -f vm input_gen list_test
