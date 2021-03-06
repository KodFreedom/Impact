//--------------------------------------------------------------------------------
//
//　GameFinish.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "game_start.h"
#include "fade.h"
#include "sound.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define GAME_FINISH_POS_X (SCREEN_WIDTH * 0.5f)//ポリゴンの表示位置X
#define GAME_FINISH_POS_Y (SCREEN_HEIGHT * 0.5f)//ポリゴンの表示位置Y
#define GAME_FINISH_WIDTH (1265.0f * 8.0f)
#define GAME_FINISH_HEIGHT (380.0f * 8.0f)
#define GAME_FINISH_SPEED (0.90f)
#define GAME_FINISH_TEXTURENAME "data/TEXTURE/time.png"//ファイル名

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGameFinish(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureGameFinish = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferGameFinish = NULL;//頂点バッファ管理インターフェースポインタ
D3DXVECTOR3 g_vHalfSize;
int g_nCntFinish;
float g_fFinishAlpha;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitGameFinish(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_nCntFinish = 0;
	g_fFinishAlpha = 0.0f;
	g_vHalfSize = D3DXVECTOR3(GAME_FINISH_WIDTH * 0.5f, GAME_FINISH_HEIGHT * 0.5f, 0.0f);

	if (FAILED(MakeVerTexGameFinish(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexGameFinish ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, GAME_FINISH_TEXTURENAME, &g_pTextureGameFinish)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitGameFinish(void)
{
	StopSound(SOUND_LABEL_BGM_GAME);

	//safe release
	if (g_pVtxBufferGameFinish != NULL)
	{
		g_pVtxBufferGameFinish->Release();
		g_pVtxBufferGameFinish = NULL;
	}

	if (g_pTextureGameFinish != NULL)
	{
		g_pTextureGameFinish->Release();
		g_pTextureGameFinish = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
bool UpdateGameFinish(void)
{
	if (g_nCntFinish == 0)
	{
		return false;
	}

	if (g_nCntFinish == 60)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

	g_nCntFinish--;

	if (g_nCntFinish >= 2 * 60 + 30)
	{
		g_vHalfSize *= GAME_FINISH_SPEED;
		g_fFinishAlpha += 0.05f;
	}
	else if(g_nCntFinish <= 1 * 60)
	{
		g_vHalfSize /= GAME_FINISH_SPEED;
		g_fFinishAlpha -= 0.05f;
	}
	
	g_fFinishAlpha = g_fFinishAlpha >= 1.0f ? 1.0f : g_fFinishAlpha <= 0.0f ? 0.0f : g_fFinishAlpha;

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferGameFinish->Lock(0, 0, (void**)&pVtx, 0);

	//texture頂点
	pVtx[0].pos = D3DXVECTOR3(GAME_FINISH_POS_X - g_vHalfSize.x, GAME_FINISH_POS_Y - g_vHalfSize.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(GAME_FINISH_POS_X + g_vHalfSize.x, GAME_FINISH_POS_Y - g_vHalfSize.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(GAME_FINISH_POS_X - g_vHalfSize.x, GAME_FINISH_POS_Y + g_vHalfSize.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(GAME_FINISH_POS_X + g_vHalfSize.x, GAME_FINISH_POS_Y + g_vHalfSize.y, 0.0f);

	//頂点カラーの設定(0〜255の整数値)
	pVtx[0].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, g_fFinishAlpha);
	pVtx[1].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, g_fFinishAlpha);
	pVtx[2].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, g_fFinishAlpha);
	pVtx[3].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, g_fFinishAlpha);

	//仮想アドレス解放
	g_pVtxBufferGameFinish->Unlock();

	return true;
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawGameFinish(void)
{
	if (g_nCntFinish == 0) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferGameFinish,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

								   //Textureの設定
	pDevice->SetTexture(0, g_pTextureGameFinish);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//オフセット（頂点数）
		NUM_POLYGON);

}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGameFinish(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferGameFinish,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferGameFinish->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(GAME_FINISH_POS_X - g_vHalfSize.x, GAME_FINISH_POS_Y - g_vHalfSize.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(GAME_FINISH_POS_X + g_vHalfSize.x, GAME_FINISH_POS_Y - g_vHalfSize.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(GAME_FINISH_POS_X - g_vHalfSize.x, GAME_FINISH_POS_Y + g_vHalfSize.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(GAME_FINISH_POS_X + g_vHalfSize.x, GAME_FINISH_POS_Y + g_vHalfSize.y, 0.0f);

	//rhwの設定(必ず1.0f)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//頂点カラーの設定(0〜255の整数値)
	pVtx[0].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
	pVtx[1].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
	pVtx[2].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
	pVtx[3].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);

	//texture頂点
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//仮想アドレス解放
	g_pVtxBufferGameFinish->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  GameFinish開始
//--------------------------------------------------------------------------------
void StartGameFinish(void)
{
	if (g_nCntFinish != 0) { return; }

	PlaySound(SOUND_LABEL_SE_TIME);
	StopSound(SOUND_LABEL_BGM_GAME);
	g_nCntFinish = 3 * 60;
}