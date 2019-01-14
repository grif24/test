CC = gcc 
CCFLAGS = -m64 -std=gnu99 -O3 -march=native -s

dsum256_avx: dsum256.o dream256_avx.o
		$(CC) -o dsum256_avx dsum256.o avx_ref/dream256_avx.o

dsum128_avx: dsum128.o dream128_avx.o
		$(CC) -o dsum128_avx dsum128.o avx_ref/dream128_avx.o

dsum256: dsum256.o dream256.o
		$(CC) -o dsum256 dsum256.o ref/dream256.o

dsum128: dsum128.o dream128.o
		$(CC) -o dsum128 dsum128.o ref/dream128.o

tests_avx: tests.o dream128_avx.o dream256_avx.o
		$(CC) -o tests_avx tests.o avx_ref/dream128_avx.o avx_ref/dream256_avx.o

tests: tests.o dream128.o dream256.o
		$(CC) -o tests tests.o ref/dream128.o ref/dream256.o

dsum256.o: dsum.c common/dream.h
		$(CC) $(CCFLAGS) -DV256 -c dsum.c -o dsum256.o

dsum128.o: dsum.c common/dream.h
		$(CC) $(CCFLAGS) -DV128 -c dsum.c -o dsum128.o

tests.o: tests.c common/dream.h
		$(CC) $(CCFLAGS) -c tests.c

dream256_avx.o: avx_ref/dream256_avx.c avx_ref/dream_round256_avx.h common/dream.h common/dream_impl.h
		$(CC) $(CCFLAGS) -c avx_ref/dream256_avx.c -o avx_ref/dream256_avx.o

dream128_avx.o: avx_ref/dream128_avx.c avx_ref/dream_round128_avx.h common/dream.h common/dream_impl.h
		$(CC) $(CCFLAGS) -c avx_ref/dream128_avx.c -o avx_ref/dream128_avx.o

dream256.o: ref/dream256.c common/dream.h common/dream_impl.h
		$(CC) $(CCFLAGS) -c ref/dream256.c -o ref/dream256.o

dream128.o: ref/dream128.c common/dream.h common/dream_impl.h
		$(CC) $(CCFLAGS) -c ref/dream128.c -o ref/dream128.o

clean_avx: 
		rm dsum128_avx dsum256_avx avx_tests *.o avx_ref/*.o
	
clean:
		rm dsum128 dsum256 tests *.o ref/*.o
