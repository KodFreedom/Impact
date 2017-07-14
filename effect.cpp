//--------------------------------------------------------------------------------
//
//　effect
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "effect.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define EFFECT_TEXTURENAME "data/TEXTURE/circle.png"//ファイル名
#define EFFECT_ANGLE (D3DX_PI * 0.25f)
#define GOAL_EFFECT_NUM (10)
#define GOAL_EFFECT_ANGLE (D3DX_PI * 2.0f / GOAL_EFFECT_NUM)
#define GOAL_EFFECT_SPEED (6.0f)

#define REBORN_RADIUS (50.0f)
#define REBORN_EFFECT_NUM (10)
#define REBORN_EFFECT_ANGLE (D3DX_PI * 2.0f / GOAL_EFFECT_NUM)
#define REBORN_EFFECT_SPEED (-9.0f)

#define COLOR_R (0.968f)
#define COLOR_G (0.313f)
#define COLOR_B (0.698f)
#define NUM_EFFECT (100)

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct {
	D3DXVECTOR3 vPosCenter;//中心座標
	D3DXVECTOR3 vSpeed;//速度
	float fLength;//対角線長さ
	float fLengthSpeed;//対角線長さ変換速度
	float fRotation;//転換
	int nLife;//寿命
	bool bUsed;
}EFFECT;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexEffect(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureEffect = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferEffect = NULL;//頂点バッファ管理インターフェースポインタ
EFFECT g_aEffect[NUM_EFFECT];

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	//effect初期化
	for (int nCnt = 0; nCnt < NUM_EFFECT; nCnt++)
	{
		g_aEffect[nCnt].vPosCenter = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aEffect[nCnt].vSpeed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aEffect[nCnt].fLength = 0.0f;
		g_aEffect[nCnt].fLengthSpeed = 0.0f;
		g_aEffect[nCnt].fRotation = 0.0f;
		g_aEffect[nCnt].nLife = 0;
		g_aEffect[nCnt].bUsed = false;
	}

	if (FAILED(MakeVerTexEffect(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		EFFECT_TEXTURENAME,
		&g_pTextureEffect)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitEffect(void)
{
	if (g_pVtxBufferEffect != NULL)
	{
		g_pVtxBufferEffect->Release();
		g_pVtxBufferEffect = NULL;
	}

	if (g_pTextureEffect != NULL)
	{
		g_pTextureEffect->Release();
		g_pTextureEffect = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateEffect(void)
{
	//for test
	VERTEX_2D *pVtx = NULL;

	g_pVtxBufferEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < NUM_EFFECT; nCnt++)
	{
		if (g_aEffect[nCnt].bUsed)
		{
			//pos
			pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + cosf(g_aEffect[nCnt].fRotation + D3DX_PI + EFFECT_ANGLE) * g_aEffect[nCnt].fLength, g_aEffect[nCnt].vPosCenter.y + sin(g_aEffect[nCnt].fRotation + D3DX_PI + EFFECT_ANGLE) * g_aEffect[nCnt].fLength, 0.0f);
			pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + cosf(g_aEffect[nCnt].fRotation - EFFECT_ANGLE) * g_aEffect[nCnt].fLength, g_aEffect[nCnt].vPosCenter.y + sinf(g_aEffect[nCnt].fRotation - EFFECT_ANGLE) * g_aEffect[nCnt].fLength, 0.0f);
			pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + cosf(g_aEffect[nCnt].fRotation - D3DX_PI - EFFECT_ANGLE) * g_aEffect[nCnt].fLength, g_aEffect[nCnt].vPosCenter.y + sinf(g_aEffect[nCnt].fRotation - D3DX_PI - EFFECT_ANGLE) * g_aEffect[nCnt].fLength, 0.0f);
			pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + cosf(g_aEffect[nCnt].fRotation + EFFECT_ANGLE) * g_aEffect[nCnt].fLength, g_aEffect[nCnt].vPosCenter.y + sinf(g_aEffect[nCnt].fRotation + EFFECT_ANGLE) * g_aEffect[nCnt].fLength, 0.0f);

			//color
			if (g_aEffect[nCnt].nLife < 10)
			{
				pVtx[nCnt * 4 + 0].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].nLife * 0.08f);
				pVtx[nCnt * 4 + 1].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].nLife * 0.08f);
				pVtx[nCnt * 4 + 2].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].nLife * 0.08f);
				pVtx[nCnt * 4 + 3].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].nLife * 0.08f);
			}
			else
			{
				pVtx[nCnt * 4 + 0].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 0.8f);
				pVtx[nCnt * 4 + 1].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 0.8f);
				pVtx[nCnt * 4 + 2].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 0.8f);
				pVtx[nCnt * 4 + 3].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 0.8f);
			}
			
			//update
			g_aEffect[nCnt].nLife--;

			if (g_aEffect[nCnt].nLife >= 10)
			{
				g_aEffect[nCnt].fLength += g_aEffect[nCnt].fLengthSpeed;
				g_aEffect[nCnt].vPosCenter += g_aEffect[nCnt].vSpeed;
				g_aEffect[nCnt].fLengthSpeed *= 0.8f;
				g_aEffect[nCnt].vSpeed *= 0.8f;
			}

			if (g_aEffect[nCnt].nLife == 0)
			{
				g_aEffect[nCnt].bUsed = false;
			}
		}
	}

	g_pVtxBufferEffect->Unlock();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferEffect,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

						   //頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureEffect);

	for (int nCnt = 0; nCnt < NUM_EFFECT; nCnt++)
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
HRESULT MakeVerTexEffect(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NUM_EFFECT,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferEffect,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferEffect->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	for (int nCnt = 0; nCnt < NUM_EFFECT; nCnt++)
	{
		//頂点座標の設定（2D座標、右回り）
		pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + cosf(g_aEffect[nCnt].fRotation + D3DX_PI + EFFECT_ANGLE) * g_aEffect[nCnt].fLength, g_aEffect[nCnt].vPosCenter.y + sin(g_aEffect[nCnt].fRotation + D3DX_PI + EFFECT_ANGLE) * g_aEffect[nCnt].fLength, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + cosf(g_aEffect[nCnt].fRotation - EFFECT_ANGLE) * g_aEffect[nCnt].fLength, g_aEffect[nCnt].vPosCenter.y + sinf(g_aEffect[nCnt].fRotation - EFFECT_ANGLE) * g_aEffect[nCnt].fLength, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + cosf(g_aEffect[nCnt].fRotation - D3DX_PI - EFFECT_ANGLE) * g_aEffect[nCnt].fLength, g_aEffect[nCnt].vPosCenter.y + sinf(g_aEffect[nCnt].fRotation - D3DX_PI - EFFECT_ANGLE) * g_aEffect[nCnt].fLength, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + cosf(g_aEffect[nCnt].fRotation + EFFECT_ANGLE) * g_aEffect[nCnt].fLength, g_aEffect[nCnt].vPosCenter.y + sinf(g_aEffect[nCnt].fRotation + EFFECT_ANGLE) * g_aEffect[nCnt].fLength, 0.0f);

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
	g_pVtxBufferEffect->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  goal effect設定
//--------------------------------------------------------------------------------
void SetGoalEffect(D3DXVECTOR3 vSetPos)
{
	for (int nCnt = 0;nCnt < GOAL_EFFECT_NUM;nCnt++)
	{
		for (int nCntEffect = 0;nCntEffect < NUM_EFFECT;nCntEffect++)
		{
			if (!g_aEffect[nCntEffect].bUsed)
			{
				g_aEffect[nCntEffect].bUsed = true;
				g_aEffect[nCntEffect].nLife = 30;
				g_aEffect[nCntEffect].fLength = 1.0f;
				g_aEffect[nCntEffect].fLengthSpeed = 2.0f;
				g_aEffect[nCntEffect].fRotation = GOAL_EFFECT_ANGLE * nCnt;
				g_aEffect[nCntEffect].vPosCenter = vSetPos;
				g_aEffect[nCntEffect].vSpeed = D3DXVECTOR3(GOAL_EFFECT_SPEED * cosf(g_aEffect[nCntEffect].fRotation),
					GOAL_EFFECT_SPEED * sinf(g_aEffect[nCntEffect].fRotation),
					0.0f);
				break;
			}
		}
	}
}

//--------------------------------------------------------------------------------
//  reborn effect設定
//--------------------------------------------------------------------------------
void SetRebornEffect(D3DXVECTOR3 vSetPos)
{
	for (int nCnt = 0;nCnt < REBORN_EFFECT_NUM;nCnt++)
	{
		for (int nCntEffect = 0;nCntEffect < NUM_EFFECT;nCntEffect++)
		{
			if (!g_aEffect[nCntEffect].bUsed)
			{
				g_aEffect[nCntEffect].bUsed = true;
				g_aEffect[nCntEffect].nLife = 40;
				g_aEffect[nCntEffect].fLength = 15.0f;
				g_aEffect[nCntEffect].fLengthSpeed = -1.5f;
				g_aEffect[nCntEffect].fRotation = REBORN_EFFECT_ANGLE * nCnt;
				g_aEffect[nCntEffect].vPosCenter = vSetPos + 
					D3DXVECTOR3(REBORN_RADIUS * cosf(g_aEffect[nCntEffect].fRotation),
						REBORN_RADIUS * sinf(g_aEffect[nCntEffect].fRotation), 0.0f);
				g_aEffect[nCntEffect].vSpeed = D3DXVECTOR3(REBORN_EFFECT_SPEED * cosf(g_aEffect[nCntEffect].fRotation),
					REBORN_EFFECT_SPEED * sinf(g_aEffect[nCntEffect].fRotation),
					0.0f);
				break;
			}
		}
	}
}