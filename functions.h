#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#define FLOAT 1
#define INT 2

enum mathsobject_types {
	NUM,
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	POW
};
typedef struct mathsobject mathsobject;
struct mathsobject {
	enum mathsobject_types type;
	long double value;
	mathsobject *leftoperand;
	mathsobject *rightoperand;
};

int str_replace(char* src, char from, char to);
mathsobject* init_operand();
mathsobject* parse_string(char* string);
void parse_error(const char* fmt, ...);
void release_tree(mathsobject* root);
long double calculate_tree(mathsobject* root);


int symbol_type(char c);
char* get_next_symbol(char *str);
int has_priority(enum mathsobject_types s1, enum mathsobject_types s2);
int numeric(char* str);
char* trim(char* str);
