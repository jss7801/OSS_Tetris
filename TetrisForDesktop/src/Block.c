#include <time.h>
#include <windows.h>
#include <stdio.h>
#include "Block.h"
#include "Util.h"
#include "Constant.h"

#define NEXT_QUEUE_SIZE 2
#define BLOCK_EXAMPLES_SIZE 7

const static Point blockExamples[BLOCK_EXAMPLES_SIZE][POSITIONS_SIZE][POSITIONS_SIZE] = {
	//けけけけ
	{
		{ { 0, 5 }, { 0, 6 }, { 0, 7 }, { 0, 8 } },
		{ { -1, 6 }, { 0, 6 }, { 1, 6 }, { 2, 6 } },
		{ { 0, 5 }, { 0, 6 }, { 0, 7 }, { 0, 8 } },
		{ { -1, 6 }, { 0, 6 }, { 1, 6 }, { 2, 6 } }
	},
	//    け
	//けけけ
	{
		{ { 0, 8 }, { 1, 6 }, { 1, 7 }, { 1, 8 } },
		{ { -1, 7 }, { 0, 7 }, { 1, 7 }, { 1, 8 } },
		{ { 0, 6 }, { 0, 7 }, { 0, 8 }, { 1, 6 } },
		{ { -1, 6 }, { -1, 7 }, { 0, 7 }, { 1, 7 } }
	},
	//  けけ
	//けけ
	{
		{ { 0, 7 }, { 0, 8 }, { 1, 6 }, { 1, 7 } },
		{ { -1, 6 }, { 0, 6 }, { 0, 7 }, { 1, 7 } },
		{ { 0, 7 }, { 0, 8 }, { 1, 6 }, { 1, 7 } },
		{ { -1, 6 }, { 0, 6 }, { 0, 7 }, { 1, 7 } }
	},
	//けけ
	//  けけ
	{
		{ { 0, 6 }, { 0, 7 }, { 1, 7 }, { 1, 8 } },
		{ { -1, 8 }, { 0, 8 }, { 0, 7 }, { 1, 7 } },
		{ { 0, 6 }, { 0, 7 }, { 1, 7 }, { 1, 8 } },
		{ { -1, 8 }, { 0, 8 }, { 0, 7 }, { 1, 7 } }
	},
	//  け
	//けけけ
	{
		{ { 0, 7 }, { 1, 6 }, { 1, 7 }, { 1, 8 } },
		{ { -1, 7 }, { 0, 7 }, { 0, 8 }, { 1, 7 } },
		{ { 0, 6 }, { 0, 7 }, { 0, 8 }, { 1, 7 } },
		{ { -1, 7 }, { 0, 6 }, { 0, 7 }, { 1, 7 } }
	},
	//け
	//けけけ
	{
		{ { 0, 6 }, { 1, 6 }, { 1, 7 }, { 1, 8 } },
		{ { -1, 8 }, { -1, 7 }, { 0, 7 }, { 1, 7 } },
		{ { 0, 6 }, { 0, 7 }, { 0, 8 }, { 1, 8 } },
		{ { -1, 7 }, { 0, 7 }, { 1, 7 }, { 1, 6 } }
	},
	//けけ
	//けけ
	{
		{ { 0, 6 }, { 0, 7 }, { 1, 6 }, { 1, 7 } },
		{ { 0, 6 }, { 0, 7 }, { 1, 6 }, { 1, 7 } },
		{ { 0, 6 }, { 0, 7 }, { 1, 6 }, { 1, 7 } },
		{ { 0, 6 }, { 0, 7 }, { 1, 6 }, { 1, 7 } }
	}
};

static Block _Block_MoveToDown(Block block);
static Block _Block_MoveToLeft(Block block);
static Block _Block_MoveToRight(Block block);
static Block _Block_RotateRight(Block block);
static void _Block_PrintDefaultBlock(int blockNumber, int x, int* y);

Block Block_Make(int isFirst, Block block){
	int i;
	int j;
	int next1;
	int next2;
	srand((unsigned int)time(NULL));
	if (isFirst){
		block.current = rand() % BLOCK_EXAMPLES_SIZE;
		block.hold = -1;
	}
	else{
		Queue_Get(&block.next, &block.current, sizeof(int));
	}
	for (i = 0; i < POSITIONS_SIZE; i++){
		for (j = 0; j < POSITIONS_SIZE; j++){
			block.positions[i][j] = blockExamples[block.current][i][j];
		}
	}
	if (isFirst){
		Queue_Create(&block.next, NEXT_QUEUE_SIZE, sizeof(int));
		do{
			next1 = rand() % BLOCK_EXAMPLES_SIZE;
		} while (block.current == next1);
		Queue_Put(&block.next, &next1, sizeof(int));
	}
	Queue_At(&block.next, &next1, 0, sizeof(int));
	do{
		next2 = rand() % BLOCK_EXAMPLES_SIZE;
	} while (block.current == next2 || next1 == next2);
	Queue_Put(&block.next, &next2, sizeof(int));
	block.direction = UP;
	block.color = block.current+1; /////鷺薫 事薗 壱舛
	return block;
}

void Block_Destroy(Block block){
	Queue_Destroy(&block.next);
}

Block Block_Move(Block block, int direction){
	switch (direction){
	case LEFT:
		return _Block_MoveToLeft(block);
	case RIGHT:
		return _Block_MoveToRight(block);
	case DOWN:
		return _Block_MoveToDown(block);
	case UP:
		return _Block_RotateRight(block);
	}
	return _Block_MoveToDown(block);
}

Point* Block_GetPositions(Block block){
	return block.positions[block.direction];
}

void Block_ChangeCurrentForHold(Block* block){
	int i;
	int j;
	int temp = block->current;
	block->current = block->hold;
	block->hold = temp;
	if (block->current != -1){
		for (i = 0; i < POSITIONS_SIZE; i++){
			for (j = 0; j < POSITIONS_SIZE; j++){
				block->positions[i][j] = blockExamples[block->current][i][j];
			}
		}
		block->direction = UP;
	}
	else{
		*block = Block_Make(False, *block);
	}
}

void Block_PrintNext(Block block, int index, int x, int y){
	int next;
	ScreenUtil_ClearRectangle(x + 2, y + 1, 12, 2); // use temp size (magic number)
	CursorUtil_GotoXY(x, y++);
	printf("ΞΜ Next %d ΜΟ", index + 1);
	CursorUtil_GotoXY(x, y++);
	Queue_At(&block.next, &next, index, sizeof(int));
	_Block_PrintDefaultBlock(next, x, &y);
	CursorUtil_GotoXY(x, y++);
	printf("ΡΜΜΜΜΜΜΠ");
}

void Block_PrintHold(Block block, int x, int y){
	ScreenUtil_ClearRectangle(x + 2, y + 1, 12, 2); // use temp size (magic number)
	CursorUtil_GotoXY(x, y++);
	printf("ΞΜ  Hold  ΜΟ");
	CursorUtil_GotoXY(x, y++);
	_Block_PrintDefaultBlock(block.hold, x, &y);
	CursorUtil_GotoXY(x, y++);
	printf("ΡΜΜΜΜΜΜΠ");
}

static Block _Block_MoveToDown(Block block){
	int i;
	int j;
	for (i = 0; i < POSITIONS_SIZE; i++){
		for (j = 0; j < POSITIONS_SIZE; j++){
			block.positions[i][j].x++;
		}
	}
	return block;
}

static Block _Block_MoveToLeft(Block block){
	int i;
	int j;
	for (i = 0; i < POSITIONS_SIZE; i++){
		for (j = 0; j < POSITIONS_SIZE; j++){
			block.positions[i][j].y--;
		}
	}
	return block;
}

static Block _Block_MoveToRight(Block block){
	int i;
	int j;
	for (i = 0; i < POSITIONS_SIZE; i++){
		for (j = 0; j < POSITIONS_SIZE; j++){
			block.positions[i][j].y++;
		}
	}
	return block;
}

static Block _Block_RotateRight(Block block){
	block.direction = (block.direction + 1) % POSITIONS_SIZE;
	return block;
}

static void _Block_PrintDefaultBlock(int blockNumber, int x, int* y){
	switch (blockNumber){
	case -1:
		printf("Ν            Ν");
		CursorUtil_GotoXY(x, (*y)++);
		printf("Ν            Ν");
		break;
	case 0:
		printf("Ν  ＝＝＝＝  Ν");
		CursorUtil_GotoXY(x, (*y)++);
		printf("Ν            Ν");
		break;
	case 1:
		printf("Ν       ＝   Ν");
		CursorUtil_GotoXY(x, (*y)++);
		printf("Ν   ＝＝＝   Ν");
		break;
	case 2:
		printf("Ν     ＝＝   Ν");
		CursorUtil_GotoXY(x, (*y)++);
		printf("Ν   ＝＝     Ν");
		break;
	case 3:
		printf("Ν   ＝＝     Ν");
		CursorUtil_GotoXY(x, (*y)++);
		printf("Ν     ＝＝   Ν");
		break;
	case 4:
		printf("Ν     ＝     Ν");
		CursorUtil_GotoXY(x, (*y)++);
		printf("Ν   ＝＝＝   Ν");
		break;
	case 5:
		printf("Ν   ＝       Ν");
		CursorUtil_GotoXY(x, (*y)++);
		printf("Ν   ＝＝＝   Ν");
		break;
	case 6:
		printf("Ν    ＝＝    Ν");
		CursorUtil_GotoXY(x, (*y)++);
		printf("Ν    ＝＝    Ν");
		break;
	}
}

void Block_ChangeNext(Block block){
	int next,next1,next2;
	int temp;

	Queue_At(&block.next, &next, 0, sizeof(int));		//queue税 0腰走拭 眼延 next舛左(鷺薫曽嫌)研 亜閃紳陥.
	next1=next;											//亜閃紳 鷺系 舛左研 next1拭 差紫廃陥.

	Queue_At(&block.next, &next, 1, sizeof(int));		//queue税 1腰走拭 眼延 next舛左(鷺薫曽嫌)研 亜閃紳陥.
	next2=next;											//亜閃紳 鷺系 舛左研 next2拭 差紫廃陥.

	temp=next1;											//temp研 戚遂背 next1税 舛左人 next2税 舛左研 郊菓陥.
	next1=next2;
	next2=temp;

	Queue_Modify(&block.next, &next1, 0, sizeof(int));	//queue税 0腰走税 鎧遂聖 next1税 舛左稽 呪舛廃陥.
	Queue_Modify(&block.next, &next2, 1, sizeof(int));	//queue税 1腰走税 鎧遂聖 next2税 舛左稽 呪舛廃陥.
}

void Block_BlindNext(Block block, int index, int x, int y){
	ScreenUtil_ClearRectangle(x + 2, y + 1, 12, 2); // use temp size (magic number)
	CursorUtil_GotoXY(x, y++);
	printf("ΞΜ Next %d ΜΟ", index + 1);
	CursorUtil_GotoXY(x, y++);
	_Block_PrintDefaultBlock(-1, x, &y);
	CursorUtil_GotoXY(x, y++);
	printf("ΡΜΜΜΜΜΜΠ");
}