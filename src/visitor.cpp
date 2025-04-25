#include "visitor.hpp"

void ASTVisitor::visit(LiteralExpr& expr){}
void ASTVisitor::visit(IdExpr& expr){}
void ASTVisitor::visit(BinaryExpr& expr){}
void ASTVisitor::visit(UnaryExpr& expr){}
void ASTVisitor::visit(SizeofExpr& expr){}
void ASTVisitor::visit(TernaryExpr& expr){}
void ASTVisitor::visit(CallExpr& expr){}
void ASTVisitor::visit(MemberAccessExpr& expr){}
void ASTVisitor::visit(ArrayAccessExpr& expr){}
void ASTVisitor::visit(LogicalExpr& expr){}
void ASTVisitor::visit(PostfixExpr& expr){}
void ASTVisitor::visit(AssignExpr& expr){}
void ASTVisitor::visit(ArrayInitExpr& expr){}

void ASTVisitor::visit(ExprStmt& stmt){}
void ASTVisitor::visit(BlockStmt& stmt){}
void ASTVisitor::visit(IfStmt& stmt){}
void ASTVisitor::visit(WhileStmt& stmt){}
void ASTVisitor::visit(ForStmt& stmt){}
void ASTVisitor::visit(ReturnStmt& stmt){}
void ASTVisitor::visit(BreakStmt& stmt){}
void ASTVisitor::visit(ContinueStmt& stmt){}
void ASTVisitor::visit(PrintStmt& stmt){}
void ASTVisitor::visit(ReadStmt& stmt){}
void ASTVisitor::visit(ExitStmt& stmt){}

void ASTVisitor::visit(VarDecl& decl){}
void ASTVisitor::visit(StructDecl& decl){}
void ASTVisitor::visit(FunctionDecl& decl){}
void ASTVisitor::visit(AssertDecl& decl){}
void ASTVisitor::visit(TypedefDecl& decl){}

void ASTVisitor::visit(TranslationUnitNode& node){}