#include "lexicalanalyzer.h"

LexicalAnalyzer::LexicalAnalyzer()
{

}

std::vector<Token*> LexicalAnalyzer::analyze(std::ifstream &stream, std::map<std::string, int> *keyword, std::map<std::string, int> *identifier, \
                                             std::map<std::string, int> *constant, std::vector<std::string> *error)
{
    std::vector<Token*> tokens;
    int x = 1, y = 1;

    Symbol symbol;
    std::string buff;
    int lexCode;
    bool SuppressOutput = false;

    symbol = gets(stream);

    while (!stream.eof()) {
        buff.clear();
        lexCode = 0;
        SuppressOutput = false;

        switch ((int)symbol.attr) {
        case Whitespace: {
            while (!stream.eof() && symbol.attr == Whitespace) {
                if (symbol.value == '\n') {
                    x = 1;
                    y++;
                } else { x++; }
                symbol = gets(stream);
            }
            SuppressOutput = true;
            break;
        }
        case Constant: {
            while (!stream.eof() && symbol.attr == Constant) {
                buff += symbol.value;
                symbol = gets(stream);
            }
            if (search(constant, buff) >= 0) {
                lexCode = constant->at(buff);
            } else {
                lexCode = append(constant, constantsBegin, buff);
            }
            break;
        }
        case Identifier: {
            while (!stream.eof() && (symbol.attr == Identifier || symbol.attr == Constant)) {
                buff += symbol.value;
                symbol = gets(stream);
            }
            if (search(keyword, buff) >= 0) {
                lexCode = keyword->at(buff);
            } else if (search(identifier, buff) >= 0) {
                lexCode = identifier->at(buff);
            } else {
                lexCode = append(identifier, identifiersBegin, buff);
            }
            break;
        }
        case Comment: {
            if (stream.eof()) {
                lexCode = symbol.value;
                buff += symbol.value;
                break;
            }

            symbol = gets(stream);
            if (symbol.value == '*') {
                int x1 = ++x, y1 = y;

                if (stream.eof()) {
                    SuppressOutput = true;
                    error->push_back("Lexer: Error (line " + std::to_string(y1) + ", column " + std::to_string(x1) \
                                     + "): *) expected but end of file found");
                    break;
                }

                x++;
                symbol = gets(stream);
                do {
                    while (!stream.eof() && (symbol.value != '*' || symbol.attr == Whitespace)) {
                        if (symbol.value == '\n') {
                            x = 1;
                            y++;
                        } else { x++; }
                        symbol = gets(stream);
                    }
                    if (stream.eof()) {
                        SuppressOutput = true;
                        error->push_back("Lexer: Error (line " + std::to_string(y1) + ", column " + std::to_string(x1) \
                                         + "): *) expected but end of file found");
                        break;
                    }

                    x++;
                    symbol = gets(stream);
                } while (symbol.value != ')');

                if (symbol.value == ')') {
                    x++;
                    SuppressOutput = true;
                }
            } else {
                tokens.push_back(new Token({ "(", '(', x++, y }));
                buff += symbol.value;
                lexCode = symbol.value;
            }
            if (!stream.eof()) {
                symbol = gets(stream);
            }
            break;
        }
        case Separator: {
            lexCode = symbol.value;
            buff += symbol.value;
            symbol = gets(stream);
            break;
        }
        case Invalid: {
            symbol = gets(stream);
            break;
        }
        }
        if (!SuppressOutput) {
            tokens.push_back(new Token({ buff, lexCode, x, y }));
            x += buff.size();
        }
    }

    return tokens;
}

LexicalAnalyzer::Symbol LexicalAnalyzer::gets(std::ifstream &stream)
{
    Symbol symbol;
    stream.get(symbol.value);
    symbol.attr = (Attributes)m_Attributes[(int)symbol.value];

    return symbol;
}

int LexicalAnalyzer::search(std::map<std::string,int> *table, std::string &token)
{
    if (table->find(token) == table->end()) {
        return -1;
    }
    return table->at(token);
}

int LexicalAnalyzer::append(std::map<std::string,int> *table, int RangeBegin, std::string token)
{
    if (search(table, token) >= 0) {
        return table->at(token);
    }
    table->insert(std::pair<std::string,int>(token, RangeBegin + table->size()));
    return table->at(token);
}