//--------------------------------------------------------------------------------
//
//　Fade.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "fade.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define FADE_POS_X (-0.5f)//ポリゴンの表示位置X
#define FADE_POS_Y (-0.5f)//ポリゴンの表示位置Y
#define FADE_RATE (1.0f / (60.0f * 0.5f))//フェード係数
#define FADE_TEXTURENAME "data/TEXTURE/fade.png"//ファイル名

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexFade(LPDIRECT3DDEVICE9 pDevice);
void SetColorFade(D3DXCOLOR color);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureFade = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferFade = NULL;//頂点バッファ管理インターフェースポインタ
D3DXCOLOR g_colorFade;
FADE g_fade = FADE_IN;//fade mode
MODE g_modeNext = MODE_TITLE;//roop mode
int g_nCnt;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitFade(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_nCnt = 0;
	g_colorFade = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);//真っ黒にする

	if (FAILED(MakeVerTexFade(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexFade ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, FADE_TEXTURENAME, &g_pTextureFade)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitFade(void)
{
	//safe release
	if (g_pVtxBufferFade != NULL)
	{
		g_pVtxBufferFade->Release();
	}

	if (g_pTextureFade != NULL)
	{
		g_pTextureFade->Release();
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateFade(void)
{
	if (g_fade == FADE_NONE) { return; }

	//fade時間カウント
	if (g_nCnt != 0) 
	{
		g_nCnt -= 1;
		return;
	}

	if (g_fade == FADE_IN)
	{
		g_colorFade.a -= FADE_RATE;//alphaを減算して後ろの画面を浮き上がらせる

		if (g_colorFade.a <= 0.0f)
		{
			g_colorFade.a = 0.0f;
			g_fade = FADE_NONE;
		}
	}
	else if (g_fade == FADE_OUT)
	{
		g_colorFade.a += FADE_RATE;

		if (g_colorFade.a >= 1.0f)
		{
			g_colorFade.a = 1.0f;
			g_fade = FADE_IN;
			SetMode(g_modeNext);
		}
	}

	SetColorFade(g_colorFade);
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawFade(void)
{
	if (g_fade == FADE_NONE)
	{
		return;
	}

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferFade,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureFade);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//オフセット（頂点数）
		NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexFade(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferFade,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferFade->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(FADE_POS_X, FADE_POS_Y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(FADE_POS_X + SCREEN_WIDTH, FADE_POS_Y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(FADE_POS_X, FADE_POS_Y + SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(FADE_POS_X + SCREEN_WIDTH, FADE_POS_Y + SCREEN_HEIGHT, 0.0f);

	//rhwの設定(必ず1.0f)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//頂点カラーの設定(0～255の整数値)
	pVtx[0].color = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[1].color = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[2].color = D3DCOLOR_RGBA(255, 255, 255, 255);
	pVtx[3].color = D3DCOLOR_RGBA(255, 255, 255, 255);

	//texture頂点
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//仮想アドレス解放
	g_pVtxBufferFade->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
void SetColorFade(D3DXCOLOR color)
{
	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx = NULL;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferFade->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点カラーの設定(0～255の整数値)
	pVtx[0].color = color;
	pVtx[1].color = color;
	pVtx[2].color = color;
	pVtx[3].color = color;

	//仮想アドレス解放
	g_pVtxBufferFade->Unlock();
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
void SetFade(FADE fade, MODE modeNext)
{
	if (g_fade != FADE_NONE) { return; }

	g_fade = fade;
	g_modeNext = modeNext;
	g_nCnt = 10;
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
FADE GetFade(void)
{
	return g_fade;
}