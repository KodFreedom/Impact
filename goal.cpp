//--------------------------------------------------------------------------------
//
//　Goal.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "goal.h"
#include "player.h"
#include "enemy.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define PLAYER_GOAL_POS_X (GAME_AREA_LEFT - 0.5f)//ポリゴンの表示位置X
#define PLAYER_GOAL_POS_Y (GAME_AREA_TOP - 0.5f)//ポリゴンの表示位置Y
#define PLAYER_GOAL_WIDTH (39.0f)
#define PLAYER_GOAL_HEIGHT (GAME_AREA_BOTTOM - GAME_AREA_TOP)
#define ENEMY_GOAL_WIDTH (39.0f)
#define ENEMY_GOAL_HEIGHT (GAME_AREA_BOTTOM - GAME_AREA_TOP)
#define ENEMY_GOAL_POS_X (GAME_AREA_RIGHT - ENEMY_GOAL_WIDTH - 0.5f)//ポリゴンの表示位置X
#define ENEMY_GOAL_POS_Y (GAME_AREA_TOP - 0.5f)//ポリゴンの表示位置Y
#define GOAL_TEXTURENAME "data/TEXTURE/fade.png"//ファイル名

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGoal(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct {
	float fAlpha;
	int nLife;
}GOAL;

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureGoal = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferGoal = NULL;//頂点バッファ管理インターフェースポインタ
GOAL g_aGoal[2];

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitGoal(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		g_aGoal[nCnt].nLife = 0;
		g_aGoal[nCnt].fAlpha = 0.5f;
	}

	if (FAILED(MakeVerTexGoal(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexGoal ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, GOAL_TEXTURENAME, &g_pTextureGoal)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitGoal(void)
{
	//safe release
	if (g_pVtxBufferGoal != NULL)
	{
		g_pVtxBufferGoal->Release();
		g_pVtxBufferGoal = NULL;
	}

	if (g_pTextureGoal != NULL)
	{
		g_pTextureGoal->Release();
		g_pTextureGoal = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateGoal(void)
{
	VERTEX_2D *pVtx;
	g_pVtxBufferGoal->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aGoal[nCnt].fAlpha);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aGoal[nCnt].fAlpha);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aGoal[nCnt].fAlpha);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aGoal[nCnt].fAlpha);

		if (g_aGoal[nCnt].nLife % 10 < 5)
		{
			g_aGoal[nCnt].fAlpha = 0.5f;
		}
		else
		{
			g_aGoal[nCnt].fAlpha = 0.0f;
		}

		g_aGoal[nCnt].nLife = g_aGoal[nCnt].nLife == 0 ? 0 : g_aGoal[nCnt].nLife - 1;
	}

	g_pVtxBufferGoal->Unlock();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawGoal(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferGoal,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

								   //Textureの設定
	pDevice->SetTexture(0, g_pTextureGoal);

	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		//プリミティブ描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//オフセット（頂点数）
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGoal(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * 2,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferGoal,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferGoal->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(PLAYER_GOAL_POS_X, PLAYER_GOAL_POS_Y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PLAYER_GOAL_POS_X + PLAYER_GOAL_WIDTH, PLAYER_GOAL_POS_Y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PLAYER_GOAL_POS_X, PLAYER_GOAL_POS_Y + PLAYER_GOAL_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PLAYER_GOAL_POS_X + PLAYER_GOAL_WIDTH, PLAYER_GOAL_POS_Y + PLAYER_GOAL_HEIGHT, 0.0f);

	pVtx[4].pos = D3DXVECTOR3(ENEMY_GOAL_POS_X, ENEMY_GOAL_POS_Y, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(ENEMY_GOAL_POS_X + ENEMY_GOAL_WIDTH, ENEMY_GOAL_POS_Y, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(ENEMY_GOAL_POS_X, ENEMY_GOAL_POS_Y + ENEMY_GOAL_HEIGHT, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(ENEMY_GOAL_POS_X + ENEMY_GOAL_WIDTH, ENEMY_GOAL_POS_Y + ENEMY_GOAL_HEIGHT, 0.0f);

	//rhwの設定(必ず1.0f)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	pVtx[4].rhw = 1.0f;
	pVtx[5].rhw = 1.0f;
	pVtx[6].rhw = 1.0f;
	pVtx[7].rhw = 1.0f;

	//頂点カラーの設定(0～255の整数値)
	pVtx[0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

	pVtx[4].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[5].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[6].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[7].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

	//texture頂点
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx[4].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[5].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[6].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[7].tex = D3DXVECTOR2(1.0f, 1.0f);

	//仮想アドレス解放
	g_pVtxBufferGoal->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  goal check
//--------------------------------------------------------------------------------
bool GoalContact(D3DXVECTOR3 vBallPos, BALL_TYPE type)
{
	//player goal
	if (vBallPos.x <= PLAYER_GOAL_POS_X + PLAYER_GOAL_WIDTH)
	{
		g_aGoal[0].nLife = 60;
		AddEnemyScore((int)type * 150 + 100);
		return true;
	}

	//enemy goal
	if (vBallPos.x >= ENEMY_GOAL_POS_X)
	{
		g_aGoal[1].nLife = 60;
		AddPlayerScore((int)type * 150 + 100);
		return true;
	}

	return false;
}