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
	FILE *ifp=NULL;
	f_sys fs;
	int check;	
	if((ifp=fopen("info.txt","rb")) == NULL){
		fprintf(stderr, "error : no myfs");
		fclose(ifp);
		return 0;
	}
	
	check = fread(&fs,sizeof(f_sys),1,ifp);
	while(check){
		printf("%d\n",fs.super_block.s_inode[0]);
		check = fread(&fs, sizeof(f_sys),1,ifp);
	}
	return 0;
}

		
	


