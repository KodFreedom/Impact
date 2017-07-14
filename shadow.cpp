//--------------------------------------------------------------------------------
//
//　Shadow.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "shadow.h"
#include "player.h"
#include "enemy.h"
#include "ball.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define SHADOW_CAR_TEXTURENAME "data/TEXTURE/shadow_car.png"//ファイル名
#define SHADOW_BALL_TEXTURENAME "data/TEXTURE/shadow_ball.png"//ファイル名
#define NUM_SHADOW (3)
#define SHADOW_CAR_WIDTH (CAR_WIDTH * 1.0f)
#define SHADOW_CAR_HEIGHT (CAR_HEIGHT * 1.0f)
#define SHADOW_BALL_RADIUS (BALL_RADIUS * 1.2f)

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexShadow(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureShadowCar = NULL;//textureインターフェース
LPDIRECT3DTEXTURE9 g_pTextureShadowBall = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferShadow = NULL;//頂点バッファ管理インターフェースポインタ

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitShadow(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	if (FAILED(MakeVerTexShadow(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexShadow ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, SHADOW_CAR_TEXTURENAME, &g_pTextureShadowCar))
		|| FAILED(D3DXCreateTextureFromFile(pDevice, SHADOW_BALL_TEXTURENAME, &g_pTextureShadowBall)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitShadow(void)
{
	//safe release
	if (g_pVtxBufferShadow != NULL)
	{
		g_pVtxBufferShadow->Release();
		g_pVtxBufferShadow = NULL;
	}

	if (g_pTextureShadowCar != NULL)
	{
		g_pTextureShadowCar->Release();
		g_pTextureShadowCar = NULL;
	}

	if (g_pTextureShadowBall != NULL)
	{
		g_pTextureShadowBall->Release();
		g_pTextureShadowBall = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateShadow(void)
{
	D3DXVECTOR3 vPos;
	float fRot;
	float fAngle = atan2f(SHADOW_CAR_HEIGHT, SHADOW_CAR_WIDTH);
	float fLength = sqrtf(SHADOW_CAR_WIDTH * SHADOW_CAR_WIDTH + SHADOW_CAR_HEIGHT * SHADOW_CAR_HEIGHT) * 0.5f;
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferShadow->Lock(0, 0, (void**)&pVtx, 0);

	//player
	vPos = GetPlayerPos();
	fRot = GetPlayerRot();
	pVtx[0].pos = D3DXVECTOR3(vPos.x + cosf(fRot + D3DX_PI + fAngle) * fLength, vPos.y + sin(fRot + D3DX_PI + fAngle) * fLength, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(vPos.x + cosf(fRot - fAngle) * fLength, vPos.y + sinf(fRot - fAngle) * fLength, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(vPos.x + cosf(fRot - D3DX_PI - fAngle) * fLength, vPos.y + sinf(fRot - D3DX_PI - fAngle) * fLength, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(vPos.x + cosf(fRot + fAngle) * fLength, vPos.y + sinf(fRot + fAngle) * fLength, 0.0f);

	//enemy
	vPos = GetEnemyPos();
	fRot = GetEnemyRot();
	pVtx[4].pos = D3DXVECTOR3(vPos.x + cosf(fRot + D3DX_PI + fAngle) * fLength, vPos.y + sin(fRot + D3DX_PI + fAngle) * fLength, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(vPos.x + cosf(fRot - fAngle) * fLength, vPos.y + sinf(fRot - fAngle) * fLength, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(vPos.x + cosf(fRot - D3DX_PI - fAngle) * fLength, vPos.y + sinf(fRot - D3DX_PI - fAngle) * fLength, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(vPos.x + cosf(fRot + fAngle) * fLength, vPos.y + sinf(fRot + fAngle) * fLength, 0.0f);

	//ball
	vPos = GetBallPos();
	pVtx[8].pos = D3DXVECTOR3(vPos.x - SHADOW_BALL_RADIUS, vPos.y - SHADOW_BALL_RADIUS, 0.0f);
	pVtx[9].pos = D3DXVECTOR3(vPos.x + SHADOW_BALL_RADIUS, vPos.y - SHADOW_BALL_RADIUS, 0.0f);
	pVtx[10].pos = D3DXVECTOR3(vPos.x - SHADOW_BALL_RADIUS, vPos.y + SHADOW_BALL_RADIUS, 0.0f);
	pVtx[11].pos = D3DXVECTOR3(vPos.x + SHADOW_BALL_RADIUS, vPos.y + SHADOW_BALL_RADIUS, 0.0f);

	//仮想アドレス解放
	g_pVtxBufferShadow->Unlock();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawShadow(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferShadow,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureShadowCar);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, NUM_POLYGON);

	if (GetBallCnt() != 0) { return; }

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureShadowBall);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexShadow(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NUM_SHADOW,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferShadow,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferShadow->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	D3DXVECTOR3 vPos;
	float fRot;
	float fAngle = atan2f(SHADOW_CAR_HEIGHT, SHADOW_CAR_WIDTH);
	float fLength = sqrtf(SHADOW_CAR_WIDTH * SHADOW_CAR_WIDTH + SHADOW_CAR_HEIGHT * SHADOW_CAR_HEIGHT) * 0.5f;

	//player
	vPos = GetPlayerPos();
	fRot = GetPlayerRot();
	pVtx[0].pos = D3DXVECTOR3(vPos.x + cosf(fRot + D3DX_PI + fAngle) * fLength, vPos.y + sin(fRot + D3DX_PI + fAngle) * fLength, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(vPos.x + cosf(fRot - fAngle) * fLength, vPos.y + sinf(fRot - fAngle) * fLength, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(vPos.x + cosf(fRot - D3DX_PI - fAngle) * fLength, vPos.y + sinf(fRot - D3DX_PI - fAngle) * fLength, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(vPos.x + cosf(fRot + fAngle) * fLength, vPos.y + sinf(fRot + fAngle) * fLength, 0.0f);

	//enemy
	vPos = GetEnemyPos();
	fRot = GetEnemyRot();
	pVtx[4].pos = D3DXVECTOR3(vPos.x + cosf(fRot + D3DX_PI + fAngle) * fLength, vPos.y + sin(fRot + D3DX_PI + fAngle) * fLength, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(vPos.x + cosf(fRot - fAngle) * fLength, vPos.y + sinf(fRot - fAngle) * fLength, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(vPos.x + cosf(fRot - D3DX_PI - fAngle) * fLength, vPos.y + sinf(fRot - D3DX_PI - fAngle) * fLength, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(vPos.x + cosf(fRot + fAngle) * fLength, vPos.y + sinf(fRot + fAngle) * fLength, 0.0f);

	//ball
	vPos = GetBallPos();
	pVtx[8].pos = D3DXVECTOR3(vPos.x - SHADOW_BALL_RADIUS, vPos.y - SHADOW_BALL_RADIUS, 0.0f);
	pVtx[9].pos = D3DXVECTOR3(vPos.x + SHADOW_BALL_RADIUS, vPos.y - SHADOW_BALL_RADIUS, 0.0f);
	pVtx[10].pos = D3DXVECTOR3(vPos.x - SHADOW_BALL_RADIUS, vPos.y + SHADOW_BALL_RADIUS, 0.0f);
	pVtx[11].pos = D3DXVECTOR3(vPos.x + SHADOW_BALL_RADIUS, vPos.y + SHADOW_BALL_RADIUS, 0.0f);

	//頂点座標の設定（2D座標、右回り）
	for (int nCnt = 0;nCnt < NUM_SHADOW;nCnt++)
	{
		//pVtx[nCnt * 4 + 0].pos = VEC3_NULL;
		//pVtx[nCnt * 4 + 1].pos = VEC3_NULL;
		//pVtx[nCnt * 4 + 2].pos = VEC3_NULL;
		//pVtx[nCnt * 4 + 3].pos = VEC3_NULL;

		//rhwの設定(必ず1.0f)
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//頂点カラーの設定(0～255の整数値)
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);

		//texture頂点
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}
	
	//仮想アドレス解放
	g_pVtxBufferShadow->Unlock();

	return S_OK;
}