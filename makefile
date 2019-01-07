CCFLAGS = -m64 -std=gnu99 -O3 -march=native -s

avx_tests: tests.o dream128_avx.o dream256_avx.o
		gcc -o avx_tests tests.o avx_ref/dream128_avx.o avx_ref/dream256_avx.o

tests: tests.o dream128.o dream256.o
		gcc -o tests tests.o ref/dream128.o ref/dream256.o

tests.o: tests.c common/dream.h
		gcc $(CCFLAGS) -c tests.c

dream256_avx.o: avx_ref/dream256_avx.c avx_ref/dream_round256_avx.h common/dream.h common/dream_impl.h
		gcc $(CCFLAGS) -c avx_ref/dream256_avx.c -o avx_ref/dream256_avx.o

dream128_avx.o: avx_ref/dream128_avx.c avx_ref/dream_round128_avx.h common/dream.h common/dream_impl.h
		gcc $(CCFLAGS) -c avx_ref/dream128_avx.c -o avx_ref/dream128_avx.o

dream256.o: ref/dream256.c common/dream.h common/dream_impl.h
		gcc $(CCFLAGS) -c ref/dream256.c -o ref/dream256.o

dream128.o: ref/dream128.c common/dream.h common/dream_impl.h
		gcc $(CCFLAGS) -c ref/dream128.c -o ref/dream128.o

clean: 
		rm avx_tests tests *.o avx_ref/*.o ref/*.o
