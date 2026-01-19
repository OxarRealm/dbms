/**
 * @file parser_user_permission.cpp
 * @brief SQL解析器权限相关语句解析实现
 */

#include "sql_parser/parser.h"
#include <algorithm>
#include <cctype>

// CREATE USER username IDENTIFIED BY 'password';
std::unique_ptr<ASTNode> Parser::parseCreateUser() {
    if (!expect(TokenType::CREATE, "CREATE")) return nullptr;
    if (!expect(TokenType::USER, "USER")) return nullptr;
    
    auto node = std::make_unique<CreateUserNode>();
    
    // 用户名
    node->userName = parseIdentifier();
    if (node->userName.empty()) return nullptr;
    
    // IDENTIFIED BY
    if (!expect(TokenType::IDENTIFIED, "IDENTIFIED")) return nullptr;
    if (!expect(TokenType::BY, "BY")) return nullptr;
    
    // 密码（字符串字面量）
    if (m_currentToken.type != TokenType::STRING_LITERAL) {
        setError("Expected string literal for password, but got: " + m_currentToken.value);
        return nullptr;
    }
    
    // 移除字符串字面量的引号
    std::string password = m_currentToken.value;
    if (password.length() >= 2 && 
        ((password[0] == '\'' && password[password.length()-1] == '\'') ||
         (password[0] == '"' && password[password.length()-1] == '"'))) {
        password = password.substr(1, password.length() - 2);
    }
    node->password = password;
    advance();
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

// ALTER USER username IDENTIFIED BY 'newpassword';
// ALTER USER username ENABLE/DISABLE;
std::unique_ptr<ASTNode> Parser::parseAlterUser() {
    if (!expect(TokenType::ALTER, "ALTER")) return nullptr;
    if (!expect(TokenType::USER, "USER")) return nullptr;
    
    auto node = std::make_unique<AlterUserNode>();
    
    // 用户名
    node->userName = parseIdentifier();
    if (node->userName.empty()) return nullptr;
    
    // IDENTIFIED BY 或 ENABLE/DISABLE
    if (m_currentToken.type == TokenType::IDENTIFIED) {
        // 修改密码
        node->isPasswordChange = true;
        if (!expect(TokenType::IDENTIFIED, "IDENTIFIED")) return nullptr;
        if (!expect(TokenType::BY, "BY")) return nullptr;
        
        if (m_currentToken.type != TokenType::STRING_LITERAL) {
            setError("Expected string literal for password, but got: " + m_currentToken.value);
            return nullptr;
        }
        
        // 移除字符串字面量的引号
        std::string password = m_currentToken.value;
        if (password.length() >= 2 && 
            ((password[0] == '\'' && password[password.length()-1] == '\'') ||
             (password[0] == '"' && password[password.length()-1] == '"'))) {
            password = password.substr(1, password.length() - 2);
        }
        node->newPassword = password;
        advance();
    } else if (m_currentToken.type == TokenType::IDENTIFIER) {
        // ENABLE 或 DISABLE
        std::string action = m_currentToken.value;
        // 转换为大写
        for (char& c : action) {
            if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 'A';
            }
        }
        if (action == "ENABLE") {
            node->disable = false;
            advance();
        } else if (action == "DISABLE") {
            node->disable = true;
            advance();
        } else {
            setError("Expected IDENTIFIED BY, ENABLE, or DISABLE, but got: " + action);
            return nullptr;
        }
    } else {
        setError("Expected IDENTIFIED BY, ENABLE, or DISABLE, but got: " + m_currentToken.value);
        return nullptr;
    }
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

// DROP USER username;
std::unique_ptr<ASTNode> Parser::parseDropUser() {
    if (!expect(TokenType::DROP, "DROP")) return nullptr;
    if (!expect(TokenType::USER, "USER")) return nullptr;
    
    auto node = std::make_unique<DropUserNode>();
    
    node->userName = parseIdentifier();
    if (node->userName.empty()) return nullptr;
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

// CREATE ROLE role_name;
std::unique_ptr<ASTNode> Parser::parseCreateRole() {
    if (!expect(TokenType::CREATE, "CREATE")) return nullptr;
    if (!expect(TokenType::ROLE, "ROLE")) return nullptr;
    
    auto node = std::make_unique<CreateRoleNode>();
    
    node->roleName = parseIdentifier();
    if (node->roleName.empty()) return nullptr;
    
    // 角色描述（可选）
    // 暂时不支持，如果有需求可以后续添加
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

// DROP ROLE role_name;
std::unique_ptr<ASTNode> Parser::parseDropRole() {
    if (!expect(TokenType::DROP, "DROP")) return nullptr;
    if (!expect(TokenType::ROLE, "ROLE")) return nullptr;
    
    auto node = std::make_unique<DropRoleNode>();
    
    node->roleName = parseIdentifier();
    if (node->roleName.empty()) return nullptr;
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

// GRANT privilege_list ON object_type object_name TO username [WITH GRANT OPTION];
// GRANT role_name TO username;
std::unique_ptr<ASTNode> Parser::parseGrant() {
    if (!expect(TokenType::GRANT, "GRANT")) return nullptr;
    
    auto node = std::make_unique<GrantNode>();
    
    // 检查是GRANT ROLE还是GRANT PRIVILEGES
    // 如果下一个token是ROLE，则是角色授予（GRANT ROLE role_name TO username）
    // 如果下一个token是权限类型或ALL PRIVILEGES，则是权限授予
    // 如果下一个token是标识符，且再下一个token是TO（而不是ON），则可能是角色授予（GRANT role_name TO username）
    Token nextToken = m_lexer.peekToken();
    
    bool isRoleGrant = false;
    
    if (nextToken.type == TokenType::ROLE || 
        (nextToken.type == TokenType::IDENTIFIER && nextToken.value == "ROLE")) {
        // GRANT ROLE role_name TO username;
        isRoleGrant = true;
        advance();  // 跳过ROLE
        if (m_currentToken.type == TokenType::ROLE) {
            advance();
        }
        
        node->roleName = parseIdentifier();
        if (node->roleName.empty()) return nullptr;
    } else if (nextToken.type == TokenType::IDENTIFIER) {
        // 可能是 GRANT role_name TO username 或 GRANT privilege ON ...
        // 先peek下一个token来判断
        advance();  // 移动到标识符
        std::string firstIdent = m_currentToken.value;
        Token afterIdent = m_lexer.peekToken();
        
        // 关键判断：如果下一个token是TO，则是角色授予；否则是权限授予
        // 检查afterIdent的类型和值，确保正确识别TO关键字（大小写不敏感）
        std::string afterIdentValueUpper = afterIdent.value;
        std::transform(afterIdentValueUpper.begin(), afterIdentValueUpper.end(), 
                      afterIdentValueUpper.begin(), ::toupper);
        
        // 检查是否是TO关键字（类型或值匹配，大小写不敏感）
        // 注意：只要值是"TO"（不区分大小写），就认为是TO关键字，无论类型
        // 这样可以处理TO被识别为IDENTIFIER的情况
        // 重要：我们只检查值（转换为大写后），不检查类型，因为TO可能被识别为IDENTIFIER
        bool isTOKeyword = false;
        if (afterIdent.type == TokenType::TO) {
            isTOKeyword = true;
        } else if (afterIdentValueUpper == "TO") {
            // 即使类型不是TO，只要值是"TO"（不区分大小写），也认为是TO关键字
            isTOKeyword = true;
        }
        
        // 调试：如果afterIdent不是TO，但在后续解析中遇到TO，说明peekToken()可能有问题
        // 为了确保不会漏掉，我们再次检查afterIdent的值
        // 实际上，上面的检查应该已经足够了
        
        // 如果仍然不是TO，可能是peekToken()没有正确工作
        // 让我们再次检查afterIdent的值，确保不会漏掉
        // 实际上，上面的检查应该已经足够了
        // 如果afterIdentValueUpper == "TO"，isTOKeyword应该已经是true了
        
        if (isTOKeyword) {
            // GRANT role_name TO username; (没有ROLE关键字)
            isRoleGrant = true;
            node->roleName = firstIdent;
            // m_currentToken已经是角色名标识符，需要advance到TO
            advance();  // 跳过角色名，移动到TO
            // 现在m_currentToken应该是TO
        } else {
            // 这是权限授予，不是角色授予
            // m_currentToken已经是标识符（权限类型），继续解析权限列表
            isRoleGrant = false;
            // 但是，如果afterIdent不是TO，可能是peekToken()没有正确工作
            // 我们可以在权限列表为空时检查当前token是否是TO
            // 如果是，说明可能是角色授予，应该重新判断
            // 但是，由于firstIdent是局部变量，我们无法在else分支外访问
            // 所以，我们需要在权限列表为空时检查当前token是否是TO
            // 如果是，说明可能是角色授予，应该重新判断
        }
    } else {
        // 下一个token不是IDENTIFIER也不是ROLE，可能是权限关键字（SELECT等）
        // 这是权限授予
        isRoleGrant = false;
    }
    
    if (isRoleGrant) {
        // GRANT role_name TO username; 或 GRANT ROLE role_name TO username;
        node->isRoleGrant = true;
        // roleName已经在上面设置了
        // 如果是从IDENTIFIER识别出来的，已经advance到TO了
        // 如果是从ROLE识别出来的，parseIdentifier()已经advance到TO了（因为parseIdentifier内部会advance）
        // 所以现在m_currentToken应该是TO，直接继续处理TO
    } else {
        // GRANT privilege_list ON object_type object_name TO username [WITH GRANT OPTION];
        node->isRoleGrant = false;
        
        // 解析权限列表（如SELECT, INSERT或ALL PRIVILEGES）
        // 权限类型可以是关键字（SELECT, INSERT, UPDATE, DELETE等）或标识符（ALTER, DROP等）
        // 但是，如果当前token是TO，说明可能是角色授予语法，应该重新判断
        // 检查当前token是否是TO（可能是角色授予）
        // 注意：如果当前token是TO，说明peekToken()没有正确识别TO关键字
        // 这种情况下，我们无法获取角色名，所以只能报错
        bool isCurrentTokenTO = false;
        if (m_currentToken.type == TokenType::TO) {
            isCurrentTokenTO = true;
        } else if (m_currentToken.type == TokenType::IDENTIFIER) {
            std::string currentTokenUpper = m_currentToken.value;
            std::transform(currentTokenUpper.begin(), currentTokenUpper.end(), 
                          currentTokenUpper.begin(), ::toupper);
            if (currentTokenUpper == "TO") {
                isCurrentTokenTO = true;
            }
        }
        
        if (isCurrentTokenTO) {
            // 这是角色授予，不是权限授予
            // 但是roleName还没有设置，我们需要从之前的token获取
            // 由于我们已经advance了，我们无法回退获取角色名
            // 所以只能报错，提示这是一个解析器bug
            setError("Detected role grant syntax (GRANT role_name TO username), but 'TO' keyword was not recognized by peekToken() earlier. This is a parser bug. The 'TO' keyword should have been recognized when peeking the next token after the role name. Please check if the SQL statement is correct: GRANT role_name TO username;");
            return nullptr;
        }
        
        while (true) {
            std::string priv;
            bool isPrivilegeToken = false;
            
            // 检查是否是权限关键字
            if (m_currentToken.type == TokenType::SELECT) {
                priv = "SELECT";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::INSERT) {
                priv = "INSERT";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::UPDATE) {
                priv = "UPDATE";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::DELETE) {
                priv = "DELETE";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::ALL) {
                priv = "ALL";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::IDENTIFIER) {
                // 检查是否是TO关键字（可能是角色授予）
                std::string identUpper = m_currentToken.value;
                std::transform(identUpper.begin(), identUpper.end(), 
                              identUpper.begin(), ::toupper);
                if (identUpper == "TO") {
                    // 这是TO关键字，说明可能是角色授予
                    // 退出循环，让后续代码处理
                    break;
                }
                priv = m_currentToken.value;
                // 转换为大写
                for (char& c : priv) {
                    if (c >= 'a' && c <= 'z') {
                        c = c - 'a' + 'A';
                    }
                }
            } else {
                // 不是权限类型，退出循环
                break;
            }
            
            if (!priv.empty()) {
                node->privilegeTypes.push_back(priv);
                advance();
                
                // 检查是否是ALL PRIVILEGES（两个词）
                if (priv == "ALL") {
                    if (m_currentToken.type == TokenType::PRIVILEGES ||
                        (m_currentToken.type == TokenType::IDENTIFIER && 
                         m_currentToken.value == "PRIVILEGES")) {
                        if (m_currentToken.type == TokenType::PRIVILEGES) {
                            advance();
                        } else {
                            advance();  // 跳过PRIVILEGES标识符
                        }
                        // 将最后一个ALL替换为ALL_PRIVILEGES
                        node->privilegeTypes.back() = "ALL_PRIVILEGES";
                    }
                }
                
                if (m_currentToken.type == TokenType::COMMA) {
                    advance();
                    continue;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        
        if (node->privilegeTypes.empty()) {
            // 如果权限列表为空，且当前token是TO，说明可能是角色授予语法（GRANT role_name TO username）
            // 这种情况下，我们应该重新判断是否是角色授予
            // 检查当前token是否是TO（类型或值匹配，大小写不敏感）
            bool isCurrentTokenTO = false;
            if (m_currentToken.type == TokenType::TO) {
                isCurrentTokenTO = true;
            } else if (m_currentToken.type == TokenType::IDENTIFIER) {
                std::string currentTokenUpper = m_currentToken.value;
                std::transform(currentTokenUpper.begin(), currentTokenUpper.end(), 
                              currentTokenUpper.begin(), ::toupper);
                if (currentTokenUpper == "TO") {
                    isCurrentTokenTO = true;
                }
            }
            
            if (isCurrentTokenTO) {
                // 这是角色授予，不是权限授予
                // 但是roleName还没有设置，我们需要从之前的token获取
                // 由于我们已经advance了，我们无法回退获取角色名
                // 所以只能报错，提示这是一个解析器bug
                // 但是，我们可以尝试从m_currentToken之前的位置获取角色名
                // 实际上，更好的方法是：在之前就正确识别
                setError("Detected role grant syntax (GRANT role_name TO username), but 'TO' keyword was not recognized by peekToken() earlier. This is a parser bug. The 'TO' keyword should have been recognized when peeking the next token after the role name. Please check if the SQL statement is correct: GRANT role_name TO username;");
                return nullptr;
            }
            // 添加更详细的错误信息
            std::string errorMsg = "Expected privilege type (SELECT, INSERT, UPDATE, DELETE, ALL), but got: " + m_currentToken.value;
            errorMsg += ". Hint: If you want to grant a role, use: GRANT role_name TO username;";
            setError(errorMsg);
            return nullptr;
        }
        
        // ON
        if (!expect(TokenType::ON, "ON")) {
            // 添加更详细的错误信息
            // 检查当前token是否是TO（类型或值匹配，大小写不敏感）
            bool isCurrentTokenTO = false;
            if (m_currentToken.type == TokenType::TO) {
                isCurrentTokenTO = true;
            } else if (m_currentToken.type == TokenType::IDENTIFIER) {
                std::string currentTokenUpper = m_currentToken.value;
                std::transform(currentTokenUpper.begin(), currentTokenUpper.end(), 
                              currentTokenUpper.begin(), ::toupper);
                if (currentTokenUpper == "TO") {
                    isCurrentTokenTO = true;
                }
            }
            
            if (isCurrentTokenTO) {
                // 这种情况说明可能是角色授予语法，但被误判为权限授予
                // 添加调试信息：显示当前token和之前的标识符
                std::string debugMsg = "Expected ON (for privilege grant), but got: TO. ";
                debugMsg += "This is a role grant statement (GRANT role_name TO username), ";
                debugMsg += "but the parser failed to recognize 'TO' keyword when peeking the next token. ";
                debugMsg += "This indicates a parser bug. ";
                debugMsg += "The 'TO' keyword should have been recognized by peekToken() after the role name.";
                setError(debugMsg);
            }
            return nullptr;
        }
        
        // object_type (TABLE, DATABASE等)
        // TABLE和DATABASE都是关键字TokenType，需要特殊处理
        if (m_currentToken.type == TokenType::TABLE) {
            node->objectType = "TABLE";
            advance();
        } else if (m_currentToken.type == TokenType::DATABASE) {
            node->objectType = "DATABASE";
            advance();
        } else if (m_currentToken.type == TokenType::IDENTIFIER) {
            std::string objType = m_currentToken.value;
            // 转换为大写
            for (char& c : objType) {
                if (c >= 'a' && c <= 'z') {
                    c = c - 'a' + 'A';
                }
            }
            node->objectType = objType;
            advance();
        } else {
            setError("Expected object type (TABLE, DATABASE), but got: " + m_currentToken.value);
            return nullptr;
        }
        
        // object_name（表名或数据库名，如果是DATABASE级别则为"DATABASE"）
        if (node->objectType == "DATABASE") {
            // DATABASE级别权限，object_name为空或为数据库名
            // 如果下一个token是TO，则object_name为空
            if (m_currentToken.type == TokenType::TO) {
                node->objectName = "";
            } else {
                node->objectName = parseIdentifier();
                if (node->objectName.empty()) return nullptr;
            }
        } else {
            // TABLE级别权限，object_name为表名
            node->objectName = parseIdentifier();
            if (node->objectName.empty()) return nullptr;
        }
    }
    
    // TO
    if (!expect(TokenType::TO, "TO")) return nullptr;
    
    // username
    node->grantee = parseIdentifier();
    if (node->grantee.empty()) return nullptr;
    
    // [WITH GRANT OPTION]（可选，仅权限授予时有效）
    if (!node->isRoleGrant && m_currentToken.type == TokenType::WITH) {
        advance();  // 跳过WITH
        if (m_currentToken.type == TokenType::GRANT) {
            advance();  // 跳过GRANT
            if (m_currentToken.type == TokenType::OPTION) {
                advance();  // 跳过OPTION
                node->withGrantOption = true;
            } else {
                setError("Expected OPTION after GRANT, but got: " + m_currentToken.value);
                return nullptr;
            }
        } else {
            setError("Expected GRANT after WITH, but got: " + m_currentToken.value);
            return nullptr;
        }
    }
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}

// REVOKE privilege_list ON object_type object_name FROM username;
// REVOKE role_name FROM username;
std::unique_ptr<ASTNode> Parser::parseRevoke() {
    if (!expect(TokenType::REVOKE, "REVOKE")) return nullptr;
    
    auto node = std::make_unique<RevokeNode>();
    
    // 检查是REVOKE ROLE还是REVOKE PRIVILEGES
    // 支持两种格式：
    // 1. REVOKE ROLE role_name FROM username;
    // 2. REVOKE role_name FROM username; (不带ROLE关键字)
    Token nextToken = m_lexer.peekToken();
    
    bool isRoleRevoke = false;
    
    if (nextToken.type == TokenType::ROLE || 
        (nextToken.type == TokenType::IDENTIFIER && nextToken.value == "ROLE")) {
        // REVOKE ROLE role_name FROM username;
        isRoleRevoke = true;
        advance();  // 跳过ROLE
        if (m_currentToken.type == TokenType::ROLE) {
            advance();
        }
        
        node->roleName = parseIdentifier();
        if (node->roleName.empty()) return nullptr;
    } else if (nextToken.type == TokenType::IDENTIFIER) {
        // 可能是 REVOKE role_name FROM username 或 REVOKE privilege ON ...
        // 先peek下一个token来判断
        advance();  // 移动到标识符
        std::string firstIdent = m_currentToken.value;
        Token afterIdent = m_lexer.peekToken();
        
        // 关键判断：如果下一个token是FROM，则是角色撤销；否则是权限撤销
        // 检查afterIdent的类型和值，确保正确识别FROM关键字（大小写不敏感）
        std::string afterIdentValueUpper = afterIdent.value;
        std::transform(afterIdentValueUpper.begin(), afterIdentValueUpper.end(), 
                      afterIdentValueUpper.begin(), ::toupper);
        
        // 检查是否是FROM关键字（类型或值匹配，大小写不敏感）
        bool isFROMKeyword = false;
        if (afterIdent.type == TokenType::FROM) {
            isFROMKeyword = true;
        } else if (afterIdentValueUpper == "FROM") {
            // 即使类型不是FROM，只要值是"FROM"（不区分大小写），也认为是FROM关键字
            isFROMKeyword = true;
        }
        
        if (isFROMKeyword) {
            // REVOKE role_name FROM username; (没有ROLE关键字)
            isRoleRevoke = true;
            node->roleName = firstIdent;
            // m_currentToken已经是角色名标识符，需要advance到FROM
            advance();  // 跳过角色名，移动到FROM
            // 现在m_currentToken应该是FROM
        } else {
            // 这是权限撤销，不是角色撤销
            // m_currentToken已经是标识符（权限类型），继续解析权限列表
            isRoleRevoke = false;
        }
    } else {
        // 下一个token不是IDENTIFIER也不是ROLE，可能是权限关键字（SELECT等）
        // 这是权限撤销
        isRoleRevoke = false;
    }
    
    if (isRoleRevoke) {
        // REVOKE role_name FROM username; 或 REVOKE ROLE role_name FROM username;
        node->isRoleRevoke = true;
        // roleName已经在上面设置了
        // 如果是从IDENTIFIER识别出来的，已经advance到FROM了
        // 如果是从ROLE识别出来的，parseIdentifier()已经advance到FROM了（因为parseIdentifier内部会advance）
        // 所以现在m_currentToken应该是FROM，直接继续处理FROM
    } else {
        // REVOKE privilege_list ON object_type object_name FROM username;
        node->isRoleRevoke = false;
        
        // 解析权限列表（类似GRANT）
        // 权限类型可以是关键字（SELECT, INSERT, UPDATE, DELETE等）或标识符（ALTER, DROP等）
        while (true) {
            std::string priv;
            bool isPrivilegeToken = false;
            
            // 检查是否是权限关键字
            if (m_currentToken.type == TokenType::SELECT) {
                priv = "SELECT";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::INSERT) {
                priv = "INSERT";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::UPDATE) {
                priv = "UPDATE";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::DELETE) {
                priv = "DELETE";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::ALL) {
                priv = "ALL";
                isPrivilegeToken = true;
            } else if (m_currentToken.type == TokenType::IDENTIFIER) {
                priv = m_currentToken.value;
                // 转换为大写
                for (char& c : priv) {
                    if (c >= 'a' && c <= 'z') {
                        c = c - 'a' + 'A';
                    }
                }
            } else {
                // 不是权限类型，退出循环
                break;
            }
            
            if (!priv.empty()) {
                node->privilegeTypes.push_back(priv);
                advance();
                
                // 检查是否是ALL PRIVILEGES
                if (priv == "ALL") {
                    if (m_currentToken.type == TokenType::PRIVILEGES ||
                        (m_currentToken.type == TokenType::IDENTIFIER && 
                         m_currentToken.value == "PRIVILEGES")) {
                        if (m_currentToken.type == TokenType::PRIVILEGES) {
                            advance();
                        } else {
                            advance();
                        }
                        node->privilegeTypes.back() = "ALL_PRIVILEGES";
                    }
                }
                
                if (m_currentToken.type == TokenType::COMMA) {
                    advance();
                    continue;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        
        if (node->privilegeTypes.empty()) {
            // 如果权限列表为空，且当前token是FROM，说明可能是角色撤销语法（REVOKE role_name FROM username）
            // 这种情况下，我们应该重新判断是否是角色撤销
            // 检查当前token是否是FROM（类型或值匹配，大小写不敏感）
            bool isCurrentTokenFROM = false;
            if (m_currentToken.type == TokenType::FROM) {
                isCurrentTokenFROM = true;
            } else if (m_currentToken.type == TokenType::IDENTIFIER) {
                std::string currentTokenUpper = m_currentToken.value;
                std::transform(currentTokenUpper.begin(), currentTokenUpper.end(), 
                              currentTokenUpper.begin(), ::toupper);
                if (currentTokenUpper == "FROM") {
                    isCurrentTokenFROM = true;
                }
            }
            
            if (isCurrentTokenFROM) {
                // 这是角色撤销，不是权限撤销
                // 但是roleName还没有设置，我们需要从之前的token获取
                // 由于我们已经advance了，我们无法回退获取角色名
                // 所以只能报错，提示这是一个解析器bug
                setError("Detected role revoke syntax (REVOKE role_name FROM username), but 'FROM' keyword was not recognized by peekToken() earlier. This is a parser bug. The 'FROM' keyword should have been recognized when peeking the next token after the role name. Please check if the SQL statement is correct: REVOKE ROLE role_name FROM username; or REVOKE role_name FROM username;");
                return nullptr;
            }
            
            // 添加更详细的错误信息
            std::string errorMsg = "Expected privilege type (SELECT, INSERT, UPDATE, DELETE, ALL), but got: " + m_currentToken.value;
            errorMsg += ". Hint: If you want to revoke a role, use: REVOKE ROLE role_name FROM username; or REVOKE role_name FROM username;";
            setError(errorMsg);
            return nullptr;
        }
        
        // ON
        if (!expect(TokenType::ON, "ON")) {
            // 添加更详细的错误信息
            // 检查当前token是否是FROM（类型或值匹配，大小写不敏感）
            bool isCurrentTokenFROM = false;
            if (m_currentToken.type == TokenType::FROM) {
                isCurrentTokenFROM = true;
            } else if (m_currentToken.type == TokenType::IDENTIFIER) {
                std::string currentTokenUpper = m_currentToken.value;
                std::transform(currentTokenUpper.begin(), currentTokenUpper.end(), 
                              currentTokenUpper.begin(), ::toupper);
                if (currentTokenUpper == "FROM") {
                    isCurrentTokenFROM = true;
                }
            }
            
            if (isCurrentTokenFROM) {
                // 这种情况说明可能是角色撤销语法，但被误判为权限撤销
                std::string debugMsg = "Expected ON (for privilege revoke), but got: FROM. ";
                debugMsg += "This is a role revoke statement (REVOKE role_name FROM username), ";
                debugMsg += "but the parser failed to recognize 'FROM' keyword when peeking the next token. ";
                debugMsg += "This indicates a parser bug. ";
                debugMsg += "The 'FROM' keyword should have been recognized by peekToken() after the role name.";
                setError(debugMsg);
            }
            return nullptr;
        }
        
        // object_type
        // TABLE和DATABASE都是关键字TokenType，需要特殊处理
        if (m_currentToken.type == TokenType::TABLE) {
            node->objectType = "TABLE";
            advance();
        } else if (m_currentToken.type == TokenType::DATABASE) {
            node->objectType = "DATABASE";
            advance();
        } else if (m_currentToken.type == TokenType::IDENTIFIER) {
            std::string objType = m_currentToken.value;
            // 转换为大写
            for (char& c : objType) {
                if (c >= 'a' && c <= 'z') {
                    c = c - 'a' + 'A';
                }
            }
            node->objectType = objType;
            advance();
        } else {
            setError("Expected object type (TABLE, DATABASE), but got: " + m_currentToken.value);
            return nullptr;
        }
        
        // object_name
        if (node->objectType == "DATABASE") {
            if (m_currentToken.type == TokenType::FROM) {
                node->objectName = "";
            } else {
                node->objectName = parseIdentifier();
                if (node->objectName.empty()) return nullptr;
            }
        } else {
            node->objectName = parseIdentifier();
            if (node->objectName.empty()) return nullptr;
        }
    }
    
    // FROM
    if (!expect(TokenType::FROM, "FROM")) return nullptr;
    
    // username
    node->revokee = parseIdentifier();
    if (node->revokee.empty()) return nullptr;
    
    if (!expect(TokenType::SEMICOLON, ";")) return nullptr;
    
    return node;
}




