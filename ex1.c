#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct node{
	char name[5];
	struct node *sibling;
	struct node *child;
}node;

node *head;
void myflush();
void init_root();
void print_prompt();
char **tokenize(char **, char *);
int main()
{
	char command[30];
	char *string_token[4];
	char **token_head;
	init_root();			//root directory setting
	while(1){
		print_prompt();
		token_head = tokenize(string_token,command);
		myflush();
			
	}
	
	return 0;
}
void myflush()
{
	while(getchar()!='\n');
}
void init_root()
{
	head=(node*)malloc(sizeof(node));
	strcpy(head->name,"/");
	head->sibling=NULL;
	head->child=NULL;
}
void print_prompt()
{
	int cnt=0;
	do{
		if((cnt++)!=0)
			printf("/");
		printf("[");
		printf("%s",head->name);
		printf(" ]$ ");
		
	}while(head->sibling != NULL);
}
char **tokenize(char **string_token,char *command)
{
	int i=1;
	char **token_head;
	char *word;
	scanf("%[^\n]",command);
	word = strtok(command," ");
	while(word!=NULL){
		strcpy(token_head[i],command);
		i++;
		word = strtok(NULL," ");
	}
	myflush();
	return token_head;
}


