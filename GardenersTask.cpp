//
//  main.cpp
//
//  Created by Ярослав Карпунькин on 13.12.2020.
//  группа 191
//  Вариант 17

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

using namespace std;
//Мьютекс для синхронизации
mutex mtx;

//Генерирую матрицу, где каждая клетка - false
vector<vector<bool>> getEmptyGarden(const int &rows, const int &col) {
    auto garden = vector<vector<bool>>();
    for(int i = 0; i < rows; i++) {
        auto row = vector<bool>();
        for(int j = 0; j < col; j++) {
            row.push_back(false);
        }
        garden.push_back(row);
    }

    return garden;
}

//Генерирую план сада, где каждая клетка требует или не требует работы(рандом)
vector<vector<bool>> getGardenPlan(const int &rows, const int &col) {
    auto plan = vector<vector<bool>>();
    cout << "Plan:" << endl;
    for(int i = 0; i < rows; i++) {
        auto row = vector<bool>();
        for(int j = 0; j < col; j++) {
            auto value = rand() % 2;
            switch (value) {
                case 0:
                    cout << "Coordinates: row - " + to_string(i) + ", column - " + to_string(j) + ". This cell doesn't need a worker" << endl;
                    row.push_back(false);
                    break;
                case 1:
                    cout << "Coordinates: row - " + to_string(i) + ", column - " + to_string(j) + ". This cell should be handled" << endl;
                    row.push_back(true);
                    break;
                default:
                    break;
            }
        }
        plan.push_back(row);
    }
    return plan;
}

//Принт, если клетка пустая
void emptyCellPrint(string name, int curRow, int curCol) {
    cout << name + " are on cell with coordinates: column - " + to_string(curCol) + ", row - " + to_string(curRow)
    + " There's nothing to do here."<< endl;
}

//Принт, если эта клетка уже обработана
void alreadyDoneCell(string name, int curRow, int curCol)  {
    cout << name + " are on cell with coordinates: column - " + to_string(curCol) + ", row - " + to_string(curRow)
            + " This cell have already handled."<< endl;
}

//Принт, если клетка требует работы
void cellWithWork(string name, int curRow, int curCol)  {
    cout << name + " are on cell with coordinates: column - " + to_string(curCol) + ", row - " + to_string(curRow)
            + " I'm going to handle it. Done." << endl;
}

//Метод, который отвечает за работу садовода
void doWork(bool isFirstTypeGardener, //Можно конечно сделать через энамы
            string name,
            int rows,
            int col,
            vector<vector<bool>> &garden,
            vector<vector<bool>> plan) {
    //Если садовод первого типа(движение слева направо)
    if(isFirstTypeGardener) {
        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < col; j++) {
                mtx.lock();
                if(!plan[i][j]) {
                    emptyCellPrint(name, i, j);
                } else if (garden[i][j]) {
                    alreadyDoneCell(name, i, j);
                } else {
                    garden[i][j] = true;
                    cellWithWork(name, i, j);
                }
                mtx.unlock();
                //Для теста, чтобы разные потоки могли обращаться к ресурсу
                this_thread::sleep_for(chrono::milliseconds(15));
            }
        }
    } else {     //Если садовод второго типа(движение снизу вверх)
        for(int i = col - 1; i >= 0; i--) {
            for(int j = rows - 1; j >= 0; j--) {
                mtx.lock();
                if(!plan[j][i]) {
                    emptyCellPrint(name, j, i);
                } else if (garden[j][i]) {
                    alreadyDoneCell(name, j, i);
                } else {
                    garden[j][i] = true;
                    cellWithWork(name, j, i);
                }
                mtx.unlock();
                this_thread::sleep_for(chrono::milliseconds(15));
            }
        }

    }
}

int main() {
    int rows, col;
    //считываю количество строк и колонок поля
    cout << "Enter the number of rows in range [1, 100] "<< endl;
    cin >> rows;
    if (rows < 1 || rows > 100) {
        cout << "Incorrect count of rows" << endl;
        return 1;
    }

    cout << "Enter the number of columns in range [1, 100]"<< endl;
    cin >> col;
    if (col < 1 || col > 100) {
        cout << "Incorrect count of columns"<< endl;
        return 1;
    }

    auto garden = getEmptyGarden(rows, col);
    auto plan = getGardenPlan(rows, col);

    thread gardener1(doWork, true, "Bob", rows, col, ref(garden), plan);
    thread gardener2(doWork, false, "Mark", rows, col, ref(garden), plan);

    gardener1.join();
    gardener2.join();

    return 0;
}
