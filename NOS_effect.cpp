//--------------------------------------------------------------------------------
//
//　ウインドウ表示プログラム
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "NOS_effect.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define NOS_EFFECT_TEXTURENAME "data/TEXTURE/effect000.jpg"//ファイル名
#define NOS_EFFECT_RADIUS (10.0f)
#define NOS_EFFECT_RADIUS_VALUE (0.9f)
#define NOS_EFFECT_NUM (500)
#define NOS_EFFECT_LIFE (20)

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexNOSEffect(LPDIRECT3DDEVICE9 pDevice);
void SetColorEffect(int nCnt, D3DXCOLOR cColor);
void SetVertexEffect(int nCnt, D3DXVECTOR3 vPos, float fRadius);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct
{
	bool bUse;
	D3DXVECTOR3 vPos;//座標
	D3DXCOLOR cColor;//色
	float fColorAlphaValue;//アルファ変化量
	float fRadius;//半径
	float fRadiusValue;//半径変化量
	int nLife;//寿命
}NOS_EFFECT;

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureNOSEffect = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferNOSEffect = NULL;//頂点バッファ管理インターフェースポインタ
NOS_EFFECT g_aNOSEffect[NOS_EFFECT_NUM];

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitNOSEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
	{
		g_aNOSEffect[nCnt].bUse = false;
		g_aNOSEffect[nCnt].vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aNOSEffect[nCnt].cColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		g_aNOSEffect[nCnt].fRadius = 0.0f;
		g_aNOSEffect[nCnt].nLife = 0;
	}

	if (FAILED(MakeVerTexNOSEffect(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexNOSEffect ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		NOS_EFFECT_TEXTURENAME,
		&g_pTextureNOSEffect)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitNOSEffect(void)
{
	if (g_pVtxBufferNOSEffect != NULL)
	{
		g_pVtxBufferNOSEffect->Release();
		g_pVtxBufferNOSEffect = NULL;
	}

	if (g_pTextureNOSEffect != NULL)
	{
		g_pTextureNOSEffect->Release();
		g_pTextureNOSEffect = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateNOSEffect(void)
{
	for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
	{
		if (!g_aNOSEffect[nCnt].bUse) { continue; }

		g_aNOSEffect[nCnt].nLife--;

		if (g_aNOSEffect[nCnt].nLife <= 0)
		{
			g_aNOSEffect[nCnt].bUse = false;
		}
		else
		{
			//color
			g_aNOSEffect[nCnt].cColor.a -= g_aNOSEffect[nCnt].fColorAlphaValue;
			if (g_aNOSEffect[nCnt].cColor.a < 0.0f)
			{
				g_aNOSEffect[nCnt].cColor.a = 0.0f;
			}

			SetColorEffect(nCnt, g_aNOSEffect[nCnt].cColor);

			//半径の設定
			g_aNOSEffect[nCnt].fRadius -= g_aNOSEffect[nCnt].fRadiusValue;
			if (g_aNOSEffect[nCnt].fRadius < 0.0f)
			{
				g_aNOSEffect[nCnt].fRadius = 0.0f;
			}
			SetVertexEffect(nCnt, g_aNOSEffect[nCnt].vPos, g_aNOSEffect[nCnt].fRadius);
		}

	}
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawNOSEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	//加算合成
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferNOSEffect,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

								   //Textureの設定
	pDevice->SetTexture(0, g_pTextureNOSEffect);

	for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
	{
		//プリミティブ描画
		if (g_aNOSEffect[nCnt].bUse)
		{
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCnt * 4,//オフセット（頂点数）
				NUM_POLYGON);
		}
	}

	//乗算合成
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexNOSEffect(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NOS_EFFECT_NUM,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferNOSEffect,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferNOSEffect->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
	{
		//頂点座標の設定（2D座標、右回り）
		pVtx[nCnt * 4].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x - NOS_EFFECT_RADIUS, g_aNOSEffect[nCnt].vPos.y - NOS_EFFECT_RADIUS, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x + NOS_EFFECT_RADIUS, g_aNOSEffect[nCnt].vPos.y - NOS_EFFECT_RADIUS, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x - NOS_EFFECT_RADIUS, g_aNOSEffect[nCnt].vPos.y + NOS_EFFECT_RADIUS, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x + NOS_EFFECT_RADIUS, g_aNOSEffect[nCnt].vPos.y + NOS_EFFECT_RADIUS, 0.0f);

		//rhwの設定(必ず1.0f)
		pVtx[nCnt * 4].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//頂点カラーの設定(0～255の整数値)
		pVtx[nCnt * 4].color = g_aNOSEffect[nCnt].cColor;
		pVtx[nCnt * 4 + 1].color = g_aNOSEffect[nCnt].cColor;
		pVtx[nCnt * 4 + 2].color = g_aNOSEffect[nCnt].cColor;
		pVtx[nCnt * 4 + 3].color = g_aNOSEffect[nCnt].cColor;

		//texture頂点
		pVtx[nCnt * 4].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferNOSEffect->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  setEffect
//--------------------------------------------------------------------------------
void SetNOSEffect(D3DXVECTOR3 vPos, D3DXCOLOR cColor)
{
	for (int nCntNum = 0;nCntNum < 2;nCntNum++)
	{
		for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
		{
			if (!g_aNOSEffect[nCnt].bUse)
			{
				g_aNOSEffect[nCnt].bUse = true;
				g_aNOSEffect[nCnt].vPos = vPos;
				g_aNOSEffect[nCnt].nLife = NOS_EFFECT_LIFE;
				g_aNOSEffect[nCnt].cColor = cColor;
				g_aNOSEffect[nCnt].fRadius = NOS_EFFECT_RADIUS;
				g_aNOSEffect[nCnt].fRadiusValue = NOS_EFFECT_RADIUS * NOS_EFFECT_RADIUS_VALUE / NOS_EFFECT_LIFE;
				g_aNOSEffect[nCnt].fColorAlphaValue = 1.0f / NOS_EFFECT_LIFE;
				break;
			}
		}
	}
}

//--------------------------------------------------------------------------------
//  SetColorEffect
//--------------------------------------------------------------------------------
void SetColorEffect(int nCnt, D3DXCOLOR cColor)
{
	VERTEX_2D *pVtx = NULL;

	g_pVtxBufferNOSEffect->Lock(0, 0, (void**)&pVtx, 0);

	//頂点カラーの設定
	pVtx[nCnt * 4].color = cColor;
	pVtx[nCnt * 4 + 1].color = cColor;
	pVtx[nCnt * 4 + 2].color = cColor;
	pVtx[nCnt * 4 + 3].color = cColor;

	g_pVtxBufferNOSEffect->Unlock();
}

//--------------------------------------------------------------------------------
//  SetVertexEffect
//--------------------------------------------------------------------------------
void SetVertexEffect(int nCnt, D3DXVECTOR3 vPos, float fRadius)
{
	VERTEX_2D *pVtx = NULL;

	g_pVtxBufferNOSEffect->Lock(0, 0, (void**)&pVtx, 0);

	//頂点カラーの設定
	pVtx[nCnt * 4].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x - g_aNOSEffect[nCnt].fRadius, g_aNOSEffect[nCnt].vPos.y - g_aNOSEffect[nCnt].fRadius, 0.0f);
	pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x + g_aNOSEffect[nCnt].fRadius, g_aNOSEffect[nCnt].vPos.y - g_aNOSEffect[nCnt].fRadius, 0.0f);
	pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x - g_aNOSEffect[nCnt].fRadius, g_aNOSEffect[nCnt].vPos.y + g_aNOSEffect[nCnt].fRadius, 0.0f);
	pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x + g_aNOSEffect[nCnt].fRadius, g_aNOSEffect[nCnt].vPos.y + g_aNOSEffect[nCnt].fRadius, 0.0f);

	g_pVtxBufferNOSEffect->Unlock();
}