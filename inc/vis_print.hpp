#pragma once
#include "visitor.hpp"

struct PrintVisitor : ASTVisitor {
    void visit(LiteralExpr& expr) override;
    void visit(IdExpr& expr) override;
    void visit(BinaryExpr& expr) override;
    void visit(UnaryExpr& expr) override;
    void visit(SizeofExpr& expr) override;
    void visit(TernaryExpr& expr) override;
    void visit(CallExpr& expr) override;
    void visit(MemberAccessExpr& expr) override;
    void visit(ArrayAccessExpr& expr) override;
    void visit(LogicalExpr& expr) override;
    void visit(PostfixExpr& expr) override;
    void visit(AssignExpr& expr) override;
    void visit(ArrayInitExpr& expr) override;

    void visit(ExprStmt& stmt) override;
    void visit(BlockStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(WhileStmt& stmt) override;
    void visit(ForStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;
    void visit(BreakStmt& stmt) override;
    void visit(ContinueStmt& stmt) override;
    void visit(PrintStmt& stmt) override;
    void visit(ReadStmt& stmt) override;
    void visit(ExitStmt& stmt) override;

    void visit(VarDecl& decl) override;
    void visit(StructDecl& decl) override;
    void visit(FunctionDecl& decl) override;
    void visit(AssertDecl& decl) override;
    void visit(TypedefDecl& decl) override;

    void visit(TranslationUnitNode& node) override;
};