#include<string.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>

typedef struct super{
	char inode[512];
	char data[1024];
}super;                                         
typedef struct single{
	int single[102];
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
char *ptr[4],path[20]="/";
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
void mycpto();
void myshowinode(filesys *fs);
void myshowblock(filesys *fs);
void mystate(filesys *fs);
int main()
{
	FILE *fp;
	char command[40];
	int i,j;
	char *word;
	filesys fs;
	tree *root=(tree*)malloc(sizeof(tree));
	directory *rootdir=(directory *)malloc(sizeof(directory));     //디렉터리 링스크리스트 처음은 root 디렉터리

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

	if((fp=fopen("myfs","rb"))==NULL){  
		printf("error : no myfs");
		return 0;
	}
	else
		fread(&fs,sizeof(filesys),1,fp);           //myfs 정보 읽기

	fclose(fp);

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
		else if(strcmp("mycpto",ptr[0])==0){
			mycpto(&fs,rootdir);
		}
		else if(strcmp("myshowinode",ptr[0])==0){
			myshowinode(&fs);
		}
		else if(strcmp("myshowblock",ptr[0])==0){
			myshowblock(&fs);
		}
		else if(strcmp("mystate",ptr[0])==0){
			mystate(&fs);
		}
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
/*
   void moveSuperInode(filesys *fs,int count){
   fs->superblock.inode[count]='0';
   }
   void moveSuperData(filesys *fs,int count){
   fs->superblock.data[count]='0';
   }
   */

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
					sprintf(fs->datablock[headd->datablocknum].minidata[i],"%s %d",ptr[1],newd->inodeblocknum);
					break;
				}
			}
			break;
		}
		else
			headd=headd->next;
	}
	headd=rootdir;                      //디버깅용
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
					printf("%c",ch);
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
			printf("%d\n",tmp[i]);
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


void myls(filesys *fs,directory *rootdir){
	int i,rootcnt=0,cnt;
	char tmp[10];
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
				for(i=0;i<8;++i){
					printf("%s\n",fs->datablock[head->datablocknum].minidata[i]);    // 현재 디렉터리의 데이타블럭 정보를 아이노드와 같이 출력

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



}
void printNoInode(filesys *fs,int num){
	int i,j;
	for(i=0;i<16;++i){
		for(j=0;j<8;++j){                                          //////////////////////////////////////////// 16이 아니라 20이엿네??? 왜??
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
	int i,j,cnt,inodenum;
	char tmp[16],tmp2[16];
	directory *head=rootdir;
	for(i=0;i<16;++i){
		if(fs->datablock[num].minidata[i][0]=='\0')      //디렉터리 데이터블럭 정보 없으면 종료
			break;
		strcpy(tmp,fs->datablock[num].minidata[i]);    //데이터블럭에 있는 정보 한줄씩 카피
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
				printf("%c %d %s ",fs->inodeblock[inodenum].f_d,fs->inodeblock[inodenum].size,fs->inodeblock[inodenum].time);
				printf("%s",tmp2);
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

	int i,cnt;
	directory *head=rootdir;
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



	//if(rootcnt==0)
	//	strcpy(ntmp,ptr[1]);
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

	super_inode_num=superInodeCheck(fs);
	super_data_num=superDataCheck(fs);

	addSuperInode(fs,super_inode_num);
	addSuperData(fs,super_data_num);

	fs->inodeblock[super_inode_num].f_d='-';
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
void mycpto(filesys *fs, directory *rootdir)
{
	int super_inode_num,super_data_num,super_data_num_single;
	int k,i,j,cnt,end=0;
	int tmp[16];
	directory *head=rootdir;
	char ch,name[20];
	int c;
	FILE *ifp=NULL,*ofp=NULL;
	strcpy(name,ptr[1]);                     
	name[4]='\0';                           

	while(1){
		if(strcmp(head->name,findNowD())==0){                //디렉터리 링크드 리스트 에서 현재 디렉터리 찾기
			if(strcmp(name,head->name)==0)
				break;
		}
		else{
			if(head->next==NULL)
				break;
			else
				head=head->next;
		}
	}
	printf("현재디렉터리:%s\n",head->name);

	if((ifp=fopen(name,"r"))==NULL){
		fprintf(stderr,"ERROR!\n");
		return;
	}
	if((ofp=fopen(ptr[2],"w"))==NULL){
		fprintf(stderr, "ERROR!\n");
		return;
	}
	while((c = getc(ifp)) != EOF)
		putc(c,ofp);

	fclose(ifp);
	fclose(ofp);
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
