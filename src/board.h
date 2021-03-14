#define FPS		15

#define MAP_HEIGHT	27
#define INNER_MASK	0x3FF0
#define BULLET_TYPE	48
#define EXPLODE_TYPE	64

const char piececount[3]={
	4*7,4*13,4*16,
};

const char piece[]={
	//I
	0x2,0x2,0x2,0x2,
	0x0,0xF,0x0,0x0,
	0x2,0x2,0x2,0x2,
	0x0,0xF,0x0,0x0,
	//o
	0x0,0x6,0x6,0x0,
	0x0,0x6,0x6,0x0,
	0x0,0x6,0x6,0x0,
	0x0,0x6,0x6,0x0,
	//L
	0x6,0x2,0x2,0x0,
	0x0,0x7,0x4,0x0,
	0x2,0x2,0x3,0x0,
	0x1,0x7,0x0,0x0,
	//J
	0x3,0x2,0x2,0x0,
	0x4,0x7,0x0,0x0,
	0x2,0x2,0x6,0x0,
	0x0,0x7,0x1,0x0,
	//t
	0x2,0x7,0x0,0x0,
	0x2,0x6,0x2,0x0,
	0x0,0x7,0x2,0x0,
	0x2,0x3,0x2,0x0,
	//s
	0x3,0x6,0x0,0x0,
	0x4,0x6,0x2,0x0,
	0x3,0x6,0x0,0x0,
	0x4,0x6,0x2,0x0,
	//z
	0x6,0x3,0x0,0x0,
	0x1,0x3,0x2,0x0,
	0x6,0x3,0x0,0x0,
	0x1,0x3,0x2,0x0,
	//n
	0x5,0x7,0x0,0x0,
	0x6,0x2,0x6,0x0,
	0x0,0x7,0x5,0x0,
	0x3,0x2,0x3,0x0,
	//+
	0x2,0x7,0x2,0x0,
	0x2,0x7,0x2,0x0,
	0x2,0x7,0x2,0x0,
	0x2,0x7,0x2,0x0,
	//T
	0x2,0x2,0x7,0x0,
	0x1,0x7,0x1,0x0,
	0x7,0x2,0x2,0x0,
	0x4,0x7,0x4,0x0,
	//S
	0x3,0x2,0x6,0x0,
	0x4,0x7,0x1,0x0,
	0x3,0x2,0x6,0x0,
	0x4,0x7,0x1,0x0,
	//Z
	0x6,0x2,0x3,0x0,
	0x1,0x7,0x4,0x0,
	0x6,0x2,0x3,0x0,
	0x1,0x7,0x4,0x0,
	//.
	0x0,0x2,0x0,0x0,
	0x0,0x2,0x0,0x0,
	0x0,0x2,0x0,0x0,
	0x0,0x2,0x0,0x0,
	//del gun
	0x0,0x2,0x2,0x0,
	0x0,0x2,0x2,0x0,
	0x0,0x2,0x2,0x0,
	0x0,0x2,0x2,0x0,
	//add gun
	0x0,0x2,0x2,0x2,
	0x0,0x2,0x2,0x2,
	0x0,0x2,0x2,0x2,
	0x0,0x2,0x2,0x2,
	//bomb
	0x0,0x6,0x6,0x9,
	0x0,0x6,0x6,0x9,
	0x0,0x6,0x6,0x9,
	0x0,0x6,0x6,0x9,
	//explosion
	0x9,0x6,0x6,0x9,
	0x6,0x9,0x9,0x6,
	0x9,0x6,0x6,0x9,
	0x6,0x9,0x9,0x6,
};

const int wall[MAP_HEIGHT]={
	0x3FFFF,
	0x3FFFF,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3C00F,
	0x3FFFF,
};

#ifdef LANG_EN
#include "lang_en/board_lose.h"
#endif
#ifdef LANG_CH
#include "lang_ch/board_lose.h"
#endif
#ifdef LANG_JP
#include "lang_jp/board_lose.h"
#endif

int pause,gameover,score,mode,level,current,next,row,shift,counter,fullline,explode,bulletRowD,bulletShiftD,bulletRowA,bulletShiftA;
int board[MAP_HEIGHT]={};

void board_init(){
	int i;

	pause=0;
	gameover=0;
	score=0;
	mode=0;
	level=0;
	current=-1;
	next=-1;
	fullline=0;
	explode=0;
	bulletRowD=-1;
	bulletRowA=-1;
	for (i=0;i<MAP_HEIGHT;++i){
		board[i]=0;
	}
}

int board_hittest(int _current,int _row,int _shift){
	int i;

	for (i=0;i<4;++i){
		if ((wall[_row+i]|board[_row+i])&(piece[(_current<<2)+i]<<_shift)) return 1;
	}
	return 0;
}

int board_lasttest(int _current,int _row,int _shift){
	int i;

	for (i=1;i<=row;++i){
		if (!board_hittest(_current,i,_shift)) return 0;
	}
	return 1;
}

int board_add(int _current,int _row,int _shift){
	int i;

	for (i=0;i<4;++i){
		board[_row+i]|=piece[(_current<<2)+i]<<_shift;
	}
	if (board[22]|board[23]|board[24]|board[25]){
		gameover=39;
		current=-1;
		next=-1;
		return 1;
	}
	return 0;
}

void board_del(int _current,int _row,int _shift){
	int i;

	for (i=0;i<4;++i){
		board[_row+i]&=~(piece[(_current<<2)+i]<<_shift);
	}
}

void board_explode(int _current,int _row,int _shift){
	int i;

	for (i=-1;i<5;++i){
		board[_row+i]&=~(0x3F<<(_shift-1));
	}
}

void board_fullline(){
	int i,n;

	n=0;
	for (i=2;i<22;++i){
		if (board[i]==INNER_MASK){
			++n;
			fullline|=1<<i;
		}
	}
	score+=(n*n+n)/2;
}

void board_killlines(){
	int i,j;

	for (i=2;i<22;++i){
		if (fullline&(1<<i)){
			for (j=i;j<22;++j){
				board[j]=board[j+1];
			}
			fullline>>=1;
			--i;
		}
	}
	fullline=0;
}

void board_appear(){
	explode=0;
	current=next;
	next=rand()%piececount[mode];
	row=22;
	shift=7;
}

void board_newgame(){
	int i;

	pause=0;
	gameover=-1;
	score=0;
	counter=FPS;
	fullline=0;
	explode=0;
	bulletRowD=-1;
	bulletRowA=-1;
	for (i=0;i<MAP_HEIGHT;++i){
		board[i]=0;
	}
	for (i=0;i<level;++i){
		board[i+2]=rand()&INNER_MASK;
	}
	next=rand()%piececount[mode];
	board_appear();
	sound_event(ST_start);
}

void board_down(int isButton){
	counter=FPS;
	--row;
	switch (current>>2){
	case 15://bomb
		if (board_hittest(current,row,shift)){
			++row;
			board_explode(current,row,shift);
			explode=1;
			current=EXPLODE_TYPE;
			sound_event(ST_explosion);
		}
		break;
	case 14://add gun
	case 13://del gun
		if (board_hittest(current,row,shift)){
			board_appear();
		}
		break;
	case 12://.
		if (board_lasttest(current,row,shift)){
			if (board_add(current,row+1,shift)){
				sound_event(ST_gameover);
				return;
			};
			board_fullline();
			if (fullline){
				current=-1;
				sound_event(ST_line);
				return;
			}
			board_appear();
			sound_event(ST_crash);
			return;
		}
		if (!isButton){
			sound_event(ST_ghost);
		}
		break;
	default:
		if (board_hittest(current,row,shift)){
			if (board_add(current,row+1,shift)){
				sound_event(ST_gameover);
				return;
			};
			board_fullline();
			if (fullline){
				current=-1;
				sound_event(ST_line);
				return;
			}
			board_appear();
			sound_event(ST_crash);
		}
		break;
	}
}

void board_next(){
	int i;

	if (pause) return;
	if (!gameover) return;
	if (gameover>0){
		--gameover;
		if (gameover==20){
			for (i=0;i<MAP_HEIGHT;++i){
				board[i]=lose[i];
			}
		}
		return;
	}
	--counter;
	if (fullline){
		fullline^=1;
		if (!counter){
			counter=FPS;
			board_killlines();
			sound_event(ST_crash);
			if (current>>2<13){
				board_appear();
			}
		}
		return;
	}
	if (explode){
		current^=1;
		if (!counter){
			counter=FPS;
			board_appear();
		}
		return;
	}
	if (bulletRowD>=0){
		--bulletRowD;
		if (board_hittest(BULLET_TYPE,bulletRowD,bulletShiftD)){
			board_del(BULLET_TYPE,bulletRowD,bulletShiftD);
			bulletRowD=-1;
		}
	}
	if (bulletRowA>=0){
		--bulletRowA;
		if (board_hittest(BULLET_TYPE,bulletRowA,bulletShiftA)){
			if (board_add(BULLET_TYPE,bulletRowA+1,bulletShiftA)){
				sound_event(ST_gameover);
				return;
			}
			bulletRowA=-1;
			board_fullline();
			if (fullline){
				counter=FPS;
				sound_event(ST_line);
				return;
			}
		}
	}
	if (!counter){
		board_down(0);
	}
}

void board_rotate(){
	int temp;

	if (pause) return;
	if (gameover>0) return;
	if (!gameover){
		mode=(mode+1)%3;
		return;
	}
	if (fullline) return;
	if (explode) return;
	switch (current>>2){
	case 15://bomb
		board_explode(current,row,shift);
		counter=FPS;
		explode=1;
		current=EXPLODE_TYPE;
		sound_event(ST_explosion);
		break;
	case 14://add gun
		if (bulletRowA>=0) return;
		bulletRowA=row;
		bulletShiftA=shift;
		sound_event(ST_gun);
		break;
	case 13://del gun
		if (bulletRowD>=0) return;
		bulletRowD=row;
		bulletShiftD=shift;
		sound_event(ST_gun);
		break;
	default:
		temp=((current+1)&0x3)|(current&~0x3);
		if (board_hittest(temp,row,shift)) return;
		current=temp;
		sound_event(ST_rotate);
		break;
	}
}

void board_accelerate(){
	if (pause) return;
	if (gameover>0) return;
	if (!gameover){
		mode=(mode+2)%3;
		return;
	}
	if (fullline) return;
	if (explode) return;
	sound_event(ST_accelerate);
	board_down(1);
}

void board_left(){
	if (pause) return;
	if (gameover>0) return;
	if (!gameover){
		--level;
		if (level<0){
			level=0;
		}
		return;
	}
	if (fullline) return;
	if (explode) return;
	if (board_hittest(current,row,shift-1)) return;
	--shift;
	sound_event(ST_accelerate);
}

void board_right(){
	if (pause) return;
	if (gameover>0) return;
	if (!gameover){
		++level;
		if (level>9){
			level=9;
		}
		return;
	}
	if (fullline) return;
	if (explode) return;
	if (board_hittest(current,row,shift+1)) return;
	++shift;
	sound_event(ST_accelerate);
}
