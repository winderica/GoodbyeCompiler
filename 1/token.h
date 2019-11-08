#ifndef _TOKEN_H
#define _TOKEN_H

#include <string>
#include <vector>

using namespace std;

namespace MiniC {
    class Token {
    public:
        Token(string name, string value);

        explicit Token(string name);

        Token();

        ~Token();

        void addChild(const Token &child);

        [[nodiscard]] string toString(const string& baseIndent = "") const;

        [[nodiscard]] string getName() const;

    private:
        string name;
        string value;
        vector<Token> children;
    };
}

#endif // _TOKEN_H
