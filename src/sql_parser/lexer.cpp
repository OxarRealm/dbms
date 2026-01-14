/**
 * @file lexer.cpp
 * @brief SQL词法分析器实现
 */

#include "sql_parser/lexer.h"
#include <cctype>
#include <algorithm>

Lexer::Lexer() : m_position(0), m_line(1), m_column(1) {
}

Lexer::Lexer(const std::string& sql) : m_input(sql), m_position(0), m_line(1), m_column(1) {
}

void Lexer::setInput(const std::string& sql) {
    m_input = sql;
    m_position = 0;
    m_line = 1;
    m_column = 1;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    if (isEOF()) {
        return Token(TokenType::EOF_TOKEN, "", m_line, m_column);
    }
    
    char ch = peekChar();
    size_t startLine = m_line;
    size_t startColumn = m_column;
    
    // 标识符或关键词
    if (std::isalpha(ch) || ch == '_') {
        return readIdentifierOrKeyword();
    }
    
    // 数字
    if (std::isdigit(ch)) {
        return readNumber();
    }
    
    // 字符串字面量
    if (ch == '\'' || ch == '"') {
        return readStringLiteral();
    }
    
    // 运算符和分隔符
    switch (ch) {
        case '=':
            readChar();
            return Token(TokenType::EQUALS, "=", startLine, startColumn);
        case ',':
            readChar();
            return Token(TokenType::COMMA, ",", startLine, startColumn);
        case ';':
            readChar();
            return Token(TokenType::SEMICOLON, ";", startLine, startColumn);
        case '(':
            readChar();
            return Token(TokenType::LEFT_PAREN, "(", startLine, startColumn);
        case ')':
            readChar();
            return Token(TokenType::RIGHT_PAREN, ")", startLine, startColumn);
        case '*':
            readChar();
            return Token(TokenType::ASTERISK, "*", startLine, startColumn);
        default:
            readChar();
            return Token(TokenType::ERROR, std::string(1, ch), startLine, startColumn);
    }
}

Token Lexer::peekToken() {
    size_t savedPosition = m_position;
    size_t savedLine = m_line;
    size_t savedColumn = m_column;
    
    Token token = nextToken();
    
    m_position = savedPosition;
    m_line = savedLine;
    m_column = savedColumn;
    
    return token;
}

bool Lexer::hasMoreTokens() const {
    size_t pos = m_position;
    while (pos < m_input.length() && std::isspace(m_input[pos])) {
        pos++;
    }
    return pos < m_input.length();
}

size_t Lexer::getPosition() const {
    return m_position;
}

void Lexer::reset() {
    m_position = 0;
    m_line = 1;
    m_column = 1;
}

void Lexer::skipWhitespace() {
    while (!isEOF()) {
        char ch = peekChar();
        if (ch == '\n') {
            readChar();
            m_line++;
            m_column = 1;
        } else if (std::isspace(ch)) {
            readChar();
            m_column++;
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    // 暂时不支持注释，后续可以添加
}

Token Lexer::readIdentifierOrKeyword() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    std::string value;
    
    while (!isEOF()) {
        char ch = peekChar();
        if (std::isalnum(ch) || ch == '_') {
            value += readChar();
        } else {
            break;
        }
    }
    
    // 检查是否为关键词
    TokenType type = keywordToTokenType(value);
    if (type == TokenType::IDENTIFIER) {
        // 确实是标识符
        return Token(TokenType::IDENTIFIER, value, startLine, startColumn);
    } else {
        // 是关键词
        return Token(type, value, startLine, startColumn);
    }
}

Token Lexer::readNumber() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    std::string value;
    
    bool hasDot = false;
    while (!isEOF()) {
        char ch = peekChar();
        if (std::isdigit(ch)) {
            value += readChar();
        } else if (ch == '.' && !hasDot) {
            value += readChar();
            hasDot = true;
        } else {
            break;
        }
    }
    
    return Token(TokenType::NUMBER, value, startLine, startColumn);
}

Token Lexer::readStringLiteral() {
    size_t startLine = m_line;
    size_t startColumn = m_column;
    char quote = readChar();  // 读取开始引号
    std::string value;
    
    while (!isEOF()) {
        char ch = readChar();
        if (ch == quote) {
            // 结束引号
            break;
        } else if (ch == '\\' && !isEOF()) {
            // 转义字符
            char next = readChar();
            switch (next) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '\'': value += '\''; break;
                case '"': value += '"'; break;
                default: value += next; break;
            }
        } else {
            value += ch;
        }
    }
    
    return Token(TokenType::STRING_LITERAL, value, startLine, startColumn);
}

char Lexer::peekChar() const {
    if (isEOF()) {
        return '\0';
    }
    return m_input[m_position];
}

char Lexer::readChar() {
    if (isEOF()) {
        return '\0';
    }
    char ch = m_input[m_position];
    m_position++;
    m_column++;
    return ch;
}

bool Lexer::isEOF() const {
    return m_position >= m_input.length();
}

