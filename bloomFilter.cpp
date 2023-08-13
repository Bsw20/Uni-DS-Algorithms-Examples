//
//  main.cpp
//  BloomFilter
//
//  Created by Ярослав Карпунькин on 11.11.2020.
//

#include <iostream>
#include <fstream>
#include <unordered_map>

using namespace std;

class Hashes {
public:
//    Первый хэш, закидываем его в отрезок остатком от деления на m
    static int hash1(string videoId, int m) {

        return (hash<string>{}(videoId)) % m;
    }
    //Второй хэш, закидываем его в отрезок остатком от деления на m
    static int hash2(string videoId, int m) {

        return (hash<string>{}(videoId) + hash<string>{}(videoId)) % m;
    }

    //Третий хэш, закидываем его в отрезок остатком от деления на m
    static int hash3(string videoId, int m) {

        return ((hash<string>{}(videoId) << 3) ^ hash<string>{}(videoId)) % m;
    }
};

//Класс контейнер для фильтра(ака битмап)
class BM {
private:
    //Массив, который является
    unsigned long* bitmap = new unsigned long[0];

public:
    BM(int bitsCount) {
        int chanksCount = bitsCount / 32;
        if(bitsCount % 32 != 0) {
            chanksCount++;
        }
        bitmap = new unsigned long[chanksCount];
        for (int i = 0; i < chanksCount; i++) {
            bitmap[i] = 0;
        }
    }
public:
    //метод сет
    void set(int i)
    {

        bitmap[i / 32] = (unsigned long)pow(2, (i % 32)) | bitmap[i / 32];
    }
    //метод гет
    bool get(int i) {

        return (bitmap[i / 32] & ((int)pow(2, (i % 32)))) == ((int)pow(2, (i % 32)));
    }

};

//Класс фильтра
class Filter {
private:
    //контейнер
    BM* bm;
    //его размер
    int bmSize;

public:
    //конструктов
    Filter(int bmSize) {
        bm = new BM(bmSize);
        this->bmSize = bmSize;
    }
    //функция check из условия (идет проверка на 1 для трех хэшей в bm)
    string check(string videoId) {
        if(bm->get(Hashes::hash1(videoId, bmSize)) &&
           bm->get(Hashes::hash2(videoId, bmSize)) && bm->get(Hashes::hash3(videoId, bmSize))) {
            return "Probably";
        }
        return "No";
    }
    //функция watch из условия (вставляем 1 на нужные индексы)
    void watch(string videoId) {
        bm->set(Hashes::hash1(videoId, bmSize));
        bm->set(Hashes::hash2(videoId, bmSize));
        bm->set(Hashes::hash3(videoId, bmSize));
    }
    //Удаление массива
    void deleteBm() {
        delete[] bm;
    }
};

class Helpers {
public:
    //Метод для нахождения размера контейнера(количества ячеек)
    static int filterSize(int n) {
        return round(n * 9.585);
    }

};

int main(int argc, const char * argv[]) {
    int n;
    string cmd;
    //input stream
    ifstream in;
    //output stream
    ofstream out;

    in.open(argv[1]);
    out.open(argv[2]);
//    in.open("test5.txt");
//    out.open("answer5.txt");
//    in.open("Myinput/test4.txt");
//    out.open("Myoutput/answer4.txt");

//    in.open("test1.txt");
//    out.open("input/MyAnswer.txt");

    in >> cmd >> n;
    out << "Ok\n";

    int okN = 0;
    int probN = 0;
    int noN = 0;
    //map для хранения id user и соотв им фильтров
    unordered_map<string, Filter*> users;
    string userId, videoId;
    //считываем три слова из строки с соотв переменные
    while (in >> cmd >> userId >> videoId) {
        //если данного юзера нет в мэпе, добавляем его
        if(users.find(userId) == users.end()) {
            users[userId] = new Filter(Helpers::filterSize(n));
        }
        //если check
        if(cmd == "check") {
            string resp = users[userId]->check(videoId);
            if(resp == "No") {
                noN++;
            } else {
                probN++;
            }
            out << resp + "\n";
        } else { //watch или объявление n
            okN++;
            users[userId]->watch(videoId);
            out << "Ok\n";
        }
    }

    in.close();
    out.close();
    okN++;
    std::cout<< okN << endl;
    std::cout<< probN << endl;
    std::cout<< noN << endl;
    //чистим память
    for (auto i = users.begin(); i != users.end(); ++i)
        (i->second)->deleteBm();


    return 0;
}

