#ifndef __HEAP_H__
#define __HEAP_H__

#include "LiteralandClause.h"
#include <bits/stdc++.h>
using namespace std;

class myHeap
{
public:
    void init(vector<LiteralData> &d);
    void push(LiteralData &d);
    void pop();
    LiteralData *top();
    LiteralData *rand_top_and_pop();
    void incValue(int d, double score, int pn);
    size_t size();
    void decay(bool overflow);
    void print();
    void clean();

private:
    size_t sz = 0;
    vector<LiteralData *> datas;
    vector<int> map;

    double ttScore(int i);
    void siftDown(int node);
    void siftUp(int node);
};

#endif