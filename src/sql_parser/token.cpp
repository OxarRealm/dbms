/**
 * @file token.cpp
 * @brief Token实现
 */

#include "sql_parser/token.h"
#include <map>

std::string Token::getTypeName() const {
    switch (type) {
        case TokenType::CREATE: return "CREATE";
        case TokenType::TABLE: return "TABLE";
        case TokenType::EDIT: return "EDIT";
        case TokenType::RENAME: return "RENAME";
        case TokenType::DROP: return "DROP";
        case TokenType::INSERT: return "INSERT";
        case TokenType::INTO: return "INTO";
        case TokenType::DELETE: return "DELETE";
        case TokenType::FROM: return "FROM";
        case TokenType::UPDATE: return "UPDATE";
        case TokenType::SET: return "SET";
        case TokenType::SELECT: return "SELECT";
        case TokenType::WHERE: return "WHERE";
        case TokenType::VALUES: return "VALUES";
        case TokenType::IN: return "IN";
        case TokenType::INTO_KEYWORD: return "INTO";
        case TokenType::INT: return "INT";
        case TokenType::CHAR: return "CHAR";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::DOUBLE: return "DOUBLE";
        case TokenType::STRING: return "STRING";
        case TokenType::KEY: return "KEY";
        case TokenType::NOT_KEY: return "NOT_KEY";
        case TokenType::NULL_KEYWORD: return "NULL";
        case TokenType::NO_NULL: return "NO_NULL";
        case TokenType::VALID: return "VALID";
        case TokenType::INVALID: return "INVALID";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::COMMA: return "COMMA";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::ASTERISK: return "ASTERISK";
        case TokenType::DOT: return "DOT";
        case TokenType::JOIN: return "JOIN";
        case TokenType::ON: return "ON";
        case TokenType::INNER: return "INNER";
        case TokenType::LEFT: return "LEFT";
        case TokenType::RIGHT: return "RIGHT";
        case TokenType::FULL: return "FULL";
        case TokenType::OUTER: return "OUTER";
        case TokenType::NATURAL: return "NATURAL";
        case TokenType::ORDER: return "ORDER";
        case TokenType::BY: return "BY";
        case TokenType::ASC: return "ASC";
        case TokenType::DESC: return "DESC";
        case TokenType::DISTINCT: return "DISTINCT";
        case TokenType::LIMIT: return "LIMIT";
        case TokenType::GROUP: return "GROUP";
        case TokenType::HAVING: return "HAVING";
        case TokenType::COUNT: return "COUNT";
        case TokenType::SUM: return "SUM";
        case TokenType::AVG: return "AVG";
        case TokenType::MAX: return "MAX";
        case TokenType::MIN: return "MIN";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::LIKE: return "LIKE";
        case TokenType::BETWEEN: return "BETWEEN";
        case TokenType::UNION: return "UNION";
        case TokenType::ALL: return "ALL";
        case TokenType::EXISTS: return "EXISTS";
        case TokenType::NOT_EQUALS: return "NOT_EQUALS";
        case TokenType::GREATER_THAN: return "GREATER_THAN";
        case TokenType::LESS_THAN: return "LESS_THAN";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

bool Token::isKeyword() const {
    return type >= TokenType::CREATE && type <= TokenType::INVALID;
}

TokenType keywordToTokenType(const std::string& keyword) {
    static std::map<std::string, TokenType> keywordMap = {
        {"CREATE", TokenType::CREATE},
        {"TABLE", TokenType::TABLE},
        {"EDIT", TokenType::EDIT},
        {"RENAME", TokenType::RENAME},
        {"DROP", TokenType::DROP},
        {"INSERT", TokenType::INSERT},
        {"INTO", TokenType::INTO},
        {"DELETE", TokenType::DELETE},
        {"FROM", TokenType::FROM},
        {"UPDATE", TokenType::UPDATE},
        {"SET", TokenType::SET},
        {"SELECT", TokenType::SELECT},
        {"WHERE", TokenType::WHERE},
        {"VALUES", TokenType::VALUES},
        {"IN", TokenType::IN},
        {"INT", TokenType::INT},
        {"CHAR", TokenType::CHAR},
        {"FLOAT", TokenType::FLOAT},
        {"DOUBLE", TokenType::DOUBLE},
        {"STRING", TokenType::STRING},
        {"KEY", TokenType::KEY},
        {"NOT_KEY", TokenType::NOT_KEY},
        {"NULL", TokenType::NULL_KEYWORD},
        {"NO_NULL", TokenType::NO_NULL},
        {"VALID", TokenType::VALID},
        {"INVALID", TokenType::INVALID},
        {"JOIN", TokenType::JOIN},
        {"ON", TokenType::ON},
        {"INNER", TokenType::INNER},
        {"LEFT", TokenType::LEFT},
        {"RIGHT", TokenType::RIGHT},
        {"FULL", TokenType::FULL},
        {"OUTER", TokenType::OUTER},
        {"NATURAL", TokenType::NATURAL},
        {"ORDER", TokenType::ORDER},
        {"BY", TokenType::BY},
        {"ASC", TokenType::ASC},
        {"DESC", TokenType::DESC},
        {"DISTINCT", TokenType::DISTINCT},
        {"LIMIT", TokenType::LIMIT},
        {"GROUP", TokenType::GROUP},
        {"HAVING", TokenType::HAVING},
        {"COUNT", TokenType::COUNT},
        {"SUM", TokenType::SUM},
        {"AVG", TokenType::AVG},
        {"MAX", TokenType::MAX},
        {"MIN", TokenType::MIN},
        {"AND", TokenType::AND},
        {"OR", TokenType::OR},
        {"NOT", TokenType::NOT},
        {"LIKE", TokenType::LIKE},
        {"BETWEEN", TokenType::BETWEEN},
        {"UNION", TokenType::UNION},
        {"ALL", TokenType::ALL},
        {"EXISTS", TokenType::EXISTS}
    };
    
    // 转换为大写进行比较（关键词不区分大小写）
    std::string upperKeyword = keyword;
    for (char& c : upperKeyword) {
        if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
    }
    
    auto it = keywordMap.find(upperKeyword);
    if (it != keywordMap.end()) {
        return it->second;
    }
    
    return TokenType::IDENTIFIER;
}

