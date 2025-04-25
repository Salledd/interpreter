#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "vis_print.hpp"

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Couldn't open the file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    try{
        //std::string code = "int main() { int x = 3; bool y = True; // bla bla bla\n if (x >= 15) { print(x); } }";
        std::string code = readFile("C:\\MGU\\Proga\\GitHub\\interpreter\\code.txt");
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
