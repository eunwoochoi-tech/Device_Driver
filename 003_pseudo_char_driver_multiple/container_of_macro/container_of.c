#include <stdio.h>

// 1. offset을 구한다

#define container_of(ptr, type, member) (type*)((int)(ptr) - (int)(&(((type*)0)->member)))

struct some_data
{
	int a;
	char b;
	int c;
	char d;
};

struct some_data data;

int init()
{
	data.a = 10;
	data.b = 5;
	data.c = 'a';
	data.d = 100;

	printf("data address : %X \n", &data);
	printf("get address : %X \n", container_of(&data.c, struct some_data, c));


	return 0;
}

int main(void)
{
	init();

	return 0;
}
