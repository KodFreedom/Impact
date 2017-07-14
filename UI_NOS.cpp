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
#include "UI_NOS.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define PLAYER_NOS_POS_X (163.0f - 0.5f)//ポリゴンの表示位置X
#define PLAYER_NOS_POS_Y (185.0f - 0.5f)//ポリゴンの表示位置Y
#define ENEMY_NOS_POS_X (930.0f - 0.5f)//ポリゴンの表示位置X
#define ENEMY_NOS_POS_Y (185.0f - 0.5f)//ポリゴンの表示位置Y
#define UI_NOS_WIDTH (265.0f)
#define UI_NOS_HEIGHT (30.0f)
#define NOS_TEXTURENAME "data/TEXTURE/NOS.png"//ファイル名

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexUINOS(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureUINOS = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferUINOS = NULL;//頂点バッファ管理インターフェースポインタ

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitUINOS(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_pTextureUINOS = NULL;

	if (FAILED(MakeVerTexUINOS(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexUINOS ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, NOS_TEXTURENAME, &g_pTextureUINOS)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitUINOS(void)
{
	//safe release
	if (g_pVtxBufferUINOS != NULL)
	{
		g_pVtxBufferUINOS->Release();
		g_pVtxBufferUINOS = NULL;
	}

	if (g_pTextureUINOS != NULL)
	{
		g_pTextureUINOS->Release();
		g_pTextureUINOS = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateUINOS(void)
{
	VERTEX_2D *pVtx = NULL;
	int aNOS[2];
	float aPosX[2];
	float aPosY[2];

	aNOS[0] = GetPlayerNOS();
	aNOS[1] = GetEnemyNOS();
	aPosX[0] = PLAYER_NOS_POS_X;
	aPosX[1] = ENEMY_NOS_POS_X;
	aPosY[0] = PLAYER_NOS_POS_Y;
	aPosY[1] = ENEMY_NOS_POS_Y;

	g_pVtxBufferUINOS->Lock(0, 0, (void**)&pVtx, 0);

	//スコア計算
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(aPosX[nCnt] + UI_NOS_WIDTH * (float)(NOS_MAX - aNOS[nCnt]) / (float)NOS_MAX, aPosY[nCnt], 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(aPosX[nCnt] + UI_NOS_WIDTH, aPosY[nCnt], 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(aPosX[nCnt] + UI_NOS_WIDTH * (float)(NOS_MAX - aNOS[nCnt]) / (float)NOS_MAX, aPosY[nCnt] + UI_NOS_HEIGHT, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(aPosX[nCnt] + UI_NOS_WIDTH, aPosY[nCnt] + UI_NOS_HEIGHT, 0.0f);

		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(1.0f * (float)(NOS_MAX - aNOS[nCnt]) / (float)NOS_MAX, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(1.0f * (float)(NOS_MAX - aNOS[nCnt]) / (float)NOS_MAX, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}
	g_pVtxBufferUINOS->Unlock();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawUINOS(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferUINOS,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

	//NOS
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		//Textureの設定
		pDevice->SetTexture(0, g_pTextureUINOS);

		//プリミティブ描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//オフセット（頂点数）
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexUINOS(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * 2,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferUINOS,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferUINOS->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(PLAYER_NOS_POS_X , PLAYER_NOS_POS_Y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PLAYER_NOS_POS_X + UI_NOS_WIDTH, PLAYER_NOS_POS_Y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PLAYER_NOS_POS_X, PLAYER_NOS_POS_Y + UI_NOS_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PLAYER_NOS_POS_X + UI_NOS_WIDTH, PLAYER_NOS_POS_Y + UI_NOS_HEIGHT, 0.0f);

	pVtx[4].pos = D3DXVECTOR3(ENEMY_NOS_POS_X, ENEMY_NOS_POS_Y, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(ENEMY_NOS_POS_X + UI_NOS_WIDTH, ENEMY_NOS_POS_Y, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(ENEMY_NOS_POS_X, ENEMY_NOS_POS_Y + UI_NOS_HEIGHT, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(ENEMY_NOS_POS_X + UI_NOS_WIDTH, ENEMY_NOS_POS_Y + UI_NOS_HEIGHT, 0.0f);

	//頂点カラーの設定(0～255の整数値)
	pVtx[0].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pVtx[1].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pVtx[2].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pVtx[3].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	pVtx[4].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[5].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[6].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[7].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	//rhwの設定(必ず1.0f)
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//texture頂点
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferUINOS->Unlock();

	return S_OK;
}