#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <string>

enum Relation { LESS, EQUAL, GREATER, NONE };

std::map<std::string, std::map<std::string, Relation>> precedenceTable =
{
    {"var", {{"id", LESS}, {";", NONE}, {"begin", LESS}, {"end", LESS}, {"for", NONE},
             {"to", NONE}, {"do", LESS}, {":=", LESS}, {"#", LESS}, {"!", NONE}, {"&", NONE}, {"$", GREATER}}},

    {"id", {{"id", NONE}, {";", LESS}, {"begin", GREATER}, {"end", GREATER}, {"for", NONE},
            {"to", LESS}, {"do", GREATER}, {":=", LESS}, {"#", LESS}, {"!", LESS}, {"&", LESS}, {"$", NONE}}},

    {";", {{"id", LESS}, {";", NONE}, {"begin", GREATER}, {"end", NONE}, {"for", LESS},
           {"to", NONE}, {"do", LESS}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"begin", {{"id", LESS}, {";", NONE}, {"begin", NONE}, {"end", GREATER}, {"for", GREATER},
               {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"end", {{"id", NONE}, {";", NONE}, {"begin", NONE}, {"end", EQUAL}, {"for", NONE},
             {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", GREATER}}},

    {"for", {{"id", GREATER}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", NONE},
             {"to", GREATER}, {"do", GREATER}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"to", {{"id", LESS}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", NONE},
            {"to", NONE}, {"do", GREATER}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"do", {{"id", NONE}, {";", NONE}, {"begin", GREATER}, {"end", NONE}, {"for", NONE},
            {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {":=", {{"id", LESS}, {";", NONE}, {"begin", NONE}, {"end", GREATER}, {"for", NONE},
            {"to", NONE}, {"do", GREATER}, {":=", NONE}, {"#", GREATER}, {"!", LESS}, {"&", NONE}, {"$", NONE}}},

    {"#", {{"id", GREATER}, {";", NONE}, {"begin", NONE}, {"end", GREATER}, {"for", NONE},
           {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", LESS}, {"&", NONE}, {"$", NONE}}},

    {"!", {{"id", LESS}, {";", NONE}, {"begin", NONE}, {"end", GREATER}, {"for", NONE},
           {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", LESS}, {"$", NONE}}},

    {"&", {{"id", LESS}, {";", NONE}, {"begin", LESS}, {"end", GREATER}, {"for", NONE},
           {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", GREATER}, {"&", NONE}, {"$", NONE}}},

    {"$", {{"var", LESS}, {"id", NONE}, {";", NONE}, {"begin", NONE}, {"end", GREATER}, {"for", NONE},
           {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", EQUAL}}}
};


std::vector<std::string> tokenize(const std::string& expr)
{
    std::vector<std::string> tokens;
    size_t i = 0;

    while (i < expr.size())
    {
        if (isspace(expr[i])) i++;

        else if (expr[i] == ';' || expr[i] == '&' || expr[i] == '#' || expr[i] == '!')
        {
            tokens.push_back(std::string(1, expr[i]));

            i++;
        }
        else if (expr.substr(i, 2) == ":=" || expr.substr(i, 2) == "to" || expr.substr(i, 2) == "do")
        {
            tokens.push_back(expr.substr(i, 2));
            i += 2;
        }
        else if (expr.substr(i, 3) == "for" || expr.substr(i, 3) == "end" || expr.substr(i, 3) == "var")
        {
            tokens.push_back(expr.substr(i, 3));
            i += 3;
        }
        else if (expr.substr(i, 5) == "begin")
        {
            tokens.push_back(expr.substr(i, 5));
            i += 5;
        }
        else if (isdigit(expr[i]) || (isalpha(expr[i]) && isupper(expr[i])))
        {
            tokens.push_back("id");

            while (i < expr.size() && (isdigit(expr[i]) || (isalpha(expr[i]) && isupper(expr[i])))) i++;
        }
        else throw std::runtime_error(std::string("Unsupported character: ") + expr[i]);
    }

    tokens.push_back("$");

    return tokens;
}

Relation getRelation(const std::string& top, const std::string& current) {
    auto topEntry = precedenceTable.find(top);
    if (topEntry != precedenceTable.end() && topEntry->second.find(current) != topEntry->second.end()) {
        return topEntry->second[current];
    }
    return NONE;
}

bool parse2(const std::vector<std::string>& tokens) {
    std::stack<std::string> parseStack;
    parseStack.push("$");

    size_t i = 0;
    while (i < tokens.size()) {
        std::string current = tokens[i];
        std::cout << "Current token: " << current << "\n";

        if (parseStack.empty()) {
            std::cout << "Error: stack is empty before processing token '" << current << "'\n";
            return false;
        }

        std::string top = parseStack.top();

        if (current == "$" && top == "$") {
            std::cout << "End of parsing detected. Successful parsing.\n";
            return true;
        }

        Relation relation = getRelation(top, current);
        std::cout << "Relation between '" << top << "' and '" << current << "': "
            << (relation == LESS ? "<" : relation == GREATER ? ">" : relation == EQUAL ? "=" : "NONE") << "\n";

        if (!parseStack.empty() && parseStack.top() == "?" && current == ":") {
            std::cout << "Reducing '?' with ':'\n";
            parseStack.pop();
        }

        if (relation == LESS || relation == EQUAL) {
            parseStack.push(current);
            i++;
            std::cout << "Pushed '" << current << "' to stack\n";
        }
        else if (relation == GREATER) {
            do {
                if (parseStack.empty()) {
                    std::cout << "Error: can't reduce - stack empty\n";
                    return false;
                }
                std::cout << "Popped '" << parseStack.top() << "' from stack\n";
                parseStack.pop();
            } while (!parseStack.empty() && getRelation(parseStack.top(), current) == GREATER);

            if (!parseStack.empty() && getRelation(parseStack.top(), current) == NONE) {
                std::cout << "Error: no relation after reduction between '" << parseStack.top() << "' and '" << current << "'\n";
                return false;
            }
        }
        else {
            std::cout << "Syntax Error: no valid relation between '" << top
                << "' and '" << current << "'\n";
            return false;
        }
    }
    while (!parseStack.empty() && parseStack.top() != "$") {
        if (parseStack.top() == "?") {
            std::cout << "Error: '?' must be reduced before finishing.\n";
            return false;
        }
        std::cout << "Final reduction: popped '" << parseStack.top() << "'\n";
        parseStack.pop();
    }

    return parseStack.size() == 1 && parseStack.top() == "$";
}

int main()
{
    std::string line = "var A;B;C begin A:=12 # !B&9 ; for B:=A to 8 do begin C:=! 5 end end";

    std::cout << "Input Expression: ";
    //getline(std::cin, line);


    std::cout << parse2(tokenize(line));

    return 0;
}