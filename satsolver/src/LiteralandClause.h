#ifndef __LITERALANDCLAUSE_H__
#define __LITERALANDCLAUSE_H__

#include <bits/stdc++.h>
using namespace std;

#define vi vector<int>
#define vvi vector<vector<int>>
#define vd vector<double>
#define all(v) (v).begin(), (v).end()
#define ull unsigned long long

using Clause = vector<int>;

class LiteralData;
class Assignment;
class TwoLitWatching;

class LiteralData
{
public:
    array<double, 2> counters = {0.0, 0.0}; //neg counters, pos counters

    vector<TwoLitWatching *> posClauses;
    vector<TwoLitWatching *> negClauses;

    int replit = 0;
    int status = -1; // -1 unassigned, 0 false, 1 true
    Assignment *assignby = nullptr;
};

class Assignment
{
public:
    bool virgin;
    int level;
    bool implication;
    LiteralData &data;
    Clause *antecedent;

    Assignment(int lv, bool imp, LiteralData &d, Clause *ant) : virgin(1), level(lv), implication(imp), data(d), antecedent(ant) {}
};

class TwoLitWatching
{
public:
    array<int, 2> watching;
    Clause &cls;

    TwoLitWatching(Clause &c) : watching({-1, -1}), cls(c) {}
};

#endif