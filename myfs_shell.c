#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct tree{
	char name[5];
	struct tree * sibling;
	struct tree * child;
}tree;

tree *head = NULL;
tree *tail = NULL;

void myflush();
void myls(tree **head);
void mymkdir(char *name);
void func(char *p1, char *p2, char *p3, char *p4, tree *root);
void node_swap(tree *prev1, tree **node1, tree **prev2, tree **node2);
void print_list(tree *head,char *option);

int main()

{
	FILE *ifp=NULL;
	//f_sys fs;
	int check;	
	if((ifp=fopen("myfs","rb")) == NULL){
		fprintf(stderr, "error : no myfs");
		return 0;
	}

/*	check = fread(&fs,sizeof(f_sys),1,ifp);		
	while(check){
		check = fread(&fs, sizeof(f_sys),1,ifp); 	//이진파일 읽어서 저장하는 과정
	}
*/
	while(1){
		char prompt[100];
		char *word;
		char *ptr[4];
		int j=0;	
		
		tree *root;								//make root directory
		root = (tree *)malloc(sizeof(tree));
		strcpy(root->name,"/");
		root->sibling = NULL;
		root->child = NULL;
		head = root;
		tail = root;

		//inode sturct 만들어지면 추가하기
		for(int i=0; i<4; i++){
			ptr[i] = (char *)malloc(sizeof(char) * 32);
		}

		printf("[%s ]$ ",root->name);
		scanf("%[^\n]",prompt);
		word=strtok(prompt," ");
		while(word != NULL){
			strcpy(ptr[j++],word);
			word=strtok(NULL," ");
		}
		func(ptr[0], ptr[1], ptr[2], ptr[3], root);

		for(int i=0; i<4; i++)
			free(ptr[i]);
		for(int i=0; i<100; i++)
			prompt[i] = '\0';
		myflush();

	}
	return 0;
}
void myflush()
{
	while(getchar()!='\n');
}
void func(char *p1, char *p2, char *p3, char *p4, tree *root)
{
	if(!(strcmp(p1,"mymkdir"))){
		mymkdir(p2);
	}
	if(!(strcmp(p1,"myls"))){
		myls(&head);
		print_list(head,p2);
	}
	if(!(strcmp(p1,"byebye"))){
		exit(1);
	}
}
void mymkdir(char *name)
{
	tree *new=(tree *)malloc(sizeof(tree));
	if(head->child == NULL){					// 서브디렉터리 없는 경우
		head->child=new;
		strcpy(new->name,name);
		new->sibling=NULL;
		new->child=NULL;
	}
	else{
		tail->sibling=new;
		strcpy(new->name,name);
		new->sibling=NULL;
		new->child=NULL;
		tail=new;
	}
}
void myls(tree **head)
{
	tree *tmp_head=NULL;
	tree *prev1=NULL;
	tree *prev2=NULL;
	tree *node1=NULL;
	tree *node2=NULL;

	if(*head==NULL)								//디렉터리가 없는 경우
		return;
	if((*head)->sibling==NULL)					//하위 디렉터리가 1개인 경우
		return;
	tmp_head=(tree *)malloc(sizeof(tree));
	tmp_head->sibling=*head;

	prev1=tmp_head;

	while(prev1 != NULL && prev1->sibling != NULL){
		node1=prev1->sibling;
		prev2=node1;

		while(prev2 != NULL && prev2->sibling != NULL){
			node2=prev2->sibling;

			if(strcmp(node1->name,node2->name) > 0)
				node_swap(prev1,&node1,&prev2,&node2);
		}
		*head=tmp_head->sibling;
	}
	free(tmp_head);
}
void node_swap(tree *prev1, tree **node1, tree **prev2, tree **node2)
{
	tree *tmp=NULL;

	if(*node1 == *prev2){
		prev1->sibling=*node2;
		*prev2=*node2;

		tmp=*node1;
		*node1=*node2;
		*node2=tmp;

		(*node2)->sibling =(*node1)->sibling;
		(*node1)->sibling =(*node2);
	}
	else{
		prev1->sibling=*node2;
		(*prev2)->sibling=*node1;

		tmp=*node1;
		*node1=*node2;
		*node2=tmp;

		tmp=(*node1)->sibling;
		(*node1)->sibling=(*node2)->sibling;
		(*node2)->sibling = tmp;
	}
}
void print_list(tree *head,char *option)
{
	while(head != NULL){
		printf("%s\n",head->name);
		head=head->sibling;
	}
}
		
