#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<string.h>

typedef struct super{
	char inode[512];
	char data[1024];
}super;

typedef struct single{
	char directblock[102];
}single;

typedef struct Double{
	single singleblock;
}Double;

typedef struct inode{
	char f_d;
	char time[20];
	int size;
	char direct;
	single singleblock;
	Double Doubleblock;
}inode;

typedef struct data{
	char minidata[8][16];
}data;

typedef struct filesys{
	char bootblock[2];
	super superblock;
	inode inodeblock[512];
	data datablock[1024];
}filesys;

typedef struct node{
	char name[5];
	struct node *sibling;
	struct node *child;
}node;

node *head;
node *current_location_head;
char *string_token[4];

void myflush();
void init_root();
void print_prompt();
void tokenize(char *);
char *timeString();
int superInodeCheck(filesys *fs);
int superDataCheck(filesys *fs);
void addSuperInode(filesys *fs, int count);
void addSuperData(filesys *fs, int count);
void mymkdir(filesys *fs);
void mycat();
void myls(filesys *fs);
void mypwd();
void mycd(node *);
filesys check_command(filesys *fs,char **);
int main()
{
	FILE *fp;
	filesys fs;
	filesys *ptr_fs;
	if((fp=fopen("myfs","rb"))==NULL){
		printf("error : no myfs");
		return 0;
	}
	else
		fread(&fs,sizeof(filesys),1,fp);
	fclose(fp);

	char command[30];

	init_root();			//root directory setting
	while(1){
		print_prompt();
	    tokenize(command);
		myflush();
		if(!strcmp("mycat",string_token[0])){
			mycat();
		}
		if(!strcmp("mymkdir",string_token[0])){
			mymkdir(&fs);
		}	
		if(!strcmp("myls",string_token[0])){
			myls(&fs);
		}
		if(!strcmp("byebye",string_token[0])){
			fp=fopen("myfs","wb");
			fwrite(&fs,sizeof(filesys),1,fp);
			fclose(fp);
			exit(1);
		}else{
			for(int i=0;i<30;i++)
				command[i]='\0';
			for(int i=0;i<4;i++)
				string_token[i]=NULL;
		}
		
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
	current_location_head=head;				//follow the location of head for 'mycd','mypwd' and so on...
	head->sibling=NULL;
	head->child=NULL;
}
void print_prompt()
{
	int cnt=0;
	node *tmp;
	do{
		if((cnt++)!=0)
			printf("/");
		printf("[");
		printf("%s",head->name);
		printf(" ]$ ");
	}while(head->sibling != NULL);
}
void tokenize(char *command)
{
	int i=0;
	char *word;
	scanf("%[^\n]",command);
	word = strtok(command," ");
	while(word!=NULL){
		string_token[i]=word;
		i++;
		word = strtok(NULL," ");
	}
}
char *timeString()
{
	struct tm today;
	time_t tt;
	char static tim[20];
	int ex[6];

	time(&tt);
	today = *localtime(&tt);

    ex[0]=today.tm_year + 1900;
    sprintf(tim,"%d/",ex[0]);
    ex[1]=today.tm_mon+1;
    sprintf(tim+5,"%d/",ex[1]);
    ex[2]=today.tm_mday;
    sprintf(tim+8,"%02d ",ex[2]);
    ex[3]=today.tm_hour;
    sprintf(tim+11,"%02d:",ex[3]);
    ex[4]=today.tm_min;
    sprintf(tim+14,"%02d:",ex[4]);
    ex[5]=today.tm_sec;
    sprintf(tim+17,"%02d",ex[5]);
    return tim;
}
int superInodeCheck(filesys *fs)
{
	int i,count;
	for(i=0;i<512;i++){
		if(fs->superblock.inode[i]=='0'){
			count=i;
			break;
		}
	}
	return count;
}
int superDataCheck(filesys *fs)
{
	int i,count;
	for(i=0; i<1024; i++){
		if(fs->superblock.data[i] == '0'){
			count=i;
			break;
		}
	}
	return count;
}
void addSuperInode(filesys *fs, int count){
	fs->superblock.inode[count]='1';
}
void addSuperData(filesys *fs, int count){
	fs->superblock.data[count]='1';
}
void mycat()
{
    int ch;
    FILE *fp;


    if((fp=fopen(string_token[1],"rb"))==NULL){
        fprintf(stderr, "ERROR!\n");
        return;
    }

    while((ch=getc(fp))!=EOF){
        putchar(ch);
    }
    printf("\n");
    fclose(fp);
}
void mymkdir(filesys *fs)
{
	int static cnt=0;
	int super_inode_num, super_data_num;
	char tmp[10];
	node *new = (node *)malloc(sizeof(node));
	head -> child = new;
	strcpy(head->child->name,string_token[1]);

	super_inode_num = superInodeCheck(fs);
	super_data_num = superDataCheck(fs);

	addSuperInode(fs,super_inode_num);
	addSuperData(fs,super_data_num);
	printf("%d\n",super_inode_num);
	printf("%d\n",super_data_num);

	fs -> inodeblock[super_inode_num].f_d='d';
	strcpy(fs->inodeblock[super_inode_num].time,timeString());
	fs -> inodeblock[super_inode_num].size=0;
	fs -> inodeblock[super_inode_num].direct=(char)super_data_num;
	cnt++;
	sprintf(fs->datablock[super_data_num].minidata[cnt],". %d",super_inode_num);
}
void myls(filesys *fs)
{
	int i;
	if(head->child == NULL)
		printf("\n%s\n",fs->datablock[0].minidata[i]);
}

