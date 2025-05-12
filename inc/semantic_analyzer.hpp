#pragma once
#include "symbol_table.hpp"
#include "visitor.hpp"

class SemanticAnalyzer {
public:
    SemanticAnalyzer();

    void analyze(std::shared_ptr<TranslationUnitNode> root);

private:
    SymbolTable symbolTable;

    void checkVariableDeclarations(std::shared_ptr<Decl> decl);
    void checkFunctionCalls(std::shared_ptr<Expr> expr);
    void checkControlFlow(std::shared_ptr<Stmt> stmt);
    void checkTypeCompatibility(const std::string& expected, const std::string& actual);
};