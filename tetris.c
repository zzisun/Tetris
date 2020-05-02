﻿#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
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

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2, WIDTH + 10);
	printw("NEXT BLOCK");
	DrawBox(3, WIDTH + 10, 4, 8);
	move(10, WIDTH + 10);
	printw("NEXT BLOCK");
	DrawBox(11, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(18, WIDTH + 10);
	printw("SCORE");
	DrawBox(19, WIDTH + 10, 1, 8);
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
			if(field[j][i]==1){
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
				attron(COLOR_PAIR(nextBlock[2] + 1));
				printw(" ");
				attroff(COLOR_PAIR(nextBlock[2] + 1));
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

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate)
{
	DrawBlock(y, x, blockID, blockRotate, ' ');
	DrawShadow(y, x, blockID, blockRotate);
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
	// user code
	int i,j;
	for (i = 0; i < BLOCK_HEIGHT; i++)
	{
		for (j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				if (((i + blockY >= 0) && (i + blockY < HEIGHT)) && ((j + blockX >= 0) && (j + blockX < WIDTH)))
				{
					if (f[i + blockY][j + blockX] == 1)
						return 0;

				}
				else return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int i, j, check_num;
	int pre_y = blockY; int pre_x = blockX; int pre_rot = blockRotate;

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	switch (command)
	{
	case KEY_RIGHT:
		pre_x--; break;
	case KEY_LEFT:
		pre_x++; break;
	case KEY_UP:
		pre_rot--; break;
	case KEY_DOWN:
		pre_y--; break;
	}
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	for (i = pre_y; i <= pre_y + BLOCK_HEIGHT; i++)
	{
		for (j = pre_x; j <= pre_x + BLOCK_WIDTH; j++)
		{
			if (((i >= 0) && (i < HEIGHT)) && ((j >= 0) && (j < WIDTH)))
			{
				if (f[i][j] == 0)
				{
					move(i + 1, j + 1);
					printw(".");
				}
			}
		}
	}
	//3. 새로운 블록 정보를 그린다. 
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	//4.블록 출력 후 커서가 필드상에 있으므로 move함수를 이용하여 커서를 필드 밖으로 이동해준다.
	move(HEIGHT, WIDTH + 10);
}

void BlockDown(int sig){

	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX))
	{
		blockY++;
	}
	else {

		if (blockY == -1)
			gameOver = 1;
		else{

		score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);
		score += DeleteLine(field);
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand() % 7;
		
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;

		DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
		}
	}
	DrawChange(field, KEY_DOWN, nextBlock[0],blockRotate,blockY,blockX);
	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	
	//Block이 추가된 영역의 필드값을 바꾼다.
	int i,j,touched=0;
	for (i = 0; i < BLOCK_HEIGHT; i++)
	{
		for (j = 0; j < BLOCK_WIDTH; j++)
		{
			if (block[currentBlock][blockRotate][i][j] == 1)
			{
				field[blockY + i][blockX + j] = 1;
				if (f[i + blockY + 1][j+blockX] || i +blockY+ 1 >= HEIGHT)
				{
					touched++;
				}
			}
		}
	}
	return 10 * touched;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	
	//1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	//2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
	int i, j, k, l;
	int n = 0;
	int fillFlag;
	int line = 0;
	for (j = 1; j < HEIGHT; j++) {
		fillFlag = 0;
		n = 0;

		for (i = 0; i < WIDTH; i++)
		{
			if (f[j][i])
				n++;

		}
		if (n == WIDTH)
		{
			fillFlag = 1;
			line++;
		}
		if (fillFlag == 1) {

			for (k = j; k > 0; k--)
			{
				for (l = 0; l < WIDTH; l++)
					f[k][l] = f[k - 1][l];

			}
		}

	}
	return 100 * line * line;
}


void DrawShadow(int y, int x, int blockID,int blockRotate){
	/*while ((y < HEIGHT) && CheckToMove(field, blockID, blockRotate, y + 1, x))
	{
		 y++;
	}*/
	while (1)
	{
		if (y < HEIGHT&&CheckToMove(field, blockID, blockRotate, y + 1, x) == 1) y++;

		else break;
	}
	DrawBlock(y, x, blockID, blockRotate, '/');
	
}


void createRankList(){
	// user code
}

void rank(){
	// user code
}

void writeRankFile(){
	// user code
}

void newRank(int score){
	// user code
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}
