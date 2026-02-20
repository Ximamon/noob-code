#include <iostream>

extern "C" void bubblesort(int* arr, int n);

int main() {
	int arr[] = { 64, 34, 25, 12, 22, 11, 90 };
	int n = sizeof(arr) / sizeof(arr[0]);

	bubblesort(arr, n);

	for (int i = 0; i < n; i++) {
		std::cout << arr[i] << " ";
	}
	return 0;
}