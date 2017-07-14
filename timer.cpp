//--------------------------------------------------------------------------------
//
//　timer
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "timer.h"
#include "main.h"
#include "game_finish.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define TIMER_TEXTURENAME "data/TEXTURE/num00.png"//ファイル名
#define TIMER_POS_X (522.0f - 0.5f)
#define TIMER_POS_Y (91.0f - 0.5f)
#define TIMER_WIDTH (72.0f)
#define TIMER_HEIGHT (128.0f)
#define TIMER_SPACE (8.0f)//数字間の長さ
#define TIMER_MAX (99 * 60)
#define DEGITAL_MAX (3)//桁数

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexTimer(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureTimer = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferTimer = NULL;//頂点バッファ管理インターフェースポインタ
int g_nTimer;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitTimer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_nTimer = TIMER_MAX;

	if (FAILED(MakeVerTexTimer(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexTimer ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		TIMER_TEXTURENAME,
		&g_pTextureTimer)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitTimer(void)
{
	if (g_pVtxBufferTimer != NULL)
	{
		g_pVtxBufferTimer->Release();
		g_pVtxBufferTimer = NULL;
	}

	if (g_pTextureTimer != NULL)
	{
		g_pTextureTimer->Release();
		g_pTextureTimer = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateTimer(void)
{
	VERTEX_2D *pVtx = NULL;
	int nCnt;
	int nWork = g_nTimer / 60;

	g_pVtxBufferTimer->Lock(0, 0, (void**)&pVtx, 0);

	//texture計算
	for (nCnt = DEGITAL_MAX - 1;nCnt >= 0;nCnt--)
	{
		pVtx[nCnt * 4].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 1.0f);
		nWork /= 10;
	}
	g_pVtxBufferTimer->Unlock();

	g_nTimer--;

	//時間切り
	if (g_nTimer == 0)
	{
		StartGameFinish();
	}
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawTimer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得
	int nCnt;

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferTimer,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

								   //Textureの設定
	pDevice->SetTexture(0, g_pTextureTimer);

	for (nCnt = 0;nCnt < DEGITAL_MAX;nCnt++)
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
HRESULT MakeVerTexTimer(LPDIRECT3DDEVICE9 pDevice)
{
	int nCnt;
	//
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * DEGITAL_MAX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferTimer,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferTimer->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	for (nCnt = 0;nCnt < DEGITAL_MAX;nCnt++)
	{
		//頂点座標の設定（2D座標、右回り）
		pVtx[nCnt * 4].pos = D3DXVECTOR3(TIMER_POS_X + (TIMER_WIDTH + TIMER_SPACE) * nCnt, TIMER_POS_Y, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(TIMER_POS_X + (TIMER_WIDTH + TIMER_SPACE) * nCnt + TIMER_WIDTH, TIMER_POS_Y, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(TIMER_POS_X + (TIMER_WIDTH + TIMER_SPACE) * nCnt, TIMER_POS_Y + TIMER_HEIGHT, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(TIMER_POS_X + (TIMER_WIDTH + TIMER_SPACE) * nCnt + TIMER_WIDTH, TIMER_POS_Y + TIMER_HEIGHT, 0.0f);

		//rhwの設定(必ず1.0f)
		pVtx[nCnt * 4].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//頂点カラーの設定(0～255の整数値)
		pVtx[nCnt * 4].color = D3DCOLOR_RGBA(221, 221, 221, 255);
		pVtx[nCnt * 4 + 1].color = D3DCOLOR_RGBA(221, 221, 221, 255);
		pVtx[nCnt * 4 + 2].color = D3DCOLOR_RGBA(221, 221, 221, 255);
		pVtx[nCnt * 4 + 3].color = D3DCOLOR_RGBA(221, 221, 221, 255);

		//texture頂点
		pVtx[nCnt * 4].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(0.0f, 0.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferTimer->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  時間取得
//--------------------------------------------------------------------------------
int GetTimer(void)
{
	return g_nTimer / 60;
}