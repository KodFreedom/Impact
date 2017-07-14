//--------------------------------------------------------------------------------
//
//　PlayerGoal.h
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _GOAL_H_
#define _GOAL_H_

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "KF_Body.h"
#include "ball.h"
//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void InitGoal(void);
void UninitGoal(void);
void UpdateGoal(void);
void DrawGoal(void);
bool GoalContact(D3DXVECTOR3 vBallPos, BALL_TYPE type);

#endif