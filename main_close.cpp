//--------------------------------------------------------------------------------
//
//　MainClose.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "main_close.h"
#include "input.h"
#include "joystick.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define MAIN_CLOSE_POS_X (SCREEN_WIDTH * 0.5f)//ポリゴンの表示位置X
#define MAIN_CLOSE_POS_Y (SCREEN_HEIGHT * 0.5f)//ポリゴンの表示位置Y
#define MAIN_CLOSE_WIDTH (400.0f)
#define MAIN_CLOSE_HEIGHT (200.0f)
#define MAIN_CLOSE_NO_POS_X (MAIN_CLOSE_POS_X + 58.0f)//ポリゴンの表示位置X
#define MAIN_CLOSE_NO_POS_Y (MAIN_CLOSE_POS_Y + 30.0f)//ポリゴンの表示位置Y
#define MAIN_CLOSE_NO_WIDTH (50.0f)
#define MAIN_CLOSE_NO_HEIGHT (35.0f)
#define MAIN_CLOSE_YES_POS_X (MAIN_CLOSE_POS_X - 58.0f)//ポリゴンの表示位置X
#define MAIN_CLOSE_YES_POS_Y (MAIN_CLOSE_POS_Y + 30.0f)//ポリゴンの表示位置Y
#define MAIN_CLOSE_YES_WIDTH (70.0f)
#define MAIN_CLOSE_YES_HEIGHT (35.0f)
#define MAIN_CLOSE_TEXTURENAME "data/TEXTURE/mainclose.png"//ファイル名

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexMainClose(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef enum {
	MAIN_CLOSE_NONE,
	MAIN_CLOSE_NO,
	MAIN_CLOSE_YES,
	MAIN_CLOSE_MAX
}CLOSE;

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureMainClose = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferMainClose = NULL;//頂点バッファ管理インターフェースポインタ
CLOSE g_close;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitMainClose(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_close = MAIN_CLOSE_NONE;

	if (FAILED(MakeVerTexMainClose(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexMainClose ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, MAIN_CLOSE_TEXTURENAME, &g_pTextureMainClose)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitMainClose(void)
{
	//safe release
	if (g_pVtxBufferMainClose != NULL)
	{
		g_pVtxBufferMainClose->Release();
		g_pVtxBufferMainClose = NULL;
	}

	if (g_pTextureMainClose != NULL)
	{
		g_pTextureMainClose->Release();
		g_pTextureMainClose = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
bool UpdateMainClose(HWND *pWnd)
{
	//key入力
	if (GetKeyboardTrigger(DIK_ESCAPE))//Pause
	{
		g_close = MAIN_CLOSE_NO;
	}

	if (g_close == MAIN_CLOSE_NONE) { return false; }

	//key入力
	if (GetKeyboardTrigger(DIK_LEFT) || GetKeyboardTrigger(DIK_RIGHT))
	{
		g_close = g_close == MAIN_CLOSE_NO ? MAIN_CLOSE_YES : MAIN_CLOSE_NO;
	}

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		switch (g_close)
		{
		case MAIN_CLOSE_NO://resume
			g_close = MAIN_CLOSE_NONE;
			return false;
			break;
		case MAIN_CLOSE_YES://close window
			//WM_DESTROYメッセージを送信
			DestroyWindow(*pWnd);
			return true;
			break;
		default:
			break;
		}
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;
	float fWork;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferMainClose->Lock(0, 0, (void**)&pVtx, 0);

	//texture頂点
	for (int nCnt = 0;nCnt < MAIN_CLOSE_MAX - 1;nCnt++)
	{
		fWork = (g_close - nCnt) % 2 * 0.5f + 0.5f;
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferMainClose->Unlock();

	return true;
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawMainClose(void)
{
	if (g_close == MAIN_CLOSE_NONE) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferMainClose,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureMainClose);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexMainClose(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * 3,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferMainClose,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferMainClose->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	
	pVtx[0].pos = D3DXVECTOR3(MAIN_CLOSE_NO_POS_X - MAIN_CLOSE_NO_WIDTH * 0.5f, MAIN_CLOSE_NO_POS_Y - MAIN_CLOSE_NO_HEIGHT * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(MAIN_CLOSE_NO_POS_X + MAIN_CLOSE_NO_WIDTH * 0.5f, MAIN_CLOSE_NO_POS_Y - MAIN_CLOSE_NO_HEIGHT * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(MAIN_CLOSE_NO_POS_X - MAIN_CLOSE_NO_WIDTH * 0.5f, MAIN_CLOSE_NO_POS_Y + MAIN_CLOSE_NO_HEIGHT * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(MAIN_CLOSE_NO_POS_X + MAIN_CLOSE_NO_WIDTH * 0.5f, MAIN_CLOSE_NO_POS_Y + MAIN_CLOSE_NO_HEIGHT * 0.5f, 0.0f);
	
	pVtx[4].pos = D3DXVECTOR3(MAIN_CLOSE_YES_POS_X - MAIN_CLOSE_YES_WIDTH * 0.5f, MAIN_CLOSE_YES_POS_Y - MAIN_CLOSE_YES_HEIGHT * 0.5f, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(MAIN_CLOSE_YES_POS_X + MAIN_CLOSE_YES_WIDTH * 0.5f, MAIN_CLOSE_YES_POS_Y - MAIN_CLOSE_YES_HEIGHT * 0.5f, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(MAIN_CLOSE_YES_POS_X - MAIN_CLOSE_YES_WIDTH * 0.5f, MAIN_CLOSE_YES_POS_Y + MAIN_CLOSE_YES_HEIGHT * 0.5f, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(MAIN_CLOSE_YES_POS_X + MAIN_CLOSE_YES_WIDTH * 0.5f, MAIN_CLOSE_YES_POS_Y + MAIN_CLOSE_YES_HEIGHT * 0.5f, 0.0f);

	pVtx[8].pos = D3DXVECTOR3(MAIN_CLOSE_POS_X - MAIN_CLOSE_WIDTH * 0.5f, MAIN_CLOSE_POS_Y - MAIN_CLOSE_HEIGHT * 0.5f, 0.0f);
	pVtx[9].pos = D3DXVECTOR3(MAIN_CLOSE_POS_X + MAIN_CLOSE_WIDTH * 0.5f, MAIN_CLOSE_POS_Y - MAIN_CLOSE_HEIGHT * 0.5f, 0.0f);
	pVtx[10].pos = D3DXVECTOR3(MAIN_CLOSE_POS_X - MAIN_CLOSE_WIDTH * 0.5f, MAIN_CLOSE_POS_Y + MAIN_CLOSE_HEIGHT * 0.5f, 0.0f);
	pVtx[11].pos = D3DXVECTOR3(MAIN_CLOSE_POS_X + MAIN_CLOSE_WIDTH * 0.5f, MAIN_CLOSE_POS_Y + MAIN_CLOSE_HEIGHT * 0.5f, 0.0f);

	//rhwの設定(必ず1.0f)
	for (int nCnt = 0;nCnt < MAIN_CLOSE_MAX - 1;nCnt++)
	{
		float fWork = (g_close - nCnt) % 2 * 0.5f + 0.5f;
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
	}

	//頂点カラーの設定(0～255の整数値)
	pVtx[8 + 0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[8 + 1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[8 + 2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[8 + 3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	for (int nCnt = 0;nCnt < MAIN_CLOSE_MAX;nCnt++)
	{
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//texture頂点
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 1.0f / 3.0f * nCnt);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 1.0f / 3.0f * nCnt);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f / 3.0f * nCnt + 1.0f / 3.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f / 3.0f * nCnt + 1.0f / 3.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferMainClose->Unlock();

	return S_OK;
}