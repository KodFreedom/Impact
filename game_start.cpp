//--------------------------------------------------------------------------------
//
//　GameStart.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "game_start.h"
#include "sound.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define GAME_START_POS_X (SCREEN_WIDTH * 0.5f)//ポリゴンの表示位置X
#define GAME_START_POS_Y (SCREEN_HEIGHT * 0.5f)//ポリゴンの表示位置Y
#define GAME_START_WIDTH (474.0f)
#define GAME_START_HEIGHT (164.0f)
#define GAME_START_TEXTURENAME "data/TEXTURE/gamestart.png"//ファイル名

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGameStart(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureGameStart = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferGameStart = NULL;//頂点バッファ管理インターフェースポインタ
int g_nCntStart;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitGameStart(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_nCntStart = 4 * 60;

	if (FAILED(MakeVerTexGameStart(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexGameStart ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, GAME_START_TEXTURENAME, &g_pTextureGameStart)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitGameStart(void)
{
	//safe release
	if (g_pVtxBufferGameStart != NULL)
	{
		g_pVtxBufferGameStart->Release();
		g_pVtxBufferGameStart = NULL;
	}

	if (g_pTextureGameStart != NULL)
	{
		g_pTextureGameStart->Release();
		g_pTextureGameStart = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
bool UpdateGameStart(void)
{
	if (g_nCntStart == 0) { return false; }

	if (g_nCntStart == 1)
	{
		PlaySound(SOUND_LABEL_BGM_GAME);
	}

	if (g_nCntStart == 4 * 60)
	{
		PlaySound(SOUND_LABEL_SE_SIGNAL);
	}

	g_nCntStart--;
	int nWork = 3 - g_nCntStart / 60;
	

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferGameStart->Lock(0,0,(void**)&pVtx,0);

	//texture頂点
	pVtx[0].tex = D3DXVECTOR2(0.0f, nWork * 0.25f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, nWork * 0.25f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, nWork * 0.25f + 0.25f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, nWork * 0.25f + 0.25f);

	//仮想アドレス解放
	g_pVtxBufferGameStart->Unlock();

	return true;
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawGameStart(void)
{
	if (g_nCntStart == 0) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferGameStart,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

								   //Textureの設定
	pDevice->SetTexture(0, g_pTextureGameStart);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//オフセット（頂点数）
		NUM_POLYGON);

}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGameStart(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferGameStart,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferGameStart->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(GAME_START_POS_X - GAME_START_WIDTH * 0.5f, GAME_START_POS_Y - GAME_START_HEIGHT * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(GAME_START_POS_X + GAME_START_WIDTH * 0.5f, GAME_START_POS_Y - GAME_START_HEIGHT * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(GAME_START_POS_X - GAME_START_WIDTH * 0.5f, GAME_START_POS_Y + GAME_START_HEIGHT * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(GAME_START_POS_X + GAME_START_WIDTH * 0.5f, GAME_START_POS_Y + GAME_START_HEIGHT * 0.5f, 0.0f);

	//rhwの設定(必ず1.0f)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//頂点カラーの設定(0～255の整数値)
	pVtx[0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//texture頂点
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 0.25f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 0.25f);

	//仮想アドレス解放
	g_pVtxBufferGameStart->Unlock();

	return S_OK;
}