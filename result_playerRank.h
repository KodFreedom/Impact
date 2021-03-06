//--------------------------------------------------------------------------------
//  ( PlayerRank.h )
//  Author : XU WENJIE
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  マクロ定義
//--------------------------------------------------------------------------------
#ifndef _RESULT_PLAYER_RANK_H_
#define _RESULT_PLAYER_RANK_H_

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void InitPlayerRank(void);
void UninitPlayerRank(void);
void UpdatePlayerRank(void);
void DrawPlayerRank(void);
void SetPlayerRank(int nRank);
void SetEnemyRank(int nRank);
#endif