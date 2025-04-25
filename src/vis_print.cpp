#include "vis_print.hpp"
#include <iostream>
#include <memory>

void PrintVisitor::visit(LiteralExpr& expr)  { 
    std::cout << "Literal(";
    if(std::holds_alternative<int>(expr.value)) {
        std::cout << std::get<int>(expr.value) << ")";
    } else if (std::holds_alternative<double>(expr.value)) {
        std::cout << std::get<double>(expr.value) << ")";
    } else if (std::holds_alternative<bool>(expr.value)) {
        std::cout << (std::get<bool>(expr.value) ? "true" : "false") << ")";
    } else if (std::holds_alternative<char>(expr.value)) {
        std::cout << "'" << std::get<char>(expr.value) << "')";
    } else if (std::holds_alternative<std::string>(expr.value)) {
        std::cout << "\"" << std::get<std::string>(expr.value) << "\")";
    } else {
        std::cout << "Unknown type)";
    }
}

void PrintVisitor::visit(IdExpr& expr)  { 
    std::cout << "ID(" << expr.name << ")";
}

void PrintVisitor::visit(BinaryExpr& expr)  { 
    std::cout << "Binary(" << expr.op << ", ";
    expr.left.get()->accept(*this); 
    std::cout << ", ";
    expr.right.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(UnaryExpr& expr)  { 
    std::cout << "Unary(" << expr.op << ", ";
    expr.operand.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(SizeofExpr& expr)  { 
    std::cout << "Sizeof(";
    if (expr.type) {
        std::cout << expr.type->get_name() << ")";
    } else {
        expr.operand.get()->accept(*this); 
        std::cout << ")";
    }
}

void PrintVisitor::visit(TernaryExpr& expr)  { 
    std::cout << "Ternary(";
    expr.cond.get()->accept(*this); 
    std::cout << " ? ";
    expr.true_expr.get()->accept(*this); 
    std::cout << " : ";
    expr.false_expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(CallExpr& expr)  { 
    if (auto callee = dynamic_cast<IdExpr*>(expr.callee.get())) {
        std::cout << "Call(" << callee->name << ", [";
    } else {
        std::cout << "Call(" << "!null" << ", [";
    }
    for (auto& arg : expr.args) {
        arg.get()->accept(*this);
        if (&arg != &expr.args.back()) std::cout << ", ";
    }
    std::cout << "])";
}

void PrintVisitor::visit(MemberAccessExpr& expr)  { 
    std::cout << "Access(";
    expr.object.get()->accept(*this); 
    std::cout << "." << expr.member << ")";
}

void PrintVisitor::visit(ArrayAccessExpr& expr)  { 
    std::cout << "Array(";
    expr.array.get()->accept(*this); 
    std::cout << "[";
    expr.index.get()->accept(*this); 
    std::cout << "])";
}

void PrintVisitor::visit(LogicalExpr& expr)  { 
    std::cout << "Logical(" << expr.op << ", ";
    expr.left.get()->accept(*this); 
    std::cout << ", ";
    expr.right.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(PostfixExpr& expr)  { 
    std::cout << "Postfix(";
    expr.operand.get()->accept(*this); 
    std::cout << expr.op << ")";
}

void PrintVisitor::visit(AssignExpr& expr)  { 
    std::cout << "Assign(";
    expr.left.get()->accept(*this); 
    std::cout << " = ";
    expr.right.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(ExprStmt& stmt)  { 
    std::cout << "Expr(";
    stmt.expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(BlockStmt& stmt)  { 
    std::cout << "Block([";
    for (auto& statement : stmt.statements) {
        statement.get()->accept(*this);
        if (&statement != &stmt.statements.back()) std::cout << ", ";
    }
    std::cout << "])";
}

void PrintVisitor::visit(IfStmt& stmt)  { 
    std::cout << "If(";
    stmt.condition.get()->accept(*this); 
    std::cout << ", ";
    stmt.then_branch.get()->accept(*this); 
    if (stmt.else_branch) {
        std::cout << " else ";
        stmt.else_branch.get()->accept(*this); 
    }
    std::cout << ")";
}

void PrintVisitor::visit(WhileStmt& stmt)  { 
    std::cout << "While(";
    stmt.condition.get()->accept(*this); 
    std::cout << ", ";
    stmt.body.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(ForStmt& stmt)  { 
    std::cout << "For(";
    stmt.init.get()->accept(*this); 
    std::cout << "; ";
    stmt.condition.get()->accept(*this); 
    std::cout << "; ";
    stmt.increment.get()->accept(*this); 
    std::cout << ", ";
    stmt.body.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(ReturnStmt& stmt)  { 
    std::cout << "Return(";
    if (stmt.expr) stmt.expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(BreakStmt&)  { 
    std::cout << "Break";
}

void PrintVisitor::visit(ContinueStmt&)  { 
    std::cout << "Continue";
}

void PrintVisitor::visit(PrintStmt& stmt)  { 
    std::cout << "Print(";
    stmt.expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(ReadStmt& stmt)  { 
    std::cout << "Read(" << stmt.expr << ")";
}

void PrintVisitor::visit(ExitStmt& stmt)  { 
    std::cout << "Exit(";
    if (stmt.expr) stmt.expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(VarDecl& decl) {
    std::cout << "VarDecl(";
    for (const auto& mod : decl.modifiers.mods) {
        if (mod == Modifier::Const) std::cout << "Const";
        else if (mod == Modifier::Static) std::cout << "Static";
        else if (mod == Modifier::Unsigned) std::cout << "Unsigned";
        std::cout << " ";
    }
    std::cout << decl.type->get_name() << ", [";
    for (const auto& variable : decl.variables) {
        std::cout << variable.name;
        if (variable.size) {
            std::cout << "[";
            variable.size->accept(*this);
            std::cout << "]";
        }
        if (variable.init) {
            std::cout << " = ";
            variable.init->accept(*this);
        }
        if (&variable != &decl.variables.back()) std::cout << ", ";
    }    
    std::cout << "])";
}

void PrintVisitor::visit(StructDecl& decl)  { 
    std::cout << "Struct(" << decl.name << ", [";
    for (auto& field : decl.fields) {
        visit(field);
        if (&field != &decl.fields.back()) std::cout << ", ";
    }
    std::cout << "])";
}

void PrintVisitor::visit(FunctionDecl& decl)  { 
    std::cout << "Func(" << decl.return_type << " " << decl.name << ", [";
    for (auto& param : decl.params) {
        std::cout << param.first << " " << param.second;
        if (&param != &decl.params.back()) std::cout << ", ";
    }
    std::cout << "], ";
    decl.body.get()->accept(*this);
    std::cout << ")";
}

void PrintVisitor::visit(TranslationUnitNode& node)  {
    std::cout << "Program([" << std::endl;
    for (auto& stmt : node.statements) {
        stmt.get()->accept(*this);
        std::cout << std::endl;
    }
    std::cout << "])" << std::endl;
}

void PrintVisitor::visit(ArrayInitExpr& expr) {
    std::cout << "ArrayInit([";
    for (auto& element : expr.elements) {
        element->accept(*this);
        if (&element != &expr.elements.back()) std::cout << ", ";
    }
    std::cout << "])";
}

void PrintVisitor::visit(AssertDecl& decl) {
    std::cout << "Assert(";
    decl.expr->accept(*this);
    if (!decl.message.empty()) {
        std::cout << ", \"" << decl.message << "\"";
    }
    std::cout << ")";
}

void PrintVisitor::visit(TypedefDecl& decl) {
    std::cout << "Typedef(" << decl.original_type << " as " << decl.alias_name << ")";
}

#include "ast.hpp"

// Реализация методов accept
void LiteralExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void IdExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void BinaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void UnaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void SizeofExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void AssignExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void LogicalExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void PostfixExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void TernaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void CallExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void MemberAccessExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ArrayAccessExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ExprStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void BlockStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void IfStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void WhileStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ForStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ReturnStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void BreakStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ContinueStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void PrintStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ReadStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void AssertDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ExitStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void VarDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void StructDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void FunctionDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void TranslationUnitNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ArrayInitExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void TypedefDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }

