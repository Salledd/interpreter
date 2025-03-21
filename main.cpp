#include "lexer.hpp"
#include <iostream>

int main() {
    try{
        std::string code = "int x = 3.; bool y = True; \" \n str\" A.b; // bla bla bla\n if (x >= 15) { print(x); }";
        Lexer lexer(code);
        
        std::vector<Token> tokens = lexer.tokenize();
        
        for (const auto& token : tokens) {
            std::cout << "Token: " << static_cast<int>(token.type) << ", Value: " << token.value << '\n';
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error occurred!" << std::endl;
    }
}
