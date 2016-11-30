#include<stdio.h>

typedef struct{
	unsigned s_inode[64];
	unsigned s_data[128]; 
}super;

typedef struct{
	char boot_block[2];
	super super_block;
}f_sys;
int main()
{
	FILE *fp=NULL;
	int check=0;	
	fp = fopen("info.txt", "wb");
	f_sys fs;
	fs.super_block.s_inode[0] = 1;

	while((check++) != 1){
		fwrite(&fs,sizeof(f_sys),1,fp);
	}
	return 0;
}
	

