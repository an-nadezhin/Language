#include "Language.h"


Definition_fun *cur_fun;

Root *root;

int main() {
    const char *buf1 = "if(a < b) {a = -a;\n} return a;";
    const char *buf2 = "while(a > 0) {a = a - 1; b = b + 1;} return b;";
    const char *buf3 = "double myfunc(double p, double i, double c) {while(p > 0) {p = p - 1; i = i + 1;} return i;}";
    const char *buf = "double myfunc(double p, double i, double c) {while(p > 0) {p = p - 1; i = i + 1;} return i;} "
            "double main(double a, double b, double c) {return myfunc(3, 6, 5);}";

    Root *node_1 = GetG0(buf);

    Code_generator(node_1);
    print_JPEG(node_1);
}


Lex next() {
    skip_str();
    switch (s[p++]) {
        case '{':
            return lex = L_LBRA;
        case '}':
            return lex = L_RBRA;
        case '=':
            if (s[p] == '=') {
                p++;
                return lex = L_EQ;
            } else
                return lex = L_ASSIGN;
        case '+':
            return lex = L_ADD;
        case '-':
            return lex = L_MINUS;
        case '/':
            return lex = L_DIV;
        case ';':
            return lex = L_SEMICOLON;
        case ',':
            return lex = L_COMMA;
        case '*':
            return lex = L_MUL;
        case '(':
            return lex = L_LPAR;
        case ')':
            return lex = L_RPAR;
        case '\0':
            p--;
            return lex = L_EOF;
        case '^':
            return lex = L_DEG;
        case '!':
            if (s[p] == '=') {
                p++;
                return lex = L_NE;
            } else
                return lex = L_ERROR;
        case '>':
            if (s[p] == '=') {
                p++;
                return lex = L_GE;
            } else
                return lex = L_GT;
        case '<':
            if (s[p] == '=') {
                p++;
                return lex = L_LE;
            } else
                return lex = L_LT;
    }

    p--;

    if (isdigit(s[p])) {
        int count = 0;
        char num_arr[BUF_LENGTH] = {0};
        while (isdigit(s[p]) || s[p] == '.') {
            assert(count < BUF_LENGTH - 1);
            num_arr[count++] = s[p++];
        }
        num_valG = atof(num_arr);
        return lex = L_CONST;
    }

    if (isalpha(s[p])) {
        int count = 0;
        while (isalpha(s[p]) || isdigit(s[p]) || s[p] == '_') {
            assert(count < BUF_LENGTH - 1);
            id_val[count++] = s[p++];
        }
        id_val[count] = '\0';

        if (!strcmp(id_val, "while"))
            return lex = L_WHILE;
        if (!strcmp(id_val, "if"))
            return lex = L_IF;
        if (!strcmp(id_val, "sin"))
            return lex = L_SIN;
        if (!strcmp(id_val, "cos"))
            return lex = L_COS;
        if (!strcmp(id_val, "ln"))
            return lex = L_LN;
        if (!strcmp(id_val, "sqrt"))
            return lex = L_SQRT;
        if (!strcmp(id_val, "return"))
            return lex = L_RETURN;
        if (!strcmp(id_val, "var"))
            return lex = L_VAR;
        if (!strcmp(id_val, "double"))
            return lex = L_DOUBLE;


        return lex = L_ID;
    }
    return lex = L_ERROR;
}

Root *GetG0(const char *text) {
    s = strdup(text);
    p = 0;
    next();
    root = new Root();
    while (lex != L_EOF)
        root->add_fun(GetF());
    assert(lex = L_EOF);
    return root;
}

Definition_fun *GetF() {
    expect(L_DOUBLE);
    if (lex != L_ID)
        error("argname expected");
    cur_fun = new Definition_fun(id_val, amount_of_labels++);
    next();
    expect(L_LPAR);
    expect(L_DOUBLE);
    if (lex != L_ID)
        error("argname expected");
    cur_fun->add_var(id_val);
    next();
    expect(L_COMMA);
    expect(L_DOUBLE);
    if (lex != L_ID)
        error("argname expected");
    cur_fun->add_var(id_val);
    next();
    expect(L_COMMA);
    expect(L_DOUBLE);
    if (lex != L_ID)
        error("argname expected");
    cur_fun->add_var(id_val);
    next();
    expect(L_RPAR);
    expect(L_LBRA);
    while (lex != L_RBRA)
        cur_fun->add_stmt(GetS());
    next();
    return cur_fun;
}

Statement *GetS() {
    Expression *val = nullptr;
    std::list<Statement *> stmt;
    Relation *val1;

    switch (lex) {
        case L_RETURN:
            next();
            val = GetE();
            expect(L_SEMICOLON);
            return new Return(val);
        case L_WHILE:
            next();
            expect(L_LPAR);
            val1 = GetB();
            expect(L_RPAR);
            expect(L_LBRA);
            while (lex != L_RBRA) {
                stmt.push_back(GetS());
            }
            expect(L_RBRA);
            return new While(val1, stmt);
        case L_IF:
            next();
            expect(L_LPAR);
            val1 = GetB();
            expect(L_RPAR);
            expect(L_LBRA);
            while (lex != L_RBRA) {
                stmt.push_back(GetS());
            }
            expect(L_RBRA);
            return new If(val1, stmt);
        case L_ID: {
            Definition_var *c = cur_fun->find_var(id_val);
            if (c == nullptr)
                error("no variable");
            next();
            expect(L_ASSIGN);
            val = GetE();
            expect(L_SEMICOLON);
            return new ASSIGN(c, val);
        }
        default:
            error("not implemented");
    }
}

Relation *GetB() {
    Expression *val = GetE();
    Expression *val2 = nullptr;
    Lex op = lex;
    switch (lex) {
        case L_EQ:
        case L_NE:
        case L_GE:
        case L_GT:
        case L_LE:
        case L_LT:
            break;
        default:
            error("unknow relation");
    }
    next();
    val2 = GetE();
    return new Relation(op, val, val2);
}

Expression *GetE() {
    Expression *val = nullptr;

    if (lex == L_MINUS) {
        next();
        val = GetT();
        val = new Operator_un(L_MINUS, val);
    } else {
        val = GetT();
    }

    while (lex == L_ADD || lex == L_MINUS) {
        Lex op = lex;
        next();
        Expression *val2 = GetT();
        val = new Operator_bin(op, val, val2);
    }
    return val;
}


Expression *GetT() {

    Expression *val = GetD();

    while (lex == L_MUL || lex == L_DIV) {
        Lex op = lex;
        next();
        Expression *val2 = GetD();
        val = new Operator_bin(op, val, val2);
    }
    return val;
}

Expression *GetD() {
    Expression *val = GetP();
    if (lex == L_DEG) {
        next();
        Expression *val2 = GetD();
        val = new Operator_bin(L_DEG, val, val2);
    }
    return val;
}

Expression *GetP() {
    Expression *val = nullptr;
    switch (lex) {
        case L_LPAR:
            next();
            val = GetE();
            expect(L_RPAR);
            if (lex == L_DEG) {
                next();
                Expression *val2 = GetE();
                val = new Operator_bin(L_DEG, val, val2);
            }
            return val;

        case L_CONST:
            val = new Number(num_valG);
            next();
            return val;
        case L_SIN:
            next();
            expect(L_LPAR);
            val = GetE();
            expect(L_RPAR);
            return new Operator_un(L_SIN, val);
        case L_COS:
            next();
            expect(L_LPAR);
            val = GetE();
            expect(L_RPAR);
            return new Operator_un(L_COS, val);
        case L_SQRT:
            next();
            expect(L_LPAR);
            val = GetE();
            expect(L_RPAR);
            return new Operator_un(L_SQRT, val);
        case L_LN:
            next();
            expect(L_LPAR);
            val = GetE();
            expect(L_RPAR);
            return new Operator_un(L_LN, val);
        case L_ID: {
            char name[BUF_LENGTH] = {0};
            strcpy(name, id_val);

            next();
            if (lex == L_LPAR) {
                next();
                Definition_fun * fun = root->find_fun(id_val);
                if (!fun)
                    error("unknown variable");
                val = GetE();
                expect(L_COMMA);
                Expression *val2 = GetE();
                expect(L_COMMA);
                Expression *val3 = GetE();
                expect(L_RPAR);
                val = new Call(fun, val, val2, val3);
            } else {
                Definition_var *var = cur_fun->find_var(id_val);
                if (!var)
                    error("unknown variable");
                val = new Variable(var);
            }
            return val;
        }
        default:
            error("invalid expression");
    }
}

Call::Call(Definition_fun *str, Expression *node, Expression *node1, Expression *node2) {
    fun_def = str;
    arg = node;
    arg2 = node1;
    arg3 = node2;
}

ASSIGN::ASSIGN(Definition_var *name, Expression *argumR) {
    var_def = name;
    arg_r = argumR;
}

int Definition_var::address() {
    return pos;
}

Relation::Relation(int type, Expression *node_1, Expression *node_2) {
    type_op = type;
    node1 = node_1;
    node2 = node_2;
}


Definition_var::Definition_var(char *name, int position) {
    name_of_var = strdup(name);
    pos = position;
}

Definition_fun::Definition_fun(char *name, int labels) {
    name_of_fun = strdup(name);
    label = labels;
}

void Definition_fun::add_var(char *name) {
    if (find_var(name))
        error("such var name is already used");

    Definition_var *var = new Definition_var(name, vars.size());
    vars.push_back(var);
}


Definition_var *Definition_fun::find_var(char *name) {
    std::list<Definition_var *>::const_iterator iterator;
    for (iterator = vars.begin(); iterator != vars.end(); ++iterator) {
        Definition_var *var = *iterator;
        if (!strcmp(var->name(), name))
            return var;
    }
    return nullptr;
}

void Definition_fun::add_stmt(Statement *stmt) {
    stmts.push_back(stmt);
}

Root::Root() {
}


Definition_fun *Root::find_fun(char *name) {
    std::list<Definition_fun *>::const_iterator iterator;
    for (iterator = funs.begin(); iterator != funs.end(); ++iterator) {
        Definition_fun *fun = *iterator;
        if (!strcmp(fun->name(), name))
            return fun;
    }
    return nullptr;
}


void Root::add_fun(Definition_fun * func) {
    if (find_fun(func->name()))
        error("such var name is already used");
    funs.push_back(func);
}


char *Definition_fun::name() {
    return name_of_fun;
}

void Definition_fun::print_dot(FILE *code) {

    std::list<Statement *>::const_iterator iterator;
    for (iterator = stmts.begin(); iterator != stmts.end(); ++iterator) {
        Statement *stmt = *iterator;
        fprintf(code, "\"");
        print_dot_name(code);
        fprintf(code, "\"->\"");
        stmt->print_dot_name(code);
        fprintf(code, "\"\n");
        stmt->print_dot(code);
    }

}

void Root::print_dot(FILE *code) {

    std::list<Definition_fun *>::const_iterator iterator;
    for (iterator = funs.begin(); iterator != funs.end(); ++iterator) {
        Definition_fun *fun = *iterator;
        fprintf(code, "\"");
        print_dot_name(code);
        fprintf(code, "\"->\"");
        fun->print_dot_name(code);
        fprintf(code, "\"\n");
        fun->print_dot(code);
    }
}

void Root::print_dot_name(FILE *code) {
    fprintf(code, "prog");
}


void Definition_fun::print_dot_name(FILE *code) {
    fprintf(code, "function %s", name_of_fun);
}

char *Definition_var::name() {
    return name_of_var;
}

void error(const char *er) {
    std::cout << er << std::endl;
    assert(0);
}

void expect(Lex expected) {
    assert(lex == expected);
    next();
}

void skip_str() {
    while (isspace(s[p]))
        p++;
}

Expression::Expression() {
}

void Expression::print_dot_name(FILE *code, Priority pr) {
}

void Expression::print_tex_name(FILE *code) {
}

void Operator_un::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code, PR_E);
    fprintf(code, "\"->\"");
    arg_f->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    arg_f->print_dot(code);
}

void Call::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code, PR_E);
    fprintf(code, "\"->\"");
    arg->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    arg->print_dot(code);

    fprintf(code, "\"");
    print_dot_name(code, PR_E);
    fprintf(code, "\"->\"");
    arg2->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    arg2->print_dot(code);

    fprintf(code, "\"");
    print_dot_name(code, PR_E);
    fprintf(code, "\"->\"");
    arg3->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    arg3->print_dot(code);
}





void Operator_bin::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code, PR_E);
    fprintf(code, "\"->\"");
    arg_f->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    arg_f->print_dot(code);

    fprintf(code, "\"");
    print_dot_name(code, PR_E);
    fprintf(code, "\"->\"");
    arg_s->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    arg_s->print_dot(code);
}

void While::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    arg->print_dot_name(code);
    fprintf(code, "\"\n");
    arg->print_dot(code);


    std::list<Statement *>::const_iterator iterator;
    for (iterator = stmts.begin(); iterator != stmts.end(); ++iterator) {
        Statement *stmt = *iterator;
        fprintf(code, "\"");
        print_dot_name(code);
        fprintf(code, "\"->\"");
        stmt->print_dot_name(code);
        fprintf(code, "\"\n");
        stmt->print_dot(code);
    }
}


void If::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    arg->print_dot_name(code);
    fprintf(code, "\"\n");
    arg->print_dot(code);

    std::list<Statement *>::const_iterator iterator;
    for (iterator = stmts.begin(); iterator != stmts.end(); ++iterator) {
        Statement *stmt = *iterator;
        fprintf(code, "\"");
        print_dot_name(code);
        fprintf(code, "\"->\"");
        stmt->print_dot_name(code);
        fprintf(code, "\"\n");
        stmt->print_dot(code);
    }
}

void Relation::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    node1->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    node1->print_dot(code);

    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    node2->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    node2->print_dot(code);
}

void Return::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    arg->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    arg->print_dot(code);
}

Node::Node() {
}

Statement::Statement() {
}

While::While(Relation *argum, std::list<Statement *> stm) : stmts(stm) {
    arg = argum;
}

If::If(Relation *argum, std::list<Statement *> stm) : stmts(stm) {
    arg = argum;
}

void Statement::print_dot(FILE *code) {
}


void Statement::print_dot_name(FILE *code) {
}

Return::Return(Expression *node1) {
    arg = node1;
}

void Expression::print_dot(FILE *code) {
}

Variable::Variable(Definition_var *val) {
    var_def = val;
}

Variable::~Variable() {
    free(var_def);
}

Number::Number(double val) {
    num_valN = val;
}

Operator_un::Operator_un(int val, Expression *arg) {
    assert(val == L_SIN || val == L_MINUS || val == L_COS || val == L_SQRT || val == L_LN);
    assert(arg != nullptr);
    type_un_op = val;
    arg_f = arg;
}

Operator_bin::Operator_bin(int val, Expression *arg_l, Expression *arg_r) {
    assert(val == L_MUL || val == L_DIV || val == L_DEG || val == L_ADD || val == L_MINUS);
    assert(arg_l != nullptr && arg_r != nullptr);
    type_bin_op = val;
    arg_f = arg_l;
    arg_s = arg_r;
}

void print_JPEG(Node *diff) {
    FILE *code = fopen("diff.dot", "w");
    fprintf(code, "digraph G{\n");
    diff->print_dot(code);
    fprintf(code, "}");
    fclose(code);
    system("dot -Tjpeg diff.dot -o diff.jpeg");
}

void Node::print_dot(FILE *code) {
}

void Call::print_dot_name(FILE *code, Priority pr) {
    fprintf(code, "%s", fun_def->name());
    fprintf(code, "(");
    arg->print_dot_name(code, PR_E);
    fprintf(code, ",");
    arg2->print_dot_name(code, PR_E);
    fprintf(code, ",");
    arg3->print_dot_name(code, PR_E);
    fprintf(code, ")");

}

int Definition_fun::label_r() {
    return label;
}

void ASSIGN::print_dot(FILE *code) {
    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    fprintf(code, "%s", var_def->name());
    fprintf(code, "\"\n");
    fprintf(code, "\"");
    print_dot_name(code);
    fprintf(code, "\"->\"");
    arg_r->print_dot_name(code, PR_E);
    fprintf(code, "\"\n");
    arg_r->print_dot(code);
}

void ASSIGN::print_dot_name(FILE *code) {
    fprintf(code, "%s = ", var_def->name());
    arg_r->print_dot_name(code, PR_E);
    fprintf(code, ";");
}

void Operator_un::print_dot_name(FILE *code, Priority pr) {

    if (type_un_op == L_MINUS) {
        fprintf(code, "-");
        if (pr > PR_E)
            fprintf(code, "(");
        arg_f->print_dot_name(code, PR_D);
        if (pr > PR_E)
            fprintf(code, ")");
        return;
    }

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
    fprintf(code, "(");
    arg_f->print_dot_name(code, PR_E);
    fprintf(code, ")");
}

void Return::print_dot_name(FILE *code) {
    fprintf(code, "return ");
    arg->print_dot_name(code, PR_E);
    fprintf(code, ";");
}

void Operator_un::print_tex_name(FILE *code) {

    switch (type_un_op) {
        case L_SIN :
            fprintf(code, "\\sin");
            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            break;
        case L_COS :
            fprintf(code, "\\cos");
            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            break;
        case L_SQRT :
            fprintf(code, "\\sqrt{");
            arg_f->print_tex_name(code);
            fprintf(code, "}");
            break;

        case L_LN:
            fprintf(code, "\\ln");
            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            break;
        case L_MINUS :
            fprintf(code, "-");
            fprintf(code, "(");
            arg_f->print_tex_name(code);
            fprintf(code, ")");
            break;

    }

}

void Variable::print_dot_name(FILE *code, Priority pr) {
    fprintf(code, "%s", var_def->name());
}

void Variable::print_tex_name(FILE *code) {
    fprintf(code, "%s", var_def);
}

void Relation::print_dot_name(FILE *code) {
    node1->print_dot_name(code, PR_E);
    switch (type_op) {
        case L_EQ:
            fprintf(code, " == ");
            break;
        case L_NE:
            fprintf(code, " != ");
            break;
        case L_GE:
            fprintf(code, " >= ");
            break;
        case L_GT:
            fprintf(code, " > ");
            break;
        case L_LE:
            fprintf(code, " <= ");
            break;
        case L_LT:
            fprintf(code, " < ");
            break;
    }
    node2->print_dot_name(code, PR_E);
}

void Operator_bin::print_dot_name(FILE *code, Priority pr) {

    switch (type_bin_op) {
        case L_ADD :
            if (pr > PR_E)
                fprintf(code, "(");
            arg_f->print_dot_name(code, PR_E);
            fprintf(code, " ");
            fprintf(code, "+");
            fprintf(code, " ");
            arg_s->print_dot_name(code, PR_T);
            if (pr > PR_E)
                fprintf(code, ")");
            break;
        case L_MINUS :
            if (pr > PR_E)
                fprintf(code, "(");
            arg_f->print_dot_name(code, PR_E);
            fprintf(code, " ");
            fprintf(code, "-");
            fprintf(code, " ");
            arg_s->print_dot_name(code, PR_T);
            if (pr > PR_E)
                fprintf(code, ")");
            break;
        case L_MUL:
            if (pr > PR_T)
                fprintf(code, "(");
            arg_f->print_dot_name(code, PR_T);
            fprintf(code, "*");
            arg_s->print_dot_name(code, PR_D);
            if (pr > PR_T)
                fprintf(code, ")");
            break;
        case L_DIV :
            if (pr > PR_T)
                fprintf(code, "(");
            arg_f->print_dot_name(code, PR_T);
            fprintf(code, "/");
            arg_s->print_dot_name(code, PR_D);
            if (pr > PR_T)
                fprintf(code, ")");
            break;
        case L_DEG:
            if (pr > PR_D)
                fprintf(code, "(");
            arg_f->print_dot_name(code, PR_P);
            fprintf(code, "^");
            arg_s->print_dot_name(code, PR_D);
            if (pr > PR_D)
                fprintf(code, ")");
            break;
    }
}

void While::print_dot_name(FILE *code) {
    fprintf(code, "while(");
    arg->print_dot_name(code);
    fprintf(code, ") \n{\n");

    std::list<Statement *>::const_iterator iterator;
    for (iterator = stmts.begin(); iterator != stmts.end(); ++iterator) {
        Statement *stmt = *iterator;
        stmt->print_dot_name(code);
    }
    fprintf(code, "}");
}

void If::print_dot_name(FILE *code) {
    fprintf(code, "if(");
    arg->print_dot_name(code);
    fprintf(code, ") \n{\n");

    std::list<Statement *>::const_iterator iterator;
    for (iterator = stmts.begin(); iterator != stmts.end(); ++iterator) {
        Statement *stmt = *iterator;
        stmt->print_dot_name(code);
    }
    fprintf(code, "}");
}

void Number::print_dot_name(FILE *code, Priority pr) {
    fprintf(code, "%g", num_valN);
}

void Number::print_tex_name(FILE *code) {
    fprintf(code, "%g", num_valN);
}

double Number::num() {
    return num_valN;
}

