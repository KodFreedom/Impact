//--------------------------------------------------------------------------------
//
//　title.cpp
//	Author : Xu Wenjie
//	Date   : 2016-07-01
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "title_word.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define TITLE_WORD_POS_X (SCREEN_WIDTH * 0.5f - 0.5f)//ポリゴンの表示位置X
#define TITLE_WORD_POS_Y (SCREEN_HEIGHT * 0.5f - 0.5f)//ポリゴンの表示位置Y
#define TITLE_WORD_WIDTH (913.0f)
#define TITLE_WORD_HEIGHT (212.0f)
#define TITLE_WORD_TEXTURENAME "data/TEXTURE/title_word.png"//ファイル名
#define TITLE_WORD_UPDATE_TIME (42)

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexTitleWord(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureTitleWord = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferTitleWord = NULL;//頂点バッファ管理インターフェースポインタ
int g_nCntTitleWord;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitTitleWord(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_nCntTitleWord = 0;

	if (FAILED(MakeVerTexTitleWord(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexTitleWord ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, TITLE_WORD_TEXTURENAME, &g_pTextureTitleWord)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitTitleWord(void)
{
	//safe release
	if (g_pVtxBufferTitleWord != NULL)
	{
		g_pVtxBufferTitleWord->Release();
		g_pVtxBufferTitleWord = NULL;
	}

	if (g_pTextureTitleWord != NULL)
	{
		g_pTextureTitleWord->Release();
		g_pTextureTitleWord = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateTitleWord(void)
{
	VERTEX_2D *pVtx;
	float fWidth;
	float fHeight;
	float fColor;

	g_nCntTitleWord = (g_nCntTitleWord + 1) % TITLE_WORD_UPDATE_TIME;

	if (g_nCntTitleWord < TITLE_WORD_UPDATE_TIME / 4)
	{
		fWidth = TITLE_WORD_WIDTH * 0.05f * ((float)g_nCntTitleWord / ((float)TITLE_WORD_UPDATE_TIME / 4.0f));
		fHeight = TITLE_WORD_HEIGHT * 0.05f * ((float)g_nCntTitleWord / ((float)TITLE_WORD_UPDATE_TIME / 4.0f));
		fColor = 0.3f * ((float)g_nCntTitleWord / ((float)TITLE_WORD_UPDATE_TIME / 4.0f)) + 0.7f;
	}
	else
	{
		fWidth = TITLE_WORD_WIDTH * 0.05f * ((float)(TITLE_WORD_UPDATE_TIME - g_nCntTitleWord) / ((float)TITLE_WORD_UPDATE_TIME * 3.0f / 4.0f));
		fHeight = TITLE_WORD_HEIGHT * 0.05f * ((float)(TITLE_WORD_UPDATE_TIME - g_nCntTitleWord) / ((float)TITLE_WORD_UPDATE_TIME * 3.0f / 4.0f));
		fColor = 0.3f * ((float)(TITLE_WORD_UPDATE_TIME - g_nCntTitleWord) / ((float)TITLE_WORD_UPDATE_TIME * 3.0f / 4.0f)) + 0.7f;
	}

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferTitleWord->Lock(0,0,(void**)&pVtx,0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(TITLE_WORD_POS_X - TITLE_WORD_WIDTH * 0.5f - fWidth, TITLE_WORD_POS_Y - TITLE_WORD_HEIGHT * 0.5f - fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TITLE_WORD_POS_X + TITLE_WORD_WIDTH * 0.5f + fWidth, TITLE_WORD_POS_Y - TITLE_WORD_HEIGHT * 0.5f - fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TITLE_WORD_POS_X - TITLE_WORD_WIDTH * 0.5f - fWidth, TITLE_WORD_POS_Y + TITLE_WORD_HEIGHT * 0.5f + fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TITLE_WORD_POS_X + TITLE_WORD_WIDTH * 0.5f + fWidth, TITLE_WORD_POS_Y + TITLE_WORD_HEIGHT * 0.5f + fHeight, 0.0f);

	pVtx[0].color = D3DXCOLOR(fColor, fColor, fColor, 1.0f);
	pVtx[1].color = D3DXCOLOR(fColor, fColor, fColor, 1.0f);
	pVtx[2].color = D3DXCOLOR(fColor, fColor, fColor, 1.0f);
	pVtx[3].color = D3DXCOLOR(fColor, fColor, fColor, 1.0f);

	//仮想アドレス解放
	g_pVtxBufferTitleWord->Unlock();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawTitleWord(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferTitleWord,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureTitleWord);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//オフセット（頂点数）
		NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexTitleWord(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferTitleWord,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferTitleWord->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(TITLE_WORD_POS_X - TITLE_WORD_WIDTH * 0.5f, TITLE_WORD_POS_Y - TITLE_WORD_HEIGHT * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TITLE_WORD_POS_X + TITLE_WORD_WIDTH * 0.5f, TITLE_WORD_POS_Y - TITLE_WORD_HEIGHT * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TITLE_WORD_POS_X - TITLE_WORD_WIDTH * 0.5f, TITLE_WORD_POS_Y + TITLE_WORD_HEIGHT * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TITLE_WORD_POS_X + TITLE_WORD_WIDTH * 0.5f, TITLE_WORD_POS_Y + TITLE_WORD_HEIGHT * 0.5f, 0.0f);

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
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//仮想アドレス解放
	g_pVtxBufferTitleWord->Unlock();

	return S_OK;
}