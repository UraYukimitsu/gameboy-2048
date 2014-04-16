#include <stdio.h>
#include <stdlib.h>
#include <gb/gb.h>
#include <rand.h>
#include <time.h>
#include <string.h>

#include "tiles.h"
#include "gamemap.h"
#include "titleScreen.h"
#include "options.h"
#include "gameOverScreen.h"

#define array(i, j) (*(arrayAccess + (4 * i) + (j)))
#define array1(i, j) (*(array1Access + (4 * i) + (j)))
#define array2(i, j) (*(array2Access + (4 * i) + (j)))
//NYI #define backup1(i, j) (*(backup1Access + (4 * i) + (j)))
//NYI #define backup2(i, j) (*(backup2Access + (4 * i) + (j)))


void updateScreen();
void dispTitleScreen();
void backGrid();
void invertGrid();
void rotateGrid();
void updateScore(UINT16 sc, UINT8 pos);
void dispGameOverScreen(UINT8* text);
UINT16 pow(UINT16 x, UINT16 y);
UINT8 gameOver();
UINT8 randCell();
UINT8 compare();

UINT8 numberTile[4];
UINT8 tile[1];
UINT8 array[4][4];
UINT8 array1[4][4];
UINT8 array2[4][4];
//NYI UINT8 backup1[4][4];
//NYI UINT8 backup2[4][4];
UINT8 str[6];
UINT8 *arrayAccess = &array; //array cannot be always written if not accessed with its address... Yay, GBDK is *really* glitched...
UINT8 *array1Access = &array1;
UINT8 *array2Access = &array2;
//NYI UINT8 *backup1Access = &backup1;
//NYI UINT8 *backup2Access = &backup2;
UINT8 i, j, k, maxTile, randgen, undo, x, y;
UINT16 score, bestScore;

void main()
{
	UINT8 joyState;
	bestScore = 0;
	maxTile = 4;
	set_bkg_data(0, 125, tiles);
	dispTitleScreen();
	
	start:
	
	score = 0;
	undo = 2;

	set_bkg_tiles(0, 0, 20, 18, gamemap);
	
	for(i = 0; i<4; i++)
	{
		for(j = 0; j<4; j++)
		{
			array(i, j) = 0;//4*i + j + 1;
		}
	}
	randCell();
	randCell();
	
	updateScreen();
	updateScore(bestScore, 18);
	SHOW_BKG;
	
	while(1)
	{
		joyState = joypad();
		while(!(joyState&J_B || joyState&J_UP || joyState&J_DOWN || joyState&J_RIGHT || joyState&J_LEFT || joyState&J_START))
			joyState = joypad();
		if(joyState&J_B)
		{
			if(undo)
			{
				undo--;
				tile[0] = undo + 80;
				set_bkg_tiles(9, 4, 1, 1, tile);
			}
			updateScreen();
		}
		
		if(joyState&J_LEFT || joyState&J_UP)
			invertGrid();
		if(joyState&J_DOWN || joyState&J_UP)
			rotateGrid();
		
		if(joyState&J_RIGHT || joyState&J_LEFT || joyState&J_UP || joyState&J_DOWN)
		{
			backGrid();
			for(y = 0; y < 4; y++)
			{
				for(i = 0; i < 4; i++)
				{
					for(x = 1; x < 4; x++)
					{
						j = 3 - x;
						if(!array(y, j +1))
						{
							array(y, j + 1) = array(y, j);
							array(y, j) = 0;
						}
					}
				}
				for(x = 0; x < 3; x++)
				{
					i = 3 - x;
					if(array(y, i) == array(y, i - 1) && array(y, i))
					{
						array(y, i - 1) = 0;
						array(y, i)++;
						score += pow(2, array(y, i) - 1);
						if(score >= bestScore)
							bestScore = score;
						if(array[y][i] > maxTile - 1)
						{
							updateScreen();
							waitpadup();
							waitpad(J_A|J_B|J_SELECT|J_START|J_UP|J_DOWN|J_LEFT|J_RIGHT);
							waitpadup();
							dispGameOverScreen(".......you.win......");
							goto start;
						}
						for(j = 0; j < i; j++)
						{
							if(!array(y, i - j))
							{
								array(y, i - j) = array(y, (i-1) - j);
								array(y, (i-1) - j) = 0;
							}
						}
					}
				}
			}
			if(compare())
				randCell();
			if(gameOver())
			{
				updateScreen();
				waitpadup();
				waitpad(J_A|J_B|J_SELECT|J_START|J_UP|J_DOWN|J_LEFT|J_RIGHT);
				waitpadup();
				dispGameOverScreen("......game.over.....");
				goto start;
			}
		}
		
		if(joyState&J_UP || joyState&J_DOWN)
		{
			rotateGrid();
			rotateGrid();
			rotateGrid();
		}
		
		if(joyState&J_LEFT || joyState&J_UP)
			invertGrid();
		
		if(joyState&J_UP || joyState&J_DOWN || joyState&J_LEFT || joyState&J_RIGHT)
			updateScreen(); updateScore(score, 6); updateScore(bestScore, 18);
			
		if(joyState&J_START)
			goto start;
		
		while(joyState)
			joyState = joypad();
	}
}

void backGrid()
{
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			array1(i, j) = array(i, j);
}

void invertGrid()
{
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			array2[3 - i][3 - j] = array[i][j];
			
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			array[i][j] = array2[i][j];
}

void rotateGrid()
{
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			array2(i, j) = array(j, i);
			
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			array(i, j) = array2(i, j);
}

UINT8 compare()
{
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if(array1(i, j) != array(i, j))
				return 1;
	return 0;
}

UINT8 gameOver()
{
	UINT8 count = 1;
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
			if(!array(i, j))
				return 0;
	for(i = 0; i < 4; i++)
		for(j = 0; j < 4; j++)
		{
			switch(i)
			{
				case 0:
					if(array(i, j) == array(i + 1, j))
						count = 0;
					break;
				case 3:
					if(array(i, j) == array(i - 1, j))
						count = 0;
					break;
				default:
					if(array(i, j) == array(i - 1, j) || array(i, j) == array(i + 1, j))
						count = 0;
					break;
			}
			switch(j)
			{
				case 0:
					if(array(i, j) == array(i, j + 1))
						count = 0;
					break;
				case 3:
					if(array(i, j) == array(i, j - 1))
						count = 0;
					break;
				default:
					if(array(i, j) == array(i, j - 1) || array(i, j) == array(i, j + 1))
						count = 0;
					break;
			}
		}
	return count;
}

UINT16 pow(UINT16 x, UINT16 y)
{
	UINT16 c, d;
	d = 1;
	for(c = 0; c <= y; c++)
		d *= x;
	return d;
}

void updateScore(UINT16 sc, UINT8 pos)
{
	for(i = 0; i < 6; i++)
		str[i] = 0;
	sprintf(str, "%u", sc);
	for(i = 0; i < strlen(str); i++)
	{
		tile[0] = str[strlen(str) - i - 1] + (80 - '0');
		set_bkg_tiles(pos - i, 2, 1, 1, tile);
	}
}


void updateScreen()
{
	//set_bkg_data(0, 125, tiles);
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(array(i, j) == 0) {
				numberTile[0] = 92;
				numberTile[1] = 94;
				numberTile[2] = 93;
				numberTile[3] = 95;
			}
			else {
				numberTile[0] = 4*(array(i, j) - 1) + 12;
				numberTile[1] = 4*(array(i, j) - 1) + 14;
				numberTile[2] = 4*(array(i, j) - 1) + 13;
				numberTile[3] = 4*(array(i, j) - 1) + 15;
			}
			set_bkg_tiles(2*j + 6, 2*i + 6, 2, 2, numberTile);
		}
	}
}

UINT8 randCell()
{
	UINT16 test = clock();
	do{
		randgen = (UINT8)(randw()%8 + 1);
		x = (UINT8)(randw()%4);
		//delay(rand());
		y = (UINT8)(randw()%4);
		//delay(rand());
		if(clock() - test >= 9000)
			return 1;
	}while(array(x, y));
	array(x, y) = (((randgen) <= 7)?1:2);
	return 0;
}

void dispTitleScreen()
{
	UINT8 choice, cont, joyState;
	tile[0] = 123;
	choice = 0;
	cont = 1;
	set_bkg_tiles(0, 0, 20, 18, titleScreen);
	SHOW_BKG;
	while(cont)
	{
		joyState = joypad();
		if(joyState&J_A)
			cont = 0;
		if(joyState&J_DOWN || joyState&J_UP)
		{
			if(!choice)
			{
				choice = 1;
				set_bkg_tiles(6, 12, 1, 1, tile);
				tile[0] = 0;
				set_bkg_tiles(6, 10, 1, 1, tile);
				tile[0] = 123;
			}
			else
			{
				choice = 0;
				set_bkg_tiles(6, 10, 1, 1, tile);
				tile[0] = 0;
				set_bkg_tiles(6, 12, 1, 1, tile);
				tile[0] = 123;
			}
		}
		while(joyState)
		{
			joyState = joypad();
		}
	}
	joyState = 1;
	while(joyState)
	{
		joyState = joypad();
	}
	cont = 1;
	
	if(choice)
	{
		set_bkg_tiles(0, 0, 20, 18, optionScreen);
		choice = 0;
		while(cont)
		{
			joyState = joypad();
			if(joyState&J_B)
				cont = 0;
			if(joyState&J_DOWN)
			{
					set_bkg_tiles(10, 2*((choice + 1)%7) + 2, 1, 1, tile);
					tile[0] = 0;
					set_bkg_tiles(10, 2*choice + 2, 1, 1, tile);
					tile[0] = 123;
					choice = ++choice % 7;
			}
			if(joyState&J_UP)
			{
				tile[0] = 0;
				set_bkg_tiles(10, 2*choice + 2, 1, 1, tile);
				if(choice == 0)
					choice = 7;
				tile[0] = 123;
				set_bkg_tiles(10, 2*(choice - 1) + 2, 1, 1, tile);
				choice = --choice;
			}
			while(joyState)
			{
				joyState = joypad();
			}
		}
		maxTile = choice + 10;
	}
	initarand(clock() >> 8);
}

void dispGameOverScreen(UINT8 *text)
{
	UINT8 choice, cont, joyState;
	tile[0] = 123;
	choice = 0;
	cont = 1;
	set_bkg_tiles(0, 0, 20, 18, gameOverScreen);
	updateScore(score, 6);
	updateScore(bestScore, 18);
	for(i = 0; i < strlen(text); i++)
	{
		if(i < 20)
		{
			if(text[i] >= 'a' && text[i] <= 'z')
				tile[0] = text[i] - 'a' + 97;
			else
				tile[0] = 0;
			set_bkg_tiles(i, 6, 1, 1, tile);
		}
	}
	tile[0] = 123;
	SHOW_BKG;
	while(cont)
	{
		joyState = joypad();
		if(joyState&J_A)
			cont = 0;
		if(joyState&J_DOWN || joyState&J_UP)
		{
			if(!choice)
			{
				choice = 1;
				set_bkg_tiles(6, 12, 1, 1, tile);
				tile[0] = 0;
				set_bkg_tiles(6, 10, 1, 1, tile);
				tile[0] = 123;
			}
			else
			{
				choice = 0;
				set_bkg_tiles(6, 10, 1, 1, tile);
				tile[0] = 0;
				set_bkg_tiles(6, 12, 1, 1, tile);
				tile[0] = 123;
			}
		}
		while(joyState)
		{
			joyState = joypad();
		}
	}
	joyState = 1;
	while(joyState)
	{
		joyState = joypad();
	}
	cont = 1;
	
	if(choice)
	{
		set_bkg_tiles(0, 0, 20, 18, optionScreen);
		choice = 0;
		while(cont)
		{
			joyState = joypad();
			if(joyState&J_B)
				cont = 0;
			if(joyState&J_DOWN)
			{
					set_bkg_tiles(10, 2*((choice + 1)%7) + 2, 1, 1, tile);
					tile[0] = 0;
					set_bkg_tiles(10, 2*choice + 2, 1, 1, tile);
					tile[0] = 123;
					choice = ++choice % 7;
			}
			if(joyState&J_UP)
			{
				tile[0] = 0;
				set_bkg_tiles(10, 2*choice + 2, 1, 1, tile);
				if(choice == 0)
					choice = 7;
				tile[0] = 123;
				set_bkg_tiles(10, 2*(choice - 1) + 2, 1, 1, tile);
				choice = --choice;
			}
			while(joyState)
			{
				joyState = joypad();
			}
		}
		maxTile = choice + 10;
	}
	initarand(clock() >> 8);
}
