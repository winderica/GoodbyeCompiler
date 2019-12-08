#include <iomanip>
#include "symbolTable.h"
#include "color.h"

using namespace MiniC;

int SymbolTable::insertSymbol(const Symbol &symbol) {
    if (table.find(symbol.name) != table.end()) {
        return 1;
    }
    stringstream ss;
    ss << BLUE << setw(10) << symbol.name
       << YELLOW << setw(3) << symbol.scope
       << GREEN << setw(7) << symbol.type
       << MAGENTA << (symbol.isFunction ? " function" : "");
    for (const auto &param: symbol.parameters) {
        ss << CYAN << setw(7) << param;
    }
    ss << RESET_COLOR << endl;
    cout << ss.str();
    table.insert({symbol.name, symbol});
    return 0;
}

optional<Symbol> SymbolTable::findSymbol(const string &name) {
    try {
        return table.at(name);
    } catch (...) {
        return nullopt;
    }
}
