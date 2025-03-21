#pragma once
#include <memory>
#include <vector>
#include <string>

struct ASTNode {
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

struct Expr : ASTNode {};

struct LiteralExpr : Expr {
    std::string value;
    explicit LiteralExpr(const std::string& val) : value(val) {}
};

struct VariableExpr : Expr {
    std::string name;
    explicit VariableExpr(const std::string& n) : name(n) {}
};

struct BinaryExpr : Expr {
    std::string op;
    ASTNodePtr left;
    ASTNodePtr right;
    BinaryExpr(const std::string& o, ASTNodePtr l, ASTNodePtr r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
};

struct UnaryExpr : Expr {
    std::string op;
    ASTNodePtr operand;
    UnaryExpr(const std::string& o, ASTNodePtr e) : op(o), operand(std::move(e)) {}
};

struct TernaryExpr : Expr {
    ASTNodePtr cond;
    ASTNodePtr true_expr;
    ASTNodePtr false_expr;
    TernaryExpr(ASTNodePtr c, ASTNodePtr t, ASTNodePtr f)
        : cond(std::move(c)), true_expr(std::move(t)), false_expr(std::move(f)) {}
};

struct CallExpr : Expr {
    std::string callee;
    std::vector<ASTNodePtr> args;
    explicit CallExpr(const std::string& name) : callee(name) {}
};

struct MemberAccessExpr : Expr {
    ASTNodePtr object;
    std::string member;
    MemberAccessExpr(ASTNodePtr obj, const std::string& mem)
        : object(std::move(obj)), member(mem) {}
};

struct ArrayAccessExpr : Expr {
    ASTNodePtr array;
    ASTNodePtr index;
    ArrayAccessExpr(ASTNodePtr arr, ASTNodePtr idx)
        : array(std::move(arr)), index(std::move(idx)) {}
};


struct Stmt : ASTNode {};

struct ExprStmt : Stmt {
    ASTNodePtr expr;
    explicit ExprStmt(ASTNodePtr e) : expr(std::move(e)) {}
};

struct VarDeclStmt : Stmt {
    std::string type;
    std::string name;
    ASTNodePtr init;  // Может быть nullptr
};

struct BlockStmt : Stmt {
    std::vector<ASTNodePtr> statements;
};

struct IfStmt : Stmt {
    ASTNodePtr condition;
    ASTNodePtr then_branch;
    ASTNodePtr else_branch;  // может быть nullptr
};

struct WhileStmt : Stmt {
    ASTNodePtr condition;
    ASTNodePtr body;
};

struct ForStmt : Stmt {
    ASTNodePtr init;
    ASTNodePtr condition;
    ASTNodePtr increment;
    ASTNodePtr body;
};

struct ReturnStmt : Stmt {
    ASTNodePtr expr;  // может быть nullptr
};

struct BreakStmt : Stmt {};
struct ContinueStmt : Stmt {};

struct PrintStmt : Stmt {
    ASTNodePtr expr;
};

struct ReadStmt : Stmt {
    std::string var_name;
};

struct AssertStmt : Stmt {
    ASTNodePtr expr;
};

struct ExitStmt : Stmt {
    ASTNodePtr expr;  // может быть nullptr
};


struct Decl : ASTNode {};

struct StructDecl : Decl {
    std::string name;
    std::vector<VarDeclStmt> fields;
};

struct FunctionDecl : Decl {
    std::string return_type;
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;  // (type, name)
    ASTNodePtr body;  // BlockStmt
};

struct GlobalVarDecl : Decl {
    VarDeclStmt var;
};
