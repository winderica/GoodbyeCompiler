#ifndef _QUADRUPLE_H
#define _QUADRUPLE_H

#include "token.h"

namespace MiniC {
    class Interpreter;

    class Quadruple {
    public:
        string op;
        string arg1;
        string arg2;
        string res;

        Quadruple(string op, string arg1, string arg2, string res);

        string toString();
    };
}

#endif // _QUADRUPLE_H
