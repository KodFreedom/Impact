//--------------------------------------------------------------------------------
//
//�@game.cpp
//	Author : Xu Wenjie
//	Date   : 2016-07-01
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "input.h"
#include "game.h"
#include "KF_BodyContact.h"
#include "player.h"
#include "enemy.h"
#include "ball.h"
#include "goal.h"
#include "timer.h"
#include "bg.h"
#include "effect.h"
#include "tire_effect.h"
#include "NOS_effect.h"
#include "shadow.h"
#include "fade.h"
#include "UI_Score.h"
#include "UI_NOS.h"
#include "game_start.h"
#include "game_finish.h"
#include "game_pause.h"

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitGame(void)
{
	InitGameStart();
	InitGameFinish();
	InitGamePause();
	InitPlayer();
	InitEnemy();
	InitBall();
	InitGoal();
	InitBodyContact();
	InitBG();
	InitTimer();

	//effect
	InitEffect();
	InitTireEffect();
	InitNOSEffect();
	InitShadow();

	//UI
	InitUIScore();
	InitUINOS();
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitGame(void)
{
	UninitGameStart();
	UninitGameFinish();
	UninitGamePause();
	UninitPlayer();
	UninitEnemy();
	UninitBall();
	UninitGoal();
	UninitBodyContact();
	UninitBG();
	UninitTimer();

	//Effect
	UninitEffect();
	UninitTireEffect();
	UninitNOSEffect();
	UninitShadow();

	//UI
	UninitUIScore();
	UninitUINOS();
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateGame(void)
{
	//�Q�[���X�^�[�g�J�E���^�[
	if (UpdateGamePause()) { return; }
	if (UpdateGameStart()) { return; }

	//UI
	UpdateUIScore();
	UpdateUINOS();

	if (UpdateGameFinish()) { return; }

	UpdateBall();
	UpdateEnemy();
	UpdatePlayer();
	/*UpdateBodyContact();*/

	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		UpdateEnemyContact();
		UpdatePlayerContact();
		UpdateBodyContact();
	}

	UpdateEnemyControl();
	UpdatePlayerControl();

	UpdateGoal();
	UpdateBG();

	UpdateTimer();

	//Effect
	UpdateEffect();
	UpdateTireEffect();
	UpdateNOSEffect();
	UpdateShadow();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawGame(void)
{
	DrawBG();
	DrawGoal();
	DrawTireEffect();
	DrawNOSEffect();
	DrawShadow();
	DrawBall();
	DrawPlayer();
	DrawEnemy();
	DrawTimer();
	DrawEffect();

	//UI
	DrawUIScore();
	DrawUINOS();

	//�Q�[���X�^�[�g�J�E���^�[
	DrawGameStart();
	DrawGameFinish();
	DrawGamePause();
}