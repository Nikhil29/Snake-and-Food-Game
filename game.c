#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<ncurses.h>
//use -lnurses while compilation
/*see the initscr function defination in the library*/
// this a shell snake and food game
bool curses_started = false;
void endCurses() 
{
	if (curses_started && !isendwin())
	{
	        endwin();		/* clean up all allocated resources from ncurses and put terminal in normal mode */
	}
}
void startCurses() 
{
  if (curses_started) 
  {
     refresh();		/* there are 2 screens one is cur_scr,other is std_scr.The data is first written to std_scr i.e a buffer and on calling the refresh function, the std_scr buffer is written to actual screen i.e cur_scr.thus it is used after a set of print commands to actually display on screen*/
  }
  else 
  {
     initscr();		/*initializes the curses library*/
     cbreak();		/* take input chars one at a time, no wait for \n */
    // raw();		/* same as cbreak but receives ctrl+c and ctrl+z as input and not as signal */
     noecho();		/* donot echo input */
     intrflush(stdscr, false);	/* no(false) data is flushed if interrupt key or signal is there */
     keypad(stdscr, true); 	/* enable keyboard mapping */
     atexit(endCurses);		/* starts the function endcurses on exit */
     curs_set(0);	/* sets the cursor to invisible,visible,very visible by passing parameter 0,1,2. here 0 is passed for invisible cursor*/
     curses_started = true;	/*flag used so that repeated calling doesnot take place*/ 
  }
}
struct turninfo
    {
    int xcoord,ycoord,previousdirection;
    struct turninfo *node;
};
struct turninfo *rear=NULL,*front=NULL;
int screen[400][400];
int tailstartx,tailstarty,tailendx,tailendy,direction,screen_y,screen_x,score_size,applex,appley,score;
char tail='O',apple='a';
void initialstate();
int movetail();
void printtail();
void gameover();
struct turninfo deque();
int enque(int x,int y,int direc);
struct turninfo* createturninfo();
void generateapple();
void drawborders(int x,int y);
void initialisescreencoordinates();
struct turninfo* createturninfo()
    {
    struct turninfo *p;
    p=(struct turninfo *)malloc(sizeof(struct turninfo));
    p->xcoord=0;
    p->ycoord=0;
    p->previousdirection=KEY_RIGHT;
    p->node=NULL;
    return p;
}
int enque(int x,int y,int direc)
    {
    struct turninfo *p;
    p=createturninfo();
    if(front==NULL)
        {
        rear=p;
        front=rear;
        p->xcoord=x;
    	p->ycoord=y;
    	p->previousdirection=direc;
    }
    else
        {
        rear->node=p;
        rear=p;
        p->xcoord=x;
    	p->ycoord=y;
    	p->previousdirection=direc;
    }
    return 0;
}
struct turninfo deque()
    {
    struct turninfo t,*p;
    if(front!=NULL)
        {
        t.xcoord=front->xcoord;
    	t.ycoord=front->ycoord;
    	t.previousdirection=front->previousdirection;
        p=front->node;
        free(front);
        front=p;
        return t;
    }
    else
        {
        printf("stack empty\n");
        t.xcoord=-1;
        t.ycoord=-1;
        t.previousdirection=-1;
        return t;
    }
}
void initialstate()
{
	int input,i,j;
	score=0;
	for(i=0;i<400;i++)
	{
		for(j=0;j<400;j++)
		{
			screen[i][j]=0;
		}
	}
	direction=KEY_RIGHT;
	tailstartx=(rand()%(screen_x-2))+1;
	tailstarty=(rand()%(screen_x-2))+1;
	tailendx=tailstartx;
	tailendy=tailstarty;
	screen[tailstartx][tailstarty]=1;
	mvprintw(tailstartx,tailstarty,"%c",tail);
	mvprintw(screen_x+1,screen_y/2-7,"score = %d",score);
	generateapple();
	timeout(-1);
	input=getch();
	if(input==KEY_LEFT||input==KEY_RIGHT||input==KEY_UP||input==KEY_DOWN)
		direction=input;
	enque(tailstartx,tailstarty,direction);
	refresh();
}
void generateapple()
{
	applex=(rand()%(screen_x-2))+1;
	appley=(rand()%(screen_y-2))+1;
	if(screen[applex][appley]==1)
	{
		generateapple();
	}
	else
	{
		screen[applex][appley]=2;
		mvprintw(applex,appley,"%c",apple);
	}
}
int movetail()
{
	if(direction!=rear->previousdirection)
	{
		enque(tailstartx,tailstarty,direction);
	}
	switch(direction)
	{
		case KEY_RIGHT:
			tailstarty+=1;
			break;
		case KEY_DOWN:
			tailstartx+=1;
			break;
		case KEY_LEFT:
			tailstarty-=1;
			break;
		case KEY_UP:
			tailstartx-=1;
			break;
	}
	if(tailstartx<1)
		tailstartx=(screen_x-2);
	if(tailstarty<1)
		tailstarty=(screen_y-2);
	if(tailstartx>(screen_x-2))
		tailstartx=1;
	if(tailstarty>(screen_y-2))
		tailstarty=1;
	screen[tailendx][tailendy]=0;
	mvprintw(tailendx,tailendy," ");
	if(screen[tailstartx][tailstarty]==2)
	{
		screen[tailstartx][tailstarty]=1;
		screen[tailendx][tailendy]=1;
		score++;
		mvprintw(tailendx,tailendy,"%c",tail);
		mvprintw(tailstartx,tailstarty,"%c",tail);
		mvprintw(screen_x+1,screen_y/2-7,"score = %d",score);
		generateapple();
		return 0;
	}
	else if(screen[tailstartx][tailstarty]!=1)
	{
		screen[tailstartx][tailstarty]=1;
		mvprintw(tailstartx,tailstarty,"%c",tail);
	}
	else
	{
		return -1;
	}
	if(front->node!=NULL&&front->node->xcoord==tailendx&&front->node->ycoord==tailendy)
	{
		deque();
	}
	switch(front->previousdirection)
	{
		case KEY_RIGHT:
			tailendy+=1;
			break;
		case KEY_DOWN:
			tailendx+=1;
			break;
		case KEY_LEFT:
			tailendy-=1;
			break;
		case KEY_UP:
			tailendx-=1;
			break;
	}
	if(tailendx<1)
		tailendx=(screen_x-2);
	if(tailendy<1)
		tailendy=(screen_y-2);
	if(tailendx>(screen_x-2))
		tailendx=1;
	if(tailendy>(screen_y-2))
		tailendy=1;
	return 0;
}
void printtail()
{
	clear();
	initialisescreencoordinates();
	drawborders(screen_x,screen_y);
	int i,j;
	for(i=1;i<screen_x-1;i++)
	{
		for(j=1;j<screen_y-1;j++)
		{
			if(screen[i][j]==1)
			{
				move(i,j);
				printw("%c",tail);
			}
			else if(screen[i][j]==2)
			{
				move(i,j);
				printw("%c",apple);
			}
		}
	}
	mvprintw(screen_x+1,screen_y/2-7,"score = %d",score);
	refresh();
}
void gameover()
{
	//clear();
	move(screen_x/2,screen_y/2-7);
	printw("GAME OVER");
	refresh();
	timeout(-1);			/* there will be default behaviour of getch i.e it will wait for the input until any key is pressed*/
	getch();
}
void initialisescreencoordinates()
{	
	getmaxyx(stdscr,screen_x, screen_y);
	screen_x=screen_x-3;
}
void drawborders(int x,int y) 
{
	int i;
    	// 4 corners
    	mvprintw(0, 0, "+");
    	mvprintw(x - 1, 0, "+");
    	mvprintw(0, y - 1, "+");
    	mvprintw(x - 1, y - 1, "+");
    	mvprintw(x,0,"+");
    	mvprintw(x,y-1,"+");
    	mvprintw(x+2,0,"+");
    	mvprintw(x+2,y-1,"+");
    	// sides
    	for (i = 1; i < (x - 1); i++) 
    	{
    		mvprintw(i, 0, "|");
    		mvprintw(i, y - 1, "|");
    	}
    	// top and bottom
    	for (i = 1; i < (y - 1); i++) 
    	{
    		mvprintw(0, i, "-");
    		mvprintw(x - 1, i, "-");
    	}
    	// sides
    	for (i = x+1; i < (x + 2); i++) 
    	{
    		mvprintw(i, 0, "|");
    		mvprintw(i, y - 1, "|");
    	}
    	for (i = 1; i < (y - 1); i++) 
    	{
    		mvprintw(x, i, "-");
    		mvprintw(x + 2, i, "-");
    	}
}
int main()
{
int checkstatus,input,x,y;
srand((unsigned int)time(NULL));	/* give the key to srand*/
startCurses();
initialisescreencoordinates();
drawborders(screen_x,screen_y);
initialstate();
input=direction;
while(1)
	{
	timeout(100);	/* the system waits for 200ms for the next echo command to execute. thus if no input is received in 100ms then next command is executed*/
	input=getch();	/*to take input*/
	getmaxyx(stdscr,x,y);
	if(x!=screen_x+3||y!=screen_y)
	{
		printtail();
	}
	switch(input)
	{
		case KEY_RIGHT:			/* internal keyword name for right key*/
			if(direction!=KEY_LEFT)
				direction=input;
			break;
		case KEY_DOWN:			/* internal keyword name for down key*/
			if(direction!=KEY_UP)
				direction=input;
			break;
		case KEY_LEFT:			/* internal keyword name for left key*/
			if(direction!=KEY_RIGHT)
				direction=input;
			break;
		case KEY_UP:			/* internal keyword name for up key*/
			if(direction!=KEY_DOWN)
				direction=input;
			break;
	}
	checkstatus=movetail();
	//printtail();
	if(checkstatus==-1)
		break;
}
gameover();
endCurses();
return 0;
}
