#pragma once
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <variant>

#include "token.hpp"
#include "types.hpp"
#include "modifiers.hpp"

struct ASTVisitor;


struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0; // Метод для посещения узла
};

typedef std::shared_ptr<ASTNode> ASTNodePtr;

struct Expr : ASTNode {
    virtual ~Expr() = default;
    void accept(ASTVisitor& visitor) override = 0; // Метод для посещения выражения
};
typedef std::shared_ptr<Expr> ExprPtr;

struct Stmt : ASTNode {
    virtual ~Stmt() = default;
    void accept(ASTVisitor& visitor) override = 0; // Метод для посещения инструкции
};
typedef std::shared_ptr<Stmt> StmtPtr;

struct Decl : ASTNode {
    virtual ~Decl() = default;
    void accept(ASTVisitor& visitor) override = 0; // Метод для посещения объявления
};
typedef std::shared_ptr<Decl> DeclPtr;

// Литеральное выражение
struct LiteralExpr : Expr {
    std::variant<bool, char, short, int, long, float, double, long double, std::string> value;
    explicit LiteralExpr(std::variant<bool, char, short, int, long, float, double, long double, std::string> val) : value(val) {}
    void accept(ASTVisitor& visitor) override;
};

// Выражение переменной
struct IdExpr : Expr {
    std::string name;
    explicit IdExpr(const std::string& n) : name(n) {}
    void accept(ASTVisitor& visitor) override;
};

// Унарное выражение
struct UnaryExpr : Expr {
    std::string op;
    ExprPtr operand;
    UnaryExpr(const std::string& op, ExprPtr operand) : op(op), operand(std::move(operand)) {}
    void accept(ASTVisitor& visitor) override;

    ~UnaryExpr() = default;
};


// Постфиксное выражение
struct PostfixExpr : UnaryExpr {
    PostfixExpr(ExprPtr e, const std::string& o) : UnaryExpr(op, operand) {}
    void accept(ASTVisitor& visitor) override;
};

// Бинарное выражение
struct BinaryExpr : Expr {
    std::string op;
    ExprPtr left;
    ExprPtr right;
    BinaryExpr(const std::string& o, ExprPtr l, ExprPtr r) : op(o), left(std::move(l)), right(std::move(r)) {}
    void accept(ASTVisitor& visitor) override;
};

// Выражение присваивания
struct AssignExpr : BinaryExpr {
    AssignExpr(const std::string& o, ExprPtr l, ExprPtr r) : BinaryExpr(o , l, r) {}
    void accept(ASTVisitor& visitor) override;
};

// Логическое выражение
struct LogicalExpr : BinaryExpr {
    LogicalExpr(const std::string& o, ExprPtr l, ExprPtr r) : BinaryExpr(o, l, r) {}
    void accept(ASTVisitor& visitor) override;
};

typedef std::shared_ptr<Type> TypePtr;

struct SizeofExpr : UnaryExpr {
    std::variant<TypePtr, ExprPtr> type;
    SizeofExpr(ExprPtr operand, std::variant<TypePtr, ExprPtr> type) : UnaryExpr("sizeof", operand), type(type) {}
    void accept(ASTVisitor& visitor) override;
};

// Тернарное выражение
struct TernaryExpr : Expr {
    ExprPtr cond;
    ExprPtr true_expr;
    ExprPtr false_expr;
    TernaryExpr(ExprPtr c, ExprPtr t, ExprPtr f) : cond(std::move(c)), true_expr(std::move(t)), false_expr(std::move(f)) {}
    void accept(ASTVisitor& visitor) override;
};

// Вызов функции
struct CallExpr : Expr {
    ExprPtr callee;
    std::vector<ExprPtr> args;
    explicit CallExpr(ExprPtr name, std::vector<ExprPtr> args) : callee(name), args(args) {}
    void accept(ASTVisitor& visitor) override;
};

// Доступ к члену объекта
struct MemberAccessExpr : Expr {
    ExprPtr object;
    ExprPtr member;
    MemberAccessExpr(ExprPtr obj, ExprPtr mem) : object(std::move(obj)), member(std::move(mem)) {}
    void accept(ASTVisitor& visitor) override;
};

// Доступ к элементу массива
struct ArrayAccessExpr : Expr {
    ExprPtr array;
    ExprPtr index;
    ArrayAccessExpr(ExprPtr arr, ExprPtr idx) : array(std::move(arr)), index(std::move(idx)) {}
    void accept(ASTVisitor& visitor) override;
};

// Узел для приведения типов
struct CastExpr : Expr {
    TypePtr type;
    ExprPtr operand;
    CastExpr(TypePtr t, ExprPtr o) : type(t), operand(std::move(o)) {}
    void accept(ASTVisitor& visitor) override;
};

// Инициализация массива
struct ArrayInitExpr : Expr {
    std::vector<ExprPtr> elements;
    ArrayInitExpr(std::vector<ExprPtr> elems) : elements(std::move(elems)) {}
    void accept(ASTVisitor& visitor) override;
};

// Инструкция-выражение
struct ExprStmt : Stmt {
    ASTNodePtr expr; 
    explicit ExprStmt(ASTNodePtr e) : expr(std::move(e)) {}
    void accept(ASTVisitor& visitor) override;
};

// Блок инструкций
struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
    void accept(ASTVisitor& visitor) override;
};

// Условная инструкция
struct IfStmt : Stmt {
    ExprPtr condition;
    StmtPtr then_branch;
    StmtPtr else_branch;
    IfStmt(ExprPtr cond, StmtPtr then_b, StmtPtr else_b = nullptr) : condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}
    void accept(ASTVisitor& visitor) override;
};

// Цикл while
struct WhileStmt : Stmt {
    ExprPtr condition;
    StmtPtr body;
    WhileStmt(ExprPtr cond, StmtPtr b) : condition(std::move(cond)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override;
};

// Цикл for
struct ForStmt : Stmt {
    DeclPtr init;
    ExprPtr condition;
    ExprPtr increment;
    StmtPtr body;
    ForStmt(DeclPtr i, ExprPtr cond, ExprPtr inc, StmtPtr b) : init(std::move(i)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override;
};

// Инструкция возврата
struct ReturnStmt : Stmt {
    ExprPtr expr;
    ReturnStmt(ExprPtr e = nullptr) : expr(std::move(e)) {}
    void accept(ASTVisitor& visitor) override;
};

// Инструкция break
struct BreakStmt : Stmt {
    void accept(ASTVisitor& visitor) override;
};

// Инструкция continue
struct ContinueStmt : Stmt {
    void accept(ASTVisitor& visitor) override;
};

// Инструкция вывода
struct PrintStmt : Stmt {
    std::vector<ExprPtr> expr;
    PrintStmt(const std::vector<ExprPtr>& e) : expr(std::move(e)) {}
    void accept(ASTVisitor& visitor) override;
};

// Инструкция ввода
struct ReadStmt : Stmt {
    ExprPtr expr;
    ReadStmt(ExprPtr name) : expr(std::move(name)) {}
    void accept(ASTVisitor& visitor) override;
};

// Инструкция выхода
struct ExitStmt : Stmt {
    ExprPtr expr;
    ExitStmt(ExprPtr e) : expr(std::move(e)) {}
    void accept(ASTVisitor& visitor) override;
};

struct Variable {
    std::string name;       
    ExprPtr init;           
    ExprPtr size;           
    bool is_array;

    Variable(const std::string& name, ExprPtr init = nullptr, ExprPtr size = nullptr, bool is_array = false) :
        name(name), init(std::move(init)), size(std::move(size)), is_array(is_array) {}
};

// Декларация переменной
struct VarDecl : Decl {
    TypePtr type;
    std::vector<Variable> variables; // name, init, size
    Modifiers modifiers;
    VarDecl(const TypePtr type, std::vector<Variable> variables, const Modifiers& mods) : 
        type(type), variables(std::move(variables)), modifiers(mods) {}
    void accept(ASTVisitor& visitor) override;
};

// Декларация typedef
struct TypedefDecl : Decl {
    Modifiers original_modifiers;
    TypePtr original_type;
    std::string alias_name;     //псевдоним

    TypedefDecl(const Modifiers& orig_mods, const TypePtr& orig_types, const std::string& alias) :
        original_modifiers(orig_mods), original_type(orig_types), alias_name(alias) {}

    void accept(ASTVisitor& visitor) override;
};

// Декларация структуры
struct StructDecl : Decl {
    std::string name;
    std::vector<VarDecl> fields;    // мб Decl
    StructDecl(const std::string& n, const std::vector<VarDecl>& f) : name(n), fields(f) {}
    void accept(ASTVisitor& visitor) override;
};

// Декларация функции
struct FunctionDecl : Decl {
    Modifiers return_mods;
    TypePtr return_type;    // enum и строка
    std::string name;
    std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>> params;    // param_decl
    std::shared_ptr<BlockStmt> body;
    FunctionDecl(const TypePtr rt, const std::string& n, const std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>>& p, std::shared_ptr<BlockStmt> b = nullptr) : 
        return_type(rt), name(n), params(p), body(std::move(b)) {}
    void accept(ASTVisitor& visitor) override;
};

// оператор assert
struct AssertDecl : Decl {
    ExprPtr expr;       
    std::string message; 
    AssertDecl(ExprPtr e, std::string m) : expr(std::move(e)), message(m) {}
    void accept(ASTVisitor& visitor) override;
};

// Узел корня AST
struct TranslationUnitNode : ASTNode {
    std::vector<DeclPtr> decls; 
    TranslationUnitNode() = default;
    TranslationUnitNode(std::vector<DeclPtr> decls) : decls(std::move(decls)) {}
    void accept(ASTVisitor& visitor) override;
};


