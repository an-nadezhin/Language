
#include "Language.h"

int main() {
    char *buf = "if(n == 3) { if(n == 4) { 3 + 7} }";

    Expression *node_1 = GetG0(buf);
    print_JPEG(node_1);
}


void texmaker(Expression *diff_1, Expression *diff_2) {
    FILE *texf = fopen("expression.tex", "w");
    fprintf(texf, "\\documentclass[a4paper, 12pt]{article}\n"
            "\\usepackage{amsmath, amsfonts,, amssymb, amsthm, mathtools}\n"
            "\\begin{document}\n");
    fprintf(texf, "Expression :\n$");
    diff_1->print_tex_name(texf);
    fprintf(texf, "$\n\n\nDerivative :\n$");
    diff_2->print_tex_name(texf);
    fprintf(texf, "$\n");
    fprintf(texf, "\\end{document}");
    fclose(texf);
    system("pdflatex expression.tex");
    system("xdg-open expression.pdf");
}


Expression *GetG0(const char *text) {
    s = strdup(text);
    p = 0;
    Expression *val = GetOp();
    return val;
}

Expression *GetE() {
    Expression *val = GetT();
    skip_str();
    while (s[p] == '+' || s[p] == '-') {
        int op = compare_str_bin(s[p]);
        p++;
        Expression *val2 = GetT();
        val = new Operator_bin(op, val, val2);
        skip_str();
    }
    return val;
}

Expression *GetOp() {
    int counter = 0;
    char value[BUF_LENGTH] = {0};

    skip_str();

    Expression *val = GetE();

    skip_str();

    while (isalpha(s[p]) || s[p] == '=' || s[p] == '{') {
        value[counter++] = s[p++];
    }

    skip_str();

    if (!strcmp(value, "=")) {
        skip_str();
        Expression *val2 = GetE();
        return new Operator_bin(EQUAL, val, val2);
    }
    if (!strcmp(value, "==")) {
        skip_str();
        Expression *val2 = GetE();
        return new Operator_bin(CMP, val, val2);
    }
    if (!strcmp(value, "{")) {
        skip_str();
        Expression *val2 = GetOp();
        return val2;
    }

    return val;
}


Expression *GetT() {
    Expression *val = GetP();

    skip_str();

    while (s[p] == '*' || s[p] == '/') {
        int op = compare_str_bin(s[p]);
        p++;

        Expression *val2 = GetP();
        val = new Operator_bin(op, val, val2);
        skip_str();
    }
    return val;
}

Expression *GetP() {
    skip_str();
    if (s[p] == '(') {
        p++;
        Expression *val = GetE();
        skip_str();
        assert(s[p == ')']);
        p++;
        if (s[p] == '^') {
            p++;
            Expression *val2 = GetE();
            val = new Operator_bin(DEG, val, val2);
        }
        return val;
    }
    int prev_p = p;
    Expression *val = GetN();
    if (prev_p != p)
        return val;
    else
        return GetId();
}


Expression *GetN() {
    int val = 0;
    while ('0' <= s[p] && s[p] <= '9') {
        val = val * 10 + s[p] - '0';
        p++;
    }
    return new Number(val);
}


Expression *GetId() {
    int counter = 0;
    char value[BUF_LENGTH] = {0};

    skip_str();
    while (isalpha(s[p])) {
        value[counter++] = s[p++];
    }
    if (!strcmp(value, "sin")) {
        Expression *val = GetP();
        return new Operator_un(SIN, val);
    }
    if (!strcmp(value, "cos")) {
        Expression *val = GetP();
        return new Operator_un(COS, val);
    }
    if (!strcmp(value, "sqrt")) {
        Expression *val = GetP();
        return new Operator_un(SQRT, val);
    }
    if (!strcmp(value, "neg")) {
        Expression *val = GetP();
        return new Operator_un(UN_MINUS, val);
    }
    if (!strcmp(value, "ln")) {
        Expression *val = GetP();
        return new Operator_un(LN, val);
    }
    if (!strcmp(value, "if")) {
        p++;
        skip_str();
        Expression *val = GetOp();
        p++;
        skip_str();
        Expression *val2 = GetOp();
        return new Operator_bin(IF, val, val2);
    }
    return new Variable(value);
}


void skip_str() {
    while (isspace(s[p]))
        p++;
}


int compare_str_bin(char value) {

    if (value == '+') return ADD;
    if (value == '-') return SUB;
    if (value == '*') return MUL;
    if (value == '/') return DIV;
    return ERROR;

}

int compare_str_un(char *value) {

    if (!strcmp(value, "sin")) return SIN;
    if (!strcmp(value, "cos")) return COS;
    if (!strcmp(value, "sqrt")) return SQRT;
    if (!strcmp(value, "neg")) return UN_MINUS;
    return ERROR;
}


Expression::Expression() {
}


void Expression::print_dot_name(FILE *code) {
}

void Expression::print_tex_name(FILE *code) {
}

void Operator_un::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    arg_f->print_dot_name(code);
    fprintf(code, "\"\n");
    arg_f->print_dot(code);
}


void Operator_bin::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    arg_f->print_dot_name(code);
    fprintf(code, "\"\n");
    arg_f->print_dot(code);

    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    arg_s->print_dot_name(code);
    fprintf(code, "\"\n");
    arg_s->print_dot(code);
}


void Expression::print_dot(FILE *code) {
}

Variable::Variable(char *val) {
    var_name = strdup(val);
}

Variable::~Variable() {
    free(var_name);
}

Number::Number(double val) {
    num_val = val;
}

Operator_un::Operator_un(int val, Expression *arg) {
    assert(val >= SIN && val <= UN_MINUS);
    assert(arg != nullptr);
    type_un_op = val;
    arg_f = arg;
}

Operator_bin::Operator_bin(int val, Expression *arg_l, Expression *arg_r) {
    assert(val >= ADD && val <= DIV);
    assert(arg_l != nullptr && arg_r != nullptr);
    type_bin_op = val;
    arg_f = arg_l;
    arg_s = arg_r;
}

void print_JPEG(Expression *diff) {
    FILE *code = fopen("diff.dot", "w");
    fprintf(code, "digraph G{\n");
    diff->print_dot(code);
    fprintf(code, "}");
    fclose(code);
    system("dot -Tjpeg diff.dot -o diff.jpeg");
}


void Operator_un::print_dot_name(FILE *code) {
    fprintf(code, "(");
    switch (type_un_op) {
        case SIN :
            fprintf(code, "sin");
            break;
        case COS :
            fprintf(code, "cos");
            break;
        case SQRT :
            fprintf(code, "sqrt");
            break;
        case UN_MINUS :
            fprintf(code, "minus");
            break;
        case LN:
            fprintf(code, "ln");
            break;
    }
    fprintf(code, " ");
    arg_f->print_dot_name(code);
    fprintf(code, ")");
}

void Operator_un::print_tex_name(FILE *code) {

    switch (type_un_op) {
        case SIN :
            fprintf(code, "\\sin");
            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            break;
        case COS :
            fprintf(code, "\\cos");
            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            break;
        case SQRT :
            fprintf(code, "\\sqrt{");
            arg_f->print_tex_name(code);
            fprintf(code, "}");
            break;
        case UN_MINUS :
            fprintf(code, "-");
            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            break;
        case LN:
            fprintf(code, "\\ln");
            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            break;
    }

}


void Variable::print_dot_name(FILE *code) {
    fprintf(code, "%s", var_name);
}

void Variable::print_tex_name(FILE *code) {
    fprintf(code, "%s", var_name);
}


void Operator_bin::print_tex_name(FILE *code) {
    switch (type_bin_op) {
        case ADD :
            arg_f->print_tex_name(code);
            fprintf(code, " ");
            fprintf(code, "+");
            fprintf(code, " ");
            arg_s->print_tex_name(code);
            break;
        case SUB :
            arg_f->print_tex_name(code);
            fprintf(code, " ");
            fprintf(code, "-");
            fprintf(code, " ");
            arg_s->print_tex_name(code);
            break;
        case MUL:

            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            fprintf(code, "\\cdot ");
            //   fprintf(code, "(");
            arg_s->print_tex_name(code);
            //     fprintf(code, ")");

            break;
        case DIV :

            fprintf(code, "\\frac{");
            arg_f->print_tex_name(code);
            fprintf(code, "}{");
            arg_s->print_tex_name(code);
            fprintf(code, "}");

            break;
        case DEG:

            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            fprintf(code, "^");
            fprintf(code, "{");
            arg_s->print_tex_name(code);
            fprintf(code, "}");
            break;
    }

}


void Operator_bin::print_dot_name(FILE *code) {

    switch (type_bin_op) {
        case ADD :
            arg_f->print_dot_name(code);
            fprintf(code, " ");
            fprintf(code, "+");
            fprintf(code, " ");
            arg_s->print_dot_name(code);
            break;
        case SUB :
            arg_f->print_dot_name(code);
            fprintf(code, " ");
            fprintf(code, "-");
            fprintf(code, " ");
            arg_s->print_dot_name(code);
            break;
        case MUL:
            fprintf(code, "(");
            arg_f->print_dot_name(code);
            fprintf(code, ")");
            fprintf(code, "*");
            fprintf(code, "(");
            arg_s->print_dot_name(code);
            fprintf(code, ")");
            break;
        case DIV :
            fprintf(code, "(");
            arg_f->print_dot_name(code);
            fprintf(code, ")");
            fprintf(code, "/");
            fprintf(code, "(");
            arg_s->print_dot_name(code);
            fprintf(code, ")");
            break;
        case DEG:
            fprintf(code, "(");
            arg_f->print_dot_name(code);
            fprintf(code, ")");
            fprintf(code, "^");
            fprintf(code, "(");
            arg_s->print_dot_name(code);
            fprintf(code, ")");
            break;
        case EQUAL:
            fprintf(code, " ");
            arg_f->print_dot_name(code);
            fprintf(code, " = ");
            arg_s->print_dot_name(code);
            fprintf(code, ";");
        case IF:
            fprintf(code, "if(");
            arg_f->print_dot_name(code);
            fprintf(code, ")");
            fprintf(code, " {");
            arg_s->print_dot_name(code);
            fprintf(code, "}");
            break;
        case CMP:
            arg_f->print_dot_name(code);
            fprintf(code, " == ");
            arg_s->print_dot_name(code);
            break;
    }
}

void Number::print_dot_name(FILE *code) {
    fprintf(code, "%g", num_val);
}

void Number::print_tex_name(FILE *code) {
    fprintf(code, "%g", num_val);
}

double Number::num() {
    return num_val;
}

