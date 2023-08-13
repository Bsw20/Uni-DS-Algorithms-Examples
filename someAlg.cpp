using System;
using System.Collections.Generic;
using System.Linq;

namespace Alg_HW2
{
    //Для решения данной задачи должно быть минимум 3  точки(иначе задача превращается в поток обрабатываемых эксепшенов)
    class Point: IComparable<Point>
    {
        private int x;
        private int y;

        public int X
        {
            get
            {
                return x;
            }
        }
        
        public int Y
        {
            get
            {
                return y;
            }
        }

        public Point(int x, int y)
        {
            this.x = x;
            this.y = y;
        }
        public override string ToString()
        {
            return $"({this.X}, {this.Y})";
        }
        
        public double Compare()
        {
            double angle = Math.Atan2((Program.minPoint).Y - Y, (Program.minPoint).X - X);
            return angle >= 0 ? angle : angle + 2 * Math.PI;
        }

        public int CompareTo(Point other)
        {
            if (ReferenceEquals(this, other)) return 0;
            var thisAngle = Compare();
            var otherAngle = other.Compare();
            if (thisAngle > otherAngle)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
    }

    class MyStack
    {
        private List<Point> stack = new List<Point>();
        public const int maxSize = 1000;
        private int size = 0;

        public MyStack(int size)
        {
            if(size > maxSize)
            {
                throw new ArgumentException("Слишком большой размер стэка");
            }
            
        }

        public void Push(Point point)
        {
            if ((size + 1) > maxSize)
            {
                throw new StackOverflowException("Переполнение");
            }
            stack.Add(point);
            size++;

        }

        public Point Pop() 
        {
            if (size == 0)
            {
                throw new NullReferenceException("Попытка удалить элемент из пустого стека"); 
            }

            var element = stack[size - 1];
            stack.RemoveAt(size - 1);
            size--;
            return element;
        }

        public Point Top()
        {
            if (size == 0)
            {
                throw new NullReferenceException("Стэк пуст, нельзя получить максимальный");
            }

            return stack[size - 1];
        }
         //По идее должно быть три неколлинеарные точки и эксепшены в Top и NextToTop не будут вылетать
        public Point NextToTop()
        {
            if (size < 2)
            {
                throw new NullReferenceException("Стэк почти пуст, нельзя получить пред максимальный");
            }

            return stack[size - 2];
        }

        public int Size()
        {
            return size;
        }

        public bool isEmpty()
        {
            return stack.Count == 0;
        }
        //TODO: DELETE
        public Point getElement(int i)
        {
            return stack[i];
        }

        public List<Point> ToList()
        {
            return stack;
        }

 
    }
    class Program
    {
        public static Point minPoint { get; set; }
        //Сортировка для поиска нижней левой точки(стартовой)
        static List<Point> FirstStepSort(List<Point> points)
        {
            var sortedByY = points.OrderBy(p => p.Y).ToList();
            return findMinX(sortedByY);
        }
        
        //Находит элемент с min x и min y (list[0]);
        static List<Point> findMinX(List<Point> sortedByY)
        {
            for (int i = 0; i < sortedByY.Count; i++)
            {
                if (sortedByY[i].Y > sortedByY[0].Y)
                {
                    break;
                }

                if (sortedByY[i].X < sortedByY[0].X)
                {
                    var help = sortedByY[0];
                    sortedByY[0] = sortedByY[i];
                    sortedByY[i] = help;

                }
            }

            return sortedByY;
        }

        static List<Point> mainSort(List<Point> points)
        {
            var newPoints = FirstStepSort(points);
            for (int i = 0; i < newPoints.Count; i++)
            {
                Console.WriteLine(newPoints[i]);
            }

            minPoint = newPoints[0];
            
            newPoints.Sort();
            Console.WriteLine("-------------------------");
            for (int i = 0; i < newPoints.Count; i++)
            {
                Console.WriteLine(newPoints[i]);
            }

            return newPoints;

        }

        static double properPosition(Point A, Point B, Point C)
        {
            return (B.X - A.X) * (C.Y - B.Y) - (B.Y - A.Y) * (C.X - B.X);
        }

        static List<Point>  thirdStep(List<Point> newPoints)
        {
            int n = newPoints.Count;
            MyStack myStack = new MyStack(n);
            myStack.Push(newPoints[0]);
            myStack.Push(newPoints[1]);
            myStack.Push(newPoints[2]);
            for (int i = 3; i < n; i++)
            {
                while (properPosition(myStack.NextToTop(), myStack.Top(), newPoints[i]) < 0)
                {
                    myStack.Pop();
                    Console.WriteLine("pop");
                }
                
                myStack.Push(newPoints[i]);
                Console.WriteLine("Push");
            }

            return myStack.ToList();
        }
        
            
        static void Main(string[] args)
        {
            Console.WriteLine("Количество точек");
            var n = 5;
            // MyStack stack = new MyStack(n);
            // stack.Push(new Point(0, 0));
            // stack.Push(new Point(0, 1));
            // stack.Push(new Point(0, 2));
            // stack.Push(new Point(1, 0));
            // stack.Push(new Point(2, 0));

            List<Point> points = new List<Point>()
            {

                new Point(0, 0),
                new Point(0, 1),
                
                new Point(0, 2),
                new Point(1, 0),
                new Point(2, 0)
                
                // new Point(0, 0),
                // new Point(3, 4),
                //
                // new Point(3, 1),
                // new Point(6, 0)


            };
            var newPoints = mainSort(points);
            var thirdStepList = thirdStep(newPoints);
            
            Console.WriteLine("-------------------------");
            for (int i = 0; i < thirdStepList.Count; i++)
            {
                Console.WriteLine(thirdStepList[i]);
            }




            // for (int i = 0; i < n; i++)
            // {
            //     string[] coord = Console.ReadLine().Split();
            //     stack.Push(new Point(int.Parse(coord[0]), int.Parse(coord[1])));
            //     Console.WriteLine(stack.Size());
            // }
        }
    }
}






//
//  main.cpp
//  AlgHomeWork3
//
//  Created by Ярослав Карпунькин on 07.10.2020.
//

#include <iostream>
#include <vector>
using namespace std;

class Point {
private:
    int x;
    int y;
public:
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




class MyStack {
private:
    //Максимально возможное количество элементов 10^3
    static const int maxSize = 1000;
    vector<Point> stack;
    int currentSize = 0;
public:
    MyStack(int size) {
        if(size > maxSize) {
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
};



int main(int argc, const char * argv[]) {
    Point a = Point(20, 30);
    cout << a.X() << "\n";
    cout << a.Y() << "\n";
    a.Print();
    MyStack stack = MyStack(5);
    stack.Push(a);
    stack.Print();
    return 0;
}

