#include "Heap.h"
using namespace std;

void myHeap::init(vector<LiteralData> &d)
{
    datas.clear();
    map.clear();
    datas.resize(d.size() + 1);
    map.resize(d.size() + 1);
    sz = d.size();

    for (size_t i = 1; i < datas.size(); ++i)
    {
        datas[i] = &d[i - 1];
        map[d[i - 1].replit] = i;
    }

    for (int i = int(datas.size()) / 2; i >= 1; --i)
        siftDown(i);
}

void myHeap::print()
{
    for (size_t i = 1; i <= sz; ++i)
        cout << datas[i]->replit << ' ' << datas[i]->counters[0] << ' ' << datas[i]->counters[1] << '\n';
}

void myHeap::push(LiteralData &d)
{
    int i = map[d.replit];
    datas[i]->assignby = d.assignby, datas[i]->counters = d.counters, datas[i]->status = d.status;
    if (i <= int(sz))
        return;
    ++sz;
    if (d.replit != datas[sz]->replit)
        swap(map[datas[sz]->replit], map[datas[i]->replit]), swap(datas[sz], datas[i]);
    siftUp(sz);
}

void myHeap::pop()
{
    swap(map[datas[1]->replit], map[datas[sz]->replit]), swap(datas[1], datas[sz]);
    sz--;
    siftDown(1);
}

LiteralData *myHeap::top()
{
    return datas[1];
}

LiteralData *myHeap::rand_top_and_pop()
{
    mt19937_64 mt(time(nullptr));
    size_t p = mt() % min(int(sz), 3) + 1;

    size_t idx = 1;
    vector<LiteralData *> pb;
    while (idx++ != p)
        pb.push_back(top()), pop();

    LiteralData *ret = top();
    pop();

    for (auto a : pb)
        push(*a);

    return ret;
}

void myHeap::incValue(int d, double score, int pn)
{
    int i = map[d];
    datas[i]->counters[pn] += score;

    if (i <= int(sz))
        siftUp(i);
}

size_t myHeap::size()
{
    return sz;
}

void myHeap::decay(bool overflow)
{
    for (size_t i = 1; i < datas.size(); ++i)
        datas[i]->counters[0] /= 0.95, datas[i]->counters[1] /= 0.95, datas[i]->counters[0] *= 1e-100, datas[i]->counters[1] *= 1e-100;
}

void myHeap::clean()
{
    map.clear();
    datas.clear();
    sz = 0;
}

double myHeap::ttScore(int i)
{
    return (i <= int(sz)) ? datas[i]->counters[0] + datas[i]->counters[1] : -1.0;
}

void myHeap::siftUp(int i)
{
    while (i > 1)
    {
        int pid = i >> 1;
        if (ttScore(pid) < ttScore(i))
            swap(map[datas[pid]->replit], map[datas[i]->replit]), swap(datas[pid], datas[i]);
        else
            break;

        i = pid;
    }
}

void myHeap::siftDown(int i)
{
    while (1)
    {
        int lc = i << 1, rc = (i << 1) | 1, mx;

        if (ttScore(lc) > ttScore(lc))
            mx = lc;
        else
            mx = rc;

        if (ttScore(mx) > ttScore(i))
            swap(map[datas[mx]->replit], map[datas[i]->replit]), swap(datas[mx], datas[i]), i = mx;
        else
            break;
    }
}