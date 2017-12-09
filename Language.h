//
// Created by andrew on 09.12.17.
//

#ifndef LANGUAGE_LANGUAGE_H
#define LANGUAGE_LANGUAGE_H

#include <cassert>
#include <iostream>
#include <cstring>

const char* s = NULL;

int p = 0;

const int BUF_LENGTH = 50;

const int ERROR = -1;

enum UnOp {
    SIN, COS, SQRT, LN, UN_MINUS
};

enum BinOp {
ADD, MUL, SUB, DEG, EQUAL, IF, CMP, DIV,
};



class Expression {
public:
    Expression();
    virtual void print_dot(FILE *code);
    virtual void print_dot_name(FILE *code);
    virtual void print_tex_name(FILE *code);
};

class Variable : public Expression {
public:
    Variable(char *);
    void print_dot_name(FILE *code);
    void print_tex_name(FILE *code);
    ~Variable();
private:
    char *var_name;
};

class Number : public Expression {
public:
    Number(double);
    void print_dot_name(FILE *code);
    void print_tex_name(FILE *code);
    double num();
private:
    double num_val;
};

class Operator_un : public Expression {
public:
    Operator_un(int, Expression *);
    void print_dot(FILE *code);
    void print_dot_name(FILE *code);
    void print_tex_name(FILE *code);
private:
    int type_un_op;
    Expression *arg_f;
};

class Operator_bin : public Expression {
public:
    Operator_bin(int, Expression *, Expression *);
    void print_dot(FILE* code);
    void print_dot_name(FILE *code);
    void print_tex_name(FILE *code);
private:
    int type_bin_op;
    Expression *arg_f;
    Expression *arg_s;
};

void print_JPEG(Expression *diff);

int compare_str_bin(char value);

int compare_str_un(char *value);

void skip_str();

Expression *GetG0(const char *text);

Expression *GetN();

Expression *GetE();

Expression *GetId();

Expression *GetT();

Expression *GetP();

Expression *GetOp();

void texmaker(Expression *diff_1, Expression *diff_2);





#endif //LANGUAGE_LANGUAGE_H
