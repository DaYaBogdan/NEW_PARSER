#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <string>

enum Relation { LESS, EQUAL, GREATER, NONE };

struct workSteckToken
{
    std::string token;
    Relation relation;
};

std::map<std::string, std::map<std::string, Relation>> precedenceTable =
{
    {"var", {{"id", LESS}, {";", NONE}, {"begin", LESS}, {"end", NONE}, {"for", NONE},
             {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", GREATER}}},

    {"id", {{"id", NONE}, {";", EQUAL}, {"begin", GREATER}, {"end", GREATER}, {"for", NONE},
            {"to", GREATER}, {"do", EQUAL}, {":=", EQUAL}, {"#", EQUAL}, {"!", NONE}, {"&", EQUAL}, {"$", NONE}}},

    {";", {{"id", EQUAL}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", GREATER},
           {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"begin", {{"id", LESS}, {";", NONE}, {"begin", NONE}, {"end", EQUAL}, {"for", LESS},
               {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"end", {{"id", NONE}, {";", NONE}, {"begin", NONE}, {"end", GREATER}, {"for", NONE},
             {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", GREATER}}},

    {"for", {{"id", LESS}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", NONE},
             {"to", EQUAL}, {"do", EQUAL}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"to", {{"id", EQUAL}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", NONE},
            {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"do", {{"id", NONE}, {";", NONE}, {"begin", LESS}, {"end", GREATER}, {"for", NONE},
            {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {":=", {{"id", EQUAL}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", NONE},
            {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", EQUAL}, {"&", NONE}, {"$", NONE}}},

    {"#", {{"id", EQUAL}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", NONE},
           {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", EQUAL}, {"&", NONE}, {"$", NONE}}},

    {"!", {{"id", EQUAL}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", NONE},
           {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"&", {{"id", EQUAL}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", NONE},
           {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}},

    {"$", {{"var", LESS}, {"id", NONE}, {";", NONE}, {"begin", NONE}, {"end", NONE}, {"for", NONE},
           {"to", NONE}, {"do", NONE}, {":=", NONE}, {"#", NONE}, {"!", NONE}, {"&", NONE}, {"$", NONE}}}
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
    std::stack<workSteckToken> parseStack;
    parseStack.push({ "$", NONE });

    size_t i = 0;
    while (i < tokens.size()) {

        workSteckToken current;
        current.token = tokens[i];

        //Visual part
        std::cout << "Current token: " << current.token << "\n";

        if (parseStack.empty()) {
            std::cout << "Error: stack is empty before processing token '" << current.token << "'\n";
            return false;
        }

        //Tokens end detection
        workSteckToken top = parseStack.top();
        if (current.token == "$" && top.token == "$") {
            std::cout << "End of parsing detected. Successful parsing.\n";
            return true;
        }

        //Relation identifier
        current.relation = getRelation(top.token, current.token);
        std::cout << "Relation between '" << top.token << "' and '" << current.token << "': "
            << (current.relation == LESS ? "<" : current.relation == GREATER ? ">" : current.relation == EQUAL ? "=" : "NONE") << "\n";

        //Working block
        if (current.relation == LESS || current.relation == EQUAL) {
            parseStack.push(current);
            i++;
            std::cout << "Pushed '" << current.token << "' to stack\n";
        }
        else if (current.relation == GREATER) 
        {
            //Deleting GREATER token
            std::cout << "Popped '" << parseStack.top().token << "' from stack\n";
            parseStack.pop();

            //Deleting all EQUAL tokens
            while (!parseStack.empty() && parseStack.top().relation == EQUAL)
            {
                if (parseStack.empty())
                {
                    std::cout << "Error: can't reduce - stack empty\n";
                    return false;
                }
                std::cout << "Popped '" << parseStack.top().token << "' from stack\n";
                parseStack.pop();
            }

            //Deleting LESS token
            if(parseStack.top().relation == LESS)
            {
                std::cout << "Popped '" << parseStack.top().token << "' from stack\n";
                parseStack.pop();
            }
        }
        else //Relation defined as NONE
        {
            std::cout << "Syntax Error: no valid relation between '" << top.token
                << "' and '" << current.token << "'\n";
            return false;
        }
    }

    //Searching for errors
    while (!parseStack.empty() && parseStack.top().token != "$") 
    {
        std::cout << "Final reduction: popped '" << parseStack.top().token << "'\n";
        parseStack.pop();
    }

    return parseStack.size() == 1 && parseStack.top().token == "$";
}

int main()
{
    std::string line = "var A;B;C begin A:=12 # !B&9 ; for B:=A to 8 do begin C:=!5 end end";

    std::cout << "Input Expression: ";
    //getline(std::cin, line);

    std::cout << parse2(tokenize(line));

    return 0;
}