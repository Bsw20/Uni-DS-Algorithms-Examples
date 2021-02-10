#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
using namespace std;

void heapify(int *numbers, int i, int heap_size) {
    int left_index = 2 * i + 1;
    int right_index = 2 * i + 2;
    
    int largest_index = i;
    if (left_index < heap_size && numbers[left_index] > numbers[largest_index]) {
        largest_index = left_index;
    }
    
    if (right_index < heap_size && numbers[right_index] > numbers[largest_index]) {
        largest_index = right_index;
    }
    if (largest_index != i) {
        swap(numbers[i], numbers[largest_index]);
        heapify(numbers, largest_index, heap_size);
    }
    
}

void heapSort(int *numbers, int array_size)
{
    //Перестройка массива и создание кучи(не проходим по листьям)
    for (int i = array_size / 2 - 1; i >= 0; i--)
        heapify(numbers, i, array_size);
    
    for (int i= array_size - 1; i>0; i--)
    {
        cout << to_string(i) + "\n";
        swap(numbers[0], numbers[i]);
        heapify(numbers, 0, i);
    }
}

int main() {
    int *brr;
    int n;

    fstream fin;
    fin.open("input.txt",ios::in);
    if(fin.is_open()) {
        fin >> n;
        brr = new int[n];
        for (int i = 0; i < n; i++) {
            fin >> brr[i];
        }

        fin.close();
    }
    heapSort(brr, n);
    fstream fout;
    fout.open("output.txt",ios::out);
    for (int i = 0; i < n; i++)
        fout << brr[i] << " ";

    fout.close();
    delete[] brr;
    return 0;

}

