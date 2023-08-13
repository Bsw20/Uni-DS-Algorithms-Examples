//
//  main.cpp
//  b-tree
//
//  Created by Ярослав Карпунькин on 26.11.2020.
//

#include <iostream>
#include <array>
#include <cassert>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

template<size_t M = 100>
class Btree{
private:
    static const int max_key = 2*M-1;
    static const int max_children = 2 * M;



    struct node{
        size_t cntKeys = 0;
        std::array<node*, max_children> nodes;
        std::array<std::pair<int, int>, max_key> pairs;
        bool isLeaf;

        node(bool leaf_, size_t fill_): isLeaf{leaf_}, cntKeys{fill_} {}

        size_t find_pos(const int& key){
            size_t i = 0;
            while(i < cntKeys && pairs[i].first < key)
                ++i;

            return i;
        }

        std::pair<int, int>& get_max(){
            node* nd = this;
            while(!nd->isLeaf)
                nd = nd->nodes[nd->cntKeys];

            return nd->pairs[nd->cntKeys - 1];
        }


        bool insert(const int& key, const int& val);
        void split_child(size_t id);
        int find(const int& key);

    };

    node* root = nullptr;

    bool root_insert(const int& key, const int& val);

public:
    bool insert(const int& key, const int& val){
        return root_insert(key, val);
    }

    int find(const int& key){
        if(root)
            return root->find(key);
        return INT_MIN;
    }

};


template<size_t M>
bool Btree<M>::root_insert(const int& key, const int& val){
    if(!root){
        root = new node(true, 1);
        root->pairs[0].first = key;
        root->pairs[0].second = val;
        return true;
    }


    if(root->cntKeys == max_key){
        node* new_root = new node(false, 0);
        new_root->nodes[0] = root;
        root = new_root;

        root->split_child(0);
    }

    return root->insert(key, val);
}

template<size_t M>
bool Btree<M>::node::insert(const int& key, const int& val){
    size_t pos = find_pos(key);

    if(isLeaf){
        //Проверка на двойное добавление.
        if(pos < cntKeys && pairs[pos].first == key)
            return false;

        for(size_t i = cntKeys; i > pos; --i)
            pairs[i] = std::move(pairs[i - 1]);

        pairs[pos].first = key;
        pairs[pos].second = val;
        ++cntKeys;
        return true;
    }
    else{
        if(nodes[pos]->cntKeys == max_key){
            this->split_child(pos);
            if(val > pairs[pos].first)
                ++pos;
        }
        //Проверка на двойное добавление.
        if(pos < cntKeys && pairs[pos].first == key)
            return false;

        return nodes[pos]->insert(key, val);
    }
}


template<size_t M>
void Btree<M>::node::split_child(size_t id){
    node* l = nodes[id];
    node* r = new node(l->isLeaf, M - 1);

    for(size_t i = 0; i < M-1; ++i)
        r->pairs[i] = std::move(l->pairs[M + i]);

    if(!l->isLeaf)
        for(size_t i = 0; i < M; ++i)
            r->nodes[i] = l->nodes[M + i];
    l->cntKeys = M - 1;

    for(size_t i = cntKeys; i > id; --i){
        nodes[i + 1] = nodes[i];
        pairs[i] = std::move(pairs[i - 1]);
    }
    nodes[id + 1] = r;
    pairs[id] = std::move(l->pairs[M - 1]);
    ++cntKeys;
}


template<size_t M>
int Btree<M>::node::find(const int& key){
    size_t i = 0;
    for(; i < cntKeys && pairs[i].first <= key; ++i)
        if(pairs[i].first == key)
            return pairs[i].second;
    if(pairs[i].first == key)
        return pairs[i].second;
    if(!isLeaf)
        return nodes[i]->find(key);
    return INT_MIN;
}

std::vector<std::string> split(const std::string& strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (getline(ss, item, delimeter))
    {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

std::string doAction(std::string line, Btree<3> &bTree) {
    if (line.find('\r') != std::string::npos) {
        line.erase(line.find('\r'), 1);
    }
    std::vector<std::string> strings = split(line, ' ');
    if (strings[0] == "insert") {
        if (!bTree.insert(stoi(strings[1]), stoi(strings[2]))){
            return "false";
        }
        return "true";
    } else {
        int key = stoi(strings[1]);
        int ans = bTree.find(key);
        if (ans == INT_MIN) return "null";
        return std::to_string(ans);
    }
}

int main(int argc, const char * argv[]) {
    string inputPath(argv[1]);
    string outputPath(argv[2]);
    std::ifstream fs(inputPath, std::ios::in | std::ios::binary);
    Btree<3> bTree;
    // Проверка файла.
    if (!fs) return 1;
    int k = 1;
    std::string line;
    std::ofstream out(outputPath);
    while (!fs.eof())
    {
        getline(fs, line);
        out << doAction(line, bTree) << std::endl;
        k++;
    }
    return 0;
}

