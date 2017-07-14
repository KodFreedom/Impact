//--------------------------------------------------------------------------------
//
//　Ball.h
//	Author : Xu Wenjie
//	Date   : 2016-05-31
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _BALL_H_
#define _BALL_H_

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "KF_Body.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define BALL_POS (D3DXVECTOR3((GAME_AREA_LEFT + GAME_AREA_RIGHT) * 0.5f, (GAME_AREA_TOP + GAME_AREA_BOTTOM) * 0.5f, 0.0f))//ポリゴンの表示位置
#define BALL_MASS (30.0f)//ボールの重さ
#define BALL_RADIUS (15.0f)//ボールの半径

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef enum {
	BALL_WHITE,//普通
	BALL_BLACK,//重い
	BALL_GOLDEN,//点数高い
	BALL_MAX
}BALL_TYPE;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void InitBall(void);
void UninitBall(void);
void UpdateBall(void);
void DrawBall(void);
//void UpdateBallContact(void);
D3DXVECTOR3 GetBallPos(void);
D3DXVECTOR3 GetBallVelocity(void);
int GetBallCnt(void);
CBodyCicle *GetBall(void);

#endif