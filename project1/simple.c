//Author: George Martin
//Version: 01-22-2022
//Class: COMP 3500, Dr Qin, Spring 2022
//Note: When compliling, use -lm for sqrt math.h.
//Program: Calculates the average square root of the elements
//	   in an array of size 10.

#include <stdio.h>
#include <math.h>

//Calculates the average of the sqrt of each number in the array.
void calculate() {
	double sqrSum;
	double result;

	printf("The average square root of the array is: ");
	double arr[10] = {1, 4, 9, 16, 25, 36, 49, 64, 81, 100};

		int i;
		for (i = 0; i < 10; i++) {
			double x = arr[i];
			double y = sqrt(x);
			sqrSum = sqrSum + y;
			}
		result = sqrSum / 10;
		printf("%.2f", result);
		printf("\n");
}

//Prints the result of the average sqrt of the array.
int main() {
	calculate();
}

