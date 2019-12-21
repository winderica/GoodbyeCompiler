#include <fstream>
#include "interpreter.h"

int main(int argc, char **argv) {
    MiniC::Interpreter interpreter;
    ifstream input(argv[1]);
    ofstream output(argv[2]);
    interpreter.switchInputStream(argc >= 2 ? &input : &cin);
    interpreter.switchOutputStream(argc >= 3 ? &output : &cout);
    string divider = string(100, '<');
    cout << "Parse started." << endl << divider << endl;
    int res = interpreter.parse();
    cout << interpreter.toString() << endl;
    cout << divider << endl << (res ? "Parse failed." : "Parse completed.") << endl << divider << endl;
    if (!res) {
        cout << "Analysis started." << endl << divider << endl;
        interpreter.analyze();
        cout << "Analysis completed." << endl << divider << endl;
        cout << "Compilation started." << endl << divider << endl;
        interpreter.compile();
        cout << "Compilation completed." << endl << divider << endl;
    }
    return res;
}
