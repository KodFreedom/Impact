//--------------------------------------------------------------------------------
//  PlayerRank.cpp
//  Author : XU WENJIE
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"
#include "result_playerRank.h"

//--------------------------------------------------------------------------------
//  マクロ定義
//--------------------------------------------------------------------------------
#define PLAYER_RANK_POS_X (116.0f)
#define PLAYER_RANK_POS_Y (330.0f)
#define ENEMY_RANK_POS_X (984.0f)
#define ENEMY_RANK_POS_Y (330.0f)
#define NUM_PLAYER (2)
#define SCORE_WIDTH (85.0f)
#define SCORE_HEIGHT (150.0f)
#define SCORE_SPACE (14.0f)
#define DEGITAL_MAX (2)//桁数
#define PLAYER_RANK_TEXTURENAME "data/TEXTURE/num00.png"//ファイル名

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPlayerRank(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  グローバル変数宣言
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTexturePlayerRank = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferPlayerRank = NULL;//頂点バッファ管理インターフェースポインタ
int g_aPlayerRank[NUM_PLAYER];
int g_aPlayerRankOutPut[NUM_PLAYER];

//--------------------------------------------------------------------------------
//  初期化関数
//--------------------------------------------------------------------------------
void InitPlayerRank(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_aPlayerRank[0] =
		g_aPlayerRank[1] = 0;

	g_aPlayerRankOutPut[0] =
		g_aPlayerRankOutPut[1] = 99;

	if (FAILED(MakeVerTexPlayerRank(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		PLAYER_RANK_TEXTURENAME,
		&g_pTexturePlayerRank)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  後片付け関数
//--------------------------------------------------------------------------------
void UninitPlayerRank(void)
{
	if (g_pVtxBufferPlayerRank != NULL)
	{
		g_pVtxBufferPlayerRank->Release();
		g_pVtxBufferPlayerRank = NULL;
	}

	if (g_pTexturePlayerRank != NULL)
	{
		g_pTexturePlayerRank->Release();
		g_pTexturePlayerRank = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新関数
//--------------------------------------------------------------------------------
void UpdatePlayerRank(void)
{
	//更新
	int nWork = 0;
	VERTEX_2D *pVtx = NULL;

	g_pVtxBufferPlayerRank->Lock(0, 0, (void**)&pVtx, 0);

	//スコア計算
	for (int nCntPlayer = 0;nCntPlayer < NUM_PLAYER;nCntPlayer++)
	{
		nWork = g_aPlayerRankOutPut[nCntPlayer];
		g_aPlayerRankOutPut[nCntPlayer] = g_aPlayerRankOutPut[nCntPlayer] <= g_aPlayerRank[nCntPlayer] ? g_aPlayerRank[nCntPlayer] : g_aPlayerRankOutPut[nCntPlayer] - 1;

		for (int nCntDig = DEGITAL_MAX - 1;nCntDig >= 0;nCntDig--)
		{
			pVtx[nCntPlayer * 8 + nCntDig * 4 + 0].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 0.0f);
			pVtx[nCntPlayer * 8 + nCntDig * 4 + 1].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 0.0f);
			pVtx[nCntPlayer * 8 + nCntDig * 4 + 2].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 1.0f);
			pVtx[nCntPlayer * 8 + nCntDig * 4 + 3].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 1.0f);
			nWork /= 10;
		}
	}

	g_pVtxBufferPlayerRank->Unlock();
}

//--------------------------------------------------------------------------------
//  描画関数
//--------------------------------------------------------------------------------
void DrawPlayerRank(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferPlayerRank,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

								   //score
	for (int nCnt = 0;nCnt < NUM_PLAYER * DEGITAL_MAX;nCnt++)
	{
		//Textureの設定
		pDevice->SetTexture(0, g_pTexturePlayerRank);

		//プリミティブ描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//オフセット（頂点数）
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPlayerRank(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NUM_PLAYER * DEGITAL_MAX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferPlayerRank,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferPlayerRank->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//player
	for (int nCntDig = 0;nCntDig < DEGITAL_MAX;nCntDig++)
	{
		//頂点座標の設定（2D座標、右回り）
		pVtx[nCntDig * 4 + 0].pos = D3DXVECTOR3(PLAYER_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), PLAYER_RANK_POS_Y, 0.0f);
		pVtx[nCntDig * 4 + 1].pos = D3DXVECTOR3(PLAYER_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, PLAYER_RANK_POS_Y, 0.0f);
		pVtx[nCntDig * 4 + 2].pos = D3DXVECTOR3(PLAYER_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), PLAYER_RANK_POS_Y + SCORE_HEIGHT, 0.0f);
		pVtx[nCntDig * 4 + 3].pos = D3DXVECTOR3(PLAYER_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, PLAYER_RANK_POS_Y + SCORE_HEIGHT, 0.0f);

		//rhwの設定(必ず1.0f)
		pVtx[nCntDig * 4 + 0].rhw = 1.0f;
		pVtx[nCntDig * 4 + 1].rhw = 1.0f;
		pVtx[nCntDig * 4 + 2].rhw = 1.0f;
		pVtx[nCntDig * 4 + 3].rhw = 1.0f;

		//頂点カラーの設定(0～255の整数値)
		pVtx[nCntDig * 4 + 0].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 1].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 2].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 3].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

		//texture頂点
		pVtx[nCntDig * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCntDig * 4 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCntDig * 4 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCntDig * 4 + 3].tex = D3DXVECTOR2(0.0f, 0.0f);
	}

	//enemy
	for (int nCntDig = 0;nCntDig < DEGITAL_MAX;nCntDig++)
	{
		//頂点座標の設定（2D座標、右回り）
		pVtx[8 + nCntDig * 4 + 0].pos = D3DXVECTOR3(ENEMY_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), ENEMY_RANK_POS_Y, 0.0f);
		pVtx[8 + nCntDig * 4 + 1].pos = D3DXVECTOR3(ENEMY_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, ENEMY_RANK_POS_Y, 0.0f);
		pVtx[8 + nCntDig * 4 + 2].pos = D3DXVECTOR3(ENEMY_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), ENEMY_RANK_POS_Y + SCORE_HEIGHT, 0.0f);
		pVtx[8 + nCntDig * 4 + 3].pos = D3DXVECTOR3(ENEMY_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, ENEMY_RANK_POS_Y + SCORE_HEIGHT, 0.0f);

		//rhwの設定(必ず1.0f)
		pVtx[8 + nCntDig * 4 + 0].rhw = 1.0f;
		pVtx[8 + nCntDig * 4 + 1].rhw = 1.0f;
		pVtx[8 + nCntDig * 4 + 2].rhw = 1.0f;
		pVtx[8 + nCntDig * 4 + 3].rhw = 1.0f;

		//頂点カラーの設定(0～255の整数値)
		pVtx[8 + nCntDig * 4 + 0].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[8 + nCntDig * 4 + 1].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[8 + nCntDig * 4 + 2].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[8 + nCntDig * 4 + 3].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		//texture頂点
		pVtx[8 + nCntDig * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[8 + nCntDig * 4 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[8 + nCntDig * 4 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[8 + nCntDig * 4 + 3].tex = D3DXVECTOR2(0.0f, 0.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferPlayerRank->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  Playerランク設定
//--------------------------------------------------------------------------------
void SetPlayerRank(int nRank)
{
	g_aPlayerRank[0] = nRank;
}

//--------------------------------------------------------------------------------
//  Enemyランク設定
//--------------------------------------------------------------------------------
void SetEnemyRank(int nRank)
{
	g_aPlayerRank[1] = nRank;
}