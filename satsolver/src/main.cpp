#include <bits/stdc++.h>
#include <thread>
#include <sys/time.h>
#include "parser.h"
#include "LiteralandClause.h"
#include "SATSolver.h"
using namespace std;

#define vi vector<int>
#define vvi vector<vector<int>>
#define all(v) (v).begin(), (v).end()

int main(int argc, char **argv)
{
    timeval st, ed;
    gettimeofday(&st, 0);

    mutex m;
    condition_variable cond;
    atomic<thread::id> val;
    atomic<int> r;

    vector<string> anss(8, "");
    vector<thread> threads;

    auto task = [&](int i)
    {
        string s;
        atomic<thread::id> temp;
        vector<unique_ptr<Clause>> cls;
        int varnum;

        parse(cls, varnum, argv[1]);
        SATSolver solver(cls, varnum);

        solver.solve();
        solver.printSol(anss[i]);

        val = this_thread::get_id();
        r = i;
        cond.notify_all();
    };

    for (int i = 0; i < 8; ++i)
        thread{task, i}.detach();

    unique_lock<mutex> lock{m};
    cond.wait(lock, [&]
              { return val != thread::id{}; });

    string ofn = argv[1];
    ofn = "results/" + string(ofn.begin() + 11, ofn.end() - 3) + "sat";
    ofstream out(ofn);

    out << anss[r];
    out.close();

    gettimeofday(&ed, 0);
    //cout << "FILE: " << argv[1] << "\n" << (ed.tv_sec - st.tv_sec + (ed.tv_usec - st.tv_usec) / 1000000.0) << '\n';
}