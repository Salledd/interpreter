#pragma once
//#include "frwrd_decl_for_ast.hpp"
#include "ast.hpp"

struct ASTVisitor {
    virtual void visit(LiteralExpr& expr) = 0;
    virtual void visit(IdExpr& expr) = 0;
    virtual void visit(BinaryExpr& expr) = 0;
    virtual void visit(UnaryExpr& expr) = 0;
    virtual void visit(TernaryExpr& expr) = 0;
    virtual void visit(CallExpr& expr) = 0;
    virtual void visit(MemberAccessExpr& expr) = 0;
    virtual void visit(ArrayAccessExpr& expr) = 0;
    virtual void visit(LogicalExpr& expr) = 0;
    virtual void visit(PostfixExpr& expr) = 0;
    virtual void visit(AssignExpr& expr) = 0;
    virtual void visit(ArrayInitExpr& expr) = 0;

    virtual void visit(ExprStmt& stmt) = 0;
    virtual void visit(VarDecl& stmt) = 0;
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

    virtual void visit(StructDecl& decl) = 0;
    virtual void visit(FunctionDecl& decl) = 0;
    virtual void visit(AssertDecl& decl) = 0;
    virtual void visit(TypedefDecl& decl) = 0;

    virtual void visit(TranslationUnitNode& node) = 0;

    virtual ~ASTVisitor() = default;
};

struct PrintVisitor : ASTVisitor {
    void visit(LiteralExpr& expr) override;
    void visit(IdExpr& expr) override;
    void visit(BinaryExpr& expr) override;
    void visit(UnaryExpr& expr) override;
    void visit(TernaryExpr& expr) override;
    void visit(CallExpr& expr) override;
    void visit(MemberAccessExpr& expr) override;
    void visit(ArrayAccessExpr& expr) override;
    void visit(LogicalExpr& expr) override;
    void visit(PostfixExpr& expr) override;
    void visit(AssignExpr& expr) override;
    void visit(ArrayInitExpr& expr) override;

    void visit(ExprStmt& stmt) override;
    void visit(VarDecl& stmt) override;
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
    void visit(StructDecl& decl) override;
    void visit(FunctionDecl& decl) override;
    void visit(AssertDecl& decl) override;
    void visit(TypedefDecl& decl) override;

    void visit(TranslationUnitNode& node) override;
};