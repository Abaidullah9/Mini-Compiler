#include <iostream>
#include <string>
#include <fstream>
#include "ast.h"
#include "symbol_table.h"
#include "semantic_analyzer.h"
#include "tac.h"
#include "optimizer.h"
#include "codegen.h"

extern int yyparse();
extern FILE* yyin;
extern std::shared_ptr<ProgramNode> rootNode;
extern int lex_errors;
extern int syn_errors;

int main(int argc, char** argv) {
    int choice = 0;

    std::cout << "================================================\n";
    std::cout << "      MINI-COMPILER EXECUTION INTERFACE\n";
    std::cout << "================================================\n";
    std::cout << "[1] Read source code from a File\n";
    std::cout << "[2] Enter source code manually at Runtime\n";
    std::cout << "Select an option (1 or 2): ";
    std::cin >> choice;

    if (choice == 1) {
        std::string filename;
        std::cout << "Enter the filename (e.g., testcases/valid_tests.txt): ";
        std::cin >> filename;
        yyin = fopen(filename.c_str(), "r");
        if (!yyin) { 
            std::cerr << "\n[ERROR] Failed to open file: " << filename << "\n"; 
            return 1; 
        }
        std::cout << "\n[INFO] Compiling from file: " << filename << "\n\n";
    } 
    else if (choice == 2) {
        std::cout << "\n[INFO] Interactive Mode. Type your C-like code below.\n";
        std::cout << "       (Press Ctrl+D on Linux/Mac or Ctrl+Z on Windows, then Enter to compile)\n";
        std::cout << "------------------------------------------------\n";
        std::cin.ignore(10000, '\n');

        std::ofstream tmp_file("temp_input.txt");
        std::string line;
        
        while (std::getline(std::cin, line)) {
            tmp_file << line << "\n";
        }
        tmp_file.close();

        yyin = fopen("temp_input.txt", "r");
        if (!yyin) {
            std::cerr << "\n[ERROR] Failed to process interactive input.\n";
            return 1;
        }
    } 
    else {
        std::cerr << "\n[ERROR] Invalid choice. Exiting compiler.\n";
        return 1;
    }

    std::cout << "\n[STAGE 1 & 2] Running Lexical and Syntax Analysis...\n";
    if (yyparse() != 0 || !rootNode) {
        std::cerr << "Parsing aborted due to critical syntax violations.\n";
    }

    if (lex_errors == 0 && syn_errors == 0 && rootNode) {
        std::cout << "\n[STAGE 2] Parse Success! Abstract Syntax Tree Generated.\n";
        printAST(rootNode);

        SymbolTable symTable;
        SemanticAnalyzer semanticParams(symTable);
        
        std::cout << "\n[STAGE 4] Running Semantic Analysis...\n";
        if (semanticParams.analyze(rootNode)) {
            std::cout << "  -> Semantic Analysis Passed: 0 Errors.\n";
            
            std::cout << "\n[STAGE 3] Final Populated Symbol Table:\n";
            symTable.printTable();
            
            TACGenerator tacGen;
            tacGen.gen(rootNode);
            std::cout << "\n[STAGE 5A] Generated Three-Address Code (TAC):\n";
            tacGen.printTAC();
            
            Optimizer::optimize(tacGen.instructions);
            std::cout << "\n[STAGE 5B] Optimized Three-Address Code:\n";
            tacGen.printTAC();
            
            std::cout << "\n[STAGE 6] Final Target Code Generation:\n";
            CodeGenerator::generateAssembly(tacGen.instructions);
            
            std::cout << "\n================================================\n";
            std::cout << "       COMPILATION FINISHED SUCCESSFULLY\n";
            std::cout << "================================================\n";
        } else {
            std::cerr << "\n================================================\n";
            std::cerr << "   COMPILATION FAILED: SEMANTIC ERRORS FOUND\n";
            std::cerr << "================================================\n";
        }
    } else {
        std::cerr << "\n================================================\n";
        std::cerr << " COMPILATION FAILED: RESOLVE SYNTAX/LEX ERRORS\n";
        std::cerr << "================================================\n";
    }

    if (yyin && yyin != stdin) fclose(yyin);
    return 0;
}
