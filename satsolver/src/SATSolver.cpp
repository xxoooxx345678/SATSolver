#include "SATSolver.h"
#include <bits/stdc++.h>
using namespace std;

#define vi vector<int>
#define vvi vector<vector<int>>
#define all(v) (v).begin(), (v).end()

inline void SATSolver::counterDecay()
{
    bool overflow = 0;
    for (auto &a : litDatas)
        a.counters[0] /= 0.95, a.counters[1] /= 0.95, overflow = (a.counters[0] >= 1e100 || a.counters[1] >= 1e100);
    if (overflow)
        for (auto &a : litDatas)
            a.counters[0] /= 1e100, a.counters[1] /= 1e100, increment /= 1e100;

    pqLitDatas.decay(overflow);
}

inline void SATSolver::imply(LiteralData &data, Clause &cls)
{
    assignments.push_back(make_unique<Assignment>(level, 1, data, &cls));
    data.assignby = assignments.back().get();
}

inline void SATSolver::decide(LiteralData &data)
{
    assignments.push_back(make_unique<Assignment>(++level, 0, data, nullptr));
    data.assignby = assignments.back().get();
}

inline void SATSolver::undoAssignment()
{
    LiteralData &data = assignments.back()->data;
    data.assignby = nullptr;
    data.status = -1;
    assignments.pop_back();

    pqLitDatas.push(data);
    //pqLitDatas.push_back(&data);
    //push_heap(all(pqLitDatas), cmpScore());
}

int SATSolver::nextLit2Watch(TwoLitWatching &watch) // -1 sat cls, -87 not found
{
    size_t c = watch.cls.size();
    auto x = watch.cls;
    int i = max(watch.watching[0], watch.watching[1]);

    for (int idx = (i + 1) % c; idx != i; idx = (idx + 1) % c)
    {
        if ((idx != watch.watching[0]) && (idx != watch.watching[1]))
        {
            int litStatus = (litDatas[x[idx] / 2 - 1].status == -1 ? -1 : (litDatas[x[idx] / 2 - 1].status == (x[idx] & 1)));
            if (litStatus == 1)
                return -1;
            if (litStatus == -1)
                return idx;
        }
    }

    return -87;
}

inline void SATSolver::addClause2Watch(TwoLitWatching &watch, int idx)
{
    int watchingLit = watch.watching[idx];
    if (watch.cls[watchingLit] & 1)
        litDatas[watch.cls[watchingLit] / 2 - 1].posClauses.push_back(&watch);
    else
        litDatas[watch.cls[watchingLit] / 2 - 1].negClauses.push_back(&watch);
}

inline void SATSolver::rmClause2Watch(TwoLitWatching &watch, int idx)
{
    int &watchingLit = watch.watching[idx];
    LiteralData &temp = litDatas[watch.cls[watchingLit] / 2 - 1];
    if (watch.cls[watchingLit] & 1)
        temp.posClauses.erase(remove(temp.posClauses.begin(), temp.posClauses.end(), &watch), temp.posClauses.end());
    else
        temp.negClauses.erase(remove(temp.negClauses.begin(), temp.negClauses.end(), &watch), temp.negClauses.end());
}

TwoLitWatching *SATSolver::update2LitWatching(LiteralData &data, int val)
{
    vector<TwoLitWatching *> &temp = (val & 1) ? data.negClauses : data.posClauses;
    data.status = val;

    auto it = temp.begin();
    while (it != temp.end())
    {
        int *lit = nullptr;
        TwoLitWatching *wat = *it;

        if (wat->watching[0] < 0)
            swap(wat->watching[0], wat->watching[1]);
        else
        {
            lit = &wat->cls[wat->watching[0]];
            if ((*lit) / 2 != data.replit)
                swap(wat->watching[0], wat->watching[1]);
        }

        if (!lit)
            lit = &wat->cls[wat->watching[0]];

        int nextWatchidx = nextLit2Watch(*wat);
        if (nextWatchidx == -1)
            ++it;
        else if (nextWatchidx == -87)
        {
            int ttlit = wat->cls[wat->watching[1]];
            int w2status = litDatas[wat->cls[wat->watching[1]] / 2 - 1].status;
            if (w2status != -1 && (wat->watching[1] == -1 || w2status != (ttlit & 1)))
                return wat;

            if (w2status == -1)
                queueUniqueClause.push(wat);
            ++it;
        }
        else
        {
            it = temp.erase(it);
            wat->watching[0] = nextWatchidx;
            addClause2Watch(*wat, 0);
        }
    }
    return nullptr;
}

bool SATSolver::jumpback(Clause *antecedent)
{
    jump = 1;
    jump_count++;
#if defined(RESTART)
    if (jump_count == restart_bound)
    {
        restart_bound = restart_bound * 3 / 2;
        restart();
        return 0;
    }
#endif
    counterDecay();
    int impn;
    while (queueUniqueClause.size())
        queueUniqueClause.pop();

    Clause cfcls;
    vector<int> &cflits = cfcls;
    cflits = *antecedent;

    while (assignments.size())
    {
        Assignment &last = *assignments.back();
        if (!last.implication)
            break;

        bool hasImp = 0;
        for (auto lit : cflits)
        {
            if (lit / 2 == last.data.replit)
            {
                hasImp = 1;
                break;
            }
        }

        undoAssignment();

        if (hasImp)
        {
            antecedent = last.antecedent;
            impn = 0;
            auto cflit = cflits.begin();

            for (auto lit = (*antecedent).begin(); lit < (*antecedent).end(); ++lit)
            {
                while (cflit != cflits.end() && (*cflit) / 2 < (*lit) / 2)
                    ++cflit;

                if ((*cflit) / 2 == (*lit) / 2 && cflit != cflits.end())
                {
                    if (((*cflit) & 1) != ((*lit) & 1))
                        cflit = cflits.erase(cflit);
                }
                else
                    cflit = cflits.insert(cflit, *lit), cflit++;
            }

            int i = 0;

            for (auto j = cflits.begin(); j < cflits.end(); ++j)
            {
                int t = *j;
                if (litDatas[t / 2 - 1].status != -1 && litDatas[t / 2 - 1].assignby->level == level)
                    i = j - cflits.begin(), ++impn;
            }

            if (impn == 1)
            {
                int jumptolv = -1;

                for (auto j = cflits.begin(); j < cflits.end(); ++j)
                {
                    int t = *j;
                    LiteralData &tlit = litDatas[t / 2 - 1];
                    if (tlit.status != -1 && tlit.assignby->level != level)
                        jumptolv = max(jumptolv, tlit.assignby->level);
                }

                if (jumptolv == -1)
                    break;

                clauses.push_back(make_unique<Clause>(cfcls));
                auto cw = make_unique<TwoLitWatching>(*clauses.back());

                cw->watching[0] = i;
                addClause2Watch(*cw, 0);
                if (cflits.size() > 1)
                {
                    cw->watching[1] = (i == 0);
                    addClause2Watch(*cw, 1);
                }
                clauseWatched.push_back(move(cw));

                updateCounters(&*clauses.back(), 0);
                while (assignments.size())
                {
                    Assignment &la = *assignments.back();
                    if (la.level == jumptolv)
                    {
                        jump = 0;
                        queueUniqueClause.push(&*clauseWatched.back());
                        level = jumptolv;
                        return 1;
                    }
                    else
                        undoAssignment();
                }
                break;
            }
        }
    }

    while (assignments.size())
    {
        Assignment &la = *assignments.back();
        if (la.implication)
            undoAssignment();
        else
        {
            if (!la.virgin)
                undoAssignment(), level--;
            else
            {
                if (la.data.status == 1)
                    la.data.status = 0;
                else
                    la.data.status = 1;

                la.virgin = 0;
                return 1;
            }
        }
    }
    sat = 0;
    return 1;
}

void SATSolver::updateCounters(Clause *clause, bool init)
{
    for (auto lit : *clause)
    {
        litDatas[lit / 2 - 1].counters[lit & 1] += increment;
        if (!init)
            pqLitDatas.incValue(lit / 2, increment, lit & 1);
    }
}

void SATSolver::restart()
{
    NiVER();

    level = 0, jump = 0, jump_count = 0, increment = 1.0;

    for (int i = clauses.size() - 1; i >= ori_cls_size; --i)
        if (clauses[i]->size() > 10)
        {
            swap(clauses[i], clauses.back());
            clauses.pop_back();
        }

    assignments.clear();
    clauseWatched.clear();
    while (queueUniqueClause.size())
        queueUniqueClause.pop();
    //pqLitDatas.clear();

    //pqLitDatas.resize(num_lit);
    pqLitDatas.clean();
    for (auto &a : litDatas)
        a.assignby = nullptr, a.counters = {0.0, 0.0}, a.negClauses.clear(), a.posClauses.clear(), a.status = -1;
    init();
}

bool SATSolver::resolve(Clause &a, Clause &b, Clause &c, int lit)
{
    size_t lp = 0, rp = 0;

    if (a.size() == 1 || b.size() == 1)
        return 0;

    while (lp < a.size() && rp < b.size())
    {
        if (a[lp] / 2 == lit || b[rp] / 2 == lit)
        {
            lp += (a[lp] / 2 == lit);
            rp += (b[rp] / 2 == lit);
            continue;
        }

        if (a[lp] < b[rp])
        {
            if (c.size())
            {
                if ((a[lp] ^ c.back()) == 1)
                    return 0;
                if (a[lp] == c.back())
                {
                    ++lp;
                    continue;
                }
            }

            c.push_back(a[lp++]);
        }
        else
        {
            if (c.size())
            {
                if ((b[rp] ^ c.back()) == 1)
                    return 0;
                if (b[rp] == c.back())
                {
                    ++rp;
                    continue;
                }
            }
            c.push_back(b[rp++]);
        }
    }

    while (lp < a.size())
    {
        if (a[lp] / 2 == lit)
        {
            ++lp;
            continue;
        }
        if (c.size())
        {
            if ((a[lp] ^ c.back()) == 1)
                return 0;
            if (a[lp] == c.back())
            {
                ++lp;
                continue;
            }
        }
        c.push_back(a[lp++]);
    }

    while (rp < b.size())
    {
        if (b[rp] / 2 == lit)
        {
            ++rp;
            continue;
        }
        if (c.size())
        {
            if ((b[rp] ^ c.back()) == 1)
                return 0;
            if (b[rp] == c.back())
            {
                ++rp;
                continue;
            }
        }
        c.push_back(b[rp++]);
    }

    return 1;
}

void SATSolver::NiVER()
{
#if defined(NIVER)
    vector<vector<int>> posL(num_lit + 1), negL(num_lit + 1);
    vector<bool> ab_cls(clauses.size(), 1);

    for (size_t i = 0; i < clauses.size(); ++i)
        for (auto lit : *clauses[i])
            if (lit & 1)
                posL[lit / 2].push_back(i);
            else
                negL[lit / 2].push_back(i);

    bool r = 1;
    while (r)
    {
        r = 0;
        for (int i = 1; i <= num_lit; ++i)
        {
            if (posL[i].empty() || negL[i].empty())
                continue;

            vector<Clause> temp;
            int old_num_lits = 0, new_num_lits = 0;

            for (size_t a = 0; a < posL[i].size(); ++a)
            {
                if (!ab_cls[posL[i][a]])
                    continue;

                for (size_t b = 0; b < negL[i].size(); ++b)
                {
                    if (!ab_cls[negL[i][b]])
                        continue;

                    int ai = posL[i][a], bi = negL[i][b];
                    Clause c;
                    if (resolve(*clauses[ai], *clauses[bi], c, i))
                        temp.push_back(c), new_num_lits += c.size();
                }
            }

            if (new_num_lits == 0)
                break;

            for (size_t a = 0; a < posL[i].size(); ++a)
                if (ab_cls[posL[i][a]])
                    old_num_lits += clauses[posL[i][a]]->size();
            for (size_t b = 0; b < negL[i].size(); ++b)
                if (ab_cls[negL[i][b]])
                    old_num_lits += clauses[negL[i][b]]->size();

            if (old_num_lits >= new_num_lits)
            {
                // cout << old_num_lits << ' ' << new_num_lits << '\n';

                for (size_t a = 0; a < posL[i].size(); ++a)
                    ab_cls[posL[i][a]] = 0;
                for (size_t b = 0; b < negL[i].size(); ++b)
                    ab_cls[negL[i][b]] = 0;
                for (size_t i = 0; i < temp.size(); ++i)
                {
                    for (auto lit : temp[i])
                        if (lit & 1)
                            posL[lit / 2].push_back(clauses.size());
                        else
                            negL[lit / 2].push_back(clauses.size());

                    auto cls = temp[i];
                    unique_ptr<Clause> ptr = make_unique<Clause>(cls);
                    clauses.push_back(move(ptr));
                    ab_cls.push_back(1);
                }
                r = 1;
            }
        }
    }

    for (size_t i = 0; i < clauses.size();)
    {
        if (!ab_cls[i])
        {
            swap(ab_cls[i], ab_cls.back());
            swap(clauses[i], clauses.back());
            ab_cls.pop_back();
            clauses.pop_back();
        }
        else
            ++i;
    }
#endif
}

void SATSolver::init()
{
    for (auto it = clauses.begin(); it < clauses.end(); ++it)
    {
        auto *clause = it->get();
        updateCounters(clause, 1);

        clauseWatched.push_back(make_unique<TwoLitWatching>(*clause));
        TwoLitWatching &watch = *clauseWatched.back();

        watch.watching[0] = 0;
        addClause2Watch(watch, 0);

        if ((*clause).size() == 1)
            queueUniqueClause.push(&watch);
        else
        {
            watch.watching[1] = 1;
            addClause2Watch(watch, 1);
        }
    }

    pqLitDatas.init(litDatas);
}

void SATSolver::go()
{
    while (1)
    {
        if (jump)
        {
            if (!sat)
                return;

            jump = 0;
            auto &data = assignments.back().get()->data;
            TwoLitWatching *cf = update2LitWatching(data, data.status);
            if (cf != nullptr)
            {
                if (!jumpback(&cf->cls))
                    continue;
            }
        }
        else
        {
            bool rt = 0;

            while (queueUniqueClause.size())
            {
                TwoLitWatching *watch = queueUniqueClause.front();
                queueUniqueClause.pop();

                int stw1, stw2;
                LiteralData td1, td2;

                if (watch->watching[0] >= 0)
                {
                    td1 = litDatas[watch->cls[watch->watching[0]] / 2 - 1];
                    stw1 = (watch->watching[0] == -1 ? 0 : (td1.status == -1 ? -1 : td1.status == (watch->cls[watch->watching[0]] & 1)));
                }
                else
                    stw1 = 0;

                if (watch->watching[1] >= 0)
                {
                    td2 = litDatas[watch->cls[watch->watching[1]] / 2 - 1];
                    stw2 = (watch->watching[1] == -1 ? 0 : (td2.status == -1 ? -1 : td2.status == (watch->cls[watch->watching[1]] & 1)));
                }
                else
                    stw2 = 0;

                if (stw1 != -1 && stw2 != -1)
                {
                    if (stw1 != 1 && stw2 != 1)
                    {
                        if (!jumpback(&watch->cls))
                            rt = 1;
                        break;
                    }
                    else
                        continue;
                }
                if (stw1 != -1)
                    swap(watch->watching[0], watch->watching[1]);

                LiteralData &data = litDatas[watch->cls[watch->watching[0]] / 2 - 1];

                imply(data, watch->cls);
                TwoLitWatching *cf = update2LitWatching(data, watch->cls[watch->watching[0]] & 1);
                if (cf != nullptr)
                    if (!jumpback(&cf->cls))
                        rt = 1;
            }

            if (rt)
                continue;

            if (jump)
                continue;

            while (pqLitDatas.size() && pqLitDatas.top()->assignby) //pqLitDatas.front()->assignby)
                pqLitDatas.pop();

            if (pqLitDatas.size() == 0)
            {
                if (jump)
                    continue;
                else
                    return;
            }

            LiteralData *next = pqLitDatas.rand_top_and_pop();

            decide(*next);
            TwoLitWatching *cf = update2LitWatching(*next, next->counters[1] > next->counters[0]);

            if (cf != nullptr)
                if (!jumpback(&cf->cls))
                    continue;
        }
    }
}

void SATSolver::verify()
{
    if (!sat)
        return;

    for (size_t i = 0; i < clauses.size(); ++i)
    {
        bool t = 0;
        for (auto lit : *clauses[i])
            t |= (litDatas[lit / 2 - 1].status == -1 ? 0 : litDatas[lit / 2 - 1].status == (lit & 1));
        sat &= t;
    }
}

void SATSolver::printSol(string &s)
{
    s += "s ";
    if (!sat)
    {
        s += "UNSATISFIABLE\n";
        return;
    }
    s += "SATISFIABLE\nv ";
    for (size_t i = 0; i < litDatas.size(); ++i)
        s += (litDatas[i].status == 0 ? "-" : "") + to_string(litDatas[i].replit) + " ";
    s += "0\n";
}

void SATSolver::solve()
{
    NiVER();
    init();
    go();
    verify();
}