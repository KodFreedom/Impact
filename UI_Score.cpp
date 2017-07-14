//--------------------------------------------------------------------------------
//
//　Result.cpp
//	Author : Xu Wenjie
//	Date   : 2016-07-01
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "enemy.h"
#include "player.h"
#include "UI_Score.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define PLAYER_SCORE_POS_X (59.0f - 0.5f)//ポリゴンの表示位置X
#define PLAYER_SCORE_POS_Y (42.0f - 0.5f)//ポリゴンの表示位置Y
#define ENEMY_SCORE_POS_X (824.0f - 0.5f)//ポリゴンの表示位置X
#define ENEMY_SCORE_POS_Y (42.0f - 0.5f)//ポリゴンの表示位置Y
#define UI_SCORE_WIDTH (74.0f)
#define UI_SCORE_HEIGHT (124.0f)
#define UI_SCORE_SPACE (7.0f)
#define SCORE_TEXTURENAME "data/TEXTURE/num00.png"//ファイル名
#define DEGITAL_MAX (5)//桁数

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexUIScore(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct {
	unsigned int nScore;
	unsigned int nScoreOutput;
}UI_SCORE;

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureUIScore = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferUIScore = NULL;//頂点バッファ管理インターフェースポインタ
UI_SCORE g_aUIScore[2];

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitUIScore(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_pTextureUIScore = NULL;

	//result score init
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		g_aUIScore[nCnt].nScore = 0;
		g_aUIScore[nCnt].nScoreOutput = 0;
	}

	if (FAILED(MakeVerTexUIScore(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexUIScore ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, SCORE_TEXTURENAME, &g_pTextureUIScore)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitUIScore(void)
{
	//safe release
	if (g_pVtxBufferUIScore != NULL)
	{
		g_pVtxBufferUIScore->Release();
		g_pVtxBufferUIScore = NULL;
	}

	if (g_pTextureUIScore != NULL)
	{
		g_pTextureUIScore->Release();
		g_pTextureUIScore = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateUIScore(void)
{
	VERTEX_2D *pVtx = NULL;
	int nWork;

	//スコア取得
	g_aUIScore[0].nScore = GetPlayerScore();
	g_aUIScore[1].nScore = GetEnemyScore();

	g_pVtxBufferUIScore->Lock(0, 0, (void**)&pVtx, 0);

	//スコア計算
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		nWork = g_aUIScore[nCnt].nScoreOutput;
		g_aUIScore[nCnt].nScoreOutput = g_aUIScore[nCnt].nScoreOutput >= g_aUIScore[nCnt].nScore ? g_aUIScore[nCnt].nScore : g_aUIScore[nCnt].nScoreOutput + 2;

		for (int nCntDig = DEGITAL_MAX - 1;nCntDig >= 0;nCntDig--)
		{
			pVtx[nCnt * 20 + nCntDig * 4 + 0].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 0.0f);
			pVtx[nCnt * 20 + nCntDig * 4 + 1].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 0.0f);
			pVtx[nCnt * 20 + nCntDig * 4 + 2].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 1.0f);
			pVtx[nCnt * 20 + nCntDig * 4 + 3].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 1.0f);
			nWork /= 10;

			if (nWork == 0) { break; }
		}
	}
	g_pVtxBufferUIScore->Unlock();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawUIScore(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferUIScore,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

	//score
	for (int nCnt = 0;nCnt < 2 * DEGITAL_MAX;nCnt++)
	{
		//Textureの設定
		pDevice->SetTexture(0, g_pTextureUIScore);

		//プリミティブ描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//オフセット（頂点数）
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexUIScore(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * DEGITAL_MAX * 2,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferUIScore,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferUIScore->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	//player
	for (int nCntDig = 0;nCntDig < DEGITAL_MAX; nCntDig++)
	{
		pVtx[nCntDig * 4 + 0].pos = D3DXVECTOR3(PLAYER_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE), PLAYER_SCORE_POS_Y, 0.0f);
		pVtx[nCntDig * 4 + 1].pos = D3DXVECTOR3(PLAYER_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE) + UI_SCORE_WIDTH, PLAYER_SCORE_POS_Y, 0.0f);
		pVtx[nCntDig * 4 + 2].pos = D3DXVECTOR3(PLAYER_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE), PLAYER_SCORE_POS_Y + UI_SCORE_HEIGHT, 0.0f);
		pVtx[nCntDig * 4 + 3].pos = D3DXVECTOR3(PLAYER_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE) + UI_SCORE_WIDTH, PLAYER_SCORE_POS_Y + UI_SCORE_HEIGHT, 0.0f);

		//頂点カラーの設定(0～255の整数値)
		pVtx[nCntDig * 4 + 0].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 1].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 2].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 3].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	}

	//enemy
	for (int nCntDig = 0;nCntDig < DEGITAL_MAX; nCntDig++)
	{
		pVtx[20 + nCntDig * 4 + 0].pos = D3DXVECTOR3(ENEMY_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE), ENEMY_SCORE_POS_Y, 0.0f);
		pVtx[20 + nCntDig * 4 + 1].pos = D3DXVECTOR3(ENEMY_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE) + UI_SCORE_WIDTH, ENEMY_SCORE_POS_Y, 0.0f);
		pVtx[20 + nCntDig * 4 + 2].pos = D3DXVECTOR3(ENEMY_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE), ENEMY_SCORE_POS_Y + UI_SCORE_HEIGHT, 0.0f);
		pVtx[20 + nCntDig * 4 + 3].pos = D3DXVECTOR3(ENEMY_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE) + UI_SCORE_WIDTH, ENEMY_SCORE_POS_Y + UI_SCORE_HEIGHT, 0.0f);

		pVtx[20 + nCntDig * 4 + 0].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[20 + nCntDig * 4 + 1].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[20 + nCntDig * 4 + 2].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[20 + nCntDig * 4 + 3].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	}

	//rhwの設定(必ず1.0f)
	for (int nCnt = 0;nCnt < DEGITAL_MAX * 2;nCnt++)
	{
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//texture頂点
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(0.0f, 0.0f);
	}
	
	//仮想アドレス解放
	g_pVtxBufferUIScore->Unlock();

	return S_OK;
}