#include <iostream>
#include <math.h>

extern "C" void bubblesort(int* arr, int n);

int main(void) {
	srand(time(0));

	int* v = new int[20];

	std::cout << "Unsorted array: " << std::endl;
	for (size_t i = 0; i < 20; i++) {
		v[i] = rand();
	}
	
	for (size_t i = 0; i < 20; i++) {
		std::cout << v[i] << " ";
	}

	std::cout << std::endl;

	bubblesort(v, 20);

	std::cout << "Sorted array: " << std::endl;
	for (int i = 0; i < 20; i++) {
		std::cout << v[i] << " ";
	}
	return 0;
}