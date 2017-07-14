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
#include "ready.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define READY_WIDTH (207.0f)//ポリゴンの表示位置Y
#define READY_HEIGHT (51.0f)//ポリゴンの表示位置Y
#define READY_TEXTURENAME "data/TEXTURE/ready.png"//ファイル名
#define READY_NUM (2)

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexReady(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureReady = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferReady = NULL;//頂点バッファ管理インターフェースポインタ
bool g_bReady[READY_NUM];

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitReady(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	for (int nCnt = 0;nCnt < READY_NUM;nCnt++)
	{
		g_bReady[nCnt] = false;
	}

	if (FAILED(MakeVerTexReady(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexReady ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, READY_TEXTURENAME, &g_pTextureReady)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitReady(void)
{
	//safe release
	if (g_pVtxBufferReady != NULL)
	{
		g_pVtxBufferReady->Release();
		g_pVtxBufferReady = NULL;
	}

	if (g_pTextureReady != NULL)
	{
		g_pTextureReady->Release();
		g_pTextureReady = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateReady(void)
{
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawReady(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferReady,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

								   //Textureの設定
	pDevice->SetTexture(0, g_pTextureReady);

	//プリミティブ描画
	for (int nCnt = 0;nCnt < READY_NUM;nCnt++)
	{
		if (g_bReady[nCnt])
		{
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCnt * 4,//オフセット（頂点数）
				NUM_POLYGON);
		}
	}
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexReady(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * READY_NUM,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferReady,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferReady->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	for (int nCnt = 0;nCnt < READY_NUM;nCnt++)
	{
		//頂点座標の設定（2D座標、右回り）
		pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(SCREEN_WIDTH * (0.205f + nCnt * 0.59f) - READY_WIDTH * 0.5f, SCREEN_HEIGHT * 0.92f - READY_HEIGHT * 0.5f, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(SCREEN_WIDTH * (0.205f + nCnt * 0.59f) + READY_WIDTH * 0.5f, SCREEN_HEIGHT * 0.92f - READY_HEIGHT * 0.5f, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(SCREEN_WIDTH * (0.205f + nCnt * 0.59f) - READY_WIDTH * 0.5f, SCREEN_HEIGHT * 0.92f + READY_HEIGHT * 0.5f, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(SCREEN_WIDTH * (0.205f + nCnt * 0.59f) + READY_WIDTH * 0.5f, SCREEN_HEIGHT * 0.92f + READY_HEIGHT * 0.5f, 0.0f);

		//rhwの設定(必ず1.0f)
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//頂点カラーの設定(0～255の整数値)
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(1.0f * nCnt, 0.0f, 1.0f*(1 - nCnt), 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(1.0f * nCnt, 0.0f, 1.0f*(1 - nCnt), 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(1.0f * nCnt, 0.0f, 1.0f*(1 - nCnt), 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(1.0f * nCnt, 0.0f, 1.0f*(1 - nCnt), 1.0f);

		//texture頂点
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferReady->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  位置と色を設定
//--------------------------------------------------------------------------------
void SetReady(int nNumReady)
{
	g_bReady[nNumReady] = true;
}