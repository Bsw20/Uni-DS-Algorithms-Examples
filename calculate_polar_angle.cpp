//
//  main.cpp
//  AlgHomeWork3
//
//  Created by Ярослав Карпунькин on 07.10.2020.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;



class Point {
private:
    int x;
    int y;
public:
    Point(){}
    int X() { return x;}
    int Y() { return y;}
    
    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }
    
    void Print() {
        cout << this->X() << " " << this->Y() << "\n";
    }
    
    
};

//Главная центральная точка(нижняя левая)
static Point minPointEver = Point(0,0);

class MyStack {
private:
    //Максимально возможное количество элементов 10^3
    static const int maxSize = 1000;
    vector<Point> stack;
    int currentSize = 0;
public:
    MyStack(int size) {
        if(size > maxSize || size < 3) {
            throw "Size must be lower than max size(1000)";
        }
    }
    
    void Push(Point point) {
        currentSize++;
        if(currentSize > maxSize) {
            throw "Size must be lower than max size(1000)";
        }
        stack.push_back(point);
    }
    
    Point Pop() {
        currentSize--;
        if (currentSize < 0) {
            throw "Size must be more than 0";
        }
        Point el = stack[currentSize];
        stack.pop_back();
        return el;
    }
    
    Point Top() {
        if (currentSize < 1) {
            throw "Size must be more than 0 to take top element";
        }
        return stack[currentSize-1];
    }
    
    Point NextToTop() {
        if (currentSize < 2) {
            throw "Size must be more than 1 to take top element";
        }
        return stack[currentSize-2];
    }
    
    int Size() {
        return stack.size();
    }
    
    bool IsEmpty() {
        return stack.empty();
    }
    
    void Print() {
        cout << "---------------------------" << "\n";
        for (int i = 0; i < stack.size(); i++) {
            Point el = stack[i];
            el.Print();
        }
        cout << "---------------------------" << "\n";
        
    }
    
    void SetSide(string str) {
        if (str == "cw") {
            reverse(stack.begin() + 1, stack.end());
        }
    }
    
    string SetFormat(string format, string multipoint) {
        if (format == "plain") {
            string finalString = to_string(Size()) + "\n";
            for (int i = 0; i < Size(); i++) {
                finalString += to_string(stack[i].X()) + " " + to_string(stack[i].Y()) + "\n";
            }
            return finalString;
        }
        else {
            string finalString = multipoint;
            finalString += "POLYGON ((";
            for (int i = 0; i < Size(); i++) {
                finalString += to_string(stack[i].X()) + " " + to_string(stack[i].Y()) + ", ";
            }
            finalString +=  to_string(stack[0].X()) + " " + to_string(stack[0].Y()) + "))";
            return finalString;
        };
    }
};

int FindMinPoint(Point points[], int n) {
    int ymin = points[0].Y();
    int minElIndex = 0;
    for (int i = 1; i < n; i++)
    {
        int x = points[i].X();
      int y = points[i].Y();

      if ((y < ymin) || (ymin == y &&
                         points[i].X() < points[minElIndex].X())) {
          ymin = y;
          minElIndex = i;
      }
    }
    
    return minElIndex;
}

//Нужно закинуть в класс, но не разобрался, метод нельзя имплементировать без точки, а точку нельзя инициализировать без класса
double ToCompare(Point point) {
    double angle = atan2(point.Y() - minPointEver.Y() , point.X() - minPointEver.X() );
    return angle >= 0 ? angle : angle + 2 * M_PI;
}



//расстояние от центральной точки(далее главная точка) до данной точки
int Distance(Point p1)
{
    return (p1.X() - minPointEver.X())*(p1.X() - minPointEver.X()) +
          (p1.Y() - minPointEver.Y())*(p1.Y() - minPointEver.Y());
}

//сортировка по полярному углу и по расстоянию от главной точки(если векторы коллинеарны)
int Comparator(Point point1, Point point2) {
    if (ToCompare(point1) == ToCompare(point2)) {
        return Distance(point1) > Distance(point2) ? 0 : 2;
    }
    return ToCompare(point1) >= ToCompare(point2) ? 0 : 2; //TODO: check
}

//проверка, создают ли 3 точки "выпуклую линию"
bool CheckPosition(Point point1, Point point2, Point point3) {
    return (point2.X() - point1.X()) * (point3.Y() - point2.Y()) - (point2.Y() - point1.Y()) * (point3.X() - point2.X()) > 0;
}


MyStack FindShell(Point points[], int n) {
    //1 шаг - найти самую нижнюю левую точку и поставить в начало массива
    int minElIndex = FindMinPoint(points, n);
    Point help = points[0];
    points[0] = points[minElIndex];
    points[minElIndex] = help;
    
    //задаем минимальную точку(условный центр полярной ск)
    minPointEver = points[0];
    
    for (int i = 0; i < n; i++) {
        points[i].Print();
    }
    
    minPointEver.Print();
    //----------------------------
    
    //2 шаг - сортировка по полярному углу и по расстоянию от главной точки(если векторы коллинеарны)
    vector<Point> sort_Point_vector (points, points+n);
    
    sort (sort_Point_vector.begin() + 1, sort_Point_vector.end(), Comparator);
    
    //2 шаг
    cout << "2 шаг ---------------------------" << "\n";
    for (int i = 0; i < n; i ++) {
        sort_Point_vector[i].Print();
        cout << ToCompare(sort_Point_vector[i]) << "\n";
    }
    
    //3 шаг - удаляем точки на одной прямой(коллинеарные) и оставляем с наибольшей длиной вектора (не до конца понял)
    cout << "3 шаг ---------------------------" << "\n";
    vector<Point> vector_Without_Duplicates;
    vector_Without_Duplicates.push_back(sort_Point_vector[0]);
    vector_Without_Duplicates.push_back(sort_Point_vector[1]);
    double previousAngle = ToCompare(sort_Point_vector[1]);
    for (int i = 2; i < n; i++) {  //по условию как минимум три неколлинеарные точки

        double currentAngle = ToCompare(sort_Point_vector[i]);
        if(currentAngle == previousAngle) {
            vector_Without_Duplicates.pop_back();
        }
        vector_Without_Duplicates.push_back(sort_Point_vector[i]);
        previousAngle = currentAngle;
    }
    
    for (int i = 0; i < vector_Without_Duplicates.size(); i++) {
        vector_Without_Duplicates[i].Print();
    }
    
    //4 шаг - создаем оболочку чз стек, удаляя лишние точки
    cout << "4 шаг ---------------------------" << "\n";
    MyStack stack = MyStack(vector_Without_Duplicates.size());
    stack.Push(vector_Without_Duplicates[0]);
    stack.Push(vector_Without_Duplicates[1]);
    stack.Push(vector_Without_Duplicates[2]);
    
    stack.Print();
    
    for (int i = 3; i < vector_Without_Duplicates.size(); i++) {
        while (!CheckPosition(stack.NextToTop(), stack.Top(), vector_Without_Duplicates[i])) {
            stack.Pop();
        }
        stack.Push(vector_Without_Duplicates[i]);
    }
    
    cout << "---------------------------" << "\n";
    
    stack.Print();
    
    return stack;

    
}


int main(int argc, char** argv) {
    

    
//    Point points[] = {Point(0, 3), Point(1, 1), Point(2, 2), Point(4, 4),
//        Point(0, 0), Point(1, 2), Point(3, 1)};
//    Point points[] = {Point(0, 0), Point(3, 4), Point(3, 1), Point(6, 0)};
//    Point points[] = {Point(0, 0), Point(0, 1), Point(0, 2), Point(1, 0),
//        Point(2, 0)};
    
    if (argc < 5) {
        cerr << "Необходимо 5 аргументов";
        return 1;
    }
    ifstream in{ argv[3] };
    int n = 0;
    in >> n;
    string side = argv[1], format = argv[2];
//    string side = "cw";
//    string format = "wkt";

    Point points[n];
    int str[5];;
    for (int i = 0; i < n; ++i) {
        int x, y;
        in >> x >> y;
        points[i] = Point(x, y);
    }
//        Point points[] = {Point(0, 0), Point(0 , 10), Point(10, 10), Point(1, 9),
//            Point(1, 1), Point(2, 9), Point(10, 9), Point(10, 0)};
//    int n = sizeof(points)/sizeof(points[0]);
    
    string multipoints = "MULTIPOINT (";
    for (int i = 0; i < n-1; i ++) {
        multipoints += "(" + to_string(points[i].X()) + " " + to_string(points[i].Y()) + "), ";
    }
    multipoints +=  "(" + to_string(points[n-1].X()) + " " + to_string(points[n-1].Y()) + "))\n";
    
    cout << sizeof(points)/sizeof(points[0]) << "\n";
    MyStack stack = FindShell(points, n);
    stack.SetSide(side);
    cout << "STRINGS---------------------------" << "\n";
    stack.Print();
    cout << "STRINGS---------------------------" << "\n";
    
    string resultString = stack.SetFormat(format, multipoints);
    cout << resultString<< "\n";
    
    if(argc > 4) ofstream{argv[4]} << resultString;
    //Не понял, как на macos запускать файлы с аргументами
    //поэтому получилось просто консольное приложение
    return 0;
}

