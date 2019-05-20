//Sinhaeng_Hhjian
#include<bits/stdc++.h>
#include<conio.h>
#include<windows.h>
using namespace std;
typedef long long ll;
#define inf 0x3f3f3f3f

HANDLE hOutput, hOutBuf;//控制台屏幕缓冲区句柄
HANDLE *houtpoint; 
COORD coord = { 0,0 };
//双缓冲处理显示
DWORD bytes = 0;
bool show=false;

int n=0, m=0, s=0, ch, mmp[1005][11][11], rec=0;//origin map, 0:space, 1:wall, 2:human, 3:box, 4:aim 

void handle(){
	//创建新的控制台缓冲区
    hOutBuf = CreateConsoleScreenBuffer(
        GENERIC_WRITE,//定义进程可以往缓冲区写数据
        FILE_SHARE_WRITE,//定义缓冲区可共享写权限
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    hOutput = CreateConsoleScreenBuffer(
        GENERIC_WRITE,//定义进程可以往缓冲区写数据
        FILE_SHARE_WRITE,//定义缓冲区可共享写权限
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    //隐藏两个缓冲区的光标
    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = 0;
    cci.dwSize = 1;
    SetConsoleCursorInfo(hOutput, &cci);
    SetConsoleCursorInfo(hOutBuf, &cci);
}

void welcome(){
	printf("******Sinhaeng_Hhjian******\n");
	printf("  欢迎来到推箱子小游戏！  \n\n\n");
	printf("规则：\n");
	printf("  请使用w，s，a，d进行移动\n");
	printf("    可使用p返回上一步\n");
	printf("     可使用q退出游戏\n\n");
	printf("■代表墙，♀代表人，☆代表箱子，◎代表终点\n");
	printf("♂代表当人站在终点时，★代表箱子到达终点\n\n\n");
	printf("请选择：\n1:继续游戏\n2:选择关卡\n"); 
} 

void init(){
	handle();
	welcome();
	scanf("%d", &ch);
	getchar();
	while(ch!=1 && ch!=2){
		printf("ERROR!请重新输入！\n");
		scanf("%d", &ch);
		getchar();
	}
}

int judge(){
	int cnt=0;
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			if(mmp[s][i][j]==3)
				cnt++;
		}
	}
	return cnt;
}

void readrecord(){
	FILE *fp=NULL;
	char ch;
	if((fp=fopen("record.txt", "r"))!=NULL){
		while((ch=fgetc(fp))!=EOF){
			rec=rec*10+ch-'0';
		}
		fclose(fp);
		fp=NULL;
	}
	else{
		printf("record.txt文件打开失败！");
		exit(0);
	}
}

void writerecord(){
	FILE *fp=NULL;
	if((fp=fopen("record.txt", "w"))!=NULL){
		fprintf(fp, "%d", rec);
		fclose(fp);
		fp=NULL;
	}
	else{
		printf("record.txt文件打开失败！\n");
	}
}

void updatemap(){
	FILE *fp=NULL;
	char filename[100]=".\\\\allmaps\\\\";
	char bod[100], dob[100];
	char hou[100]=".csv";
	
	int c=0, temp=rec;
	while(temp){
		dob[c++]=temp%10+'0';
		temp/=10;
	}
	for(int i=0;i<c;i++)
		bod[i]=dob[c-1-i];
	bod[c]='\0';
	strcat(filename, bod);
	strcat(filename, hou);
	
	memset(mmp, 0, sizeof(mmp));
	char *line, buffer[1024], *record;
	if((fp=fopen(filename, "r"))!=NULL){
		while((line=fgets(buffer, sizeof(buffer), fp))!=NULL){
			m=0;
			record=strtok(line, ",");
			while(record!=NULL){
				mmp[s][n][m++]=record[0]-'0';
				record=strtok(NULL, ",");
			}
			n++;
		}
		fclose(fp);
		fp=NULL;
		/*printf("%d %d\n", n, m);
		for(int i=0;i<n;i++){
			for(int j=0;j<m;j++){
				printf("%d\n", mmp[i][j]);
			}
			printf("\n");
		}*/
	} 
	else{
		printf("%s文件打开失败！\n", filename);
		exit(0); 
	}
}

void drawit(){
	show=!show;//double buffer 
	if(show)
		houtpoint=&hOutput;
	else
		houtpoint=&hOutBuf;
	
	char cc[10][100]={"******Sinhaeng_Hhjian******", "  请使用w，s，a，d进行移动", "    可使用p返回上一步", "     可使用q退出游戏", ""};

	sprintf(cc[5], "     第%d关,总共9关", rec);
	for(int i=0;i<6;i++){
		coord.Y=i;
		WriteConsoleOutputCharacterA(*houtpoint, cc[i], strlen(cc[i]), coord, &bytes);
	}
	
	char mp[11][33];
	memset(mp, '\0', sizeof(mp));
	for(int i=0;i<n;i++){
		strcat(mp[i], "    ");
		for(int j=0;j<m;j++){
			if(mmp[s][i][j]==0)
				strcat(mp[i], "  ");//space
			else if(mmp[s][i][j]==1)
				strcat(mp[i], "■");//wall
			else if(mmp[s][i][j]==2)
				strcat(mp[i], "♀");//human
			else if(mmp[s][i][j]==3)
				strcat(mp[i], "☆");//box
			else if(mmp[s][i][j]==4)
				strcat(mp[i], "◎");//aim
			else if(mmp[s][i][j]==6)
				strcat(mp[i], "♂");//human + aim 
			else if(mmp[s][i][j]==7)
				strcat(mp[i], "★");//box + aim
		}
		coord.Y++;
		WriteConsoleOutputCharacterA(*houtpoint, mp[i], strlen(mp[i]), coord, &bytes);
	}
	if(judge()==0){
		coord.Y++;
		sprintf(cc[6], "You Win !（使用 n 进入下一关）");
		WriteConsoleOutputCharacterA(*houtpoint, cc[6], strlen(cc[6]), coord, &bytes);
	}
	
	//设置新的缓冲区为活动显示缓冲
    SetConsoleActiveScreenBuffer(*houtpoint);
	handle();
}

void moveit(){
	//find the human
	int row=-1, col=-1;s++;
	for(int i=0;i<n;i++){
		for(int j=0;j<m;j++){
			mmp[s][i][j]=mmp[s-1][i][j];
			if(mmp[s][i][j]==2 || mmp[s][i][j]==6){
				row=i;
				col=j;
			}
		}
	}
	char dir=getch();
	if(judge() || dir!='n')
		while(dir!='w' && dir!='s' && dir!='a' && dir!='d' && dir!='p' && dir!='q')
			dir=getch();
	switch(dir){
		case 'w':
			if(mmp[s][row-1][col]==0 || mmp[s][row-1][col]==4){//top is (space) or (aim)
				mmp[s][row][col]-=2;//using calculate is a good way
				mmp[s][row-1][col]+=2;
			}
			else if(mmp[s][row-1][col]==3 || mmp[s][row-1][col]==7)//top is (box) or (aim + box)
				if(mmp[s][row-2][col]==0 || mmp[s][row-2][col]==4){//top top is (space) or (aim)
					mmp[s][row][col]-=2;
					mmp[s][row-1][col]-=1;//lose box(3) add human(2)
					mmp[s][row-2][col]+=3;
				}
			break;
		
		case 's':
			if(mmp[s][row+1][col]==0 || mmp[s][row+1][col]==4){
				mmp[s][row][col]-=2;
				mmp[s][row+1][col]+=2;
			}
			else if(mmp[s][row+1][col]==3 || mmp[s][row+1][col]==7)
				if(mmp[s][row+2][col]==0 || mmp[s][row+2][col]==4){
					mmp[s][row][col]-=2;
					mmp[s][row+1][col]-=1;
					mmp[s][row+2][col]+=3;
				}
			break;
			
		case 'a':
			if(mmp[s][row][col-1]==0 || mmp[s][row][col-1]==4){//top is (space) or (aim)
				mmp[s][row][col]-=2;//using calculate is a good way
				mmp[s][row][col-1]+=2;
			}
			else if(mmp[s][row][col-1]==3 || mmp[s][row][col-1]==7)//top is (box) or (aim + box)
				if(mmp[s][row][col-2]==0 || mmp[s][row][col-2]==4){//top top is (space) or (aim)
					mmp[s][row][col]-=2;
					mmp[s][row][col-1]-=1;//lose box(3) add human(2)
					mmp[s][row][col-2]+=3;
				}
			break;
			
		case 'd':
			if(mmp[s][row][col+1]==0 || mmp[s][row][col+1]==4){//top is (space) or (aim)
				mmp[s][row][col]-=2;//using calculate is a good way
				mmp[s][row][col+1]+=2;
			}
			else if(mmp[s][row][col+1]==3 || mmp[s][row][col+1]==7)//top is (box) or (aim + box)
				if(mmp[s][row][col+2]==0 || mmp[s][row][col+2]==4){//top top is (space) or (aim)
					mmp[s][row][col]-=2;
					mmp[s][row][col+1]-=1;//lose box(3) add human(2)
					mmp[s][row][col+2]+=3;
				}
			break;
			
		case 'p':
			s-=2;
			if(s<0)	s=0;
			break;
			
		case 'q':
			exit(0);
			
		case 'n':
			if(judge()==0){
				rec++;n=0;s=0;
				updatemap();
				if(ch==1)
					writerecord();
			}
			break;
	}
}

void choose(){
	for(int i=1;i<10;i++)
		printf("%d ", i);
	printf("\n\n");
	scanf("%d", &rec);
	getchar();
	while(rec<1 || rec>9){
		printf("ERROR!请重新输入！\n");
		scanf("%d", &rec);
		getchar();
	}
}

int main(){
	init();
	switch(ch){
		case 1:
			readrecord();
			break;
		case 2:
			choose();
			break;
	}
	updatemap();
	while(1){
		drawit();
		moveit();
	} 
	return 0;
} 
