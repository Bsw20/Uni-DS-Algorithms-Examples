#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;
class Edge
{
public:
    int A;
    int B;
    int W;
    int number;
    std::string print()
    {
        return std::to_string(A) + " " + std::to_string(B) + " " + std::to_string(W);
    }
};

class ReadWriter
{
private:

    std::fstream fin;
    std::fstream fout;

    //sort by Weight increasing
    static int comp(Edge& left, Edge& right)
    {
        return left.W < right.W;
    }

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

    // read 2 int value and empty line
    void read2Ints(int& N, int& M)
    {
        if (!fin.is_open())
            throw std::ios_base::failure("file not open");

        fin >> N >> M;
        //empty line read
        std::string s;
        std::getline(fin, s);
    }

    // read M edges, and fill vector
    void readEgdes(int M, std::vector<Edge>& edges)
    {
        if (!fin.is_open())
            throw std::ios_base::failure("file not open");

        std::string s;
        for (int i = 0; i < M; i++)
        {
            Edge e;
            fin >> e.A >> e.B >> e.W;
            e.number = i;
            edges.push_back(e);
            //empty line read
            std::getline(fin, s);
        }
    }

    // write int value and empty line
    void writeInt(int K)
    {
        if (!fout.is_open())
            throw std::ios_base::failure("file not open");

        fout << K;
        fout << "\n";         //empty line write
    }

    // write all edges from Vector
    void writeEdges(std::vector<Edge> edges)
    {
        if (!fout.is_open())
            throw std::ios_base::failure("file not open");

        std::sort(edges.begin(), edges.end(), comp);

        for (int i = 0; i < edges.size(); i++)
            fout << edges[i].print() << "\n";
    }

};



struct comparator{
    bool operator()(pair<int, int> const& l, pair<int, int> const& r){
        return l.first > r.first;
    }
};

//Основной метод решения задачи, параметры:
//N - количество вершин, M - количество ребер в графе
//edges - вектор ребер, каждое ребро представлено 3-мя числами (А,В,W), где A и B - номера вершин, которые оно соединяет, и W - вес ребра,
//передается по ссылке (&), чтобы не копировать, изменять вектор и его значения можно.
//Результат также в виде вектора ребер, передается по ссылке (&), чтобы не копировать его.
void solve(int N, int M, vector<Edge>& edges, vector<Edge>& result)
{
    vector<bool> inMST(N);
    vector<int> V(N, 15000);
    vector<int> p(N, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, comparator> Q;
    int r = 0;
    V[r] = 0;
    Q.push(make_pair(0, r));
    while (!Q.empty()) {
        // Извлекаем минимальный.
        int u = Q.top().second;
        Q.pop();
        inMST[u] = true;
        for (int i = 0; i < M; ++i) {
            if (edges[i].A == u || edges[i].B == u) {
                int weight = edges[i].W;
                // Инциндентная вершина.
                int v;
                if (edges[i].B == u)
                    v = edges[i].A;
                else
                    v = edges[i].B;
                if (!inMST[v] && V[v] > weight) {
                    V[v] = weight;
                    p[v] = u;
                    Q.push(make_pair(V[v], v));
                }
            }
        }
    }
    for (int i = 1; i < N; i++) {
        if (p[i] < i){
            Edge edge;
            edge.A = p[i];
            edge.B = i;
            edge.W = V[i];
            result.push_back(edge);
        } else {
            Edge edge;
            edge.A = i;
            edge.B = p[i];
            edge.W = V[i];
            result.push_back(edge);
        }
    }
}

int main()
{
    ReadWriter rw;
    //Входные параметры
    //N - количество вершин, M - количество ребер в графе
    int N, M;
    rw.read2Ints(N, M);

    //Вектор ребер, каждое ребро представлено 3-мя числами (А, В, W), где A и B - номера вершин, которые оно соединяет, и W - вес ребра
    //Основной структурой выбран вектор, так как из него проще добавлять и удалять элементы (а такие операции могут понадобиться).
    vector<Edge> edges;
    rw.readEgdes(M, edges);

    //Основной структурой для ответа выбран вектор, так как в него проще добавлять новые элементы.
    //(а предложенные алгоритмы работают итеративно, увеличивая количество ребер входящих в решение на каждом шаге)
    vector<Edge> result;

    //Алгоритм решения задачи
    //В решение должны входить ребра из первоначального набора!
    solve(N, M, edges, result);

    //Выводим результаты
    rw.writeInt(result.size());
    rw.writeEdges(result);

    return 0;
}
