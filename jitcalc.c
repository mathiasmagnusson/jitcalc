#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include "jitcalc.h"
#include "codegen.h"

void arithmetic_error(int signal) {
	fprintf(stderr, "ERR: Division by zero or other arithmetic error\n");
	exit(4);
}

void usage(char* program_name) {
	fprintf(stderr, "jitcalc - the JITting calculator\n\
\n\
Usage: %s [OPTIONS...]\n\
Where OPTIONS is any of:\n\
    -tokens              print all tokens read\n\
    -tree                print the expression in tree form\n\
    -no-eval             don't evaluate the expression\n\
    -h | -help           print this message\n\
\n\
Return codes:\n\
    0:                   no errors\n\
    1:                   usage shown / invalid command line arguments\n\
    2:                   errors parsing expression\n\
    3:                   errors generating maching code\n\
    4:                   errors evaluating expression e.g. division by zero\n\
\n\
Supported syntax:\n\
    decimal integers,\n\
    parenthesis,\n\
    unary operator: -\n\
    binary operators: + - * /\n\
\n\
All calculations are done as integers\n\
\n\
", program_name);
	exit(1);
}

int main(int argc, char** argv) {
	char line[1024];

	signal(SIGFPE, arithmetic_error);

	bool print_tokens = false;
	bool print_tree = false;
	bool do_eval = true;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-tokens") == 0)
			print_tokens = true;
		else if (strcmp(argv[i], "-tree") == 0)
			print_tree = true;
		else if (strcmp(argv[i], "-no-eval") == 0)
			do_eval = false;
		else if (strcmp(argv[i], "-h") == 0 ||
		         strcmp(argv[i], "-help") == 0)
			usage(argv[0]);
		else {
			fprintf(stderr, "Invalid argument %s\n", argv[i]);
			return 1;
		}
	}

	while (fgets(line, sizeof(line), stdin)) {
		Tokenizer tokenizer = new_tokenizer(line);

		if (print_tokens) {
			Tokenizer tmp = tokenizer;
			Token token;
			while ((token = next_token(&tmp)).kind != EOFToken)
				print_token(stderr, token);
			fprintf(stderr, "\n");
		}

		ParseResult res = parse_expr(&tokenizer, 0);
		if (!res.success) {
			if (res.error.expected_specific) {
				fprintf(stderr, "Parse error: Expected ");
				print_token_kind(stderr, res.error.expected);
				fprintf(stderr,
					" at %ld:%ld, got ",
					res.error.token.line, res.error.token.col
				);
				print_token(stderr, res.error.token);
				fprintf(stderr, "\n");
			} else {
				fprintf(stderr, "Parse error: Unexpected token ");
				print_token(stderr, res.error.token);
				fprintf(stderr,
					" at %ld:%ld",
					res.error.token.line, res.error.token.col
				);
				fprintf(stderr, "\n");
			}
			return 2;
		}
		Expression* expr = res.expr;
		if (print_tree) print_expr(stdout, expr, 0);

		if (do_eval) {
			Assembly assembly = gen_code(expr);
			if (!assembly.f) {
				fprintf(stderr, "Codegen errors, exiting\n");
				return 3;
			}

			s64 result = assembly.f();

			printf("%ld\n", result);

			free_assembly(assembly);
		}
		free_expr(expr);
	}
}
