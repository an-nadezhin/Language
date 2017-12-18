//
// Created by andrew on 15.12.17.
//

#include "Language.h"

//const int BUF_LENGTH = 50;

const char *s = NULL;

int p = 0;

double num_valG = 0;

char id_val[BUF_LENGTH] = {0};

Lex lex;

int amount_of_labels = 0;

void Code_generator(Root *tree) {

    FILE * asm_code = fopen ("asm_code.asm", "w");

    tree->print_asm_code(asm_code);
}

void Number::print_asm_code(FILE * asm_code) {
    fprintf(asm_code, "push %lg\n", num_valN);
}


void Expression::print_asm_code(FILE *code) {
}


void Variable::print_asm_code(FILE *code) {
    fprintf(code, "push_%cx\n", var_def->address() + 'a');
}

void Operator_bin::print_asm_code(FILE *code) {
    arg_f->print_asm_code(code);
    arg_s->print_asm_code(code);
    switch (type_bin_op) {
        case L_ADD:
            fprintf(code, "add\n");
            break;
        case L_MINUS:
            fprintf(code, "sub\n");
            break;
        case L_DIV:
            fprintf(code, "div\n");
            break;
        case L_MUL:
            fprintf(code, "mul\n");
            break;
        case L_DEG:
            fprintf(code, "pow\n");
            break;
    }

}

void Operator_un::print_asm_code(FILE *code) {
    if (type_un_op == L_MINUS) {
        fprintf(code, "push 0\n");
        arg_f->print_asm_code(code);
        fprintf(code, "sub\n");
        return;
    }

    arg_f->print_asm_code(code);
    switch (type_un_op) {
        case L_SIN :
            fprintf(code, "sin");
            break;
        case L_COS :
            fprintf(code, "cos");
            break;
        case L_SQRT :
            fprintf(code, "sqrt");
            break;
        case L_LN:
            fprintf(code, "ln");
        break;
    }
}

void Return::print_asm_code(FILE *code) {
    arg->print_asm_code(code);
    fprintf(code, "ret\n");
}


void Statement::print_asm_code(FILE *code){
    assert(0);
}


void ASSIGN::print_asm_code(FILE *code) {
    arg_r->print_asm_code(code);
    fprintf(code, "pop_%cx\n", var_def->address() + 'a');
}


void Definition_fun::print_asm_code(FILE *code) {
    fprintf(code, "%d:\npop_cx\npop_bx\npop_ax\n", label);
    std::list<Statement *>::const_iterator iterator;
    for (iterator = stmts.begin(); iterator != stmts.end(); ++iterator) {
        Statement *stmt = *iterator;
        stmt->print_asm_code(code);
    }
}


int Relation::print_asm_code(FILE *code) {
    node1->print_asm_code(code);
    node2->print_asm_code(code);
    switch (type_op) {
        case L_NE:
            fprintf(code, "jne %d$\n", amount_of_labels);
            break;
        case L_EQ:
            fprintf(code, "je %d$\n", amount_of_labels);
            break;
        case L_GE:
            fprintf(code, "jb %d$\n", amount_of_labels);
            break;
        case L_GT:
            fprintf(code, "jbe %d$\n", amount_of_labels);
            break;
        case L_LE:
            fprintf(code, "ja %d$\n", amount_of_labels);
            break;
        case L_LT:
            fprintf(code, "jae %d$\n", amount_of_labels);
            break;
    }
    return amount_of_labels++;
}


void If::print_asm_code(FILE *code) {
    int label = arg->print_asm_code(code);
    std::list<Statement *>::const_iterator iterator;
    for (iterator = stmts.begin(); iterator != stmts.end(); ++iterator) {
        Statement *stmt = *iterator;
        stmt->print_asm_code(code);
    }
    fprintf(code, "%d:\n", label);

}

void While::print_asm_code(FILE *code) {
    int label1 = amount_of_labels;
    fprintf(code, "%d:\n", label1);
    amount_of_labels++;
    int label2 = arg->print_asm_code(code);
    std::list<Statement *>::const_iterator iterator;
    for (iterator = stmts.begin(); iterator != stmts.end(); ++iterator) {
        Statement *stmt = *iterator;
        stmt->print_asm_code(code);
    }
    fprintf(code, "jmp %d$\n", label1);
    fprintf(code, "%d:\n", label2);
}

void Root::print_asm_code(FILE *code) {
    fprintf(code, "push_ax\npush_bx\npush_cx\ncall %d$\nout\nhalt\n", funs.size() - 1);
    std::list<Definition_fun *>::const_iterator iterator;
    for (iterator = funs.begin(); iterator != funs.end(); ++iterator) {
        Definition_fun *fun = *iterator;
        fprintf(code, "\n");
        fun->print_asm_code(code);
    }

}

void Call::print_asm_code (FILE *code) {
    arg->print_asm_code(code);
    arg2->print_asm_code(code);
    arg3->print_asm_code(code);
    fprintf(code, "call %d$\n", fun_def->label_r());
}