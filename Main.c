#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#define LENGTH 20
int Last_Value_type = 0;//0代表运算符,1代表运算子

int operand_type(char *variable) {
	int i = 0;
	while (variable[i] != '\0') {
		if (isalpha(variable[i]) != 0) {
			return 0;//0代表变量
		}
		if (variable[i] == '.') {
			return 2;//2代表double常量
		}
		i++;
	}
	return 1;//1代表int常量
}

struct variable_list {
	char name[LENGTH];
	int type;//1代表int,2代表double
	int int_value;
	double double_value;
	struct variable_list *next;
};
typedef struct variable_list *p_variable;
p_variable variable_head = NULL;

void variable_insert(char *variable_name, int type) {
	p_variable new_variable;
	new_variable = (p_variable)malloc(sizeof variable_list);
	strcpy(new_variable->name, variable_name);
	new_variable->type = type;
	new_variable->int_value = 0;
	new_variable->double_value = 0;
	if (variable_head == NULL) {
		new_variable->next = NULL;
		variable_head = new_variable;
	}
	else {
		new_variable->next = variable_head;
		variable_head = new_variable;
	}
}

void variable_take(char *sentence, int type) {
	int i = 0, j = 0;
	char variable_name[LENGTH];
	while (sentence[i] != '\0') {
		if (sentence[i] == ' ') {
			i++;
			continue;
		}
		if (sentence[i] == ',') {
			i++;
			variable_insert(variable_name, type);
			j = 0;
			continue;
		}
		variable_name[j] = sentence[i];
		variable_name[j + 1] = '\0';
		i++;
		j++;
	}
	if (j != 0) {
		variable_insert(variable_name, type);
	}
}

double value_get(char *variable) {
	p_variable temp = variable_head;
	while (temp != NULL) {
		if (strcmp(variable, temp->name) == 0) {
			if (temp->type == 1) {
				return temp->int_value;
			}
			else {
				return temp->double_value;
			}
		}
		temp = temp->next;
	}
	return 0;
}
int type_get(char *variable) {
	p_variable temp = variable_head;
	while (temp != NULL) {
		if (strcmp(variable, temp->name) == 0) {
			return temp->type;
		}
		temp = temp->next;
	}
	return 1;
}

struct operater_stack {
	char operater;
	struct operater_stack *next;
};
typedef struct operater_stack *p_operater;
p_operater operater_top = NULL;

struct operand_stack {
	int int_value;
	double double_value;
	int type;
	char operand[LENGTH];
	struct operand_stack *next;
};
typedef struct operand_stack *p_operand;
p_operand operand_top = NULL;

void fuck_minus_sign() {
	p_operater temp = operater_top;
	while (operater_top != NULL)
		if (operater_top->operater == '!') {
			operand_top->int_value = -operand_top->int_value;
			operand_top->double_value = -operand_top->double_value;
			operater_top = operater_top->next;
			free(temp);
		}
		else break;
}

void push_operater(char operater) {
	p_operater new_operater;
	new_operater = (p_operater)malloc(sizeof operater_stack);
	new_operater->operater = operater;
	new_operater->next = operater_top;
	operater_top = new_operater;
	Last_Value_type = 0;
}

void push_operand(char *operand) {
	p_operand new_operand;
	new_operand = (p_operand)malloc(sizeof operand_stack);
	if (operand_type(operand) == 0) {
		new_operand->type = type_get(operand);
		if (new_operand->type == 1) new_operand->int_value = value_get(operand);
		else new_operand->double_value = value_get(operand);
	}
	else if (operand_type(operand) == 1) {
		new_operand->type = 1;
		new_operand->int_value = atof(operand);
	}
	else {
		new_operand->type = 2;
		new_operand->double_value = atof(operand);
	}
	strcpy(new_operand->operand, operand);
	new_operand->next = operand_top;
	operand_top = new_operand;
	Last_Value_type = 1;
}

char pop_operater() {
	fuck_minus_sign();
	char operater;
	p_operater temp = operater_top;
	operater = operater_top->operater;
	operater_top = operater_top->next;

	free(temp);
	return operater;
}

double pop_operand() {
	fuck_minus_sign();
	double operand;
	p_operand temp = operand_top;
	if (operand_top->type == 1)operand = operand_top->int_value;
	else operand = operand_top->double_value;
	operand_top = operand_top->next;
	free(temp);
	return operand;
}

void free_operater() {
	p_operater temp = operater_top;
	while (operater_top != NULL) {
		operater_top = operater_top->next;
		free(temp);
		temp = operater_top;
	}
}
void free_operand() {
	p_operand temp = operand_top;
	while (operand_top != NULL) {
		operand_top = operand_top->next;
		free(temp);
		temp = operand_top;
	}
}

void calc(char ch) {
	double operand2, operand1;
	int temp_operand;
	int type2 = 1, type1 = 1, type = 1;
	fuck_minus_sign();
	if (operand_top != NULL) {
		type2 = operand_top->type;
		operand2 = pop_operand();
	}
	if (ch == '^') ch = pop_operater();
	fuck_minus_sign();
	if (operand_top != NULL) {
		type1 = operand_top->type;
		if (type1 == 1)operand1 = operand_top->int_value;
		else operand1 = operand_top->double_value;
	}
	type = type1*type2;
	if (type != 1 && operand_top != NULL)operand_top->type = 2;
	if (ch == '/'&&operand2 == 0) {
		printf("divided by ZERO");
		exit(0);
	}
	switch (ch) {
	case '+':if (type == 1) operand_top->int_value = operand1 + operand2;
			 else operand_top->double_value = operand1 + operand2;
			 break;
	case '-':if (type == 1) operand_top->int_value = operand1 - operand2;
			 else operand_top->double_value = operand1 - operand2;
			 break;
	case '*':if (type == 1) operand_top->int_value = operand1 * operand2;
			 else operand_top->double_value = operand1 * operand2;
			 break;
	case '/':if (type == 1) operand_top->int_value = operand1 / operand2;
			 else operand_top->double_value = operand1 / operand2;
			 break;
	case '#': {
		if (type2 == 1) {
			temp_operand = operand2;
			printf("%d\n", temp_operand);
		}
		else {
			printf("%f\n", operand2);
		}
	}
	case '=': {
		p_variable temp = variable_head;
		if (operand_top != NULL) {
			if (type2 == 1)operand_top->int_value = operand2;
			else operand_top->double_value = operand2;
		}
		while (operand_top != NULL&&temp != NULL) {
			if (strcmp(operand_top->operand, temp->name) == 0) {
				if (temp->type == 1)temp->int_value = operand2;
				else temp->double_value = operand2;
			}
			temp = temp->next;
		}
		break;
	}
	}
}

int sentence_take(FILE *fp,char *sentence) {
	int i = 0, ch;
	while ((ch = getc(fp)) != ';') {
		if (ch == EOF) return -1;
		if (ch == '\n' || (ch == ' '&&i == 0)) continue;
		sentence[i] = ch;
		i++;
	}
	sentence[i] = '\0';
	return 1;
}

int sentence_define(char *sentence) {
	if (strncmp(sentence, "int", 3) == 0) {
		sentence = sentence + 3;
		variable_take(sentence,1);
		return 1;
	}else if (strncmp(sentence, "double", 6) == 0) {
		sentence = sentence + 6;
		variable_take(sentence, 2);
		return 1;
	}
	else return 0;
}

void sentence_calc(char *sentence) {
	int i = 0, j = 0;
	char word[LENGTH],ch;
	while ((ch=sentence[i]) != '\0') {
		if (isspace(ch)) {
			i++;
		}else if (isalnum(ch) || ch == '.') {
			word[j] = ch;
			word[j + 1] = '\0';
			i++;
			j++;
		}else {
			if (strcmp(word, "print") == 0 && j != 0)push_operater('#');
			else if (j != 0) push_operand(word);
			j = 0;
			switch (ch) {
			case '+': {
				if (Last_Value_type == 0);
				else {
					while (operater_top != NULL)
						if (operater_top->operater == '*' || operater_top->operater == '/')
							calc('^');
						else break;
					push_operater(ch);
				}
				break;
			}
			case '-': {
				if (Last_Value_type == 0)push_operater('!');
				else {
					while (operater_top != NULL)
						if (operater_top->operater == '*' || operater_top->operater == '/')
							calc('^');
						else break;
					push_operater(ch);
				}
				break;
			}
			case '*':case '/': {
				if (operater_top != NULL)
					if (operater_top->operater == '*' || operater_top->operater == '/')
						calc('^');
				push_operater(ch);
				break;
			}
			case '(': {
				push_operater(ch);
				break;
			}
			case ')': {
				while(operater_top->operater!='(')
					calc('^');
				pop_operater();
				break;
			}
			case '=': {
				push_operater(ch);
				break;
			}
			}
			i++;
			Last_Value_type = 0;
		}
	}
	if (j != 0) push_operand(word);
	while(operater_top!=NULL)
		calc('^');
	free_operand();
	free_operater();
}

int main(int argc, char  *argv[])
{
	FILE *fp;
	char sentence[30],word[30];
	fp = fopen(argv[1], "r");

	while (sentence_take(fp, sentence)!=-1)
		if(sentence_define(sentence)==0)
			break;
	sentence_calc(sentence);
	while (sentence_take(fp, sentence) != -1)
		sentence_calc(sentence);
}

