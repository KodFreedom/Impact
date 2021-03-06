//--------------------------------------------------------------------------------
//
//　Effect.h
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _NOS_EFFECT_H_
#define _NOS_EFFECT_H_

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void InitNOSEffect(void);
void UninitNOSEffect(void);
void UpdateNOSEffect(void);
void DrawNOSEffect(void);
void SetNOSEffect(D3DXVECTOR3 vPos, D3DXCOLOR cColor);

#endif