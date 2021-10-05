from pyeda.inter import *
from pyeda.boolalg import picosat
import sys
import math, time
sys.setrecursionlimit(100000000)

class sudoku_solver:

    def __init__(self):
        self.n = 0
        self.m = 0
        self.x = []
        self.infile_name = ""
        self.outfile_name = ""

    def encode(self, Y, r, c, d):
        return Y*self.n*self.n*self.n + r*self.n*self.n + c*self.n + d

    def decode(self, x):
        n1 = self.n
        n2 = n1*self.n
        n3 = n2*self.n
        a3 = x // n3
        x -= a3 * n3
        a2 = x // n2
        x -= a2 * n2
        a1 = x // n1
        x -= a1 * n1
        return a3, a2, a1, x

    def read_puzzle(self):
        _, self.infile_name, self.outfile_name = sys.argv

        infile = open(self.infile_name, 'r')
        lines = infile.readlines()

        for line in lines:
            self.x.append([int(t) for t in line.split()])

        self.n = len(self.x)

    def build_and_count_BDD(self):
        V = [[[['v' + str(self.encode(y, r, c, d)) + '' for d in range(self.n)] for c in range(self.n)] for r in range(self.n)] for y in range(self.n)]

        f = "1"


        for row in range(self.n):
            for col in range(self.n):
                for d in range(self.n):
                    if (d == self.x[row][col]): 
                        f += '&(' + V[0][row][col][d] + ')'
                    else:
                        f += '&(~' + V[0][row][col][d] + ')'
                        
        for row in range(self.n):
            for col in range(self.n):
                temp_f = ""

                for d in range(self.n):
                    temp_f += ('|' if d > 0 else '') + V[1][row][col][d]
                f += '&(' + temp_f + ')'

                for d in range(self.n):
                    for dd in range(d+1, self.n):
                        f += "&(~" + V[1][row][col][d] + '|~' + V[1][row][col][dd] + ')'

        for d in range(self.n):
            for row in range(self.n):
                temp_f = ""

                for col in range(self.n):
                    temp_f += ('|' if col > 0 else '') + V[1][row][col][d]
                f += '&(' + temp_f + ')'

                for c1 in range(self.n):
                    for c2 in range(c1+1, self.n):
                        f += "&(~" + V[1][row][c1][d] + '|~' + V[1][row][c2][d] + ')'

            for col in range(self.n):
                temp_f = ""

                for row in range(self.n):
                    temp_f += ('|' if row > 0 else '') + V[1][row][col][d]
                f += '&(' + temp_f + ')'

                for r1 in range(self.n):
                    for r2 in range(r1+1, self.n):
                        f += "&(~" + V[1][r1][col][d] + '|~' + V[1][r2][col][d] + ')'

        f += '&(0'

        for d1 in range(self.n):
            for d2 in range(self.n):
                for row in range(self.n):
                    for col in range(self.n):
                        f += "|(" + V[0][row][col][d1] + "&" + V[1][row][col][d2] + ')'

        f += ')'

        for d1 in range(self.n):
            for d2 in range(self.n):
                for row in range(self.n):
                    for col in range(self.n):
                        for x in range(row*self.n+col+1, self.n*self.n):
                            f += '&((~' + V[0][row][col][d1] + "|~" + V[1][row][col][d2] + ')|(~' + V[0][x//self.n][x%self.n][d1] + '|~' + V[1][x//self.n][x%self.n][d2] + '))'


        tf = expr(f).tseitin()
        ans = tf.satisfy_one()
        outfile = open(self.outfile_name, 'w')

        wr = []

        if ans != None:

            for v, val in ans.items():
                if v.name != 'aux' and (int(v.name[1:]) >= self.n*self.n*self.n) and val==1:
                    yy, rr, cc, dd = self.decode(int(v.name[1:]))
                    wr.append(dd)

            for r in range(self.n):
                for c in range(self.n):
                    outfile.write(str(wr[r*self.n+c]) + ' ')
                outfile.write('\n')
        else:
            outfile.write("No orthogonol mate!")

def main():
    solver = sudoku_solver()
    solver.read_puzzle()
    solver.build_and_count_BDD()

if __name__ == "__main__":
    st = time.time()
    main()
    ed = time.time()
    print(ed-st)
