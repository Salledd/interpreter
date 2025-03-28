#include <iostream>
#include "lexer.hpp"
#include "parser.hpp"
#include "visitor.hpp"

int main() {
    try{
        std::string code = "int main() { int x = 3; '\n' bool y = True; // bla bla bla\n if (x >= 15) { print(x); } }";
        
        Lexer lexer(code);
        std::vector<Token> tokens = lexer.tokenize();

        for (const auto& token : tokens) {
            std::cout << "Token: " << static_cast<int>(token.type) << ", Value: " << token.value << '\n';
        }
        std::cout << "------------------------" << std::endl;

        Parser parser(tokens);
        parser.parse();
        auto ast = parser.getAST();

        PrintVisitor visitor;
        ast->accept(visitor);
        

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
    }
}
