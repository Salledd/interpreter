#include "semantic_analyzer.hpp"
#include "semantic_visitor.hpp"
#include "symbol_table.hpp"
#include "ast.hpp"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer() : symbolTable(std::make_shared<SymbolTable>()) {}

void SemanticAnalyzer::analyze(std::shared_ptr<TranslationUnitNode> root) {
    ScopeVisitor scopeVisitor(symbolTable);
    root->accept(scopeVisitor); // Create scopes and populate symbol table

    SemanticVisitor semanticVisitor(symbolTable);
    root->accept(semanticVisitor); // Perform semantic checks
}
