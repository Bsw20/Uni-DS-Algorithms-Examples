//
//  gis.cpp
//
//  Created by Ярослав Карпунькин on 20.12.2020.
//

#include <fstream>
#include <iostream>
#include "gdal.h"
#include "ogrsf_frmts.h"
#include "boost/geometry/index/rtree.hpp"

using namespace std;
//Значение из задания
typedef boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian> point;
typedef boost::geometry::model::box<point> box;
//Значение, которое вставляется в дерево
typedef pair<box, int> value;

bool sortById(pair<box, int> first, pair<box, int> second) {
    return first.second < second.second;
}

int main(int argc, char* argv[])
{
//    string dataPath = argv[1];
//    dataPath += "/building-polygon.shp";
//    string testPath = argv[2];
//    string answerPath = argv[3];

    //Я так до конца и не понял, через что будет запускаться код, поэтому оставлю ide + закоменченную консоль
    string dataPath = "/Users/yaroslavhome/Desktop/Алгосы/FINALHWGIS/HW/data/building-polygon.shp";
    string testPath = "/Users/yaroslavhome/Desktop/Алгосы/FINALHWGIS/HW/input/test5.txt";
    string answerPath = "/Users/yaroslavhome/Desktop/Алгосы/FINALHWGIS/HW/output/answer5.txt";

    GDALAllRegister();
    //Считываем датасет
    GDALDataset* dataset = static_cast<GDALDataset*>(GDALOpenEx(
            dataPath.c_str(),
            GDAL_OF_VECTOR,
            nullptr, nullptr, nullptr));

    //Проверка, что датасет существует
    if (dataset == nullptr) {
        cout << "Cannot open file!" << endl;
        return -1;
    }

    //Открываем потоки ввода и вывода
    ifstream in;
    in.open(testPath);
    ofstream out;
    out.open(answerPath);

    //Проверка, что с файлами все ок
    if (!(in.is_open() && out.is_open())) {
        cout << "Problem with files!" << endl;
        return 1;
    }
    //Инициализация r-tree(условие)
    boost::geometry::index::rtree< value, boost::geometry::index::quadratic<8, 4> > rtree;
    //Проходимся по всем слоям
    for (auto&& layer : dataset->GetLayers()) {
        for (auto&& feature : layer) {         //Проходимся по всем объектам

            auto* geometry = feature->GetGeometryRef(); //Получаем ссылку на геометрию
            auto* rectangle = new OGREnvelope(); //оболочка

            geometry->getEnvelope(rectangle);// mbr(минимальный ограничивающий прямоугольник)

            int osm_Id = feature->GetFieldAsInteger(0); //Id стоит на 0 иесте
            //Заполняем r-tree объектами
            box obj = box(point(rectangle->MinX, rectangle->MinY), point(rectangle->MaxX, rectangle->MaxY));
            rtree.insert(make_pair(obj, osm_Id));
        }
    }
    //Входные данные, инициализация нашего box
    double minX, minY, maxX, maxY;
    in >> minX >> minY >> maxX >> maxY;
    box inputRectangle = box(point(minX, minY), point(maxX, maxY));

    //Делаем запрос для поиска пересечений
    vector<pair<box, int>> res;
    rtree.query(boost::geometry::index::intersects(inputRectangle), back_inserter(res));

    //Сортируем по id
    sort(res.begin(), res.end(), sortById);

    for (auto&& el : res)
    {
        out << el.second << endl;
    }

    in.close();
    out.close();
    return 0;
}
