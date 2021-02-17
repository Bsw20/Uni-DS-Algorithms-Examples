//
//  main.cpp
//  Contest4XCODE
//
//  Created by Ярослав Карпунькин on 17.02.2021.
//

#include <iostream>
#include <fstream>
using namespace std;

class ReadWriter
{
private:

    std::fstream fin;
    std::fstream fout;

public:

    ~ReadWriter()
    {
        fin.close();
        fout.close();
    }

    ReadWriter()
    {
        fin.open("input.txt", std::ios::in);
        fout.open("output.txt", std::ios::out);
    }

    int readInt()
    {
        if (!fin.is_open())
            throw std::ios_base::failure("file not open");

        int n = 0;
        fin >> n;
        return n;
    }

    void readArray(int* arr, int length)
    {
        if (!fin.is_open())
            throw std::ios_base::failure("file not open");

        for (int i = 0; i < length; i++)
            fin >> arr[i];
    }

    void writeInt(int n)
    {
        if (!fout.is_open())
            throw std::ios_base::failure("file not open");
        fout << n << "\n";
    }
    void writeArray(int* arr, int n)
    {
        if (!fout.is_open())
            throw std::ios_base::failure("file not open");
        for (int i = 0; i < n; i++)
            fout << arr[i] << " ";
    }
};


int rec_count = 0;
void partition(int *numbers, int left_index, int right_index, int pivot) {
    while(left_index <= right_index) {
        while (numbers[left_index] < pivot) {
            left_index++;
        }
        while (numbers[right_index] > pivot) {
            right_index--;
        }
        if (left_index <= right_index) {
            swap(numbers[left_index], numbers[right_index]);
            left_index++;
            right_index--;

            
        }
    }
}

void quickSort(int *numbers, int l, int r) {
    
    int left_index = l, right_index = r;
    int pivot = numbers[(l + r) / 2];
    while(left_index <= right_index) {
        while (numbers[left_index] < pivot) {
            left_index++;
        }
        while (numbers[right_index] > pivot) {
            right_index--;
        }
        if (left_index <= right_index) {
            swap(numbers[left_index], numbers[right_index]);
            left_index++;
            right_index--;

        }
    }
    
    if(r > left_index) {
        rec_count++;
        quickSort(numbers, left_index, r);
    }
    
    if (l < right_index) {
        rec_count++;
        quickSort(numbers, l, right_index);
    }
}

int main(int argc, const char * argv[]) {
    ReadWriter rw;

    int* brr = nullptr;
    int n;

    //Ввод из файла
    n = rw.readInt();

    brr = new int[n];
    rw.readArray(brr, n);

    //Запуск сортировки, ответ в том же массиве (brr)
    quickSort(brr, 0, n - 1);

    //Запись в файл
    rw.writeInt(rec_count);
    rw.writeArray(brr, n);
    //освобождаем память
    delete[] brr;

    return 0;

}
