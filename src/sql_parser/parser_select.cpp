std::unique_ptr<ASTNode> Parser::parseSelect() {
    // SELECT * FROM TableName WHERE Field=Value;
    // SELECT Field1, Field2 FROM TableName WHERE Field=Value;
    // SELECT * FROM TableName1, TableName2 WHERE Condition;
    auto node = std::make_unique<SelectNode>();
    
    // 已经匹配了SELECT，跳过
    advance();
    
    // 解析SELECT字段列表
    if (m_currentToken.type == TokenType::ASTERISK) {
        // SELECT * 表示选择所有字段
        node->selectFields.push_back("*");
        advance();
    } else if (m_currentToken.type == TokenType::IDENTIFIER) {
        // 字段列表：Field1, Field2, ...
        while (true) {
            std::string fieldName = parseIdentifier();
            if (fieldName.empty()) {
                return nullptr;
            }
            node->selectFields.push_back(fieldName);
            
            if (m_currentToken.type == TokenType::COMMA) {
                advance();
            } else {
                break;
            }
        }
    } else {
        setError("期望字段名或*，但得到: " + m_currentToken.value);
        return nullptr;
    }
    
    // FROM
    if (!expect(TokenType::FROM, "FROM")) {
        return nullptr;
    }
    
    // 解析FROM表列表
    while (true) {
        std::string tableName = parseIdentifier();
        if (tableName.empty()) {
            return nullptr;
        }
        node->fromTables.push_back(tableName);
        
        if (m_currentToken.type == TokenType::COMMA) {
            advance();
        } else {
            break;
        }
    }
    
    // WHERE子句（可选）
    if (m_currentToken.type == TokenType::WHERE) {
        advance();
        
        // Field
        node->whereField = parseIdentifier();
        if (node->whereField.empty()) {
            return nullptr;
        }
        
        // 运算符（当前只支持=）
        if (m_currentToken.type == TokenType::EQUALS) {
            node->whereOperator = "=";
            advance();
        } else {
            setError("期望 =，但得到: " + m_currentToken.value);
            return nullptr;
        }
        
        // Value
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            node->whereValue = m_currentToken.value;
            advance();
        } else if (m_currentToken.type == TokenType::NUMBER) {
            node->whereValue = m_currentToken.value;
            advance();
        } else {
            setError("期望值（字符串或数字），但得到: " + m_currentToken.value);
            return nullptr;
        }
    }
    
    // ;
    if (!expect(TokenType::SEMICOLON, ";")) {
        return nullptr;
    }
    
    return node;
}

