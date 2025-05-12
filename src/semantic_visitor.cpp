#include "semantic_visitor.hpp"
#include "scope.hpp"
#include "ast.hpp"
#include <stdexcept>
#include <string>
#include <memory>

class SemanticVisitor : public ASTVisitor {
public:
    explicit SemanticVisitor(Scope& globalScope) : current_scope(std::make_shared<Scope>(globalScope)) {}

        void visit(TranslationUnitNode& node) override {
        bool has_main = false;
        for (const auto& decl : node.decls) {
            decl->accept(*this);

            // Проверка функции main()
            if (auto func_decl = std::dynamic_pointer_cast<FunctionDecl>(decl)) {
                if (func_decl->name == "main") {
                    has_main = true;
                    if (std::dynamic_pointer_cast<IntType>(func_decl->return_type) == nullptr) {
                        throw std::runtime_error("Function 'main' must have return type 'int'.");
                    }
                }
            }
        }
        if (!has_main) {
            throw std::runtime_error("Function 'main' is not declared.");
        }
    }

    void visit(VarDecl& decl) override {
        for (const auto& var : decl.variables) {
            TypePtr decl_type = decl.type;
            // Проверка на повторное объявление
            if (current_scope->lookup_var(var.name)) {
                throw std::runtime_error("Variable '" + var.name + "' is already declared in the current scope.");
            }

            // Проверка инициализации const
            if (decl.modifiers.has(Modifier::Const) && !var.init) {
                throw std::runtime_error("Const variable '" + var.name + "' must be initialized.");
            }

            // проверка на совместимость типа обьявления и инициализации
            cur_type = decl.type;   // для проверки при инициализации массива
            if (var.init) {
                var.init->accept(*this);
                auto init_type = cur_type;
                if(!decl.type->is_compatible_with(init_type)){
                    throw std::runtime_error("Incompatible types in variable initialization: " +
                                                decl.type->get_name() + " and " + init_type->get_name());
                }
            }

            if (var.is_array) {
                decl_type = make_type_arr(decl.type->get_name(), decl.modifiers.has(Modifier::Const));
                if(var.size) {
                    var.size->accept(*this);
                    auto size_type = cur_type;
                    if (!std::dynamic_pointer_cast<IntegralType>(size_type)) {
                        throw std::runtime_error("Array size must be an integral.");
                    }
                    // как то проверить на кол-во элементов !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!111111111
                }
            
            }
            
            // Добавление переменной в текущую область видимости
            current_scope->declare(var.name, SymbolInfo(decl.modifiers, decl_type));
        }
    }

    void visit(FunctionDecl& decl) override {
        // Проверка на повторное объявление функции
        if (current_scope->exists_func(decl.name)) {
            throw std::runtime_error("Function '" + decl.name + "' is already declared in the current scope.");
        }
        // Добавление функции в текущую область видимости
        current_scope->declare(decl.name, FunctionInfo(decl.return_mods, decl.return_type, decl.params));

        // Создание новой области видимости для параметров функции
        auto func_scope = current_scope->enterScope();
        for (const auto& param : decl.params) {
            func_scope->declare(param.second, SymbolInfo(param.first.first, param.first.second));
        }

        // Проверка тела функции
        if (decl.body) {
            return_type = decl.return_type; // Сохранение типа возвращаемого значения
            has_return = false; // Сброс флага наличия return
            current_scope = func_scope;
            decl.body->accept(*this);

            // Проверка наличия return в функциях с не-void возвращаемым типом
            if (std::dynamic_pointer_cast<VoidType>(decl.return_type) == nullptr && !has_return) {
                throw std::runtime_error("Function '" + decl.name + "' must have a return statement.");
            }
            has_return = false; // Сброс флага
            return_type = nullptr; // Сброс типа возвращаемого значения
            current_scope = current_scope->exitScope();
        }
    }

    void visit(IdExpr& expr) override {
        // Проверка, что идентификатор объявлен
        if (!current_scope->exists_var(expr.name)) {
            throw std::runtime_error("Identifier '" + expr.name + "' is not declared.");
        }
    }

    void visit(CallExpr& expr) override {
        // Проверка, что вызываемая функция объявлена
        auto callee = std::dynamic_pointer_cast<IdExpr>(expr.callee);
        if (!callee) {
            throw std::runtime_error("Callee is not a valid identifier.");
        }
        auto call_info = current_scope->exists_func(callee->name);
        if (!call_info) {
            throw std::runtime_error("Function '" + callee->name + "' is not declared.");
        }
 
        const auto& params = call_info->params;
        if (params.size() != expr.args.size()) {
            throw std::runtime_error("Function '" + callee->name + "' expects " +
                                     std::to_string(params.size()) + " arguments, but " +
                                     std::to_string(expr.args.size()) + " were provided.");
        }
        for (size_t i = 0; i < params.size(); ++i) {
            expr.args[i]->accept(*this);
            auto arg_type = cur_type; // Получаем тип аргумента
            if (params[i].first.second->is_compatible_with(arg_type)) {
                throw std::runtime_error("Argument " + std::to_string(i + 1) +
                                         " of function '" + callee->name +
                                         "' has incompatible type.");
            }
        }
    }

    void visit(StructDecl& decl) override {
        // Проверяем, что структура не была объявлена ранее
        if (current_scope->exists_var(decl.name)) {
            throw std::runtime_error("Struct '" + decl.name + "' is already declared.");
        }
        std::vector<std::pair<std::pair<Modifiers, TypePtr>, std::string>> members;
        for (const auto& field : decl.fields) {
            for(const auto& var : field.variables) {
                // Проверяем, что поле не было объявлено ранее
                for(const auto& member : members) {
                    if (member.second == var.name) {
                        throw std::runtime_error("Field '" + var.name + "' is already declared in struct '" + decl.name + "'.");
                    }
                }
                // Добавляем поле в структуру
                members.emplace_back(std::make_pair(field.modifiers, field.type), var.name);
            }
        }
        // Добавляем структуру в текущую область видимости
        current_scope->declare(decl.name, SymbolInfo(Modifiers{}, make_type_struct(decl.name, members)));
    }
    
    void visit(AssertDecl& decl) override {
        // Проверяем выражение
        decl.expr->accept(*this);
    
        // Проверяем, что тип выражения — это bool
        if (!std::dynamic_pointer_cast<BoolType>(cur_type)) {
            throw std::runtime_error("Assert expression must be of type 'bool'.");
        }
    }
    
    void visit(TypedefDecl& decl) override {
        // Проверяем, что псевдоним не был объявлен ранее
        if (current_scope->exists_var(decl.alias_name) || current_scope->exists_type(decl.alias_name)) {
            throw std::runtime_error("Typedef alias '" + decl.alias_name + "' is already declared.");
        }
        
        // Добавляем псевдоним в текущую область видимости
        current_scope->declare(decl.alias_name, TypedefInfo(decl.original_modifiers, decl.original_type));
    }


    void visit(BlockStmt& stmt) override {
        // Создаём новую область видимости
        auto block_scope = current_scope->enterScope();
        current_scope = block_scope;
    
        // Проверяем все инструкции в блоке
        for (const auto& statement : stmt.statements) {
            statement->accept(*this);
        }
    
        // Выходим из области видимости
        current_scope = current_scope->exitScope();
    }
    
    void visit(PrintStmt& stmt) override {
        // Проверяем выражение для вывода
        for(const auto& expr : stmt.expr) {
            expr->accept(*this);
            auto expr_type = cur_type; // Получаем тип выражения

        }
    }
    
    void visit(ReadStmt& stmt) override {
        // Проверяем выражение для ввода
        stmt.expr->accept(*this);
    }
    
    void visit(ExitStmt& stmt) override {
        // Проверяем выражение для выхода
        stmt.expr->accept(*this);
    
        // Проверяем, что тип выражения — это целое число
        if (!std::dynamic_pointer_cast<IntegralType>(cur_type)) {
            throw std::runtime_error("Exit statement requires an integral type.");
        }
    }
    // done
    void visit(ReturnStmt& stmt) override {
        if (stmt.expr) {
            has_return = true;
            stmt.expr->accept(*this);
            auto expr_type = cur_type; // Получаем тип возвращаемого значения
            if(return_type->is_compatible_with(expr_type)){
                throw std::runtime_error("Return type does not match function return type.");
            }
        } else {
            if (std::dynamic_pointer_cast<VoidType>(return_type) == nullptr) {
                throw std::runtime_error("Function with non-void return type must return a value.");
            }
        }
    }
    // done
    void visit(BreakStmt& stmt) override {
        if (!inside_loop) {
            throw std::runtime_error("Break statement is not inside a loop.");
        }
    }
    // done
    void visit(ContinueStmt& stmt) override {
        if (!inside_loop) {
            throw std::runtime_error("Continue statement is not inside a loop.");
        }
    }
    // done
    void visit(WhileStmt& stmt) override {
        if(!stmt.condition) {
            throw std::runtime_error("While statement condition is null.");
        }
        stmt.condition->accept(*this);
        auto condition_type = cur_type; // Получаем тип условия
        if (!std::dynamic_pointer_cast<ArithmeticType>(condition_type)) {
            throw std::runtime_error("While loop condition must be of type 'bool'.");
        }
        inside_loop = true;
        stmt.body->accept(*this);
        inside_loop = false;
    }
    //done
    void visit(ForStmt& stmt) override {
        if (stmt.init) stmt.init->accept(*this);
        if (stmt.condition) stmt.condition->accept(*this);
        auto condition_type = cur_type; // Получаем тип условия
        if (!std::dynamic_pointer_cast<ArithmeticType>(condition_type)) {
            throw std::runtime_error("For loop condition must be of type 'bool'.");
        }
        if (stmt.increment) stmt.increment->accept(*this);
        inside_loop = true;
        stmt.body->accept(*this);
        inside_loop = false;
    }
    //done
    void visit(IfStmt& stmt) override {
        if(!stmt.condition) {
            throw std::runtime_error("If statement condition is null.");
        }
        stmt.condition->accept(*this);
        auto condition_type = cur_type; // Получаем тип условия
        if (!std::dynamic_pointer_cast<ArithmeticType>(condition_type)) {
            throw std::runtime_error("If condition must be of type 'bool'.");
        }
        stmt.then_branch->accept(*this);
        if (stmt.else_branch) {
            stmt.else_branch->accept(*this);
        }
    }




    // done
    void visit(LiteralExpr& expr) override {
        // Определяем тип литерала
        if (std::holds_alternative<bool>(expr.value)) cur_type = make_type("bool");
        if (std::holds_alternative<char>(expr.value)) cur_type = make_type("char");
        if (std::holds_alternative<short>(expr.value)) cur_type = make_type("short");
        if (std::holds_alternative<int>(expr.value)) cur_type = make_type("int");
        if (std::holds_alternative<long>(expr.value)) cur_type = make_type("long");
        if (std::holds_alternative<float>(expr.value)) cur_type = make_type("float");
        if (std::holds_alternative<double>(expr.value)) cur_type = make_type("double");
        if (std::holds_alternative<long double>(expr.value)) cur_type = make_type("ldouble");
        if (std::holds_alternative<std::string>(expr.value)) cur_type = make_type("string");
        throw std::runtime_error("Unknown literal type");
    }
    // done
    void visit(IdExpr& expr) override {
        // Проверяем, что идентификатор объявлен
        auto symbol = current_scope->exists_var(expr.name);
        if (!symbol) {
            throw std::runtime_error("Identifier '" + expr.name + "' is not declared.");
        }
        cur_type = symbol->type; // Возвращаем тип переменной
    }
    // done
    void visit(BinaryExpr& expr) override {
        expr.left->accept(*this);
        auto left_type = cur_type; 
        expr.right->accept(*this);
        auto right_type = cur_type; 

        // Проверяем совместимость типов
        if (!left_type->is_compatible_with(right_type)) {
            throw std::runtime_error("Incompatible types in binary expression: " +
                                     left_type->get_name() + " and " + right_type->get_name());
        }

        // Возвращаем тип результата, например, тип левого операнда
        cur_type = left_type;
    }
    //done
    void visit(UnaryExpr& expr) override {
        expr.operand->accept(*this);
        auto operand_type = cur_type;

        auto arithmetic_type = std::dynamic_pointer_cast<ArithmeticType>(operand_type);

        // Пример проверки: унарные операции применимы только к числовым типам
        if (expr.op == "-" || expr.op == "+") {
            if (!arithmetic_type) {
                throw std::runtime_error("Unary operator '" + expr.op + "' is not applicable to type " +
                                         operand_type->get_name());
            }
        }

        if(expr.op == "--" || expr.op == "++"){
            if(arithmetic_type->is_const()){
                throw std::runtime_error("Unary operator '" + expr.op + "' is not applicable to const type " +
                                         operand_type->get_name());
            }
        }

        cur_type = operand_type;
    }
    // done
    void visit(AssignExpr& expr) override {
        expr.left->accept(*this);
        auto left_type = cur_type;

        if(left_type->is_const()){
            throw std::runtime_error("Cannot assign to const variable.");
        }

        expr.right->accept(*this);
        auto right_type = cur_type; 

        // Проверяем совместимость типов
        if (!left_type->is_compatible_with(right_type)) {
            throw std::runtime_error("Incompatible types in assignment: " +
                                     left_type->get_name() + " and " + right_type->get_name());
        }

        cur_type = left_type; // Тип присваивания — это тип левого операнда
    }
    // done
    void visit(CallExpr& expr) override {
        auto callee = std::dynamic_pointer_cast<IdExpr>(expr.callee);
        if (!callee) {
            throw std::runtime_error("Callee is not a valid identifier.");
        }
        
        // Проверяем, что вызываемая функция объявлена
        auto func_info = current_scope->exists_func(callee->name);
        if (!func_info) {
            throw std::runtime_error("Function '" + callee->name + "' is not declared.");
        }

        const auto& params = func_info->params;
        if (params.size() != expr.args.size()) {
            throw std::runtime_error("Function '" + callee->name + "' expects " +
                                     std::to_string(params.size()) + " arguments, but " +
                                     std::to_string(expr.args.size()) + " were provided.");
        }

        for (size_t i = 0; i < params.size(); ++i) {
            expr.args[i]->accept(*this);
            auto arg_type = cur_type; 
            if (!params[i].first.second->is_compatible_with(arg_type)) {
                throw std::runtime_error("Argument " + std::to_string(i + 1) +
                                         " of function '" + callee->name +
                                         "' has incompatible type.");
            }
        }

        cur_type = func_info->return_type; // Возвращаем тип возвращаемого значения функции
    }
    // done
    void visit(SizeofExpr& expr) override {
        if (expr.operand) {
            expr.operand->accept(*this); // Проверяем корректность операнда
            cur_type = make_type("int");
            return;
        } else {
            if(auto type = *std::get_if<TypePtr>(&expr.type)) {
                if (std::dynamic_pointer_cast<VoidType>(type)) {
                    throw std::runtime_error("sizeof cannot be applied to void type");
                }
            // тип структуры или typedef
            } else if (auto type = *std::get_if<ExprPtr>(&expr.type)) {
                auto id_type = std::dynamic_pointer_cast<IdExpr>(type);
                if (!id_type) {
                    throw std::runtime_error("sizeof must be followed by a valid identifier.");
                }

                auto type_info = current_scope->exists_var(id_type->name);
                if (auto type_info = current_scope->exists_type(id_type->name)) {
                    if (std::dynamic_pointer_cast<VoidType>(type_info->type)) {
                        throw std::runtime_error("sizeof cannot be applied to void type");
                    }
                } else if(auto type_info = current_scope->exists_var(id_type->name)) {
                    if(!std::dynamic_pointer_cast<StructType>(type_info->type)) {
                        throw std::runtime_error("sizeof must be followed by a valid identifier.");
                    }
                } else {
                    throw std::runtime_error("sizeof must be followed by a valid identifier.");
                } 
            }
            cur_type = make_type("int"); // sizeof возвращает целое значение
            return;
        }
        throw std::runtime_error("Invalid sizeof expression");
    }
    //done
    void visit(ArrayInitExpr& expr) override {
        auto decl_type = cur_type;
        for (const auto& element : expr.elements) {
            element->accept(*this);
            auto element_type = cur_type; 
            if (!decl_type->is_compatible_with(element_type)) {
                throw std::runtime_error("Incompatible types in array initialization: " +
                                         decl_type->get_name() + " and " +
                                         element_type->get_name());
            }
        }
    }
    //done +-
    void visit(TernaryExpr& expr) override {
        // Проверяем условие
        expr.cond->accept(*this);
        auto cond_type = cur_type;
    
        // Условие должно быть типа bool
        if (!std::dynamic_pointer_cast<ArithmeticType>(cond_type)) {
            throw std::runtime_error("Condition in ternary expression must be arithmetic.");
        }
    
        // Проверяем ветви
        expr.true_expr->accept(*this);
        auto true_type = cur_type;
    
        expr.false_expr->accept(*this);
        auto false_type = cur_type;
    
        // Проверяем совместимость типов ветвей
        if (!true_type->is_compatible_with(false_type)) {
            throw std::runtime_error("Incompatible types in ternary expression: " +
                                     true_type->get_name() + " and " + false_type->get_name());
        }
    
        cur_type = true_type; // Тип результата — это тип одной из ветвей
    }
    //done
    void visit(MemberAccessExpr& expr) override {
        // Проверяем объект
        expr.object->accept(*this);
        auto object_type = cur_type;

        auto odj_id = std::dynamic_pointer_cast<IdExpr>(expr.object);
        if(!odj_id) {
            throw std::runtime_error("Member access must be followed by a valid identifier.");
        }

        // Проверяем, что объект объявлен
        auto obj_info = current_scope->exists_var(odj_id->name);
        if(!obj_info){
            throw std::runtime_error("Object '" + odj_id->name + "' is not declared.");
        }

        // Проверяем, что объект — это структура
        auto struct_type = std::dynamic_pointer_cast<StructType>(obj_info->type);
        if(!struct_type) {
            throw std::runtime_error("Member access is only applicable to struct types.");
        }

        auto member = std::dynamic_pointer_cast<IdExpr>(expr.member);
        if (!member) {
            throw std::runtime_error("Member access must be followed by a valid identifier.");
        }

        // Проверяем, что поле существует в структуре
        auto member_info = struct_type->lookup_member(member->name);
        if (!member_info) {
            throw std::runtime_error("Member '" + member->name + "' does not exist in struct '" +
                                     struct_type->get_name() + "'.");
        }
    
        cur_type = member_info->first.second; // Тип результата — это тип члена
    }
    //done
    void visit(ArrayAccessExpr& expr) override {
        // Проверяем массив
        expr.array->accept(*this);
        auto array_id = std::dynamic_pointer_cast<IdExpr>(expr.array);
        if(!array_id) {
            throw std::runtime_error("Array access must be followed by a valid identifier.");
        }

        auto arr_info = current_scope->exists_var(array_id->name);

        // Проверяем, что массив существует
        if(!arr_info){
            throw std::runtime_error("Array '" + array_id->name + "' is not declared.");
        }
        // Проверяем, что массив — это массив в таблице символов
        auto arr_type = std::dynamic_pointer_cast<ArrayType>(arr_info->type);
        if(!arr_type) {
            throw std::runtime_error("Array access is only applicable to array types.");
        }
        
        // Проверяем индекс
        expr.index->accept(*this);
        auto index_type = cur_type;
    
        // Индекс должен быть целым числом
        if (!std::dynamic_pointer_cast<IntegralType>(index_type)) {
            throw std::runtime_error("Array index must be of integral type.");
        }
    
        cur_type = arr_type->element_type; // Тип результата — это тип элемента массива
    }
    //done
    void visit(LogicalExpr& expr) override {
        // Проверяем левый операнд
        expr.left->accept(*this);
        auto left_type = cur_type;
    
        // Проверяем правый операнд
        expr.right->accept(*this);
        auto right_type = cur_type;
    
        // Оба операнда должны быть типа bool                   
        if (!std::dynamic_pointer_cast<ArithmeticType>(left_type) ||
            !std::dynamic_pointer_cast<ArithmeticType>(right_type)) {
            throw std::runtime_error("Logical operators are only applicable to boolean types.");
        }
    
        cur_type = make_type("bool"); // Тип результата — bool
    }
    //done
    void visit(PostfixExpr& expr) override {    // -- ++ 
        // Проверяем операнд
        expr.operand->accept(*this);
        auto operand_type = cur_type;
    
        // Проверяем, что операнд — это числовой тип
        auto arithmetic_type = std::dynamic_pointer_cast<ArithmeticType>(operand_type);
        if (!arithmetic_type) {
            throw std::runtime_error("Postfix operator '" + expr.op + "' is only applicable to arithmetic types.");
        }
    
        // Проверяем, что операнд не является const
        if (arithmetic_type->is_const()) {
            throw std::runtime_error("Postfix operator '" + expr.op + "' is not applicable to const type.");
        }
    
        cur_type = operand_type; // Тип результата — это тип операнда
    }


private:
    std::shared_ptr<Scope> current_scope;
    TypePtr cur_type = nullptr;
    bool inside_loop = false;
    TypePtr return_type = nullptr;
    bool has_return = false;
};