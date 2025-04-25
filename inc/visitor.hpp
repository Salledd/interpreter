#pragma once
#include "ast.hpp"

struct ASTVisitor {
    virtual void visit(LiteralExpr& expr) = 0;
    virtual void visit(IdExpr& expr) = 0;
    virtual void visit(BinaryExpr& expr) = 0;
    virtual void visit(UnaryExpr& expr) = 0;
    virtual void visit(SizeofExpr& expr) = 0;
    virtual void visit(TernaryExpr& expr) = 0;
    virtual void visit(CallExpr& expr) = 0;
    virtual void visit(MemberAccessExpr& expr) = 0;
    virtual void visit(ArrayAccessExpr& expr) = 0;
    virtual void visit(LogicalExpr& expr) = 0;
    virtual void visit(PostfixExpr& expr) = 0;
    virtual void visit(AssignExpr& expr) = 0;
    virtual void visit(ArrayInitExpr& expr) = 0;

    virtual void visit(ExprStmt& stmt) = 0;
    virtual void visit(BlockStmt& stmt) = 0;
    virtual void visit(IfStmt& stmt) = 0;
    virtual void visit(WhileStmt& stmt) = 0;
    virtual void visit(ForStmt& stmt) = 0;
    virtual void visit(ReturnStmt& stmt) = 0;
    virtual void visit(BreakStmt& stmt) = 0;
    virtual void visit(ContinueStmt& stmt) = 0;
    virtual void visit(PrintStmt& stmt) = 0;
    virtual void visit(ReadStmt& stmt) = 0;
    virtual void visit(ExitStmt& stmt) = 0;

    virtual void visit(VarDecl& decl) = 0;
    virtual void visit(StructDecl& decl) = 0;
    virtual void visit(FunctionDecl& decl) = 0;
    virtual void visit(AssertDecl& decl) = 0;
    virtual void visit(TypedefDecl& decl) = 0;

    virtual void visit(TranslationUnitNode& node) = 0;

    virtual ~ASTVisitor() = default;
};

