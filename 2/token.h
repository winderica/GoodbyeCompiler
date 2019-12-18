#ifndef _TOKEN_H
#define _TOKEN_H

#include <sstream>
#include <vector>
#include <optional>
#include "location.hh"

using namespace std;

namespace MiniC {
    class Token {
    public:
        Token(string kind, string value);

        explicit Token(string kind);

        Token(string kind, string value, location loc);

        Token(string kind, location loc);

        Token();

        void addChild(const Token &child);

        void setKind(const string &kind);

        [[nodiscard]] string toString(const string &baseIndent = "") const;

        [[nodiscard]] string getKind() const;

        [[nodiscard]] string getValue() const;

        [[nodiscard]] vector<Token> getChildren() const;

        [[nodiscard]] Token getChildren(int index) const;

        [[nodiscard]] optional<location> getLocation() const;

        bool operator ==(const string &str) const;

    private:
        string kind;
        string value;
        optional<location> loc;
        vector<Token> children;
    };
}

#endif // _TOKEN_H
