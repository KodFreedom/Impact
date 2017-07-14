//--------------------------------------------------------------------------------
//
//　effect
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "tire_effect.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define TIRE_EFFECT_TEXTURENAME "data/TEXTURE/circle.png"//ファイル名
#define TIRE_EFFECT_RADIUS (6.0f)
#define TIRE_EFFECT_NUM (1000)

#define COLOR_R (0.52f)
#define COLOR_G (0.35f)
#define COLOR_B (0.13f)

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct {
	D3DXVECTOR3 vPosCenter;//中心座標
	float fRadius;//対角線長さ
	float fAlpha;
	int nLife;//寿命
	bool bUsed;
}TIRE_EFFECT;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexTireEffect(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureTireEffect = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferTireEffect = NULL;//頂点バッファ管理インターフェースポインタ
TIRE_EFFECT g_aEffect[TIRE_EFFECT_NUM];

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitTireEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	//effect初期化
	for (int nCnt = 0; nCnt < TIRE_EFFECT_NUM; nCnt++)
	{
		g_aEffect[nCnt].vPosCenter = VEC3_NULL;
		g_aEffect[nCnt].fRadius = TIRE_EFFECT_RADIUS;
		g_aEffect[nCnt].fAlpha = 1.0f;
		g_aEffect[nCnt].nLife = 0;
		g_aEffect[nCnt].bUsed = false;
	}

	if (FAILED(MakeVerTexTireEffect(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		TIRE_EFFECT_TEXTURENAME,
		&g_pTextureTireEffect)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitTireEffect(void)
{
	if (g_pVtxBufferTireEffect != NULL)
	{
		g_pVtxBufferTireEffect->Release();
		g_pVtxBufferTireEffect = NULL;
	}

	if (g_pTextureTireEffect != NULL)
	{
		g_pTextureTireEffect->Release();
		g_pTextureTireEffect = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateTireEffect(void)
{
	//for test
	VERTEX_2D *pVtx = NULL;

	g_pVtxBufferTireEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < TIRE_EFFECT_NUM; nCnt++)
	{
		if (g_aEffect[nCnt].bUsed)
		{
			//pos
			pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x - g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y - g_aEffect[nCnt].fRadius, 0.0f);
			pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y - g_aEffect[nCnt].fRadius, 0.0f);
			pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x - g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y + g_aEffect[nCnt].fRadius, 0.0f);
			pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y + g_aEffect[nCnt].fRadius, 0.0f);

			//color
			pVtx[nCnt * 4 + 0].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].fAlpha);
			pVtx[nCnt * 4 + 1].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].fAlpha);
			pVtx[nCnt * 4 + 2].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].fAlpha);
			pVtx[nCnt * 4 + 3].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].fAlpha);

			

			//update
			g_aEffect[nCnt].nLife--;
			g_aEffect[nCnt].fAlpha -= 0.025f;

			if (g_aEffect[nCnt].nLife == 0)
			{
				g_aEffect[nCnt].bUsed = false;
			}
		}
	}
	g_pVtxBufferTireEffect->Unlock();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawTireEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferTireEffect,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

						   //頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureTireEffect);

	for (int nCnt = 0; nCnt < TIRE_EFFECT_NUM; nCnt++)
	{
		if (g_aEffect[nCnt].bUsed)
		{
			//プリミティブ描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCnt * 4,//オフセット（頂点数）
				NUM_POLYGON);
		}
	}
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexTireEffect(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * TIRE_EFFECT_NUM,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferTireEffect,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferTireEffect->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	for (int nCnt = 0; nCnt < TIRE_EFFECT_NUM; nCnt++)
	{
		//頂点座標の設定（2D座標、右回り）
		pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x - g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y - g_aEffect[nCnt].fRadius, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y - g_aEffect[nCnt].fRadius, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x - g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y + g_aEffect[nCnt].fRadius, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y + g_aEffect[nCnt].fRadius, 0.0f);

		//rhwの設定(必ず1.0f)
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//頂点カラーの設定(0～255の整数値)
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 1.0f);

		//texture頂点
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferTireEffect->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  goal effect設定
//--------------------------------------------------------------------------------
void SetTireEffect(D3DXVECTOR3 vSetPos)
{
	for (int nCnt = 0;nCnt < TIRE_EFFECT_NUM;nCnt++)
	{
		if (!g_aEffect[nCnt].bUsed)
		{
			g_aEffect[nCnt].bUsed = true;
			g_aEffect[nCnt].nLife = 40;
			g_aEffect[nCnt].fRadius = TIRE_EFFECT_RADIUS;
			g_aEffect[nCnt].fAlpha = 1.0f;
			g_aEffect[nCnt].vPosCenter = vSetPos;
			break;
		}
	}
}