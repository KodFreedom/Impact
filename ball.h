//--------------------------------------------------------------------------------
//
//�@Ball.h
//	Author : Xu Wenjie
//	Date   : 2016-05-31
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _BALL_H_
#define _BALL_H_

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "KF_Body.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define BALL_POS (D3DXVECTOR3((GAME_AREA_LEFT + GAME_AREA_RIGHT) * 0.5f, (GAME_AREA_TOP + GAME_AREA_BOTTOM) * 0.5f, 0.0f))//�|���S���̕\���ʒu
#define BALL_MASS (30.0f)//�{�[���̏d��
#define BALL_RADIUS (15.0f)//�{�[���̔��a

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef enum {
	BALL_WHITE,//����
	BALL_BLACK,//�d��
	BALL_GOLDEN,//�_������
	BALL_MAX
}BALL_TYPE;

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
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