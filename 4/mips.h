#ifndef _MIPS_H
#define _MIPS_H

#include <map>
#include "analyzer.h"

namespace MiniC {

    /*
     * $zero    0
     * $at      unused
     * $v0      function return
     * $v1      unused
     * $a0~$a3  unused
     * $t0      result
     * $t1      operand 1
     * $t2      operand 2
     * $t3~$t7  unused
     * $s0~$s7  unused
     * $t8~$t9  unused
     * $k0~$k1  unused
     * $gp      pointer to data segment
     * $sp      pointer to top of current stack
     * $fp      pointer to base of current stack
     * $ra      return address
     */
    class MIPS {
    private:
        const map<string, string> instructions = {
                {"+",  "add"},
                {"-",  "sub"},
                {"*",  "mul"},
                {"/",  "div"},
                {"%",  "rem"},
                {"&",  "and"},
                {"|",  "or"},
                {"^",  "xor"},
                {"<<", "sll"},
                {">>", "sra"},
                {"<",  "slt"},
                {">", "sgt"},
                {"<=", "sle"},
                {">=", "sge"},
                {"==", "seq"},
                {"!=", "sne"},
        };

        map<string, pair<int, int>> offsets;

        int dataOffset = 0;

        int localOffset = 0;

        int argOffset = 1;

        stringstream code;

        stringstream data;

        string getOffset(const string &identifier);

        static bool isLiteral(const string &str);

        string load(const string &from, const string &to);

        string save(const string &res, const string &from);

        static string replaceIllegal(const string &str);

        void generateData(const vector<Quadruple> &quadruples);

        void generateText(const vector<Quadruple> &quadruples);

    public:
        string compile(const vector<Quadruple> &quadruples);
    };

}

#endif // _MIPS_H
