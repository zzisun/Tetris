#include "tetris.h"

recNode *root=NULL;
static struct sigaction act, oact;
void maketree(recNode*);
void cleantree(recNode*);

int main(){
	int exit=0;
	rankflag=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	createRankList();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_REC_PLAY:recommendedPlay(); break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	root=(recNode*)malloc(sizeof(recNode));
	root->level=0;
	root->accumulatedScore=0;
	root->curBlockID=nextBlock[0];

	for(i=0;i<HEIGHT;i++)
		for(j=0;j<WIDTH;j++)
			root->recField[i][j]=field[i][j];

	root->recBlockX=2;
	root->recBlockY=blockY;
	root->recBlockRotate=0;

	maketree(root);
	recommend(root,0);
	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	cleantree(root);
	DrawNextBlock(nextBlock);
	PrintScore(score);

}

void DrawOutline(){	
	int i,j;
	DrawBox(0,0,HEIGHT,WIDTH);

	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	move(10,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(11,WIDTH+10,4,8);

	move(18,WIDTH+10);
	printw("SCORE");
	DrawBox(19,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;

	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	case ' ': while(blockY<=HEIGHT && CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX))
				  blockY++;
			  DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
			  break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]){
				attron(A_REVERSE);
				
				printw(" ");

				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(20,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				
				printw(" ");
				
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for(i=0;i<4;i++)
	{
		move(12+i,WIDTH+13);
		for(j=0;j<4;j++)
		{
			if(block[nextBlock[2]][0][i][j]==1){

				attron(A_REVERSE);
				
				printw(" ");
				
				attroff(A_REVERSE);
			}
			else
				printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				
				printw("%c",tile);
				
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	
	int i,j,ret=1;
	if(blockRotate>3||blockRotate<0)
		return 0;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++)
			if(block[currentBlock][blockRotate][i][j]==1){
				if(f[blockY+i][blockX+j]) return 0;
				if(blockY+i>=HEIGHT) return 0;
				if(blockX+j<0) return 0;
				if(blockX+j>=WIDTH) return 0;

			}
	}

	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	
	int i,j;
	int blk=currentBlock,rot=blockRotate,y=blockY,x=blockX;
	switch(command){
		case KEY_UP:
			if(rot==0)
				rot=3;
			else
				rot--;
				break;
		case KEY_DOWN:
			y--;
			break;
		case KEY_LEFT:
			x++;
			break;
		case KEY_RIGHT:
			x--;
			break;
	}

	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blk][rot][i][j]==1){
				if(i+y>=0){
					move(i+y+1,j+x+1);
					printw(".");
				}
			}
		}
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	move(HEIGHT,WIDTH+10);
}

void BlockDown(int sig){
	int i,j,y;
	int ct;
	ct=CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX);

	if(ct==1){
		blockY++;
	}
	else{


		score+=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		score += DeleteLine(field);
		nextBlock[0]=nextBlock[1];
		nextBlock[1]=nextBlock[2];
		nextBlock[2]=rand()%7;
		if(blockY==-1)
			gameOver=1;
		blockRotate=0;//root->recBlockRotate;
		blockY=-1;//root->recBlockY;
		blockX=WIDTH/2-2;//root->recBlockX;

		root->accumulatedScore=0;
		root->level=0;
		root->recBlockX=2;
		root->recBlockY=-1;
		root->recBlockRotate=0;
		root->curBlockID=nextBlock[0];

		DrawNextBlock(nextBlock);
		for(i=0;i<HEIGHT;i++)
			for(j=0;j<WIDTH;j++)
				root->recField[i][j]=field[i][j];

		maketree(root);
		recommend(root,0);
		
		PrintScore(score);
		DrawField();
	}


		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	timed_out=0;

}
void recommend_BD(int sig)
{
	int x,y,r;
	int i,j;
	int ct;
	ct=CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX);
	if(ct==1)
	{
		blockY++;
		
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	}
	else
	{
		score+=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		score+=DeleteLine(field);
		nextBlock[0]=nextBlock[1];
		nextBlock[1]=nextBlock[2];
		nextBlock[2]=rand()%7;

		root->accumulatedScore=0;
		root->level=0;
		root->recBlockX=2;
		root->recBlockY=-1;
		root->recBlockRotate=0;
		root->curBlockID=nextBlock[0];

		for(i=0;i<HEIGHT;i++)
			for(j=0;j<WIDTH;j++)
				root->recField[i][j]=field[i][j];
		if(blockY==-1)
			gameOver=1;

		blockRotate = 0;
		blockY=-1;
		blockX=WIDTH/2-2;
		
		maketree(root);
		recommend(root,0);
		x=recommendX;
		y=recommendY;
		r=recommendR;

		blockX=x;
		blockY=y;
		blockRotate=r;
		blockX = root->recBlockX;
		blockY = root->recBlockY;
		blockRotate = root->recBlockRotate;
		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();

	}
	
	timed_out=0;
}
int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j;
	int touched=0;



	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1)
			{
				f[blockY+i][blockX+j]=1+currentBlock;
				if((f[i+blockY+1][j+blockX]==1 ) || ( i+blockY == HEIGHT-1) ) 
					touched++;
			}				
		}
	}

	return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int i,j,k,l;
	int n=0;
	int fillFlag;
	int count=0;
	for(j=1;j<HEIGHT;j++){
		fillFlag=0;
		n=0;

		for(i=0;i<WIDTH;i++)
		{
			if(f[j][i])
				n++;

		}
		if(n==WIDTH)
		{
			fillFlag=1;
			count++;
		}
		if(fillFlag==1){
             
			for(k=j;k>0;k--)
			{
				for(l=0;l<WIDTH;l++)
					f[k][l]=f[k-1][l];

			}
		}	

	}



	return 100*count*count;
}
void DrawShadow(int y, int x, int blockID,int blockRotate){


	while(1)
	{
		if(y<HEIGHT&&CheckToMove(field,blockID,blockRotate,y+1,x)==1) y++;
		
		else break;
	}
	DrawBlock(y,x,blockID,blockRotate,'/');

	
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){

	DrawBlock(root->recBlockY,root->recBlockX,root->curBlockID,root->recBlockRotate,'R');
}
	
void DrawBlockWithFeatures(int y,int x,int blockID,int blockRotate)
{
	DrawRecommend(y,x,nextBlock[0],blockRotate);
	DrawBlock(y,x,blockID,blockRotate,' ');
	DrawShadow(y,x,blockID,blockRotate);
}

void createRankList() {

	FILE* fp = fopen("rank.txt", "r");
	int n, i, userscore;
	char username[NAMELEN];
	Node* newnode = NULL;
	Node* prenode = newnode;

	fscanf(fp, "%d", &n);
	ranklen = n;

	for (i = 0; i < n; i++)
	{
		newnode = (Node*)malloc(sizeof(Node));
		fscanf(fp, "%s %d", newnode->name, &newnode->score);
		newnode->link = NULL;

		if (headnode == NULL)
		{
			headnode = newnode;
			prenode = newnode;
		}
		else
		{
			prenode->link = newnode;
			prenode = prenode->link;
		}
	}
	fclose(fp);
}

void rank() {
	int from, to, count;
	char name[NAMELEN];
	int del;

	clear();
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	switch (wgetch(stdscr)) {
	case '1':
		from = 0; to = 0; count = 1;

		echo();
		printw("X: ");
		scanw("%d", &from);

		printw("Y: ");
		scanw("%d", &to);

		noecho();

		if ((from == 0) && (to == 0)) {
			from = 1;
			to = ranklen;
		}
		else if (from == 0)
			from = 1;
		else if (to == 0)
			to = ranklen;

		printw("	name	|	score	\n");
		printw("-------------------------------\n");

		temp = headnode;
		if ((from > to) || temp == NULL) {
			printw("\nsearch failure: no rank in the list\n");
		}
		else {
			for (; count < from; count++) {
				temp = temp->link;
			}
			for (; count <= to; count++) {
				printw("%s\t\t| %d\n", temp->name, temp->score);
				temp = temp->link;
			}
		}
		break;
	case '2':
		printw("input the name: ");
		echo();
		getstr(name);
		noecho();
		printw("	name	|	score	\n");
		printw("----------------------------\n");
		int count = 0;
		for (temp = headnode; temp != NULL; temp = temp->link) {
			if (!(strcmp(temp->name, name))) {
				printw("%s\t\t|%d\n", temp->name, temp->score);
				count++;
			}

		}
		if (count == 0)
		{
			printw("\nsearch failure: no rank in the list\n");
		}
		break;
	case '3':
		printw("input the rank: ");
		echo();
		scanw("%d", &del);
		noecho();
		count = 1;
		rankflag = 1;
		Node* prenode;

		if (del == 1) {
			temp = headnode;
			headnode = headnode->link;
			free(temp);
		}
		else if (del == ranklen) {
			for (temp = headnode; temp->link->link != NULL; temp = temp->link);
			prenode = temp;
			temp = temp->link;
			free(temp);
			prenode->link = NULL;
		}
		else if ((del < ranklen) && (del > 1)) {

			for (temp = headnode; temp->link != NULL; temp = temp->link) {
				if (count == del - 1) {
					rankflag = 1;
					break;
				}
				count++;
			}
			prenode = temp;
			temp = temp->link;
			prenode->link = temp->link;
			free(temp);
		}
		else {

			printw("\nsearch failure: no rank in the list\n");
			rankflag = 0;
		}

		if (rankflag == 1) {
			printw("\nresult: the rank deleted\n");
			ranklen--;
		}
		break;
	default: break;
	}
	getch();
	writeRankFile();

}

void writeRankFile() {

	if (rankflag) {
		FILE* fp = fopen("rank.txt", "w");
		Node* p = headnode;

		if (p != NULL) {
			fprintf(fp, "%d\n", ranklen);
		}

		while (p != NULL) {
			fprintf(fp, "%s %d\n", p->name, p->score);
			p = p->link;
		}
		fclose(fp);

	}

}

void newRank(int score) {


	Node* newnode = (Node*)malloc(sizeof(Node));
	Node* prenode = headnode;

	clear();
	printw("your name: ");
	echo();
	getstr(newnode->name);
	noecho();

	newnode->score = score;
	newnode->link = NULL;

	if (headnode == NULL) {

		headnode = newnode;
		rankflag = 1;
		ranklen++;
		return;
	}

	for (temp = headnode; temp != NULL; temp = temp->link) {
		if (temp->score >= score) {
			if (temp->link == NULL) {
				temp->link = newnode;
				rankflag = 1;
				ranklen++;
				return;
			}
			else {
				prenode = temp;
			}
		}
		else {
			if (temp == headnode) {
				newnode->link = headnode;
				headnode = newnode;
			}
			else {
				newnode->link = temp;
				prenode->link = newnode;
				prenode = newnode;
			}
			rankflag = 1;
			ranklen++;
			return;
		}

	}
	writeRankFile();
}


int recommend(recNode *root,int lv){

	int r,x,y;
	int n=0,i,j,max=0,index=0;
	int temp=0,temp2=0;
	recNode **c;

	maketree(root);
	c= root->child;
	if(lv > VISIBLE_BLOCKS - 1)
		return 0;

	for(r = 0;r<4;r++)
	{
		for(x=-1;x<WIDTH;x++)
		{
			y=blockY;
			temp=0;
			if(CheckToMove(root->recField,nextBlock[lv],r,y,x)){
				while(CheckToMove(root->recField,nextBlock[lv],r,y+1,x)) y=y+1;
				for(i=0;i<HEIGHT;i++)
					for(j=0;j<WIDTH;j++)
						c[n]->recField[i][j]=root->recField[i][j];

				c[n]->recBlockX=x;
				c[n]->recBlockY=y;
				c[n]->recBlockRotate = r;
				c[n]->curBlockID = nextBlock[lv];
				c[n]->accumulatedScore = root->accumulatedScore + AddBlockToField(c[n]->recField,nextBlock[lv],r,y,x)+DeleteLine(c[n]->recField);
				temp = c[n]->accumulatedScore;

				if(temp>max)
					max = temp;
				if(lv == VISIBLE_BLOCKS-1)
					max = c[n]->accumulatedScore;
				else{
					temp2=recommend(c[n],lv+1);
					if(temp2>max){
						max=temp2;
						if(lv==0){
							root->recBlockX = x;
							root->recBlockY = y;
							root->recBlockRotate = r;
							root->curBlockID = nextBlock[lv];
							recommendX = x; 
							recommendY = y; 
							recommendR = r; 
						}
					}
					else if (temp2 == max){
						if( root->recBlockY <y){
							max = temp2;
							if(lv == 0){
								root->recBlockX = x;
								root->recBlockY = y;
								root->recBlockRotate = r;
								root->curBlockID = nextBlock[lv];
								recommendX = x; 
								recommendY = y; 
								recommendR = r; 
							}
						}
					}
					n++;
					if(n == CHILDREN_MAX - 1)
						break;
				}
			}

		}
	}
	return max;
}

void recommendedPlay(){
	int command;
	int i,j;
	int x,y,r;
	clear();

	act.sa_handler=recommend_BD;
	sigaction(SIGALRM,&act,&oact);

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;


	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	root=(recNode*)malloc(sizeof(recNode));
	root->level=0;
	root->accumulatedScore=0;
	root->curBlockID=nextBlock[0];

	for(i=0;i<HEIGHT;i++)
		for(j=0;j<WIDTH;j++)
			root->recField[i][j]=field[i][j];

	root->recBlockX=2;
	root->recBlockY=blockY;
	root->recBlockRotate=0;

	maketree(root);
	recommend(root,0);
	

	blockY = root->recBlockY;
	blockX = root->recBlockX;
	blockRotate = root->recBlockRotate;
	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);

	cleantree(root);
	DrawNextBlock(nextBlock);
	PrintScore(score);
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		command=wgetch(stdscr);
		switch(command){
			case 'q':
			case 'Q':
				command=QUIT;
				break;
			default:
				command=NOTHING;
				break;
		}

		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	
}

void maketree(recNode* p)
{
	int i=0;
	recNode **c = p->child;
	for(i=0;i<CHILDREN_MAX;i++)
	{
		c[i] = (recNode*)malloc(sizeof(recNode));
		c[i]->level = p->level + 1;
		c[i]->parent = p;
		if(c[i]->level < VISIBLE_BLOCKS-1)
			maketree(c[i]);
	}
}
void cleantree(recNode* p)
{
	int i;
	recNode **c = p->child;
	for(i=0;i<CHILDREN_MAX;i++)
	{
		if(c[i]->level < VISIBLE_BLOCKS - 1)
			cleantree(c[i]);

		free(c[i]);
	}
}
