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
    -tokens              print all tokens read (implies -no-eval and not -tree)\n\
    -tree                print the expression in tree form\n\
    -no-eval             don't evaluate the expression\n\
    -h, -help            print this message\n\
\n\
Return codes:\n\
    0:                   no errors\n\
    1:                   usage shown / invalid command line arguments\n\
    2:                   errors parsing expression\n\
    3:                   errors generating maching code\n\
    4:                   errors evaluating expression e.g. division by zero\n\
    -1 (255):            bug detected, please open an issue with given input\n\
\n\
Supported syntax:\n\
    - decimal integers\n\
    - decimal floating point numbers\n\
    - parenthesis\n\
    - unary operator: -\n\
    - binary operators: + - * /\n\
\n\
All calculations are done as 64 bit signed integers unless at least one number\n\
is a floating point value, in which case all calculations are done as 64 bit\n\
(double precision) floating point numbers.\n\
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

	u64 ptr = 0;
	while (fgets(line + ptr, sizeof line - ptr, stdin)) {
		Tokenizer tokenizer = new_tokenizer(line);

		if (print_tokens) {
			Tokenizer tmp = tokenizer;
			Token token;
			while ((token = next_token(&tmp)).kind != EOFToken)
				print_token(stderr, token);
			fprintf(stderr, "\n");
			ptr = 0;
			continue;
		}

		ParseResult res = parse_expr(&tokenizer);
		if (!res.success) {
			if (res.error.token.kind == EOFToken) {
				ptr = strlen(line) - 1;
				continue;
			} else if (res.error.expected_specific) {
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

		if (do_eval && res.value_kind == IntegerValue) {
			AssemblyInt assembly = gen_code_int(expr);
			if (!assembly.len) {
				fprintf(stderr, "Codegen errors, exiting\n");
				return 3;
			}

			s64 result = assembly.func();

			printf("%ld\n", result);

			free_assembly_int(assembly);
		} else if (do_eval && res.value_kind == FloatingValue) {
			AssemblyFloat assembly = gen_code_float(expr);
			if (!assembly.len) {
				fprintf(stderr, "Codegen errors, exiting\n");
				return 3;
			}

			double result = assembly.func();

			printf("%lg\n", result);

			free_assembly_float(assembly);
		} else if (do_eval) {
			// Unreachable code
			exit(-1);
		}

		free_expr(expr);

		ptr = 0;
	}
}
