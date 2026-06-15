%{
#include "ast.h"
#include <iostream>
#include <vector>
#include <cstring>

extern int yylex();
extern int line_num;
void yyerror(const char* s);

static int lastSyntaxErrorLine = -1;
std::shared_ptr<ProgramNode> rootNode;
%}

%code requires {
    #include "ast.h"
    #include <memory>
}

%union {
    char* str;
    ASTNode* node;
    ExprNode* expr;
    StmtNode* stmt;
    BlockNode* block;
    ProgramNode* prog;
    std::vector<std::shared_ptr<StmtNode>>* stmt_list;
    std::vector<std::shared_ptr<ASTNode>>* decl_list;
    std::vector<std::shared_ptr<ExprNode>>* expr_list;
    std::vector<std::pair<std::string, std::string>>* param_list;
}

%token <str> T_ID T_INT_LITERAL T_FLOAT_LITERAL T_BOOL_LITERAL
%token <str> T_REL_OP T_ADD_OP T_MUL_OP T_EQ
%token T_INT T_FLOAT T_BOOL T_VOID T_IF T_ELSE T_WHILE T_FOR T_RETURN T_INPUT T_OUTPUT
%token T_AND T_OR T_NOT T_INC T_DEC
%token T_ASSIGN T_SEMI T_COMMA T_LPAREN T_RPAREN T_LBRACE T_RBRACE
%token T_PLUS_ASSIGN T_MINUS_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN

%type <prog> program
%type <decl_list> declarations
%type <node> declaration function_decl
%type <param_list> param_list param_seq
%type <expr_list> arg_list arg_seq
%type <str> type
%type <block> block
%type <stmt_list> statements
%type <stmt> statement var_decl assign_stmt inc_dec_stmt if_stmt while_stmt for_stmt io_stmt return_stmt expr_stmt assign_expr
%type <expr> expression logical_and equality relational additive term unary factor function_call

%right T_NOT T_INC T_DEC
%left T_OR
%left T_AND
%left T_EQ T_REL_OP
%left T_ADD_OP
%left T_MUL_OP

%%

program:
    declarations { rootNode = std::make_shared<ProgramNode>(line_num);
                   if ($1) { rootNode->declarations = *$1; delete $1; } }
    ;

declarations:
    declarations declaration { $$ = $1; if ($2) $$->push_back(std::shared_ptr<ASTNode>($2)); }
    | declaration { $$ = new std::vector<std::shared_ptr<ASTNode>>(); if ($1) $$->push_back(std::shared_ptr<ASTNode>($1)); }
    | declarations error T_SEMI { yyerrok; $$ = $1; }
    ;

declaration:
    var_decl { $$ = $1; }
    | function_decl { $$ = $1; }
    | error T_SEMI { yyerrok; $$ = nullptr; }
    ;

type:
    T_INT { $$ = strdup("int"); } | T_FLOAT { $$ = strdup("float"); } | T_BOOL { $$ = strdup("bool"); } | T_VOID { $$ = strdup("void"); }
    ;

var_decl:
    type T_ID T_SEMI { $$ = new VarDeclNode($1, $2, line_num); free($1); free($2); }
    | type T_ID T_ASSIGN expression T_SEMI {
        BlockNode* b = new BlockNode(line_num);
        b->statements.push_back(std::shared_ptr<StmtNode>(new VarDeclNode($1, $2, line_num)));
        b->statements.push_back(std::shared_ptr<StmtNode>(new AssignNode($2, std::shared_ptr<ExprNode>($4), line_num)));
        $$ = b;
        free($1); free($2);
    }
    | type error T_SEMI { yyerror("Invalid variable declaration"); $$ = new VarDeclNode("error", "error", line_num); yyerrok; }
    ;

function_decl:
    type T_ID T_LPAREN param_list T_RPAREN block { $$ = new FunctionNode($1, $2, *$4, std::shared_ptr<BlockNode>($6), line_num); free($1); free($2); delete $4; }
    ;

param_list:
    /* empty */ { $$ = new std::vector<std::pair<std::string, std::string>>(); }
    | param_seq { $$ = $1; }
    ;

param_seq:
    type T_ID { $$ = new std::vector<std::pair<std::string, std::string>>(); $$->push_back({$1, $2}); free($1); free($2); }
    | param_seq T_COMMA type T_ID { $$ = $1; $$->push_back({$3, $4}); free($3); free($4); }
    ;

block:
    T_LBRACE statements T_RBRACE { $$ = new BlockNode(line_num); $$->statements = *$2; delete $2; }
    | T_LBRACE error T_RBRACE { yyerror("Block parse error"); $$ = new BlockNode(line_num); yyerrok; }
    ;

statements:
    /* empty */ { $$ = new std::vector<std::shared_ptr<StmtNode>>(); }
    | statements statement { $$ = $1; if ($2) $$->push_back(std::shared_ptr<StmtNode>($2)); }
    ;

statement:
    var_decl { $$ = $1; }
    | assign_stmt { $$ = $1; }
    | inc_dec_stmt { $$ = $1; } 
    | if_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    | for_stmt { $$ = $1; }
    | io_stmt { $$ = $1; }
    | return_stmt { $$ = $1; }
    | expr_stmt { $$ = $1; }
    | block { $$ = $1; }
    | error T_SEMI { yyerrok; $$ = nullptr; }
    ;

assign_stmt:
    T_ID T_ASSIGN expression T_SEMI { $$ = new AssignNode($1, std::shared_ptr<ExprNode>($3), line_num); free($1); }
    | T_ID T_PLUS_ASSIGN expression T_SEMI {
        auto varNode = new ExprNode($1, "id", line_num);
        auto opNode = new ExprNode("+", std::shared_ptr<ExprNode>(varNode), std::shared_ptr<ExprNode>($3), line_num);
        $$ = new AssignNode($1, std::shared_ptr<ExprNode>(opNode), line_num); free($1);
    }
    | T_ID T_MINUS_ASSIGN expression T_SEMI {
        auto varNode = new ExprNode($1, "id", line_num);
        auto opNode = new ExprNode("-", std::shared_ptr<ExprNode>(varNode), std::shared_ptr<ExprNode>($3), line_num);
        $$ = new AssignNode($1, std::shared_ptr<ExprNode>(opNode), line_num); free($1);
    }
    | T_ID T_MUL_ASSIGN expression T_SEMI {
        auto varNode = new ExprNode($1, "id", line_num);
        auto opNode = new ExprNode("*", std::shared_ptr<ExprNode>(varNode), std::shared_ptr<ExprNode>($3), line_num);
        $$ = new AssignNode($1, std::shared_ptr<ExprNode>(opNode), line_num); free($1);
    }
    | T_ID T_DIV_ASSIGN expression T_SEMI {
        auto varNode = new ExprNode($1, "id", line_num);
        auto opNode = new ExprNode("/", std::shared_ptr<ExprNode>(varNode), std::shared_ptr<ExprNode>($3), line_num);
        $$ = new AssignNode($1, std::shared_ptr<ExprNode>(opNode), line_num); free($1);
    }
    | T_ID T_ASSIGN error T_SEMI { yyerror("Invalid assignment"); $$ = new AssignNode($1, nullptr, line_num); free($1); yyerrok; }
    ;

assign_expr:
    T_ID T_ASSIGN expression { $$ = new AssignNode($1, std::shared_ptr<ExprNode>($3), line_num); free($1); }
    | T_ID T_INC {
        auto varNode = new ExprNode($1, "id", line_num);
        auto oneNode = new ExprNode("1", "int", line_num);
        auto addNode = new ExprNode("+", std::shared_ptr<ExprNode>(varNode), std::shared_ptr<ExprNode>(oneNode), line_num);
        $$ = new AssignNode($1, std::shared_ptr<ExprNode>(addNode), line_num); free($1);
    }
    | T_ID T_DEC {
        auto varNode = new ExprNode($1, "id", line_num);
        auto oneNode = new ExprNode("1", "int", line_num);
        auto subNode = new ExprNode("-", std::shared_ptr<ExprNode>(varNode), std::shared_ptr<ExprNode>(oneNode), line_num);
        $$ = new AssignNode($1, std::shared_ptr<ExprNode>(subNode), line_num); free($1);
    }
    ;

inc_dec_stmt:
    T_ID T_INC T_SEMI {
        auto varNode = new ExprNode($1, "id", line_num);
        auto oneNode = new ExprNode("1", "int", line_num);
        auto addNode = new ExprNode("+", std::shared_ptr<ExprNode>(varNode), std::shared_ptr<ExprNode>(oneNode), line_num);
        $$ = new AssignNode($1, std::shared_ptr<ExprNode>(addNode), line_num); free($1);
    }
    | T_ID T_DEC T_SEMI {
        auto varNode = new ExprNode($1, "id", line_num);
        auto oneNode = new ExprNode("1", "int", line_num);
        auto subNode = new ExprNode("-", std::shared_ptr<ExprNode>(varNode), std::shared_ptr<ExprNode>(oneNode), line_num);
        $$ = new AssignNode($1, std::shared_ptr<ExprNode>(subNode), line_num); free($1);
    }
    ;

for_stmt:
    T_FOR T_LPAREN assign_expr T_SEMI expression T_SEMI assign_expr T_RPAREN block {
        $$ = new ForNode(std::shared_ptr<AssignNode>((AssignNode*)$3), std::shared_ptr<ExprNode>($5), std::shared_ptr<AssignNode>((AssignNode*)$7), std::shared_ptr<BlockNode>($9), line_num);
    }
    ;

if_stmt:
    T_IF T_LPAREN expression T_RPAREN block { $$ = new IfNode(std::shared_ptr<ExprNode>($3), std::shared_ptr<BlockNode>($5), nullptr, line_num); }
    | T_IF T_LPAREN expression T_RPAREN block T_ELSE block { $$ = new IfNode(std::shared_ptr<ExprNode>($3), std::shared_ptr<BlockNode>($5), std::shared_ptr<BlockNode>($7), line_num); }
    ;

while_stmt:
    T_WHILE T_LPAREN expression T_RPAREN block { $$ = new WhileNode(std::shared_ptr<ExprNode>($3), std::shared_ptr<BlockNode>($5), line_num); }
    ;

io_stmt:
    T_INPUT T_ID T_SEMI { $$ = new IONode(true, $2, nullptr, line_num); free($2); }
    | T_OUTPUT expression T_SEMI { $$ = new IONode(false, "", std::shared_ptr<ExprNode>($2), line_num); }
    ;

return_stmt:
    T_RETURN expression T_SEMI { $$ = new ReturnNode(std::shared_ptr<ExprNode>($2), line_num); }
    ;

expr_stmt:
    expression T_SEMI { $$ = new ExprStmtNode(std::shared_ptr<ExprNode>($1), line_num); }
    ;

expression:
    expression T_OR logical_and { $$ = new ExprNode("||", std::shared_ptr<ExprNode>($1), std::shared_ptr<ExprNode>($3), line_num); }
    | logical_and { $$ = $1; }
    ;

logical_and:
    logical_and T_AND equality { $$ = new ExprNode("&&", std::shared_ptr<ExprNode>($1), std::shared_ptr<ExprNode>($3), line_num); }
    | equality { $$ = $1; }
    ;

equality:
    equality T_EQ relational { $$ = new ExprNode($2, std::shared_ptr<ExprNode>($1), std::shared_ptr<ExprNode>($3), line_num); free($2); }
    | equality T_REL_OP relational { $$ = new ExprNode($2, std::shared_ptr<ExprNode>($1), std::shared_ptr<ExprNode>($3), line_num); free($2); }
    | relational { $$ = $1; }
    ;

relational:
    relational T_REL_OP additive { $$ = new ExprNode($2, std::shared_ptr<ExprNode>($1), std::shared_ptr<ExprNode>($3), line_num); free($2); }
    | additive { $$ = $1; }
    ;

additive:
    additive T_ADD_OP term { $$ = new ExprNode($2, std::shared_ptr<ExprNode>($1), std::shared_ptr<ExprNode>($3), line_num); free($2); }
    | term { $$ = $1; }
    ;

term:
    term T_MUL_OP unary { $$ = new ExprNode($2, std::shared_ptr<ExprNode>($1), std::shared_ptr<ExprNode>($3), line_num); free($2); }
    | unary { $$ = $1; }
    ;

unary:
    T_NOT unary { $$ = new ExprNode("!", nullptr, std::shared_ptr<ExprNode>($2), line_num); }
    | T_INC unary { $$ = new ExprNode("++", nullptr, std::shared_ptr<ExprNode>($2), line_num); }
    | T_DEC unary { $$ = new ExprNode("--", nullptr, std::shared_ptr<ExprNode>($2), line_num); }
    | factor { $$ = $1; }
    ;

factor:
    T_INT_LITERAL { $$ = new ExprNode($1, "int", line_num); free($1); }
    | T_FLOAT_LITERAL { $$ = new ExprNode($1, "float", line_num); free($1); }
    | T_BOOL_LITERAL { $$ = new ExprNode($1, "bool", line_num); free($1); }
    | function_call { $$ = $1; }
    | T_ID { $$ = new ExprNode($1, "id", line_num); free($1); }
    | T_LPAREN expression T_RPAREN { $$ = $2; }
    ;

function_call:
    T_ID T_LPAREN arg_list T_RPAREN { $$ = new ExprNode($1, *$3, line_num); free($1); delete $3; }
    ;

arg_list:
    /* empty */ { $$ = new std::vector<std::shared_ptr<ExprNode>>(); }
    | arg_seq { $$ = $1; }
    ;

arg_seq:
    expression { $$ = new std::vector<std::shared_ptr<ExprNode>>(); $$->push_back(std::shared_ptr<ExprNode>($1)); }
    | arg_seq T_COMMA expression { $$ = $1; $$->push_back(std::shared_ptr<ExprNode>($3)); }
    ;

%%
int syn_errors = 0;
void yyerror(const char* s) {
    if (lastSyntaxErrorLine == line_num) return;
    lastSyntaxErrorLine = line_num;
    std::cerr << "[Syntax Error] Line " << line_num << ": " << s << "\n";
    syn_errors++;
}
