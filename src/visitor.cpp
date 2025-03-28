#include <iostream>
#include "visitor.hpp"

void PrintVisitor::visit(LiteralExpr& expr)  { 
    std::cout << "LiteralExpr(value=" << expr.value << ")";
}

void PrintVisitor::visit(VariableExpr& expr)  { 
    std::cout << "VariableExpr(name=" << expr.name << ")";
}

void PrintVisitor::visit(BinaryExpr& expr)  { 
    std::cout << "BinaryExpr(op=" << expr.op << ", left=";
    expr.left.get()->accept(*this); 
    std::cout << ", right=";
    expr.right.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(UnaryExpr& expr)  { 
    std::cout << "UnaryExpr(op=" << expr.op << ", operand=";
    expr.operand.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(TernaryExpr& expr)  { 
    std::cout << "TernaryExpr(cond=";
    expr.cond.get()->accept(*this); 
    std::cout << ", true_expr=";
    expr.true_expr.get()->accept(*this); 
    std::cout << ", false_expr=";
    expr.false_expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(CallExpr& expr)  { 
    std::cout << "CallExpr(callee=" << expr.callee << ", args=[";
    for (auto& arg : expr.args) {
        arg.get()->accept(*this);
        if (&arg != &expr.args.back()) std::cout << ", ";
    }
    std::cout << "])";
}

void PrintVisitor::visit(MemberAccessExpr& expr)  { 
    std::cout << "MemberAccessExpr(object=";
    expr.object.get()->accept(*this); 
    std::cout << ", member=" << expr.member << ")";
}

void PrintVisitor::visit(ArrayAccessExpr& expr)  { 
    std::cout << "ArrayAccessExpr(array=";
    expr.array.get()->accept(*this); 
    std::cout << ", index=";
    expr.index.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(LogicalExpr& expr)  { 
    std::cout << "LogicalExpr(op=" << expr.op << ", left=";
    expr.left.get()->accept(*this); 
    std::cout << ", right=";
    expr.right.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(PostfixExpr& expr)  { 
    std::cout << "PostfixExpr(op=" << expr.op << ", operand=";
    expr.operand.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(AssignExpr& expr)  { 
    std::cout << "AssignExpr(left=";
    expr.left.get()->accept(*this); 
    std::cout << ", right=";
    expr.right.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(ExprStmt& stmt)  { 
    std::cout << "ExprStmt(expr=";
    stmt.expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(VarDecl& stmt)  { 
    std::cout << "VarDeclStmt(type=" << stmt.type << ", name=" << stmt.name;
    if (stmt.init) {
        std::cout << ", init=";
        stmt.init.get()->accept(*this);
    }
    std::cout << ")";
}

void PrintVisitor::visit(BlockStmt& stmt)  { 
    std::cout << "BlockStmt(statements=[";
    for (auto& statement : stmt.statements) {
        statement.get()->accept(*this);
        if (&statement != &stmt.statements.back()) std::cout << ", ";
    }
    std::cout << "])";
}

void PrintVisitor::visit(IfStmt& stmt)  { 
    std::cout << "IfStmt(condition=";
    stmt.condition.get()->accept(*this); 
    std::cout << ", then_branch=";
    stmt.then_branch.get()->accept(*this); 
    if (stmt.else_branch) {
        std::cout << ", else_branch=";
        stmt.else_branch.get()->accept(*this); 
    }
    std::cout << ")";
}

void PrintVisitor::visit(WhileStmt& stmt)  { 
    std::cout << "WhileStmt(condition=";
    stmt.condition.get()->accept(*this); 
    std::cout << ", body=";
    stmt.body.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(ForStmt& stmt)  { 
    std::cout << "ForStmt(init=";
    stmt.init.get()->accept(*this); 
    std::cout << ", condition=";
    stmt.condition.get()->accept(*this); 
    std::cout << ", increment=";
    stmt.increment.get()->accept(*this); 
    std::cout << ", body=";
    stmt.body.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(ReturnStmt& stmt)  { 
    std::cout << "ReturnStmt(expr=";
    if (stmt.expr) stmt.expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(BreakStmt&)  { 
    std::cout << "BreakStmt()";
}

void PrintVisitor::visit(ContinueStmt&)  { 
    std::cout << "ContinueStmt()";
}

void PrintVisitor::visit(PrintStmt& stmt)  { 
    std::cout << "PrintStmt(expr=";
    stmt.expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(ReadStmt& stmt)  { 
    std::cout << "ReadStmt(var_name=" << stmt.expr << ")";
}

void PrintVisitor::visit(AssertStmt& stmt)  { 
    std::cout << "AssertStmt(expr=";
    stmt.expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(ExitStmt& stmt)  { 
    std::cout << "ExitStmt(expr=";
    if (stmt.expr) stmt.expr.get()->accept(*this); 
    std::cout << ")";
}

void PrintVisitor::visit(StructDecl& decl)  { 
    std::cout << "StructDecl(name=" << decl.name << ", fields=[";
    for (auto& field : decl.fields) {
        visit(field);
        if (&field != &decl.fields.back()) std::cout << ", ";
    }
    std::cout << "])";
}

void PrintVisitor::visit(FunctionDecl& decl)  { 
    std::cout << "FunctionDecl(return_type=" << decl.return_type
                << ", name=" << decl.name << ", params=[";
    for (auto& param : decl.params) {
        std::cout << "(" << param.first << ", " << param.second << ")";
        if (&param != &decl.params.back()) std::cout << ", ";
    }
    std::cout << "], body=";
    decl.body.get()->accept(*this);
    std::cout << ")";
}

void PrintVisitor::visit(GlobalVarDecl& decl)  { 
    std::cout << "GlobalVarDecl(var=";
    visit(decl.var);
    std::cout << ")";
}

void PrintVisitor::visit(TranslationUnitNode& node)  {
    std::cout << "TranslationUnitNode(statements=[" << std::endl;
    for (auto& stmt : node.statements) {
        stmt.get()->accept(*this);
        std::cout << std::endl;
    }
    std::cout << "])" << std::endl;
}

void PrintVisitor::visit(MainFunctionNode& node)  {
    std::cout << "MainFunctionNode(body=";
    node.body.get()->accept(*this);
    std::cout << ")";
}

#include "ast.hpp"

// Реализация методов accept
void LiteralExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void VariableExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void BinaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void UnaryExpr::accept(ASTVisitor& visitor) { visitor.visit(*this); }
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
void AssertStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void ExitStmt::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void VarDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void StructDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void FunctionDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void GlobalVarDecl::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void TranslationUnitNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void MainFunctionNode::accept(ASTVisitor& visitor) { visitor.visit(*this); }

