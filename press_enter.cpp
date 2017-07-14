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
#include "press_enter.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define PRESS_ENTER_WIDTH (298.0f)//ポリゴンの表示位置Y
#define PRESS_ENTER_HEIGHT (30.0f)//ポリゴンの表示位置Y
#define PRESS_ENTER_TEXTURENAME "data/TEXTURE/press.png"//ファイル名
#define PRESS_NUM (2)

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPressEnter(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct {
	D3DXCOLOR cColor;//文字色
	float fAlpha;//アルファ値
	float fChange;//アルファ値変化方向転換
	bool bUse;
}PRESS;

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTexturePressEnter = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferPressEnter = NULL;//頂点バッファ管理インターフェースポインタ
PRESS g_aPress[PRESS_NUM];

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitPressEnter(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		g_aPress[nCnt].cColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		g_aPress[nCnt].fAlpha = 0.0f;
		g_aPress[nCnt].fChange = 1.0f;
		g_aPress[nCnt].bUse = false;
	}
	
	if (FAILED(MakeVerTexPressEnter(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexPressEnter ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, PRESS_ENTER_TEXTURENAME, &g_pTexturePressEnter)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitPressEnter(void)
{
	//safe release
	if (g_pVtxBufferPressEnter != NULL)
	{
		g_pVtxBufferPressEnter->Release();
		g_pVtxBufferPressEnter = NULL;
	}

	if (g_pTexturePressEnter != NULL)
	{
		g_pTexturePressEnter->Release();
		g_pTexturePressEnter = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdatePressEnter(void)
{
	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferPressEnter->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		if (g_aPress[nCnt].bUse)
		{
			g_aPress[nCnt].fAlpha += 0.025f * g_aPress[nCnt].fChange;

			if (g_aPress[nCnt].fAlpha >= 1.0f)
			{
				g_aPress[nCnt].fAlpha = 1.0f;
				g_aPress[nCnt].fChange = -1.0f;
			}

			if (g_aPress[nCnt].fAlpha <= 0.0f)
			{
				g_aPress[nCnt].fAlpha = 0.0f;
				g_aPress[nCnt].fChange = 1.0f;
			}

			g_aPress[nCnt].cColor.a = g_aPress[nCnt].fAlpha;

			//頂点カラーの設定(0～255の整数値)
			pVtx[nCnt * 4 + 0].color = g_aPress[nCnt].cColor;
			pVtx[nCnt * 4 + 1].color = g_aPress[nCnt].cColor;
			pVtx[nCnt * 4 + 2].color = g_aPress[nCnt].cColor;
			pVtx[nCnt * 4 + 3].color = g_aPress[nCnt].cColor;
		}
	}
	
	//仮想アドレス解放
	g_pVtxBufferPressEnter->Unlock();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawPressEnter(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferPressEnter,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

								   //Textureの設定
	pDevice->SetTexture(0, g_pTexturePressEnter);

	//プリミティブ描画
	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		if (g_aPress[nCnt].bUse)
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
HRESULT MakeVerTexPressEnter(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * PRESS_NUM,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferPressEnter,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferPressEnter->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);
	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		//頂点座標の設定（2D座標、右回り）
		pVtx[nCnt * 4 + 0].pos = VEC3_NULL;
		pVtx[nCnt * 4 + 1].pos = VEC3_NULL;
		pVtx[nCnt * 4 + 2].pos = VEC3_NULL;
		pVtx[nCnt * 4 + 3].pos = VEC3_NULL;

		//rhwの設定(必ず1.0f)
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//頂点カラーの設定(0～255の整数値)
		pVtx[nCnt * 4 + 0].color = g_aPress[nCnt].cColor;
		pVtx[nCnt * 4 + 1].color = g_aPress[nCnt].cColor;
		pVtx[nCnt * 4 + 2].color = g_aPress[nCnt].cColor;
		pVtx[nCnt * 4 + 3].color = g_aPress[nCnt].cColor;

		//texture頂点
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferPressEnter->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  位置と色を設定
//--------------------------------------------------------------------------------
void SetPressEnter(D3DXVECTOR3 vPos, D3DXCOLOR cColor)
{
	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferPressEnter->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		if (!g_aPress[nCnt].bUse)
		{
			g_aPress[nCnt].bUse = true;
			g_aPress[nCnt].cColor = cColor;

			//頂点座標の設定（2D座標、右回り）
			pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(vPos.x - PRESS_ENTER_WIDTH * 0.5f, vPos.y - PRESS_ENTER_HEIGHT * 0.5f, 0.0f);
			pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(vPos.x + PRESS_ENTER_WIDTH * 0.5f, vPos.y - PRESS_ENTER_HEIGHT * 0.5f, 0.0f);
			pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(vPos.x - PRESS_ENTER_WIDTH * 0.5f, vPos.y + PRESS_ENTER_HEIGHT * 0.5f, 0.0f);
			pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(vPos.x + PRESS_ENTER_WIDTH * 0.5f, vPos.y + PRESS_ENTER_HEIGHT * 0.5f, 0.0f);

			//頂点カラーの設定(0～255の整数値)
			pVtx[nCnt * 4 + 0].color = cColor;
			pVtx[nCnt * 4 + 1].color = cColor;
			pVtx[nCnt * 4 + 2].color = cColor;
			pVtx[nCnt * 4 + 3].color = cColor;

			break;
		}
	}

	//仮想アドレス解放
	g_pVtxBufferPressEnter->Unlock();
}

//--------------------------------------------------------------------------------
//  非表示にする
//--------------------------------------------------------------------------------
void EndPressEnter(int nNumEnter)
{
	g_aPress[nNumEnter].bUse = false;
}