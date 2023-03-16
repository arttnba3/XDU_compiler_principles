#include "message/message.h"
#include "scanner/scanner.h"
#include "parser/parser.h"
#include "semantic.h"
#include <iostream>
#include <cmath>

using namespace std;

double scale_x;
double scale_y;
double origin_x;
double origin_y;
double rot;

bool drawer_init = false;
FILE *draw_py_file;

void init_drawer(void)
{
    draw_py_file = fopen("./drawer.py", "w");
    if (!draw_py_file)
        err_exit("unable to create py file, draw failed.", nullptr, -EFAULT);
    drawer_init = true;

    fprintf(draw_py_file, "from tkinter import *\n");
    fprintf(draw_py_file, "tk = Tk()\n");
    fprintf(draw_py_file, "tk.title(\"A3DrawLang Interpreter\")\n");
    fprintf(draw_py_file, "canvas = Canvas(tk, width = 1200, height = 800)\n");
    fprintf(draw_py_file, "canvas.pack()\n");
}

void close_drawer(void)
{
    if (draw_py_file)
    {
        fprintf(draw_py_file, "input()\n");
        fclose(draw_py_file);
    }
}

void draw_pixel(double x, double y, double step)
{
    if (!drawer_init)
        init_drawer();

    if (fabs(x) != NAN && fabs(x) != INFINITY && fabs(y) != NAN && fabs(y) != INFINITY)
        fprintf(draw_py_file, "canvas.create_oval(%lf, %lf, %lf, %lf)\n", x, y, x, y);
}

double get_expr_val(struct expr_node *tree)
{
    double      right_val;

    switch (tree->opcode)
    {
        case PLUS:
                return get_expr_val(tree->content.case_operator.left) + get_expr_val(tree->content.case_operator.right);
        case MINUS:
                return get_expr_val(tree->content.case_operator.left) - get_expr_val(tree->content.case_operator.right);
        case MUL:
                return get_expr_val(tree->content.case_operator.left) * get_expr_val(tree->content.case_operator.right);
        case DIV:
                return get_expr_val(tree->content.case_operator.left) / get_expr_val(tree->content.case_operator.right);
        case POWER:
                return pow(get_expr_val(tree->content.case_operator.left), get_expr_val(tree->content.case_operator.right));
        case FUNC:
                return (tree->content.case_func.func_ptr)(get_expr_val(tree->content.case_func.child));
        case T:
                return *(tree->content.case_param);
        case CONST_ID:
                return tree->content.case_const;
        default:
                return 0.0;
    }
}

static inline void get_coordinate(struct expr_node *exp_x, struct expr_node *exp_y, double &x, double &y)
{
	double      coordinate_x;
    double      coordinate_y;
    double      tmp;

	coordinate_x = get_expr_val(exp_x);
    coordinate_y = get_expr_val(exp_y);
	coordinate_x *= scale_x;
	coordinate_y *= scale_y;
	tmp = coordinate_x * cos(rot) + coordinate_y *sin(rot);
	coordinate_y = coordinate_y * cos(rot) - coordinate_x *sin(rot);
	coordinate_x = tmp;
	coordinate_x += origin_x;
	coordinate_y += origin_y;

    // set return val
	x = coordinate_x;
	y = coordinate_y;
}

void draw_loop(struct token *param, double start, double end, double step, struct expr_node *exp_x, struct expr_node *exp_y)
{
    double      x;
    double      y;
    if (!drawer_init)
        init_drawer();

    for (param->val = start; param->val <= end; param->val += step)
    {
        get_coordinate(exp_x, exp_y, x, y);
        draw_pixel(x, y, step);
    }

}