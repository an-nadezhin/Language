//
// Created by andrew on 09.12.17.
//

#ifndef LANGUAGE_LANGUAGE_H
#define LANGUAGE_LANGUAGE_H

#include <cassert>
#include <iostream>
#include <cstring>
#include <list>
#include <iterator>

const int BUF_LENGTH = 50;

extern const char *s;

extern int p;

extern double num_valG;

extern char id_val[BUF_LENGTH];

extern int amount_of_labels;




enum Lex {
    L_WHILE,
    L_IF,
    L_ID,
    L_ASSIGN, //=
    L_ADD,
    L_MUL,
    L_DIV,
    L_MINUS,
    L_LPAR,
    L_RPAR,
    L_CONST,
    L_SIN,
    L_COS,
    L_EOF, //\0
    L_LBRA,
    L_RBRA,
    L_DEG,
    L_ERROR,
    L_SQRT,
    L_LN,
    L_EQ, //==
    L_NE, // NOT EQUAL (!=)
    L_GE, // GREATER OR EQUAL (>=)
    L_GT, //GREATER THAN (>)
    L_LE, //LESS THAN OR EQUAL TO (<=)
    L_LT, //LESS THAN (<)
    L_RETURN,
    L_SEMICOLON,
    L_VAR,
    L_DOUBLE,
    L_COMMA
};

enum Priority {
    PR_E, PR_T, PR_D, PR_P
};


extern Lex lex;


class Node {
public:
    Node();

    virtual void print_dot(FILE *code);

};




class Definition_var : public Node {
public:
    Definition_var(char *, int);

    char *name();

    int address();
private:
    char *name_of_var;
    int pos;
};


class Statement : public Node {
public:
    Statement();

    void print_dot(FILE *code);

    virtual void print_dot_name(FILE *code);

    virtual void print_asm_code(FILE *code);
};

class Expression : public Node {
public:
    Expression();

    void print_dot(FILE *code);

    virtual void print_asm_code(FILE *code);

    virtual void print_dot_name(FILE *code, Priority pr);

    virtual void print_tex_name(FILE *code);
};

class Relation : public Node {
public:
    Relation(int, Expression *, Expression *);

    void print_dot(FILE *code);

    void print_dot_name(FILE *code);

    int print_asm_code(FILE *code);

private:
    int type_op;
    Expression *node1;
    Expression *node2;
};

class Return : public Statement {
public:
    Return(Expression *);

    void print_dot(FILE *code);

    void print_dot_name(FILE *code);

    void print_asm_code(FILE *code);

private:
    Expression *arg;
};

class If : public Statement {
public:
    If(Relation *, std::list<Statement *>);

    void print_dot(FILE *code);

    void print_dot_name(FILE *code);

    void print_asm_code(FILE *code);

private:
    Relation *arg;
    std::list<Statement *> stmts;
};

class ASSIGN : public Statement {
public:
    ASSIGN(Definition_var *, Expression *);

    void print_dot(FILE *code);

    void print_dot_name(FILE *code);

    void print_asm_code(FILE *code);

private:
    Definition_var *var_def;
    Expression *arg_r;
};

class While : public Statement {
public:
    While(Relation *, std::list<Statement *>);

    void print_dot(FILE *code);

    void print_dot_name(FILE *code);

    void print_asm_code(FILE *code);

private:
    Relation *arg;
    std::list<Statement *> stmts;

};


class Variable : public Expression {
public:
    Variable(Definition_var *);

    void print_dot_name(FILE *code, Priority pr);

    void print_tex_name(FILE *code);

    void print_asm_code(FILE *code);

    ~Variable();

private:
    Definition_var *var_def;
};

class Number : public Expression {
public:
    Number(double);

    void print_dot_name(FILE *code, Priority pr);

    void print_tex_name(FILE *code);

    void print_asm_code(FILE *code);

    double num();

private:
    double num_valN;
};

class Operator_un : public Expression {
public:
    Operator_un(int, Expression *);

    void print_dot(FILE *code);

    void print_dot_name(FILE *code, Priority pr);

    void print_tex_name(FILE *code);

    void print_asm_code(FILE *code);

private:
    int type_un_op;
    Expression *arg_f;
};

class Operator_bin : public Expression {
public:
    Operator_bin(int, Expression *, Expression *);

    void print_dot(FILE *code);

    void print_dot_name(FILE *code, Priority pr);

    void print_asm_code(FILE *code);

private:
    int type_bin_op;
    Expression *arg_f;
    Expression *arg_s;
};


class Definition_fun : public Node {
public:
    Definition_fun(char *, int);

    void add_var(char *);

    void add_stmt(Statement *);

    Definition_var *find_var(char *);

    void print_asm_code(FILE *code);

    void print_dot(FILE *code);

    void print_dot_name(FILE *code);

    char* name();

    int am_var();

    int ret_par();

    int label_r();

private:
    char *name_of_fun;
    std::list<Definition_var *> vars;
    std::list<Statement *> stmts;
    int label;
};


class Root: public  Node {
public:
    Root();

    Definition_fun *find_fun(char*);

    void add_fun(Definition_fun *);

    void print_dot(FILE *code);

    void print_dot_name(FILE *code);

    void print_asm_code(FILE *code);

private:
    std::list<Definition_fun *> funs;
};


class Call : public Expression {
public:
    Call(Definition_fun *);

    void print_dot_name(FILE *code, Priority pr);

    void print_dot(FILE *code);

    void add_arg(Expression*);

    void print_asm_code (FILE *code);

private:
    Definition_fun *fun_def;
   std::list<Expression *> args;
};


extern Definition_fun *cur_fun;

extern Root *root;

void print_JPEG(Node *diff);

void skip_str();

Root *GetG0(const char *text);

Definition_fun *GetF();

Statement *GetS();

Relation *GetB();

Expression *GetE();

Expression *GetT();

Expression *GetD();

Expression *GetP();

Lex next();

void expect(Lex expected);

void error(const char *er);

void Compiler(Expression *tree);


void Code_generator(Root *tree);

#endif //LANGUAGE_LANGUAGE_H
