CC = gcc

all: test_assign1_1.o storage_mgr.o dberror.o 
	$(CC) -o test1 test_assign1_1.o storage_mgr.o dberror.o

test_assign1_1.o: test_assign1_1.c dberror.h storage_mgr.h test_helper.h
	$(CC) -c test_assign1_1.c

storage_mgr.o: storage_mgr.c storage_mgr.h 
	$(CC) -c storage_mgr.c

dberror.o: dberror.c dberror.h 
	$(CC) -c dberror.c

clean: 
	$(RM) test1 *.o *~

run_test:
	./test1