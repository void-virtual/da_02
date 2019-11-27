#pragma once

#include <iostream>
#include <fstream>
#include <cstring>

#include "TVector.h"
#include "TOptional.h"

#define MAX_LEFT_ONE 128 // 10000000
#define BITS_IN_CHAR 8

bool getNthBit(const TVector<unsigned char>& lhs, size_t index);

template <typename T>
class TPatricia {
public:
    TPatricia() = default;

    ~TPatricia() {
        DeleteTree(header);
    }

    TOptional<T> operator [] (const TVector<unsigned char>& string) const {
        Node* endNode = SearchKey(string);
        if (endNode && endNode->string == string) {
            return TOptional<T>(endNode->data);
        }
        return TOptional<T>();
    }

    bool Insert(TVector<unsigned char> string, T data) { // добавляет элемент в дерево
        //возвращает true если добавление успешно и false если элемент уже есть в дереве
        Node* reachedNode = SearchKey(string);
        if (!reachedNode) { //пустое дерево
            header = new Node;
            header->left = header;
            header->right = nullptr;
            header->string = std::move(string);
            header->data = data;
            header->index_to_check = -1;
            return true;
        }

        const TVector<unsigned char>& reachedKey = reachedNode->string;
        if (reachedKey == string) { //элемент уже есть в дереве
            return false;
        }

        long long differPos = -1;
        for (size_t i = 0; i < std::max(string.Size(), reachedKey.Size()) * BITS_IN_CHAR; ++i) {
            if (getNthBit(reachedKey, i) != getNthBit(string, i)) {
                differPos = i;
                break;
            }
        }
        if (differPos == -1) {// невозможная ситуация, но если случится, значит я где-то набажил
            throw std::logic_error("Something gone wrong");
        }
        Node* new_node = new Node;
        new_node->index_to_check = differPos;
        new_node->string = std::move(string);
        new_node->data = data;

        Node* tmp_prev = header;
        Node* tmp_node = header->left;
        //снова спускаемся по дереву, ищем место для вставки
        //сломать ссылку нужно в одном из двух случаев
        //если ссылка обратная или если значение индекса в узле, в который она ведет, больше, чем во вставляемом узле
        //оставляем ссылку на предыдущий узел и осуществляем спуск
        while (tmp_node->index_to_check > tmp_prev->index_to_check /*проверка на обратную ссылку*/
               && tmp_node->index_to_check < differPos) {
            tmp_prev = tmp_node;
            tmp_node = (getNthBit(new_node->string, tmp_node->index_to_check) ? tmp_node->right : tmp_node->left);
        }
        //если дошли до сюда, значит надо сломать ссылку из tmp_prev в tmp_node, по которой мы перешли
        Node* link_to_break = tmp_node;

        if (tmp_node == tmp_prev->right) {
            tmp_prev->right = new_node;
        } else {
            tmp_prev->left = new_node;
        }

        if (getNthBit(new_node->string, differPos)) {
            new_node->right = new_node;
            new_node->left = link_to_break;
        } else {
            new_node->left = new_node;
            new_node->right = link_to_break;
        }
        return true;
    }

    bool Erase(const TVector<unsigned char>& string) {//возвращает true если было успешно удалено, false если нет такого слова
        Node* backwardPtrFrom;
        Node* parentNode;
        Node* reachedNode = SearchKey(string, &backwardPtrFrom);
        if (!reachedNode || reachedNode->string != string) {
            return false;
        }
        parentNode = SearchParentNode(reachedNode);
        if (reachedNode == header && header->left == header) {
            delete header;
            header = nullptr;
            return true;
        }

        if (reachedNode->left == reachedNode || reachedNode->right == reachedNode) {
            Node* nonSelfLink;
            if (reachedNode->left == reachedNode) {
                nonSelfLink = reachedNode->right;
            } else {
                nonSelfLink = reachedNode->left;
            }
            if (reachedNode == parentNode->right) {
                parentNode->right = nonSelfLink;
            } else if (reachedNode == parentNode->left) {
                parentNode->left = nonSelfLink;
            } else {
                throw std::logic_error("Wrong parent search");
            }
            delete reachedNode;
            return true;
        }

        //просто небольшое переименование для удобства
        Node* p = reachedNode; // © node to delete
        Node* q = backwardPtrFrom; // © q has a backward ptr to p
        Node* r = nullptr; // r has a backward to q, is found by using key of qElement
        Node* qParent = SearchParentNode(q);
        SearchKey(q->string, &r);
        bool isQChildRight = getNthBit(r->string, q->index_to_check);
        // эта булевская переменная нужна, чтобы определить, справа или слева от q находился r в дереве
        // swap p and q data and delete q
        std::swap(p->string, q->string);
        std::swap(p->data, q->data);

        if (r->right == q) {
            r->right = p;
        } else if (r->left == q) {
            r->left = p;
        } else {
            throw std::logic_error("NO BACKWARD LINK!!!!!!!");
        }

        if (q == qParent->right) {
            if (isQChildRight) {
                qParent->right = q->right;
            } else {
                qParent->right = q->left;
            }
        } else if (q == qParent->left) {
            if (isQChildRight) {
                qParent->left = q->right;
            } else {
                qParent->left = q->left;
            }
        } else {
            throw std::logic_error("WRONG PARENT!!!!!!!");
        }
        delete q;
        return true;
    }


    void ScanFromFile(const char* filename) {
        std::fstream file(filename, std::ios::in|std::ios::binary);
        if (!file.is_open()) {
            return;
        }
        if (file.peek() == EOF) {
            TPatricia<T> patr;
            std::swap(patr.header, header);
            return;
        }
        Node* new_header = nullptr;
        size_t nodesCount;
        file.read((char*)&nodesCount, sizeof(size_t));
        TVector<Node*> nodes(nodesCount);
        for (size_t i = 0; i < nodesCount; ++i) {
            size_t id, stringSize;
            T data;
            long long index;
            file.read((char*)&id, sizeof(size_t));//id
            file.read((char*)&data, sizeof(T));//data
            file.read((char*)&stringSize, sizeof(size_t));//string size
            TVector<unsigned char> string(stringSize);
            for (size_t j = 0; j < stringSize; ++j) {
                file.read((char*)&string[j],sizeof(unsigned char));
            }
            file.read((char*)&index, sizeof(long long));//
            Node* new_node = new Node;
            new_node->index_to_check = index;
            new_node->string = string;
            new_node->data = data;
            nodes[id] = new_node;
            if (index == -1) {
                new_header = new_node;
            }
        }
        new_header->right = nullptr;
        for (size_t i = 0; i < nodes.Size() * 2 - 1; ++i) {
            size_t id, link;
            char direction;
            file.read((char*)&id, sizeof(size_t));
            file.read((char*)&link, sizeof(size_t));
            file.read((char*)&direction, sizeof(char));
            if (direction == '0') {
                nodes[id]->left = nodes[link];
            } else if (direction == '1') {
                nodes[id]->right = nodes[link];
            }
        }

        TPatricia<T> patr;
        patr.header = new_header;
        std::swap(patr.header, header);
    }

    void PrintToFile(const char* filename) const {
        //суть - сначала создать вектор из пар указателей и присовенных им Id (заполняется при обходе дерева в ширину)
        //после отсортировать его по значениям указателей, тем самым достигнув сложности O(log n) при поиске Id указателя
        //отменено
        std::ofstream file;
        file.open(filename, std::ios::binary | std::ios::out);
        file.sync_with_stdio(false);
        if (!file.is_open()) {
            return;
        }
        if (header == nullptr) {
            return;
        }
        TVector<Node*> nodes;
        int counter = 0;
        CountIds(header, counter, nodes);
        size_t size = nodes.Size();
        file.write((char*)&size, sizeof(size_t));
        for (size_t i = 0; i < nodes.Size(); ++i) {
            Node* node = nodes[i];
            file.write((char*)&node->id, sizeof(size_t));//id - size_t
            file.write((char*)&node->data, sizeof(T));//data - T
            size_t string_size = node->string.Size();

            file.write((char*)&string_size, sizeof(size_t));//string size - size_t
            file.write((char*)node->string.begin(), sizeof(unsigned char) * string_size);
            file.write((char*)&node->index_to_check, sizeof(long long));
        }
        for (size_t i = 0; i < nodes.Size(); ++i) {
            Node* node = nodes[i];
            Node* nodeRight = node->right;
            Node* nodeLeft = node->left;
            if (nodeRight != nullptr) {
                file.write((char*)&node->id, sizeof(size_t));
                size_t id;
                id = nodeRight->id;
                file.write((char*)&id, sizeof(size_t));
                char right = '1';
                file.write((char*)&right, sizeof(char));
            }
            file.write((char*)&node->id, sizeof(size_t));
            size_t id;
            id = nodeLeft->id;
            file.write((char*)&id, sizeof(size_t));
            char right = '0';
            file.write((char*)&right, sizeof(char));
        }
        file.close();
    }

private:
    struct Node {
        long long index_to_check;
        TVector<unsigned char> string; //ключ - по понятным причинам строка
        T data; //значение
        //true 1 right
        //false 0 left
        Node* left;
        Node* right;
        size_t id;
    };

    void CountIds(Node* node, int& id, TVector<Node*>& nodes) const {
        node->id = id;
        nodes.PushBack(node);
        id++;
        if (node->right != nullptr && node->index_to_check < node->right->index_to_check) {
            CountIds(node->right, id, nodes);
        }
        if (node->index_to_check < node->left->index_to_check) {
            CountIds(node->left, id, nodes);
        }
     }

    void DeleteTree(Node* node) {
        if (node == nullptr) {
            return;
        }
        if(node->right != nullptr && node->index_to_check < node->right->index_to_check) {
            DeleteTree(node->right);
        }
        if (node->index_to_check < node->left->index_to_check) {
            DeleteTree(node->left);
        }
        delete node;
    }

    Node* SearchParentNode(Node* node) const {
        if (node == header->left) {
            return header;
        }
        Node* tmp = header->left;
        while(tmp->right != node && tmp->left != node) {
            if (getNthBit(node->string, tmp->index_to_check)) {
                tmp = tmp->right;
            } else {
                tmp = tmp->left;
            }
        }
        if (tmp->right == node || tmp->left == node) {
            return tmp;
        } else {
            throw std::logic_error("Parent wrong");
        }
    }

    Node* SearchKey(const TVector<unsigned char>& string, Node** back = nullptr) const {
        if (header == nullptr) {
            return nullptr;
        }
        Node* prev_node = header;
        Node* cur_node = header->left;
        while (cur_node->index_to_check > prev_node->index_to_check) {
            bool cur_bit = getNthBit(string, cur_node->index_to_check);
            //true 1 right
            //false 0 left
            prev_node = cur_node;
            if (cur_bit){
                cur_node = cur_node->right;
            } else {
                cur_node = cur_node->left;
            }
        }
        if (back != nullptr) {
            *back = prev_node;
        }
        return cur_node;
    }

    Node* header = nullptr;
/bin/bash: q: команда не найдена
