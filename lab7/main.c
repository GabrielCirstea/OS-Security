#include <x86intrin.h>
#include <stdio.h>
#include <stdlib.h>

void practic1()
{
#define SIZE 1000000
	char* long_arr = malloc(SIZE);
	if (!long_arr) {
		perror("malloc");
		exit(-1);
	}
	// make them all 1
	for (int i=0; i<SIZE; ++i) {
		long_arr[i] = 1;
	}

	for (int r=0; r<1024; ++r) {
		unsigned int sum = 0;
		for (int i=0; i<SIZE; ++i) {
			_mm_clflush(long_arr);
			sum += long_arr[i];
		}
	}
}

int do_practic1()
{
	unsigned int AUX = 0;
	unsigned long long int start_cicls = __rdtscp(&AUX);
	printf("Cicls = %lld\n", start_cicls);
	practic1();
	unsigned long long int end_cicls = __rdtscp(&AUX);
	printf("Cicls = %lld\n", end_cicls);
	printf("Diff = %lld\n", end_cicls - start_cicls);
	return 0;

}

void practic2()
{
	char * array[256] = {NULL};
	for (int i=0; i<256; ++i) {
		array[i] = (char*) malloc(4096);
		if(!array) {
			perror("malloc");
			exit(-1);
		}
	}

	for (int i=0; i<256; ++i) {
		_mm_clflush(array[i]);
	}

	// the secret is 4
	char secret = array[4][1000];

	unsigned int AUX = 0;
	int index = -1;
	unsigned long long int time = 1000000000;
	// try to access the memory in a random order to prevent the CPU from
	// preloading it
	for (int i=0; i<256; ++i) {
		unsigned long long int start_c =0, end_c = 0;
		start_c = __rdtscp(&AUX);
		char val = array[(i * 65) % 256][1000];
		end_c = __rdtscp(&AUX);
		unsigned long long int diff = end_c - start_c;
		printf("array[%d] = %lld\n", (i * 65) % 256, diff);
		if (diff < time) {
			time = diff;
			index = (i*65) % 256;
		}
	}

	printf("Pos=%d, time=%lld\n", index, time);
}

int main()
{
	practic2();
	return 0;
}
