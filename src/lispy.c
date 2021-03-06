/*
 * Lispy
 *
 * https://github.com/aouelete/lispy
 *
 * A very simple lisp implementation
 *
 * MIT License
 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <editline/readline.h>
#include "mpc.h"
#include "lvalue.h"
#include "eval.h"

/* Is the string all blank */
bool is_blank(char *string);

int main(int argc, char *argv[])
{
	// create parsers
	mpc_parser_t *Number  = mpc_new("number");
	mpc_parser_t *Symbol  = mpc_new("symbol");
	mpc_parser_t *Sexpr   = mpc_new("sexpr");
	mpc_parser_t *Qexpr   = mpc_new("qexpr");
	mpc_parser_t *Expr    = mpc_new("expr");
	mpc_parser_t *Lispy   = mpc_new("lispy");

	// define language
	mpca_lang(MPCA_LANG_DEFAULT,
	"                                                             \
	number : /-?[0-9]+\\.?[0-9]*/ ;                               \
	symbol : /[a-zA-Z0-9_+\\-*%^\\/\\\\=<>!&]+/ ;                   \
	sexpr  : '(' <expr>* ')' ;                                    \
	qexpr  : '{' <expr>* '}' ;                                    \
	expr   : <number> | <symbol> | <sexpr> | <qexpr> ;            \
	lispy  : /^/ <expr>* /$/ ;                                    \
	", Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

	puts("Lispy version 0.0.0.2");
	puts("Type \'exit\' to exit\n");

	// create new environment
	lenv *e = lenv_new();
	lenv_add_builtins(e);

	// Read, Evaluate, Print loop
	while (1) {

		// display prompt and get input
		char *input = readline("lispy> ");
		if (strcmp(input, "exit") == 0) {
			free(input);
			break;
		}
		if (is_blank(input)) {
			continue;
		}

		// add input to history
		add_history(input);

		// parse input
		mpc_result_t r;
		if (mpc_parse("<stdin>", input, Lispy, &r)) {
			// on success evaluate and print
			lval *x = lval_eval(e, lval_read(r.output));
			lval_println(x);
			lval_del(x);
			mpc_ast_delete(r.output);
		} else {
			// otherwise print error
			mpc_err_print(r.error);
			mpc_err_delete(r.error);
		}

		free(input);
	}

	// cleanup
	lenv_del(e);
	mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);
	return 0;
}


bool is_blank(char *string)
{
	int length = strlen(string);
	for (int i = 0; i < length; i++) {
		if (!isblank(string[i])) {
			return false;
		}
	}
	return true;
}

