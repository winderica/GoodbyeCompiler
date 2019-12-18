#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

#include <unordered_map>
#include "symbol.h"

namespace MiniC {
    class SymbolTable {
    public:
        int insertSymbol(const Symbol &symbol);

        optional<Symbol> findSymbol(const string &name);

    private:

        unordered_map<string, Symbol> table;
    };

}

#endif // _SYMBOLTABLE_H
