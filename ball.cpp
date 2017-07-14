//--------------------------------------------------------------------------------
//
//　ウインドウ表示プログラム
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include <time.h>
#include "ball.h"
#include "goal.h"
#include "effect.h"
#include "tire_effect.h"
#include "KF_Precision.h"
#include "KF_BodyContact.h"
#include "sound.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define BALL_TEXTURENAME "data/TEXTURE/ball.png"//ファイル名
#define BALL_COLOR_WHITE (D3DXCOLOR(1.0f,1.0f,1.0f,1.0f))
#define BALL_COLOR_BLACK (D3DXCOLOR(0.2f,0.2f,0.2f,1.0f))
#define BALL_COLOR_GOLDEN (D3DXCOLOR(1.0f,0.84f,0.0f,1.0f))
#define BALL_GOLDEN_RATE (5)//金色の確率
#define BALL_BLACK_RATE (4)//金色の確率
#define BALL_DAMPING (0.98f)

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexBall(LPDIRECT3DDEVICE9 pDevice);
void SetBallType(void);

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureBall = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferBall = NULL;//頂点バッファ管理インターフェースポインタ
CBodyCicle *g_pBall = NULL;
int g_nCntBall;
BALL_TYPE g_type;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitBall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	srand((unsigned)time(NULL));

	g_pBall = new CBodyCicle;
	g_pBall->Init();
	g_pBall->SetRadius(BALL_RADIUS);
	g_pBall->SetPosCenter(BALL_POS);
	g_pBall->SetMass(BALL_MASS);
	g_pBall->SetGravity(GRAVITY);
	g_pBall->SetDamping(BALL_DAMPING);
	g_type = BALL_WHITE;

	g_nCntBall = 0;

	if (FAILED(MakeVerTexBall(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		BALL_TEXTURENAME,
		&g_pTextureBall)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitBall(void)
{
	if (g_pBall != NULL)//アドレスの中身確認
	{
		g_pBall->Uninit();//終了処理
		delete g_pBall;//中身を削除
		g_pBall = NULL;//アドレスを空にする
	}

	if (g_pVtxBufferBall != NULL)
	{
		g_pVtxBufferBall->Release();
		g_pVtxBufferBall = NULL;
	}

	if (g_pTextureBall != NULL)
	{
		g_pTextureBall->Release();
		g_pTextureBall = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateBall(void)
{
	if (g_nCntBall != 0)
	{
		g_nCntBall--;
		if (g_nCntBall == 40)
		{
			SetRebornEffect(g_pBall->GetPosCenter());
		}
		return;
	}

	VERTEX_2D *pVtx = NULL;
	D3DXVECTOR3 vBallPos;
	D3DXCOLOR aColor[(int)BALL_MAX];

	aColor[(int)BALL_WHITE] = BALL_COLOR_WHITE;
	aColor[(int)BALL_BLACK] = BALL_COLOR_BLACK;
	aColor[(int)BALL_GOLDEN] = BALL_COLOR_GOLDEN;

	g_pBall->Update();
	AreaContact(g_pBall, BALL_RADIUS);

	g_pVtxBufferBall->Lock(0, 0, (void**)&pVtx, 0);

	vBallPos = g_pBall->GetPosCenter();

	//pos
	pVtx[0].pos = D3DXVECTOR3(vBallPos.x - BALL_RADIUS, vBallPos.y - BALL_RADIUS, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(vBallPos.x + BALL_RADIUS, vBallPos.y - BALL_RADIUS, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(vBallPos.x - BALL_RADIUS, vBallPos.y + BALL_RADIUS, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(vBallPos.x + BALL_RADIUS, vBallPos.y + BALL_RADIUS, 0.0f);

	//頂点カラーの設定(0～255の整数値)
	pVtx[0].color = aColor[(int)g_type];
	pVtx[1].color = aColor[(int)g_type];
	pVtx[2].color = aColor[(int)g_type];
	pVtx[3].color = aColor[(int)g_type];

	g_pVtxBufferBall->Unlock();

	//effect
	if (Vector3Magnitude(g_pBall->GetVelocity()) > FLOAT_MIN)
	{
		SetTireEffect(g_pBall->GetPosCenter());
	}

	//goal
	if (GoalContact(g_pBall->GetPosCenter(),g_type))
	{
		PlaySound(SOUND_LABEL_SE_GOAL);
		SetGoalEffect(g_pBall->GetPosCenter());
		g_nCntBall = 200;
		g_pBall->SetPosCenter(BALL_POS);
		g_pBall->SetVelocity(VEC3_NULL);
		g_pBall->SetAcceleration(VEC3_NULL);
		g_pBall->ClearAccumulator();

		SetBallType();//ボールのタイプをランダムで設定する

		switch (g_type)
		{
		case BALL_WHITE:
			g_pBall->SetMass(BALL_MASS);
			g_pBall->SetDamping(BALL_DAMPING);
			break;
		case BALL_BLACK:
			g_pBall->SetMass(BALL_MASS * 5.0f);
			g_pBall->SetDamping(BALL_DAMPING * BALL_DAMPING);
			break;
		case BALL_GOLDEN:
			g_pBall->SetMass(BALL_MASS);
			g_pBall->SetDamping(BALL_DAMPING);
			break;
		}
	}
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawBall(void)
{
	if (g_nCntBall != 0) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferBall,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

						   //頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureBall);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//オフセット（頂点数）
		NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexBall(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferBall,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;
	D3DXVECTOR3 vBallPos;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferBall->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	vBallPos = g_pBall->GetPosCenter();
	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(vBallPos.x - BALL_RADIUS, vBallPos.y - BALL_RADIUS, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(vBallPos.x + BALL_RADIUS, vBallPos.y - BALL_RADIUS, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(vBallPos.x - BALL_RADIUS, vBallPos.y + BALL_RADIUS, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(vBallPos.x + BALL_RADIUS, vBallPos.y + BALL_RADIUS, 0.0f);

	//rhwの設定(必ず1.0f)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//頂点カラーの設定(0～255の整数値)
	pVtx[0].color = BALL_COLOR_WHITE;
	pVtx[1].color = BALL_COLOR_WHITE;
	pVtx[2].color = BALL_COLOR_WHITE;
	pVtx[3].color = BALL_COLOR_WHITE;

	//texture頂点
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//仮想アドレス解放
	g_pVtxBufferBall->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  ボールのタイプを設定する
//--------------------------------------------------------------------------------
void SetBallType(void)
{
	if (rand() % BALL_BLACK_RATE == 0)
	{
		g_type = BALL_BLACK;
		return;
	}

	if (rand() % BALL_GOLDEN_RATE == 0)
	{
		g_type = BALL_GOLDEN;
		return;
	}
	
	g_type = BALL_WHITE;
}
//--------------------------------------------------------------------------------
//  ボール衝突検出
//--------------------------------------------------------------------------------
//void UpdateBallContact(void)
//{
//	//check table contact
//	CBodyQuadrangle *pTable = NULL;
//
//	for (int nCntTable = 0;nCntTable < NUM_TABLE;nCntTable++)
//	{
//		pTable = GetTable(nCntTable);
//		CheckContactCTQ(g_pBall, pTable);
//		pTable = NULL;
//	}
//
//	AreaContact(&g_pBall->cBody, g_pBall->GetRadius());
//
//	//check contact
//	CBodyCicle *pBall = NULL;
//
//	for (int nCnt = 0; nCnt < NUM_BALL; nCnt++)
//	{
//		if (!GetBallUsing(nCnt)) { continue; }
//
//		pBall = GetBall(nCnt);
//		CheckContactCTC(pBall, g_pBall);
//		pBall = NULL;
//	}
//}

//--------------------------------------------------------------------------------
//  敵アドレス取得
//--------------------------------------------------------------------------------
CBodyCicle *GetBall(void)
{
	return g_pBall;
}

D3DXVECTOR3 GetBallPos(void)
{
	return g_pBall->GetPosCenter();
}

D3DXVECTOR3 GetBallVelocity(void)
{
	return g_pBall->GetVelocity();
}

int GetBallCnt(void)
{
	return g_nCntBall;
}