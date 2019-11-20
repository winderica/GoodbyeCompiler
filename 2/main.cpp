#include <fstream>
#include "interpreter.h"

int main(int argc, char **argv) {
    MiniC::Interpreter interpreter;
    ifstream input(argv[1]);
    interpreter.switchInputStream(argc == 2 ? &input : &cin);
    string divider = string(100, '<');
    cout << "Parse started." << endl << divider << endl;
    int res = interpreter.parse();
    cout << interpreter.toString() << endl;
    cout << divider << endl << (res ? "Parse failed." : "Parse completed.") << endl << divider << endl;
    cout << "Analyze started." << endl << divider << endl;
    interpreter.analyze();
    cout << "Analyze completed." << endl << divider << endl;
    return res;
}
