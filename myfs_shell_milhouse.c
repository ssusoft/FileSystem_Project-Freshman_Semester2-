#include<string.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>

typedef struct super{
	char inode[512];
	char data[1024];
}super;                                         
typedef struct single{
	int directblock[102];
}single;

typedef struct Double{
	single singleblock;
}Double;

typedef struct inode{
	char f_d;
	char time[20];
	int size;
	int direct;
	int singleblock;
	Double Doubleblock;
}inode;                                         

typedef struct data{
	char minidata[16][8];
}data;

typedef struct filesys{
	char bootblock[2];
	super superblock;
	inode inodeblock[512];
	data datablock[1024];
}filesys;                                    // 파일 시스템 구조체

typedef struct tree{                         //트리구조 자기참조 구조체
	char name[4];
	struct tree *child;
	struct tree *bro;
	char inodenum;
}tree;

typedef struct directory{           //모든 디렉토리 링크드 리스트로 연결
	char name[20];
	int datablocknum;
	int inodeblocknum;
	struct directory *next;
}directory;

//tree *head=NULL;
//tree *tail=NULL;
char *ptr[5],path[20]="/";
char *timeString();                        //시간측정함수
int superInodeCheck(filesys *fs);          //superblock 아이노드가 0인곳 찾기
int superDataCheck(filesys *fs);           //superblock 데이터가 0인곳 찾기 
void addSuperInode(filesys *fs,int count); //미사용중인 수퍼블럭을 사용중으로 바꾸기
void addSuperData(filesys *fs,int count);  //   "
void mymkdir(filesys *fs,tree *,directory*);      
void myflush();                            //버퍼 비우기
void mycat();
void myls(filesys *fs,directory *);
char *findNowD();                          //최종 디렉토리 찾기 ex) /etc/abc/  ----> abc를 리턴
void mypwd();
void mycd();
void printNoInode(filesys *fs,int num);                          //데이터 블럭 출력시 아이노드 번호 미출력
void printAllInode(filesys *fs,int num,directory *rootdir);        //해당 아이노드 리스트 전체 출력
void makeFile(filesys *fs,directory *rootdir);              //mytouch 에서 파일 만들기
void mytouch(filesys *fs,directory *rootdir);
void makeFile2(filesys *fs,directory *rootdir,char *ptmp,char *ntmp);    //mytouch root/ab 할때 root 밑에 ab 생성
int fileSize();
void mycpfrom(filesys *fs,directory *rootdir);
void mymv(filesys *fs, directory *rootdir);
void readDirectory(filesys *fs,directory *rootdir,int db);
void removeSuperInode(filesys *fs,int count);
void removeSuperData(filesys *fs,int count);
void myrmdir(filesys *fs,directory *rootdir);
void myshowfile(filesys *fs,directory *rootdir);
char *Convert2Dto1D (char (*Data2D)[8], int size, int rows, int cols); 
int compareName(const void *arry1,const void *arry2);
void mycpto(filesys *fs,directory *rootdir);
void myshowinode(filesys *fs);
void myshowblock(filesys *fs);
void mystate(filesys *fs);
void myrm(filesys *fs, directory *rootdir);
void mycp(filesys *fs, directory *rootdir);
void sys_pwd();
void sys_ls();
void itoa_f(int n, char s[]);
void reverse(char s[]);
int main()
{
	FILE *fp;
	char command[40];
	int i,j;
	char *word;
	filesys fs;
	tree *root=(tree*)malloc(sizeof(tree));
	directory *rootdir=(directory *)malloc(sizeof(directory));     //디렉터리 링스크리스트 처음은 root 디렉터리
	directory *head=rootdir;

	strcpy(root->name,"/");
	root->child=NULL;
	root->bro=NULL;
	strcpy(rootdir->name,"/");    //루트 디렉터리 정보 입력
	rootdir->next=NULL;
	rootdir->datablocknum=0;
	rootdir->inodeblocknum=0;
	//	tail=head=root;


	ptr[0]=(char*)malloc(sizeof(char)*20);      //명령어 ' '기준으로 분할
	ptr[1]=(char*)malloc(sizeof(char)*20);      //
	ptr[2]=(char*)malloc(sizeof(char)*20);      //
	ptr[3]=(char*)malloc(sizeof(char)*20);      //
	ptr[4]=(char*)malloc(sizeof(char)*20);      //

	if((fp=fopen("myfs","rb"))==NULL){  
		printf("error : no myfs");
		return 0;
	}
	else
		fread(&fs,sizeof(filesys),1,fp);           //myfs 정보 읽기

	fclose(fp);

	readDirectory(&fs,rootdir,rootdir->datablocknum);
	//                                                              readDirectory 잘 되나 확인 
	while(1){
		printf("%s %d\n",head->name,head->datablocknum);
		if(head->next==NULL)
			break;
		else
			head=head->next;
	}
	//
	for(i=0;i<20;++i)
		command[i]='\0';

	for(i=0;i<4;++i)
		for(j=0;j<20;++j)
			ptr[i][j]='\0';

	while(1){
		printf("[%s ]$ ",path);     //프롬프트 
		scanf("%[^\n]",command);    //명령어 받기
		word=strtok(command," ");
		i=0;
		while(word!=NULL){               //명령어 분할해서  ptr[0],ptr[1]...넣기
			ptr[i]=word;                 //ex) 
			i++;
			word=strtok(NULL," ");
		}

		if(strcmp("mymkdir",ptr[0])==0){
			mymkdir(&fs,root,rootdir);
		}
		else if(strcmp("mycat",ptr[0])==0){
			mycat(&ptr[1]);
		}
		else if(strcmp("myls",ptr[0])==0){
			myls(&fs,rootdir);
		}
		else if(strcmp("mypwd",ptr[0])==0){
			mypwd();
		}
		else if(strcmp("mycd",ptr[0])==0){
			mycd(rootdir);
		}
		else if(strcmp("mytouch",ptr[0])==0){
			mytouch(&fs,rootdir);
		}
		else if(strcmp("mycpfrom",ptr[0])==0){
			mycpfrom(&fs,rootdir);
		}
		else if(strcmp("mymv",ptr[0])==0){
			mymv(&fs,rootdir);
		}
		else if(strcmp("myrmdir",ptr[0])==0){
			myrmdir(&fs,rootdir);
		}
		else if(strcmp("myshowfile",ptr[0])==0){
			myshowfile(&fs,rootdir);
		}
		else if(strcmp("myshowinode",ptr[0])==0){
			myshowinode(&fs);
		}
		else if(strcmp("mycpto",ptr[0])==0){
			mycpto(&fs,rootdir);
		}	
		else if(strcmp("myshowblock",ptr[0])==0){
			myshowblock(&fs);
		}	
		else if(strcmp("mystate",ptr[0])==0){
			mystate(&fs);
		}	
		else if(strcmp("myrm",ptr[0])==0){
			myrm(&fs,rootdir);
		}
		else if(strcmp("mycp", ptr[0])==0){
			mycp(&fs, rootdir);
		}
		else if(strcmp("ls", ptr[0])==0){
			sys_ls();
		}
		else if(strcmp("pwd",ptr[0])==0){
			sys_pwd();
		}
		else 
			printf("not command\n");
		myflush();

		if(strcmp(command,"byebye")==0){
			fp=fopen("myfs","wb");
			fwrite(&fs,sizeof(filesys),1,fp);
			fclose(fp);
			break;
		}
		else{
			for(i=0;i<20;++i)
				command[i]='\0';

			for(i=0;i<4;++i)
				for(j=0;j<20;++j)
					ptr[i][j]='\0';
		}  
	}
	return 0;
}

void sys_pwd(void){
	system("pwd");
}



char *timeString(){
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
int superInodeCheck(filesys *fs){
	int i;
	char count='\0';
	for(i=0;i<512;++i){
		if(fs->superblock.inode[i]=='0'){
			count=i;
			break;
		}
	}
	return count;
}
int superDataCheck(filesys *fs){
	int i;
	char count='\0';
	for(i=0;i<1024;++i){
		if(fs->superblock.data[i]=='0'){
			count=i;
			break;
		}
	}
	return count;
}
void addSuperInode(filesys *fs,int count){
	fs->superblock.inode[count]='1';
}
void addSuperData(filesys *fs,int count){
	fs->superblock.data[count]='1';
}

void removeSuperInode(filesys *fs,int count){
	fs->superblock.inode[count]='0';
}
void removeSuperData(filesys *fs,int count){
	fs->superblock.data[count]='0';
}


//int checkFileSize()



void mymkdir(filesys *fs,tree *root,directory *rootdir){

	int super_inode_num,super_data_num;
	char tmp[10];
	tree *new=(tree*)malloc(sizeof(tree));
	tree *head=root;
	tree *tail=NULL;
	directory *newd=(directory*)malloc(sizeof(directory));
	directory *headd=rootdir;

	super_inode_num=superInodeCheck(fs);       //빈 아이노드 찾기
	super_data_num=superDataCheck(fs);         //빈 데이타블럭 찾기

	addSuperInode(fs,super_inode_num);
	addSuperData(fs,super_data_num);

	fs->inodeblock[super_inode_num].f_d='d';                      //디렉터리 정보 파일시스템 구조체에 저장
	strcpy(fs->inodeblock[super_inode_num].time,timeString());
	fs->inodeblock[super_inode_num].size=0;
	fs->inodeblock[super_inode_num].direct=super_data_num;

	sprintf(fs->datablock[super_data_num].minidata[0],". %d",super_inode_num);   //데이터 블럭 정보 입력


	while(headd->next)                  //디렉터리 링스트리스트에 연결
		headd=headd->next;

	headd->next=newd;

	ptr[1][4]='\0';
	strcpy(newd->name,ptr[1]);
	newd->next=NULL;
	newd->inodeblocknum=super_inode_num;
	newd->datablocknum=super_data_num;

	headd=rootdir;

	while(1){                                                               // 부모 디렉터리 찾아서 데이터블럭에 저장
		if(strcmp(headd->name,findNowD())==0){
			sprintf(fs->datablock[super_data_num].minidata[1],".. %d",headd->inodeblocknum);
			for(int i=0;i<16;++i){
				if(fs->datablock[headd->datablocknum].minidata[i][0]=='\0'){
					sprintf(fs->datablock[headd->datablocknum].minidata[i],"%s %d",ptr[1],(int)newd->inodeblocknum);
					break;
				}
			}
			break;
		}
		else
			headd=headd->next;
	}
	/*	headd=rootdir;                      //디버깅용
		while(1){
		printf("%s\n",headd->name);
		printf("%d\n",headd->inodeblocknum);
		printf("%d\n",headd->datablocknum);
		if(headd->next!=NULL)
		headd=headd->next;
		else
		break;
		}
		for(int i=0;i<16;++i)
		printf("%s\n",fs->datablock[0].minidata[i]);




*/



	/*	strcpy(tmp,findNowD());
		if(strcmp(tmp,"/")==0){                                   //tree인데 구현하다 멈춤 (/___ 까지 구현)
		if(root->child==NULL){
		root->child=new;
		strcpy(new->name,ptr[1]);
	//printf("%s\n",new->name);
	new->child=NULL;
	new->bro=NULL;
	new->inodenum=super_inode_num;
	head=tail=new;
	}
	else{
	tail=head=root->child;
	while(tail->bro)
	tail=tail->bro;
	tail->bro=new;
	strcpy(new->name,ptr[1]);
	//printf("%s\n",new->name);
	new->child=NULL;
	new->bro=NULL;
	new->inodenum=super_inode_num;
	}
	}
	else{
	tail=root->child;
	while(1){
	if(strcmp(tail->name,tmp)==0)
	break;
	tail=tail->bro;
	}
	head=tail;
	if(head->child==NULL){
	head->child=new;
	strcpy(new->name,ptr[1]);
	new->child=NULL;
	new->bro=NULL;
	new->inodenum=super_inode_num;
	head=tail=new;
	}
	else{
	tail=tail->child;
	while(tail->bro)
	tail=tail->bro;
	tail->bro=new;
	strcpy(new->name,ptr[1]);
	new->child=NULL;
	new->bro=NULL;
	new->inodenum=super_inode_num;
	}
	}
	*/


}

void myflush()
{
	while(getchar()!='\n');
}
void mycat()
{
	int ch;
	FILE *fp;


	if((fp=fopen(ptr[1],"r"))==NULL){
		fprintf(stderr, "ERROR!\n");
		return;
	}

	while((ch=getc(fp))!=EOF){                // 파일 내용 찍기
		putchar(ch);
	}
	printf("\n");
	fclose(fp);
}
void mycpfrom(filesys *fs,directory *rootdir){
	int super_inode_num,super_data_num,super_data_num_single;
	int k,i,j,cnt,end=0;
	int tmp[16];
	directory *head=rootdir;
	char ch,name[20];
	FILE *fp;


	strcpy(name,ptr[2]);                        // 복사될 이름
	name[4]='\0';                               // 4글자만 받기  

	if((fp=fopen(ptr[1],"r"))==NULL){
		fprintf(stderr, "ERROR!\n");
		return;
	}

	super_inode_num=superInodeCheck(fs);        // 비어있는 아이노드 찾기
	super_data_num=superDataCheck(fs);          // 비어있는 데이터블럭 찾기

	addSuperInode(fs,super_inode_num);          // 사용중이라고 표시 
	addSuperData(fs,super_data_num);            //   "" 

	fs->inodeblock[super_inode_num].f_d='-';                           //파일 데이터 저장
	strcpy(fs->inodeblock[super_inode_num].time,timeString());
	fs->inodeblock[super_inode_num].size=fileSize();

	if(fileSize()<128){                                                 //파일 사이즈가 128 미만일때 
		fs->inodeblock[super_inode_num].direct=super_data_num;             //direct블럭 사용

		for(i=0;i<16;++i)                                     // 파일내용 direct블럭에 저장
			for(j=0;j<8;++j){
				if((ch=getc(fp))!=EOF){
					fs->datablock[super_data_num].minidata[i][j]=ch;
					printf("%c",fs->datablock[super_data_num].minidata[i][j]);
				}
				else{
					fs->datablock[super_data_num].minidata[i][j]='\0';     //파일 끝나면 배열에 null문자 저장
					break;
				}
			}
	}

	else if(fileSize()<128*102){                                        //파일 사이즈가 128이상일 때 
		cnt=fileSize()/128;                                           // 블럭이 몇 개 필요한지 
		fs->inodeblock[super_inode_num].direct=super_data_num;
		for(i=0;i<cnt;++i){
			tmp[i]=superDataCheck(fs);         //임시 배열에 direct 블럭 빼고 single 블럭에 저장될 데이터블럭 번호 저장
			addSuperData(fs,tmp[i]);           //사용중이라고 표시
		}
		super_data_num_single=superDataCheck(fs);            //single indirect블럭을 위한 블럭 할당 번호
		addSuperData(fs,super_data_num_single);
		for(i=0;i<cnt;++i){                                  //single indirect가 가르키는 블럭에 임시에 배열해둔거 저장
			sprintf(fs->datablock[super_data_num_single].minidata[i],"%d",tmp[i]);
		}
		fs->inodeblock[super_inode_num].singleblock=super_data_num_single;

		for(i=0;i<16;++i)                                   // direct 블럭에 데이터 저장
			for(j=0;j<8;++j){
				if((ch=getc(fp))!=EOF){
					fs->datablock[super_data_num].minidata[i][j]=ch;
					printf("%c",ch);
				}
				else{
					fs->datablock[super_data_num].minidata[i][j]='\0';     //파일 끝나면 배열에 null문자 저장
					break;
				}

			}

		for(k=0;k<cnt;++k)                                   //single indirect 블럭이 가리키고 있는 데이터블럭들에 데이터 저장
			for(i=0;i<16;++i)
				for(j=0;j<8;++j){
					if((ch=getc(fp))!=EOF){
						fs->datablock[tmp[k]].minidata[i][j]=ch;
						printf("%c",ch);
					}
					else{
						fs->datablock[tmp[k]].minidata[i][j]='\0';     //파일 끝나면 배열에 null문자 저장
						break;
					}
				}

	}
	while(1){
		if(strcmp(head->name,findNowD())==0){                    //현재 path 인 디렉터리를 찾아 저장할 파일 이름을 데이터블럭에 저장
			for(i=0;i<16;++i){
				if(fs->datablock[head->datablocknum].minidata[i][0]=='\0'){
					sprintf(fs->datablock[head->datablocknum].minidata[i],"%s %d",name,super_inode_num);
					break;
				}
			}
			break;
		}
		else{
			if(head->next==NULL)
				break;
			else
				head=head->next;
		}

	}
	fclose(fp);
}

/////


/////////////////////////////////////////////하다말앗어




int fileSize(){
	FILE *fp;
	int size;

	if((fp=fopen(ptr[1],"r"))==NULL){
		fprintf(stderr,"ERROR!\n");
		return 0;
	}

	fseek(fp,0,SEEK_END);
	size=ftell(fp);
	return size;
}

void sys_ls(void){
	if(ptr[1][0] == '\0'){
		system("ls");
	}
	else if(strcmp(ptr[1],"-i")==0){
		system("ls -i");
	}
	else if(strcmp(ptr[1],"-l")==0){
		system("ls -l");
	}
	else if(strcmp(ptr[1],"-il")==0){
		system("ls -il");
	}
	else if(strcmp(ptr[1],"-li")==0){
		system("ls -li");
	}
}


void myls(filesys *fs,directory *rootdir){
	int i,j,rootnum=0,rootcnt=0,cnt,cnt_2;
	char tmp[10],ntmp[20],ptmp[20],tmp3[16][8];
	directory *head=rootdir;

	if(ptr[1][0]=='\0'){                                       // 옵션 없을때
		while(1){
			if(strcmp(head->name,findNowD())==0){                //디렉터리 링크드 리스트 에서 현재 디렉터리 찾기
				printNoInode(fs,head->datablocknum);
				break;
			}
			else{
				if(head->next==NULL)
					break;
				else
					head=head->next;
			}
		}
	}
	else if(strcmp(ptr[1],"-i")==0){
		while(1){
			if(strcmp(head->name,findNowD())==0){
				for(i=0; i<16; i++){                              //임시 배열에 데이타블럭 복사
					for(j=0 ; j < 8 ; j++){
						tmp3[i][j] = fs->datablock[head->datablocknum].minidata[i][j];
						if(tmp3[i][j]=='\0')
							break;
					}
				}
				for(i=0;i<16;++i){                             //저장된 데이터블럭의 개수를 cnt_2에 저장
					if(tmp3[i][0]=='\0'){
						cnt_2=i;
						break;
					}
				}
				qsort(&tmp3[2],cnt_2-2,sizeof(char)*8,compareName);   //정렬
				for(i=0;i<16;++i){

					if(tmp3[i][0]=='\0')      //디렉터리의 데이터블럭 정보가 없으면 반복문 졸료
						break;
					printf("%s\n",tmp3[i]);    // 현재 디렉터리의 데이타블럭 정보를 아이노드와 같이 출력
				}
				break;
			}
			else{
				if(head->next==NULL)
					break;
				else
					head=head->next;
			}
		}
	}

	else if(strcmp(ptr[1],"-l")==0){
		while(1){
			if(strcmp(head->name,findNowD())==0){
				printAllInode(fs,head->datablocknum,rootdir);
				break;
			}
			else{
				if(head->next==NULL)
					break;
				else
					head=head->next;
			}
		}

	}
	/*	else if(strcmp(ptr[1],"-il")==0||strcmp(ptr[1],"-li")==0)
		while(1){
		if(strcmp(head->name,findNowD())==0){
		printAllInode(fs,head->datablocknum,rootdir);
		for(i=0;i<8;++i){
		printf("%s",fs->datablock[head->datablocknum].minidata[i]);    // 현재 디렉터리의 데이타블럭 정보를 아이노드와 같이 출력

		if(fs->datablock[head->datablocknum].minidata[i][0]=='\0')      //디렉터리의 데이터블럭 정보가 없으면 반복문 졸료
		break;
		}
		break;
		}
		else{
		if(head->next==NULL)
		break;
		else
		head=head->next;
		}
		}
		*/
		else{                                                           //myls root 등 옵션 없이 디렉터리 내용 보여주고 싶을때
			rootcnt=0;
			cnt=0;

			for(i=0;i<20;++i)
				if(ptr[1][i]=='/'){
					rootcnt++;     //myls 다음 명령어 에서 /개수 구하기
					rootnum=i;     // 마지막 / 의 방번호 저장
				}
			for(i=0;i<20;++i){
				if(ptr[1][i]=='/'){
					cnt++;
					if(cnt==rootcnt-1){            // 마지막 / 바로 전/      (/가 두개이상일때)
						strcpy(ptmp,ptr[1]+i+1);   //파일 만들 디렉터리 이름 저장
						ptmp[rootnum-i-1]='\0';
						strcpy(ntmp,ptr[1]+rootnum+1);    //만들 파일 이름 저장
						break;
					}
					else if(cnt==rootcnt){         // /가 한개일때
						strcpy(ptmp,ptr[1]);
						ptmp[rootnum]='\0';
						strcpy(ntmp,ptr[1]+rootnum+1);
						break;
					}

				}
			}
			if(rootcnt==0){                                   // ptr[1]에 /가 없을경우

				while(1){
					if(strcmp(head->name,ptr[1])==0){                //디렉터리 링크드 리스트 에서 ptr[1]이름의 디렉터리  찾기
						printNoInode(fs,head->datablocknum);
						break;
					}
					else{
						if(head->next==NULL)
							break;
						else
							head=head->next;
					}
				}
			}
			else{

				while(1){
					if(strcmp(head->name,ntmp)==0){                //디렉터리 링크드 리스트 에서 ntmp이름의  디렉터리 찾기
						printNoInode(fs,head->datablocknum);
						break;
					}
					else{
						if(head->next==NULL)
							break;

						else
							head=head->next;
					}
				}
			}
		}
}

void printNoInode(filesys *fs,int num){
	int i,j;
	for(i=0;i<16;++i){
		for(j=0;j<8;++j){                                          
			if(fs->datablock[num].minidata[i][j]==' ') 
				break;
			else
				printf("%c",fs->datablock[num].minidata[i][j]);        //디렉터리 데이터블럭 정보를 ' '가 나오기 전까지 출력

		}
		printf("\n");
		if(fs->datablock[num].minidata[i][0]=='\0')         //정보 없으면 종료
			break;
	}

}
void printAllInode(filesys *fs,int num,directory *rootdir){
	int i,j,cnt,inodenum,cnt_2;
	char tmp[16],tmp2[16],tmp3[16][8];
	directory *head=rootdir;

	for(i=0; i<16; i++){                              //임시 배열에 데이타블럭 복사
		for(j=0 ; j < 8 ; j++){
			tmp3[i][j] = fs->datablock[num].minidata[i][j];
			if(tmp3[i][j]=='\0')
				break;
		}
	}
	for(i=0;i<16;++i){                             //저장된 데이터블럭의 개수를 cnt_2에 저장
		if(tmp3[i][0]=='\0'){
			cnt_2=i;
			break;
		}
	}
	qsort(&tmp3[2],cnt_2-2,sizeof(char)*8,compareName);   //정렬
	for(i=0;i<16;++i){
		if(tmp3[i][0]=='\0')      //디렉터리 데이터블럭 정보 없으면 종료
			break;
		strcpy(tmp,tmp3[i]);    //데이터블럭에 있는 정보 한줄씩 카피
		for(j=0;j<16;++j){
			tmp2[j]=tmp[j];       //데이터블럭에 있는 정보를 한줄씩 아이노드 빼고 저장
			if(tmp[j]==' '){
				tmp2[j]='\0';     
				cnt=j+1;             //아이노드가 저장된 배열의 방번호
				break;
			}
		}
		inodenum=atoi(tmp+cnt);      //문자열로 저장된 아이노드를 정수로 바꿈

		while(1){
			if(head->inodeblocknum==num){                   //디렉터리 링크드 리스트에서 아이노드 번호와 같은 리렉터리 찾고 출력
				printf("%c %4d %s ",fs->inodeblock[inodenum].f_d,fs->inodeblock[inodenum].size,fs->inodeblock[inodenum].time);
				printf("%-s",tmp2);
				printf("\n");
				break;
			}
			else{
				if(head->next==NULL)
					break;
				else
					head=head->next;
			}
		}
	}
}
int compareName(const void *arry1,const void *arry2){
	return strcmp((char *)arry1,(char *)arry2);

}






void mypwd(){
	int len;
	len=strlen(path)-1;               //path에서 /제거
	if(len==0)                        //현재경로가 /일때
		printf("%s",path);
	else{                             //현재경로가 /가 아날때
		for(int i=0;i<len;++i)
			printf("%c",path[i]);
	}
	printf("\n");
}

char *findNowD(){ 
	int i,rootcnt;
	char static tmp[10];
	int end;
	end=strlen(path)-1;            //경로에서 마지막 / 제거
	if(strcmp(path,"/")!=0){         //현재 경로가 /가 아닐때
		for(i=0;i<end;++i)
			if(path[i]=='/')
				rootcnt=i;           //마지막 /가 있는 path의 방번호 저장
	}
	else{
		for(i=0;i<20;++i)
			if(path[i]=='/')
				rootcnt=i;
	}

	for(i=0;i<20;++i){
		if(path[i]=='\0'){
			if(path[1]=='\0')           //경로가 /만 일때
				strcpy(tmp,"/");
			else{
				strcpy(tmp,path+rootcnt+1);   //tmp에 현재 디렉터리 이름 저장
				end=strlen(tmp)-1;
				tmp[end]='\0';
			}
			break;
		}
	}
	return tmp;
}

void mycd(directory *rootdir){

	int i,j,cnt,rootcnt=0;
	char tmp[20],path_tmp[20];
	directory *head=rootdir;
	if(ptr[1][0]=='/'){            //ptr[1]=="/"일때 /으로 돌아가라
		if(ptr[1][1]=='\0')
			ptr[1][0]='\0';
		else{
			if(strcmp(path,"/")!=0)
				strcpy(path,"/");
			for(i=0;i<strlen(ptr[1]);++i)   //절대경로일때 한 문자씩 땡겨라
				ptr[1][i]=ptr[1][i+1];
		}
	}
	for(i=0;i<20;++i)
		if(ptr[1][i]=='/')
			rootcnt=i;
	if(rootcnt!=0){
		strcpy(path_tmp,ptr[1]);
		strcpy(tmp,ptr[1]+rootcnt+1);
		strcpy(ptr[1],tmp);
	}



	while(1){
		if(strcmp(head->name,ptr[1])==0){             //이동할 디렉터리가 현재 디렉터리에 있는지 확인

			if(strcmp(ptr[1],"\0")==0)
				strcpy(path,"/");                    //mycd 다음에 인자가 없을 때
			else{
				for(i=0;i<20;++i)
					if(path[i]=='\0'){
						cnt=i;
						break;
					}
				if(rootcnt!=0){
					strcpy(path,"/");
					strcat(path+1,path_tmp);
				}
				else
					strcat(path+cnt,ptr[1]);           //이전 경로에 이동할 디렉터리 이름 추가
				for(i=0;i<20;++i)
					if(path[i]=='\0'){
						cnt=i;
						break;
					}
				strcat(path+cnt,"/");                 //경로 마지막에 /추가
			}
			break;

		}
		else if(ptr[1][0]=='\0'){
			strcpy(path,"/");
			break;
		}

		else{
			if(head->next==NULL)
				break;
			else
				head=head->next;
		}
	}
}

void mytouch(filesys *fs,directory *rootdir){
	char tmp[16],ptmp[20],ntmp[20];
	int i,j,num,rootcnt=0,cnt=0,rootnum=0;
	int super_inode_num,super_data_num;
	directory *head=rootdir;
	for(i=0;i<20;++i)
		if(ptr[1][i]=='/'){
			rootcnt++;     //mytouch 다음 명령어 에서 /개수 구하기
			rootnum=i;     // 마지막 / 의 방번호 저장
		}
	printf("%d %d\n",rootcnt,rootnum);
	for(i=0;i<20;++i){
		if(ptr[1][i]=='/'){
			cnt++;
			if(cnt==rootcnt-1){            // 마지막 / 바로 전/      (/가 두개이상일때)
				strcpy(ptmp,ptr[1]+i+1);   //파일 만들 디렉터리 이름 저장
				ptmp[rootnum-i-1]='\0';
				strcpy(ntmp,ptr[1]+rootnum+1);    //만들 파일 이름 저장
				break;
			}
			else if(cnt==rootcnt){         // /가 한개일때
				strcpy(ptmp,ptr[1]);
				ptmp[rootnum]='\0';
				strcpy(ntmp,ptr[1]+rootnum+1);
				break;
			}

		}
	}
	printf("%s\n %s\n",ptmp,ntmp);



	if(rootcnt==0){                              //  mytouch 다음 명령어에 /가 없을때 
		while(1){
			if(strcmp(head->name,findNowD())==0){         // 현재 디렉터리찾기
				for(i=2;i<16;++i){
					for(j=0;j<8;++j){
						if(fs->datablock[head->datablocknum].minidata[i][j]!=' ')
							tmp[j]=fs->datablock[head->datablocknum].minidata[i][j];   //현재 디렉터리의 데이터블럭안에 있는 디렉터리 이름 저장
						else{
							tmp[j]='\0';
							num=atoi(tmp+j+1);                  // 아이노드 저장
							break;
						}
					}
					if(strcmp(tmp,ptr[1])==0)
						strcpy(fs->inodeblock[num].time,timeString());      // 만들려는 파일이 현재 디렉터리애 있으면 시간만 바꿔



					if(fs->datablock[head->datablocknum].minidata[i][0]=='\0'){      //데이터 블럭에서 생성 파일의 이름이 없으면 파일 생성
						makeFile(fs,rootdir);
						break;
					}
				}
				break;
			}
			else{
				if(head->next==NULL)
					break;
				else
					head=head->next;
			}
		}
	}
	else{                       // mytouch 다음 명령어에 /가 있을때
		printf("uuuuu...");
		while(1){
			if(strcmp(head->name,ptmp)==0){
				for(i=2;i<16;++i){
					for(j=0;j<8;++j){
						if(fs->datablock[head->datablocknum].minidata[i][j]!=' ')
							tmp[j]=fs->datablock[head->datablocknum].minidata[i][j];
						else{
							tmp[j]='\0';
							num=atoi(tmp+j+1);
							break;
						}
					}
					if(strcmp(tmp,ntmp)==0)
						strcpy(fs->inodeblock[num].time,timeString());



					if(fs->datablock[head->datablocknum].minidata[i][0]=='\0'){
						printf("ok!!");
						makeFile2(fs,rootdir,ptmp,ntmp);
						break;
					}
				}
				break;
			}
			else{
				if(head->next==NULL)
					break;
				else
					head=head->next;
			}
		}
	}
}

void makeFile(filesys *fs,directory *rootdir){            //mytouch  다음 명령어에 /가 없을때파일 생성
	int super_inode_num,super_data_num;
	directory *head=rootdir;

	super_inode_num=superInodeCheck(fs);
	super_data_num=superDataCheck(fs);

	addSuperInode(fs,super_inode_num);
	addSuperData(fs,super_data_num);

	fs->inodeblock[super_inode_num].f_d='-';                 //만드는 파일 정보 파일시스템에 저장
	strcpy(fs->inodeblock[super_inode_num].time,timeString());
	fs->inodeblock[super_inode_num].size=0;
	fs->inodeblock[super_inode_num].direct=super_data_num;


	while(1){
		if(strcmp(head->name,findNowD())==0){
			for(int i=0;i<16;++i){
				if(fs->datablock[head->datablocknum].minidata[i][0]=='\0'){
					sprintf(fs->datablock[head->datablocknum].minidata[i],"%s %d",ptr[1],super_inode_num);       //파일을 만드는 디렉터리 데이터블럭에 정보 저장
					break;   
				}
			}
			break;	 
		}
		else{
			if(head->next==NULL)
				break;
			else
				head=head->next;
		}
	}
}
void makeFile2(filesys *fs,directory *rootdir,char *ptmp,char *ntmp){        //mytouch  다음 명령어에 /가 있을때 파일생성
	int super_inode_num,super_data_num;
	directory *head=rootdir;

	super_inode_num=superInodeCheck(fs);                              //super블럭의 inode,data할당
	super_data_num=superDataCheck(fs);

	addSuperInode(fs,super_inode_num);                                //super block에서 inode,datablock 사용중이라고 표시
	addSuperData(fs,super_data_num);

	fs->inodeblock[super_inode_num].f_d='-';                                //파일 정보 저장
	strcpy(fs->inodeblock[super_inode_num].time,timeString());
	fs->inodeblock[super_inode_num].size=0;
	fs->inodeblock[super_inode_num].direct=super_data_num;


	while(1){
		if(strcmp(head->name,ptmp)==0){
			for(int i=0;i<16;++i){
				if(fs->datablock[head->datablocknum].minidata[i][0]=='\0'){
					sprintf(fs->datablock[head->datablocknum].minidata[i],"%s %d",ntmp,super_inode_num);
					break;
				}
			}
			break;	 
		}
		else{
			if(head->next==NULL)
				break;
			else
				head=head->next;
		}
	}
}




void mymv(filesys *fs, directory *rootdir)
{
	int i = 0, j = 0,idnum,idnum_2;
	char minidata_tmp[16][8], temp[10], res[4],temp_2[10],temp_3[10];
	char tmp[16][8];
	int cnt_1 = strlen(ptr[1]);
	int cnt_2 = strlen(ptr[2]);
	int num = 0,num_2=0,dbnum;
	directory *head=rootdir;
	directory *head_2=rootdir;

	while(1){
		if(strcmp(head->name,findNowD())==0){                                               //현재 디렉터리가 어디인지 찾아
			for(i=0; i<16; i++){                                                                      //tmp배열에 데이터블럭 정보 복사
				for(j=0 ; j < 8 ; j++){
					tmp[i][j] = fs->datablock[head->datablocknum].minidata[i][j];
					if(tmp[i][j]=='\0')
						break;
					if(tmp[i][0]=='\0')                                      
						break;

				}
			}


			for(i=0; i<16; i++){                                                          //minidata_tmp배열에 데이터블럭 정보 복사
				for(j=0 ; j < 8 ; j++){
					minidata_tmp[i][j] = fs->datablock[head->datablocknum].minidata[i][j];
					if(minidata_tmp[i][j]=='\0')
						break;
				}
			}
			for(i=0; i<16;i++){                                         //파일이름뒤에 '\0' 
				for(j=0;j<8;++j)
					if(minidata_tmp[i][j]==' '){
						minidata_tmp[i][j] = '\0';
						break;
					}
			}
			for(j=0; j<16; j++){                                   //ptr[1]의 파일이름이 있는 방번호 저장
				if(strcmp(ptr[1], minidata_tmp[j])==0){
					num = j;
					break;
				}
			}
			for(j=0; j<16; j++){                                  //ptr[2]의 파일이름이 있는 방번호 저장
				if(strcmp(ptr[2], minidata_tmp[j])==0){
					num_2 = j;
					break;
				}
			}
			if(num==0)                                           //명령어 잘못 입력시 종료
				return;
			for(i = 0; i<8 ; i++){                                                      //파일이름 복사
				temp[i] = fs->datablock[head->datablocknum].minidata[num][cnt_1+i];
				temp_2[i]=fs->datablock[head->datablocknum].minidata[num_2][cnt_2+i];
			}
			idnum=atoi(temp+1);                                                    //파일의 아이노드번호 저장
			idnum_2=atoi(temp_2+1);
			if(fs->inodeblock[idnum].f_d=='-' && num_2!=0){                                   //파일을 다른 디렉터리에 옮길때
				strcpy(temp_3,fs->datablock[head->datablocknum].minidata[num]);           //옮기는 파일 이름과 아이노드번호 복사
				i=0;
				while(1){                                                            //현재 디렉터리 데이터 블럭에서 옮기는 파일 정보 삭제
					if(tmp[num+i+1][0]=='\0'){
						tmp[num+i][0]='\0';
						break;
					}
					else{
						strcpy(tmp[num+i],tmp[num+i+1]);
						++i;
					}
				}
				for(int k=0;k<16;++k)
					strncpy(fs->datablock[head->datablocknum].minidata[k],tmp[k],7);                  //수정한 데이터블럭을 기존의 데이터블럭에 덮어씀
				dbnum=fs->inodeblock[idnum_2].direct;
				for(j=0;j<16;++j)                                                         //파일을 옮긴 데이터블럭의 파일 정보 저장
					if(fs->datablock[dbnum].minidata[j][0]=='\0'){
						strcpy(fs->datablock[dbnum].minidata[j],temp_3);
						break;
					}

			}
			else{                                                                   //파일의 이름을 수정할때

				strcpy(fs->datablock[head->datablocknum].minidata[num], ptr[2]);
				strcat(fs->datablock[head->datablocknum].minidata[num],temp);

				if(fs->inodeblock[idnum].f_d=='d'){                                          //파일이 만약 디렉터리면 디렉터리 링크드리스트에서도 이름 수정
					while(1){
						if(strcmp(head_2->name,ptr[1])==0){
							strcpy(head_2->name,ptr[2]);
							break;
						}

						else{
							if(head_2->next==NULL)
								break;
							else
								head_2=head_2->next;
						}
					}
				}

			}


			break;
		}
		else{
			if(head->next==NULL)
				break;
			else
				head=head->next;
		}
	}
}

void readDirectory(filesys *fs,directory *rootdir,int db){             //쉘 실행시 디렉터리를 링크듯리스트로 연결하는 함수
	int i,j,num,cnt;
	int datablock,inodeblock;
	directory *head=rootdir;
	directory *new=NULL;
	char tmp[6];
	for(i=2;i<16;++i){
		if(fs->datablock[db].minidata[2][0]=='\0')                    //재귀함수 중단점
			return ;
		if(fs->datablock[db].minidata[i][0]=='\0')
			break;

		strcpy(tmp,fs->datablock[db].minidata[i]);
		for(j=0;j<16;++j){
			if(tmp[j]==' '){
				cnt=j;
				break;
			}
		}
		num=atoi(tmp+cnt+1);                                  //디렉터리의 아이노드번호 저장
		if(fs->inodeblock[num].f_d=='d'){                      //현재 디렉터리 안에 있는 파일이 디렉터리이라면
			new=(directory *)malloc(sizeof(directory));        //링크드리스트로 연결
			tmp[cnt]='\0';
			strcpy(new->name,tmp);
			new->inodeblocknum=num;
			new->datablocknum=fs->inodeblock[num].direct;
			while(head->next)
				head=head->next;
			head->next=new;
			readDirectory(fs,rootdir,new->datablocknum);         //재귀함수로써 모든 디렉터리를 연결
		}
		else{
			if(fs->datablock[db].minidata[i][0]=='\0')
				break;
		}
	}
}


void myrmdir(filesys *fs,directory *rootdir){
	int i,j,dbnum,ibnum,num,cnt=0;
	char tmp[16][8],tmp2[16][8];
	directory *head=rootdir;
	directory *temp=NULL, *head2=rootdir;
	while(1){                                       //삭제하고자 하는 디렉터리가 비여있는지 확인
		if(strcmp(head->name,ptr[1])==0){
			dbnum=head->datablocknum;
			ibnum=head->inodeblocknum;
			if(fs->datablock[dbnum].minidata[2][0]!='\0'){
				printf("%s is not empty\n",ptr[1]);
				return ;
			}
			break;
		}
		else{
			if(head->next==NULL)
				return;
			else
				head=head->next;
		}
	}

	head=rootdir;

	while(1){
		if(strcmp(head->name,findNowD())==0){                           //현재 디렉터리 찾기

			for(i=0; i<16; i++){                                                                //tmp배열에 현재디렉터리의 데이터블럭 복사
				for(j=0 ; j < 8 ; j++){
					tmp[i][j] = fs->datablock[head->datablocknum].minidata[i][j];
					if(tmp[i][j]=='\0')
						break;
					if(tmp[i][0]=='\0')                                      
						break;

				}
			}
			for(i=0;i<16;++i)                                                      //tmp2배열에는 tmp배열에서 아이노드 제거하고 파일이름만 복사
				for(j=0;j<8;++j){
					tmp2[i][j]=tmp[i][j];
					if(tmp2[i][0]=='\0')
						break;
					if(tmp[i][j]==' '){
						tmp2[i][j]='\0';
						break;

					}
				}


			for(j=0; j<16; j++){                              
				if(strcmp(ptr[1], tmp2[j])==0){
					num = j;
					break;
				}
			}
			i=0;
			while(1){                                     //현재 디렉터리 데이터블럭에서 지우는 디렉터리 정보 삭제
				if(tmp[num+i+1][0]=='\0'){
					tmp[num+i][0]='\0';
					break;
				}
				else{
					strcpy(tmp[num+i],tmp[num+i+1]);
					++i;
				}
			}

			for(i=0;i<16;++i)                                                    //수정한 데이터블럭을 기존의 데이터블럭에 복사
				strncpy(fs->datablock[head->datablocknum].minidata[i],tmp[i],7);
			removeSuperInode(fs,ibnum);                                               //사용중이던 inodeblock,datablock 를 미사용표시
			removeSuperData(fs,dbnum);
			head2=rootdir;
			while(1){                              //디렉터리 링크드 리스트에서 삭제할 디렉터리 삭제
				temp=head2;
				head2=head2->next;
				if(strcmp(ptr[1],head2->name)==0){
					temp->next=head2->next;
					free(head2);
					break;
				}
				if(head2->next==NULL)
					break;

			}
			break;
		}
		else{
			if(head->next==NULL)
				break;
			else
				head=head->next;
		}
	}
}

void myshowfile(filesys *fs,directory *rootdir){
	directory *head=rootdir;
	int i,j,cnt,num,size,k;
	char tmp[16][8],tmp2[16][8],*tmp3[16];
	int direct,single,num_1,num_2;
	num_1=atoi(ptr[1]);
	num_2=atoi(ptr[2]);

	ptr[3][4]='\0';
	while(1){
		if(strcmp(head->name,"/")==0){
			for(i=2;i<16;++i){
				if(fs->datablock[head->datablocknum].minidata[i][0]=='\0')
					break;
				strcpy(tmp[i],fs->datablock[head->datablocknum].minidata[i]);
				strcpy(tmp2[i],tmp[i]);

				for(j=0;j<8;++j)
					if(tmp[i][j]==' '){
						tmp[i][j]='\0';
						cnt=j;
						break;
					}
				tmp[i][4]='\0';
				if(strcmp(tmp[i],ptr[3])==0){
					num=atoi(tmp2[i]+cnt+1);
					size=fs->inodeblock[num].size;
					direct=fs->inodeblock[num].direct;
					single=fs->inodeblock[num].singleblock;
					if(size<=128){
						for(k=0;k<16;++k)
							for(int l=0;l<8;++l)
								tmp3[0]=Convert2Dto1D(fs->datablock[direct].minidata,sizeof(char),16,8);


						for(k=num_1-1;k<=num_2;++k)
							printf("%c",tmp3[0][k]);
						printf("\n");
						free(tmp3[0]);
					}
					else if(size<128*102){

					}

				}




			}

			break;
		}

		else{
			if(head->next==NULL)
				break;
			else
				head=head->next;
		}
	}
}
char *Convert2Dto1D (char (*Data2D)[8], int size, int rows, int cols)                 //2차원 배열을 1차원배열로 변환시켜주는 함수 
{ 
	int i; 
	char *Data1D = (char *) malloc (size * rows * cols); 
	for ( i=0 ; i<rows ; i++ ) 
		memcpy (Data1D+(i*cols*size), Data2D[i], size*cols); 
	return Data1D;    
} 



void mycpto(filesys *fs,directory *rootdir){
	directory *head=rootdir;
	int i,j,num,idnum,dbnum;
	int k;
	int namesize,cnt,sdbnum,sdbnum_2;
	FILE *fp;
	char minidata_tmp[16][8];

	ptr[1][4]='\0';
	for(i=0;i<16;++i)
		for(j=0;j<8;++j)
			minidata_tmp[i][j]='\0';



	if((fp=fopen(ptr[2],"wt"))==NULL){
		fprintf(stderr, "ERROR!\n");
		return;
	}

	while(1){
		if(strcmp(head->name,findNowD())==0){
			for(i=0; i<16; i++){                                                          //minidata_tmp배열에 데이터블럭 정보 복사
				for(j=0 ; j < 8 ; j++){
					minidata_tmp[i][j] = fs->datablock[head->datablocknum].minidata[i][j];
					if(minidata_tmp[i][j]=='\0')
						break;
				}
			}
			printf("\n");	
			for(i=0; i<16;i++){  				//파일이름뒤에 '\0' 
				if(minidata_tmp[i][0]=='\0')
					break;
				for(j=0;j<8;++j)
					if(minidata_tmp[i][j]==' '){
						minidata_tmp[i][j] = '\0';
						break;
					}
			}
			for(j=0; j<16; j++){                                   //ptr[1]의 파일이름이 있는 방번호 저장
				if(strcmp(ptr[1], minidata_tmp[j])==0){
					num = j;
					break;
				}
			}
			printf("\nnum : %d\n",num);
			namesize=strlen(minidata_tmp[num]);
			printf("\nnamesize :%d\n",namesize);

			idnum=atoi(fs->datablock[head->datablocknum].minidata[num]+namesize+1);
			printf("\nidnum : %d\n",idnum);
			if(fs->inodeblock[idnum].size<=128){
				dbnum=fs->inodeblock[idnum].direct;
				for(i=0;i<16;++i)
					for(j=0;j<8;++j){
						fprintf(fp,"%c",fs->datablock[dbnum].minidata[i][j]);
						if(fs->datablock[dbnum].minidata[i][j]=='\0'){
							fclose(fp);
							return;
						}

						//fprintf(fp,"%c",fs->datablock[dbnum].minidata[i][j]);
					}
			}
			else{
				dbnum=fs->inodeblock[idnum].direct;
				for(i=0;i<16;++i)
					for(j=0;j<8;++j)
						fprintf(fp,"%c",fs->datablock[dbnum].minidata[i][j]);
				cnt=(fs->inodeblock[idnum].size)/128;
				sdbnum=fs->inodeblock[idnum].singleblock;
				for(k=0;k<cnt;++k){
					sdbnum_2=atoi(fs->datablock[sdbnum].minidata[k]);
					printf("%d\n",sdbnum_2);

					for(i=0;i<16;++i)
						for(j=0;j<8;++j){
							fprintf(fp,"%c",fs->datablock[sdbnum_2].minidata[i][j]);
							//	if(fs->datablock[sdbnum_2].minidata[i][j]=='\0'){
							//		fclose(fp);
							//		return ;
							//	}
							//fprintf(fp,"%c",fs->datablock[sdbnum_2].minidata[i][j]);
						}


				}
			}



			break;
		}
		else{
			if(head->next==NULL)
				break;
			else
				head=head->next;
		}
	}
	fclose(fp);
}
void itoa_f(int n, char s[])
{
	int i,sign;
	
	if((sign = n )<0)
		n= -n;
	i=0;
	do{
		s[i++] = n % 10 + '0';
	}while((n/=10)>0);
	if(sign<0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}
void reverse(char s[])
{
	int i,j;
	char c;
	for(i=0, j=strlen(s)-1; i<j; i++,j--){
		c=s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
void myshowinode(filesys *fs)
{
	int inodenum = atoi(ptr[1]);
	int i=0,j=0,num;
	int cnt;
	int singlenum = fs->inodeblock[inodenum].singleblock;

	for(i=0; i<16; i++){
		for(j=0; j<8; j++){
			printf("%c ", fs->datablock[singlenum].minidata[i][j]);
		}
		printf("\n");
	}

	if(fs->inodeblock[inodenum].f_d == '-')
		printf("file type : regular file\n");
	if(fs->inodeblock[inodenum].f_d == 'd')
		printf("file type : directory file\n");

	printf("file size : %d\n", fs->inodeblock[inodenum].size);
	printf("modified time : %s\n",fs->inodeblock[inodenum].time);
	for(int i=0; i!='\0'; i++)
		printf("%s\n",fs->inodeblock[inodenum].time);

	if( (fs->inodeblock[inodenum].size) % 128 != 0)
		cnt = (fs->inodeblock[inodenum].size)/128 + 1;	//파일 사이즈가 128로 딱 나누어지지 않을 경우 
	else
		cnt = (fs->inodeblock[inodenum].size)/128;		//파일 사이즈가 128로 딱 나누어 질 경우

	printf("data block list :");
	if( (fs->inodeblock[inodenum].size<128))
		printf("%d\n",inodenum);
	else{
		printf(" %d",fs->inodeblock[inodenum].direct);
		for(i=0; i<cnt; i++){
			printf(" %s",fs->datablock[singlenum].minidata[i]);
		}

		printf("%d ",singlenum);
		printf("\n");
	}

}
void myshowblock(filesys *fs)
{
	int showblock_num = atoi(ptr[1]);
	int i,j;

	for(i=0; i<16; i++){
		for(j=0; j<8; j++){
			printf("%c",fs->datablock[showblock_num].minidata[i][j]);
		}
	}
	printf("\n");
}
void mystate(filesys *fs)
{
	int i;
	int free_inode_cnt=0, free_datablock_cnt=0;
	for(i=0; i<512; i++){
		if((fs->superblock.inode[i]) == '0')
			free_inode_cnt++;
	}
	for(i=0; i<1024; i++){
		if((fs->superblock.data[i]) =='0')
			free_datablock_cnt++;
	}
	printf("free inode : %d\n",free_inode_cnt);
	printf("free data block : %d\n",free_datablock_cnt);

}	

void myrm(filesys *fs, directory *rootdir)
{
	int i = 0, j = 0, a = 0, b=0, c = 0, inodenum, cnt=0;//for문을 위한 변수들, inodenum는 아이노드 번호 저장
	char minidata_tmp[16][8], temp[10], temp2[10], inodenum_s[5];//minidata_tmp : minidata를 저장, inodenum_s : inodenum atoi 사용
	int cnt_1=0;                                                //file 길이
	int num = 0;
	char fd;                                                    //'f' => file, '-' =>directory
	directory *head = rootdir;
	while(1){                                                   //파일명을 temp에 저장
		if(ptr[1][a] == '\0'){
			break;
		}
		else if(ptr[1][a] == '/'){
			break;
		}
		else
			temp[a] = ptr[1][a];
		a++;
	}
	temp[a]='\0';
	cnt_1 = strlen(temp);
	a = 0;
	while(1){                                                   //inode num 복사
		inodenum_s[b] = fs->datablock[head->datablocknum].minidata[j][cnt_1+b];
		if(fs->datablock[head->datablocknum].minidata[j][cnt_1+b] == '\0')
			break;
		b++;
	}
	inodenum = atoi(inodenum_s);
	fd=fs->inodeblock[inodenum].f_d;                            //inodenum을 통해 file인지 directory인지 구별
	if((ptr[1][cnt_1] != '/')){                                 //ptr[1]에 '/'가 없을 때
		while(1){
			if(strcmp(head->name, findNowD())==0)
			{
				for(i=0;i<16;i++){                                          //minidata를 minidata_tmp로 복사
					for(j=0;j<8;j++){
						minidata_tmp[i][j] = fs->datablock[head->datablocknum].minidata[i][j];
						if(minidata_tmp[i][j]=='\0')
							break;
					}
				}
				for(i = 0;i<16;i++){
					minidata_tmp[i][cnt_1] = '\0';
				}
				for(j=0;j<16;j++){
					if(strcmp(temp,minidata_tmp[j])==0){
						num=j;
						break;
					}
				}
				b=0;
				while(1){                                                   //inode num 복사
					inodenum_s[b] = fs->datablock[head->datablocknum].minidata[j][cnt_1+b];
					if(fs->datablock[head->datablocknum].minidata[j][cnt_1+b] == '\0')
						break;
					b++;
				}
				inodenum = atoi(inodenum_s);
				fd=fs->inodeblock[inodenum].f_d;                            //inodenum을 통해 file인지 directory인지 구별

				temp[0]='\0';
				cnt_1=0;
				if(fd=='d'){
					printf("myrm : cannot remove. It is a directory.\n");
					break;
				}
				else if(fd == '-'){
					fs->datablock[head->datablocknum].minidata[j][0] = '\0';
					for(i = j ; i <=16 ; i++){
						for(j = 0; j<8;j++){
							fs->datablock[head->datablocknum].minidata[i][j]=fs->datablock[head->datablocknum].minidata[i+1][j];
						}
					}
				}
				break;
			}
			else{
				if(head->next == NULL)
					break;
				else
					head=head->next;
			}
		}


	}
	else if((fd == 'd')&&(ptr[1][cnt_1] == '/')){               //ptr[1]에 '/'가 있을 때
		temp2[0]='\0';
		while(1){
			if(ptr[1][cnt_1+1+a] == '/'){
				temp[a] = '\0';

				break;}
			temp[a] = ptr[1][(cnt_1+1)+a];
			if(ptr[1][cnt_1+1+a] == '\0')
				break;
			a++;
		}
		while(1){
			if(ptr[1][cnt] == '\0'){
				break;
			}
			else if(ptr[1][cnt] == '/'){
				break;
			}
			else
				temp2[cnt] = ptr[1][cnt];
			cnt++;
		}
		temp2[cnt] = '\0';
		cnt_1 = strlen(temp);
		while(1){
			if(strcmp(head->name,temp2)==0){
				for(i=0;i<16;i++){
					for(j=0;j<8;j++){
						minidata_tmp[i][j] = fs ->datablock[head->datablocknum].minidata[i][j];
						if(minidata_tmp[i][j] == '\0')
							break;
					}
				}
				for(i = 0; i<16;i++){
					minidata_tmp[i][cnt_1] = '\0';
				}
				for(j=0;j<16;j++){
					if(strcmp(temp,minidata_tmp[j]) == 0){
						num = j;
						break;
					}
				}
				b=0;
				while(1){
					inodenum_s[b] = fs->datablock[head->datablocknum].minidata[j][cnt_1+b];
					if(fs->datablock[head->datablocknum].minidata[j][cnt_1+b] == '\0')
						break;
					b++;
				}
				inodenum = atoi(inodenum_s);
				fd=fs->inodeblock[inodenum].f_d;

				if(fd == 'd'){
					printf("myrm : cannot remove. It is a directory.\n");
					break;
				}
				else {
					fs->datablock[head->datablocknum].minidata[j][0] = '\0';
					for(i = j ; i <=16 ; i++){
						for(j = 0; j<8;j++){
							fs->datablock[head->datablocknum].minidata[i][j]=fs->datablock[head->datablocknum].minidata[i+1][j];
						}
					}
				}
				break;
			}

			else{
				if(head->next == NULL)
					break;
				else
					head=head->next;
			}
		}


	}
	else
		printf("파일이 아닙니다. 지울 수 없습니다.\n");
	cnt_1 = 0;
	b=0;
	temp[0]='\0';
}

void mycp(filesys *fs, directory *rootdir)
{

	directory *head = rootdir;
	int i=0,j=0,k=0,a=0,b=0,inodenum, cnt = 0;
	int front, end, copy_count, inode;
	int direct_num, single_num;
	int super_inode_num, super_data_num, super_data_num_single;
	char minidata_tmp[16][8], temp[10], temp2[10], inodenum_s[5], origin[10];
	int cnt_1 = 0, cnt_2 = 0;
	int num = 0, num2 = 0;
	char minitemp[16][8] = {'\0'};
	char *tmp;
	char *compare;
	char ch,name[20];
	char str[20];
	char fd;
	int y_n=0;//1 : 존재  0 : 없음
	char hey[20];
	
	strcpy(hey,ptr[1]);
	strcpy(name,ptr[2]);
	name[4]='\0';

	while(1){
		if(ptr[1][b] == '\0')
			break;
		else if(ptr[1][b] == '/')
			break;
		else 
			temp2[b] = ptr[1][b];
		b++;
	}
	temp2[b] = '\0';
	b=0;
	while(1){
		if(strcmp(head->name, findNowD()) == 0)
		{
			for(i = 0; i<16; i++){
				for(j=0;j<8;j++){
					minidata_tmp[i][j] = fs->datablock[head->datablocknum].minidata[i][j];
					if(minidata_tmp[i][j] == '\0')
						break;
				}
			}
			for(i=0;i<16;i++){
				for(j=0;j<8;j++){
					if(fs->datablock[head->datablocknum].minidata[i][j] == ' ')
						break;
					else
						origin[j] = fs->datablock[head->datablocknum].minidata[i][j];

				}
				origin[j] = '\0';
				if(strcmp(origin,temp2) != 0)
					origin[0] = '\0';
				else
					break;
			}

			cnt_2 = strlen(origin);
			for(i=0;i<16;i++){
				minidata_tmp[i][cnt_2] = '\0';
			}
			for(j = 0; j<16; j++){
				if(strcmp(temp2,minidata_tmp[j])==0){
					y_n = 1;
					break;
				}
			}
			break;
		}
		else{
			if(head ->next == NULL)
				break;
			else
				head=head->next;
		}
	}
	

	if(y_n==0)
		printf("no file in this directory.\n");
	else if(y_n == 1){
	while(1){
		if(ptr[2][a] == '\0')
			break;
		else if(ptr[2][a] == '/')
			break;
		else 
			temp[a] = ptr[2][a];
		a++;
	}
	temp[a] = '\0';
	cnt_1 = strlen(temp);
	a=0;
	if((ptr[1][cnt_1] != '/')){
		while(1){
			if(strcmp(head->name, findNowD()) == 0)
			{
				for(i=0;i<16;i++){
					for(j=0;j<8;j++){
						minidata_tmp[i][j] = fs -> datablock[head->datablocknum].minidata[i][j];
						if(minidata_tmp[i][j] == '\0')
							break;
					}
				}
				for(i=0;i<16;i++){
					if(strcmp(minidata_tmp[i],"")==0){
						num = i;
						break;
					}
				}
				
				for(j=0;j<8;j++){
					fs->datablock[head->datablocknum].minidata[num][j]=temp[j];
					if(fs->datablock[head->datablocknum].minidata[num][j] == '\0'){
						cnt = j;
						break;
					}//cnt -1 == 몇번째방에 NULL이있는가
				}
				fs->datablock[head->datablocknum].minidata[num+1][cnt] = '\0';
				for(i=0;i<16;i++){
					for(j=0;j<8;j++){
						minidata_tmp[i][j] = fs -> datablock[head->datablocknum].minidata[i][j];
						if(minidata_tmp[i][j] == '\0')
							break;
					}
				}
			}
			else{
				if(head->next ==NULL)
					break;
				else
					head = head -> next;
			}
			break;
		}
	}
	}
	temp2[0]='\0';


	for(i=0; i<16; i++){
		for(j=0; j<8;j++){
			minitemp[i][j] = fs->datablock[head->datablocknum].minidata[i][j];
		}
	}

	b=0;
	for(j=0; j<16; j++){
		if(strcmp(hey,minidata_tmp[j]) == 0){
			num;
			break;
		
		}
	}
	num -=1;
	printf("num = %d\n",num);
	while(1){
		inodenum_s[b] = fs->datablock[head->datablocknum].minidata[num][cnt_1+b];
		if(fs->datablock[head->datablocknum].minidata[num][cnt_1+b] == '\0')
			break;
		b++;
	}
	inode = atoi(inodenum_s);

	for(i=0; i<16; i++){
		for(j=0; j<8; j++){
			fs->datablock[head->datablocknum].minidata[i][j]=minitemp[i][j];
		}
	}

	char abc[20];
	itoa_f(inode+1, abc);
	strcat(fs->datablock[head->datablocknum].minidata[num+1]," ");
	strcat(fs->datablock[head->datablocknum].minidata[num+1],abc);

	front = fs->inodeblock[inode].direct;				//복사할 맨 앞의 데이터블록의 방번호
	end = fs->inodeblock[inode].singleblock;			//싱글 다이렉트 방 번호
	copy_count = (fs->inodeblock[inode].singleblock) - (fs->inodeblock[inode].direct ); //복사해야할 데이터의 총 개수

	for(i=front; i<end; i++){
		for(j=0; j<16; j++){
			for(k=0; k<8; k++){
				(fs->datablock[i+copy_count+1].minidata[j][k]) = (fs->datablock[i].minidata[j][k]);
			}	
		}
	}
	



	for(i=front+1,k=0; i<end; i++,k++){
		sprintf(fs->datablock[copy_count*2+2].minidata[k],"%d \0",i+end);
	}




	super_inode_num=superInodeCheck(fs);       //빈 아이노드 찾기
	super_data_num=superDataCheck(fs);         //빈 데이타블럭 찾기

	addSuperInode(fs,super_inode_num);
	addSuperData(fs,super_data_num);

	super_inode_num=superInodeCheck(fs);
	super_data_num=superDataCheck(fs);

	addSuperInode(fs,super_inode_num);
	addSuperData(fs,super_data_num);


	fs->inodeblock[inode+1].f_d='-';                      //디렉터리 정보 파일시스템 구조체에 저장
	strcpy(fs->inodeblock[inode+1].time,timeString());
	fs->inodeblock[inode+1].size= fs->inodeblock[inode].size;
	fs->inodeblock[inode+1].direct =  front + end;
	fs->inodeblock[inode+1].singleblock = copy_count*2 + 2; 

}
