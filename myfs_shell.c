#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct{
	unsigned s_inode[64];
	unsigned s_data[128]; 
}super;

typedef struct{
	char boot_block[2];
	super super_block;
}f_sys;

char *command;		//명령어 저장 ex) mycat test면 mycat 주소 저장
char *object;		//명령어 다음에 나오는 파일 저장 ex) mycat test 이면 test주소 저장
char temp[20];		//ex)mycat test 입력하면 "mycat test" 그대로 저장

void prompt();		//[/ ]$ 출력하는 함수
void myflush();		
void mycat();		
void init_command_object(); //명령어 수행한 뒤 command & object 초기화
int main()
{
	FILE *ifp=NULL;
	f_sys fs;
	int check;	
	if((ifp=fopen("info.txt","rb")) == NULL){
		fprintf(stderr, "error : no myfs");
		return 0;
	}
	fclose(ifp);

	check = fread(&fs,sizeof(f_sys),1,ifp);		
	while(check){
		check = fread(&fs, sizeof(f_sys),1,ifp); 	//이진파일 읽어서 저장하는 과정
	}

	while(1){
		prompt();
		myflush();
		if(!strcmp(command, "mycat")){
			mycat();
		}
		if(!strcmp(command,"byebye")){
			return 0;
		}
		init_command_object();

	}


	
	return 0;
}
void prompt()
{
	char set[20]={'[','/',' ',']','$'};
	printf("%s ",set);
	scanf("%[^\n]",temp);
	command = strtok(temp," ");
	object = strtok(NULL," ");
}
void init_command_object()
{
	int i;
	for(i=0; i<20; i++){
		command[i] = '\0';
		object[i]  = '\0';
	}
}
void myflush()
{
	while(getchar()!='\n');
}
void mycat()
{
	int ch;
	FILE *fp;
		
	
	if((fp=fopen(object,"rb"))==NULL){
		fprintf(stderr, "ERROR!\n");
		return;
	}

	while((ch=getc(fp))!=EOF){
		putchar(ch);
	}
	printf("\n");
	fclose(fp);
}


