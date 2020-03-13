CC = gcc -Wall -std=c11 -ggdb

execute: 
	$(CC) -o zad1 main.c
	make execute_tests

clean:
	find . -maxdepth 1 ! -name 'main.c' ! -name 'makefile' ! -name 'wyniki1' -type f -exec rm -v {} +

execute_tests:
	find . -type f -name wyniki1 -exec rm {} \;
	echo 'RESULTS FOR SYSTEM' >> wyniki1
	make tests_system
	echo 'RESULTS FOR LIBRARY' >> wyniki1
	make tests_library
	make clean

tests_system:
	echo 'RESULTS FOR COPYING' >> wyniki1
	echo '500 records of length 1' >> wyniki1 | ./zad1 generate tmp1 500 1 >> wyniki1
	./zad1 copy tmp1 tmp1c 500 1 sys >> wyniki1
	echo '500 records of length 4' >> wyniki1 | ./zad1 generate tmp2 500 4 >> wyniki1
	./zad1 copy tmp2 tmp2c 500 4 sys >> wyniki1
	echo '500 records of length 512' >> wyniki1 | ./zad1 generate tmp3 500 512 >> wyniki1
	./zad1 copy tmp3 tmp3c 500 512 sys >> wyniki1
	echo '500 records of length 1024' >> wyniki1 | ./zad1 generate tmp4 500 1024 >> wyniki1
	./zad1 copy tmp4 tmp4c 500 1024 sys >> wyniki1
	echo '500 records of length 4096' >> wyniki1 | ./zad1 generate tmp5 500 4096 >> wyniki1
	./zad1 copy tmp5 tmp5c 500 4096 sys >> wyniki1
	echo '500 records of length 8192' >> wyniki1 | ./zad1 generate tmp6 500 8192 >> wyniki1
	./zad1 copy tmp6 tmp6c 500 8192 sys >> wyniki1
	echo '2000 records of length 1' >> wyniki1 | ./zad1 generate tmp7 2000 1 >> wyniki1
	./zad1 copy tmp7 tmp7c 2000 1 sys >> wyniki1
	echo '2000 records of length 4' >> wyniki1 | ./zad1 generate tmp8 2000 4 >> wyniki1
	./zad1 copy tmp8 tmp8c 2000 4 sys >> wyniki1
	echo '2000 records of length 512' >> wyniki1 | ./zad1 generate tmp9 2000 512 >> wyniki1
	./zad1 copy tmp9 tmp9c 2000 512 sys >> wyniki1
	echo '2000 records of length 1024' >> wyniki1 | ./zad1 generate tmp10 2000 1024 >> wyniki1
	./zad1 copy tmp10 tmp10c 2000 1024 sys >> wyniki1
	echo '2000 records of length 4096' >> wyniki1 | ./zad1 generate tmp11 2000 4096 >> wyniki1
	./zad1 copy tmp11 tmp11c 2000 4096 sys >> wyniki1
	echo '2000 records of length 8192' >> wyniki1 | ./zad1 generate tmp12 2000 8192 >> wyniki1
	./zad1 copy tmp12 tmp12c 2000 8192 sys >> wyniki1
	echo 'RESULTS FOR SORTING' >> wyniki1
	echo '500 records of length 1' >> wyniki1 | ./zad1 sort tmp1 500 1 sys >> wyniki1
	echo '500 records of length 4' >> wyniki1 | ./zad1 sort tmp2 500 4 sys >> wyniki1
	echo '500 records of length 512' >> wyniki1 | ./zad1 sort tmp3 500 512 sys >> wyniki1
	echo '500 records of length 1024' >> wyniki1 | ./zad1 sort tmp4 500 1024 sys >> wyniki1
	echo '500 records of length 4096' >> wyniki1 | ./zad1 sort tmp5 500 4096 sys >> wyniki1
	echo '500 records of length 8192' >> wyniki1 | ./zad1 sort tmp6 500 8192 sys >> wyniki1
	echo '2000 records of length 1' >> wyniki1 | ./zad1 sort tmp7 2000 1 sys >> wyniki1
	echo '2000 records of length 4' >> wyniki1 | ./zad1 sort tmp8 2000 4 sys >> wyniki1
	echo '2000 records of length 512' >> wyniki1 | ./zad1 sort tmp9 2000 512 sys >> wyniki1
	echo '2000 records of length 1024' >> wyniki1 | ./zad1 sort tmp10 2000 1024 sys >> wyniki1
	echo '2000 records of length 4096' >> wyniki1 | ./zad1 sort tmp11 2000 4096 sys >> wyniki1
	echo '2000 records of length 8192' >> wyniki1 | ./zad1 sort tmp12 2000 8192 sys >> wyniki1

tests_library:
	echo 'RESULTS FOR COPYING' >> wyniki1
	echo '500 records of length 1' >> wyniki1
	./zad1 copy tmp1 tmp1cc 500 1 lib >> wyniki1
	echo '500 records of length 4' >> wyniki1
	./zad1 copy tmp2 tmp2cc 500 4 lib >> wyniki1
	echo '500 records of length 512' >> wyniki1
	./zad1 copy tmp3 tmp3cc 500 512 lib >> wyniki1
	echo '500 records of length 1024' >> wyniki1
	./zad1 copy tmp4 tmp4cc 500 1024 lib >> wyniki1
	echo '500 records of length 4096' >> wyniki1
	./zad1 copy tmp5 tmp5cc 500 4096 lib >> wyniki1
	echo '500 records of length 8192' >> wyniki1
	./zad1 copy tmp6 tmp6cc 500 8192 lib >> wyniki1
	echo '2000 records of length 1' >> wyniki1
	./zad1 copy tmp7 tmp7cc 2000 1 lib >> wyniki1
	echo '2000 records of length 4' >> wyniki1
	./zad1 copy tmp8 tmp8cc 2000 4 lib >> wyniki1
	echo '2000 records of length 512' >> wyniki1
	./zad1 copy tmp9 tmp9cc 2000 512 lib >> wyniki1
	echo '2000 records of length 1024' >> wyniki1
	./zad1 copy tmp10 tmp10cc 2000 1024 lib >> wyniki1
	echo '2000 records of length 4096' >> wyniki1
	./zad1 copy tmp11 tmp11cc 2000 4096 lib >> wyniki1
	echo '2000 records of length 8192' >> wyniki1
	./zad1 copy tmp12 tmp12cc 2000 8192 lib >> wyniki1
	echo 'RESULTS FOR SORTING' >> wyniki1
	echo '500 records of length 1' >> wyniki1 | ./zad1 sort tmp1c 500 1 lib >> wyniki1
	echo '500 records of length 4' >> wyniki1 | ./zad1 sort tmp2c 500 4 lib >> wyniki1
	echo '500 records of length 512' >> wyniki1 | ./zad1 sort tmp3c 500 512 lib >> wyniki1
	echo '500 records of length 1024' >> wyniki1 | ./zad1 sort tmp4c 500 1024 lib >> wyniki1
	echo '500 records of length 4096' >> wyniki1 | ./zad1 sort tmp5c 500 4096 lib >> wyniki1
	echo '500 records of length 8192' >> wyniki1 | ./zad1 sort tmp6c 500 8192 lib >> wyniki1
	echo '2000 records of length 1' >> wyniki1 | ./zad1 sort tmp7c 2000 1 lib >> wyniki1
	echo '2000 records of length 4' >> wyniki1 | ./zad1 sort tmp8c 2000 4 lib >> wyniki1
	echo '2000 records of length 512' >> wyniki1 | ./zad1 sort tmp9c 2000 512 lib >> wyniki1
	echo '2000 records of length 1024' >> wyniki1 | ./zad1 sort tmp10c 2000 1024 lib >> wyniki1
	echo '2000 records of length 4096' >> wyniki1 | ./zad1 sort tmp11c 2000 4096 lib >> wyniki1
	echo '2000 records of length 8192' >> wyniki1 | ./zad1 sort tmp12c 2000 8192 lib >> wyniki1
