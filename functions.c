#include "functions.h"

char symbols[] = {'+','-','*','/','%','^'};
int str_replace(char* src, char from, char to){
	int i = 0, n = 0;
	while( src[i] != '\0' ){
		if( src[i] == from )
			src[i] = to, ++n;
		++i;
	}
	return n;
}

mathsobject* init_operand() {
	mathsobject* operand = malloc(sizeof(mathsobject));
	operand->type = NUM;
	operand->value = 0;
	operand->leftoperand = NULL;
	operand->rightoperand = NULL;
	return operand;
}
int symbol_type(char c){
	int i = 0;
	while( symbols[i] != '\0' ){
		if( c == symbols[i] )
			return i+1;
		++i;
	}
	return 0;
}
void parse_error(const char* fmt, ...){
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "Parse error: ");
	vfprintf(stderr, fmt, args);
	if( fmt[strlen(fmt)-1] != '\n' )
		fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}
char* get_next_symbol(char *str){
	unsigned int parentheses = 0;
	int i = 0;
	while( str[i] != '\0' ){
		enum mathsobject_types symbol = symbol_type(str[i]);

		if( str[i] == '(' )
			++parentheses;
		else if( str[i] == ')' )
			--parentheses;
		else if( !parentheses && symbol )
			return &str[i];
		++i;
	}
	return &str[i];
}
int has_priority(enum mathsobject_types s1, enum mathsobject_types s2){
	if( s1 == s2 )
		return s1 != POW && s1 != ADD && s1 != SUB;
	return s1 > s2;
}
int symbol_priority(enum mathsobject_types symbol){
	if( symbol == ADD || symbol == SUB )
		return 10;
	else if( symbol == DIV || symbol == MUL || symbol == MOD )
		return 20;
	else if( symbol == POW )
		return 30;
	return 0;
}
char* trim(char* str){
	while( str[0] == ' ' )
		str = &str[1];
	int len = strlen(str);
	while( len > 0 && str[len-1] == ' ' ){
		str[len-1] = '\0';
		--len;
	}
	return str;
}
mathsobject* parse_string(char* str) {
	mathsobject* root_op = init_operand();

	unsigned int min_symbol = 0;
	char *min_symbol_p = NULL;
	unsigned int parentheses = 0, opening_parentheses = 0;
	int i = -1;
	str = trim(str);
#if DEBUG
	printf("Parsing: %s\n", str);
#endif
	while( str[++i] != '\0' ){
		enum mathsobject_types symbol = symbol_type(str[i]);
		if( str[i] == '(' )
			++parentheses, ++opening_parentheses;
		else if( !parentheses && symbol ){
			if( str[i+1] == '\0' ){
				if( symbol == ADD )
					continue;
				parse_error("Operand ending with a symbol (%c)...", str[i]);
			}
			else if( i == 0 ){

				if( symbol != SUB && symbol != ADD )
					parse_error("Operand starting with symbol (%c)...", str[i]);

				root_op->type = symbol;
				root_op->leftoperand = init_operand();
				root_op->leftoperand->type = NUM;
				root_op->leftoperand->value = 0;
				root_op->rightoperand = parse_string(&str[i+1]);
				return root_op;
			}
			else {
				if( !min_symbol || has_priority(min_symbol, symbol) ){
					min_symbol = symbol;
					min_symbol_p = &str[i];
				}
				continue;
			}
		}
		else if( parentheses && str[i] == ')' ){
			--parentheses;
			if( str[i+1] == '\0' && opening_parentheses == 1 ){
				if( str[i-1] == '(' )
					parse_error("Empty parenthses...");
				if( *str == '(' ){
					root_op->type = ADD;
					root_op->leftoperand = init_operand();
					root_op->leftoperand->type = NUM;
					root_op->leftoperand->value = 0;
					str[i] = '\0';
					root_op->rightoperand = parse_string(&str[1]);
					str[i] = ')';
					return root_op;
				}
			}
		}
		else if( str[i] == ')' ){
			parse_error("Wrong parentheses count...");
		}
	}
	if( min_symbol && min_symbol_p ){
		root_op->type = min_symbol;
		char s = *min_symbol_p;
		*min_symbol_p = '\0';
		root_op->leftoperand = parse_string(str);
		*min_symbol_p = s;
		root_op->rightoperand = parse_string(&min_symbol_p[1]);
		return root_op;
	}
	else if( !opening_parentheses ){
		int is_numeric = numeric(str);
		if( is_numeric == FLOAT ){
			root_op->type = NUM;
			sscanf(str, "%Lf", &root_op->value);
			return root_op;
		}
		else if( is_numeric == INT ){
			unsigned long long num;
			root_op->type = NUM;
			sscanf(str, "%llu", &num);
			root_op->value = num;
			return root_op;
		}
		else
			parse_error("%s = NaN", str);
	}

	release_tree(root_op);
	parse_error("Unknow error...");

	return NULL;
}
void release_tree(mathsobject* root) {
	if( root->leftoperand )
		release_tree(root->leftoperand);
	if( root->rightoperand )
		release_tree(root->rightoperand);
	free(root);
}
long double calculate_tree(mathsobject* root_op) {

	if( root_op->type != NUM && (root_op->leftoperand == NULL || root_op->rightoperand == NULL ) )
		parse_error("An operand is empty for non numeric type...");

	long double right_op, left_op;
	if( root_op->leftoperand )
		left_op = calculate_tree(root_op->leftoperand);
	if( root_op->rightoperand )
		right_op = calculate_tree(root_op->rightoperand);
	switch( root_op->type ){
		case NUM:
			return root_op->value;
		break;
		case ADD:
#if DEBUG
			printf("%Lf + %Lf = %Lf\n", left_op, right_op, left_op+right_op);
#endif
			return left_op + right_op;
		break;
		case SUB:
#if DEBUG
			printf("%Lf - %Lf = %Lf\n", left_op, right_op, left_op-right_op);
#endif
			return left_op - right_op;
		break;
		case MUL:
#if DEBUG
			printf("%Lf * %Lf = %Lf\n", left_op, right_op, left_op*right_op);
#endif
			return left_op * right_op;
		break;
		case DIV:
#if DEBUG
			printf("%Lf / %Lf = %Lf\n", left_op, right_op, left_op/right_op);
#endif
			if( !right_op )
				parse_error("division by zero!");
			return left_op / right_op;
		break;
		case MOD:
#if DEBUG
			printf("%Lf %% %Lf = %llu\n", left_op, right_op, (unsigned long long)left_op%(unsigned long long)right_op);
#endif
			return (unsigned long long)left_op % (unsigned long long)right_op;
		break;
		case POW:
#if DEBUG
			printf("%Lf ^ %Lf = %Lf\n", left_op, right_op, powl(left_op, right_op));
#endif
			return pow(left_op, right_op);
		break;
	}

	return 0.0;
}

int numeric (char* str){
	int f = 0;
	int i = 0;
	while( str[i] != '\0' ){
		if( str[i] == '.' ){
			if( f ) return 0;
			f = 1;
		}
		else if( str[i] < '0' || str[i] > '9' ){
			return 0;
		}
		++i;
	}
	if( f ) return FLOAT;
	return INT;
}
