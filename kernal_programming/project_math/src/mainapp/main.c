#include<stdio.h>
#include"libmath.h"
void main()
{
	double x = 40;
	double y = 20;
  printf("addition: %f\n",add(x, y));
  printf("subtraction: %f\n",subtract(x, y));
  printf("multiply: %f\n",multiply(x, y));
  printf("Division: %f\n",divide(x, y));
  printf("mod :%d\n",mod(x, y));
  printf("square: %f\n",square(x));
}
