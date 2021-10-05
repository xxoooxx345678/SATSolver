#ifndef __SATSOLVER_H__
#define __SATSOLVER_H__

#include <bits/stdc++.h>
#include "Heap.h"
#include "LiteralandClause.h"
using namespace std;

#define vi vector<int>
#define vvi vector<vector<int>>
#define all(v) (v).begin(), (v).end()

struct cmpScore
{
    bool operator()(const LiteralData *d1, const LiteralData *d2)
    {
        return d1->counters[0] + d1->counters[1] < d2->counters[0] + d2->counters[1];
    }
};

class SATSolver
{
public:
    SATSolver(vector<unique_ptr<Clause>> &cls, int num) : num_lit(num), clauses(cls)
    {
        litDatas.resize(num);
        //pqLitDatas.resize(num);
        ori_cls_size = cls.size();

        int v = 0;
        for (auto &data : litDatas)
            data.replit = ++v;
    }

    void printSol(string &s);
    void solve();

private:
    int num_lit = 0;
    int level = 0;
    int jump_count = 0;
    int restart_bound = 100;
    int ori_cls_size = 0;
    double increment = 1.0;
    bool sat = 1;
    bool jump = 0;

    vector<unique_ptr<Clause>> &clauses;
    vector<unique_ptr<TwoLitWatching>> clauseWatched;

    vector<LiteralData> litDatas;
    //vector<LiteralData *> pqLitDatas;
    myHeap pqLitDatas;

    queue<TwoLitWatching *> queueUniqueClause;

    vector<unique_ptr<Assignment>> assignments;

    bool resolve(Clause &a, Clause &b, Clause &c, int lit);
    void NiVER();

    void counterDecay();
    void imply(LiteralData &data, Clause &cls);
    void decide(LiteralData &data);
    void undoAssignment();

    int nextLit2Watch(TwoLitWatching &watch);
    void addClause2Watch(TwoLitWatching &watch, int idx);
    void rmClause2Watch(TwoLitWatching &watch, int idx);
    TwoLitWatching *update2LitWatching(LiteralData &data, int val);

    bool jumpback(Clause *antecedent);
    void updateCounters(Clause *clause, bool init);
    void restart();

    void init();
    void go();
    void verify();
};

#endif