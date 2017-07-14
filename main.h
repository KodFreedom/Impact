//--------------------------------------------------------------------------------
//
//　main.h
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _MAIN_H_
#define _MAIN_H_

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include <Windows.h>
#include <math.h>
#include "KF_Precision.h"
#include <d3dx9.h>
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>
#include <XAudio2.h>//sound
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")//システム時刻取得に必要
#pragma comment(lib,"dinput8.lib")



//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define SCREEN_WIDTH (1280)//ウインドウ幅
#define SCREEN_HEIGHT (720)//ウインドウ高さ
#define NUM_POLYGON (2)//ポリゴン数
#define NUM_VERTEX (4)//頂点数
#define FVF_VERTEX_2D (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)//頂点フォーマット
#define TIMER_INTERVAL (1000/60)//更新間隔

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct//頂点情報構造体　上の頂点フォーマットと構造を合わせること
{
	D3DXVECTOR3 pos;//xyz
	float rhw;//rhw
	D3DCOLOR color;//color
	D3DXVECTOR2 tex;//uv
}VERTEX_2D;

typedef enum//画面遷移のモードの列挙
{
	MODE_TITLE = 0,
	MODE_SAMPLE,
	MODE_GAME,
	MODE_RESULT,
	MODE_MAX
}MODE;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
LPDIRECT3DDEVICE9 GetDevice(void);
void SetMode(MODE mode);
MODE GetMode(void);

#endif