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
#include "input.h"
#include "joystick.h"
#include "sample.h"
#include "fade.h"
#include "sound.h"
#include "press_enter.h"
#include "ready.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define SAMPLE_POS_X (-0.5f)//ポリゴンの表示位置X
#define SAMPLE_POS_Y (-0.5f)//ポリゴンの表示位置Y
#define SAMPLE_TEXTURENAME "data/TEXTURE/sample.jpg"//ファイル名

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexSample(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureSample = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferSample = NULL;//頂点バッファ管理インターフェースポインタ
bool g_bPressReady[2];

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitSample(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_bPressReady[0] = g_bPressReady[1] = false;

	if (FAILED(MakeVerTexSample(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexSample ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, SAMPLE_TEXTURENAME, &g_pTextureSample)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	InitPressEnter();
	InitReady();
	PlaySound(SOUND_LABEL_BGM_TITLE);
	SetPressEnter(D3DXVECTOR3(SCREEN_WIDTH * 0.205f, SCREEN_HEIGHT * 0.92f, 0.0f), D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.0f));
	SetPressEnter(D3DXVECTOR3(SCREEN_WIDTH * 0.795f, SCREEN_HEIGHT * 0.92f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitSample(void)
{
	//safe release
	if (g_pVtxBufferSample != NULL)
	{
		g_pVtxBufferSample->Release();
		g_pVtxBufferSample = NULL;
	}

	if (g_pTextureSample != NULL)
	{
		g_pTextureSample->Release();
		g_pTextureSample = NULL;
	}

	StopSound(SOUND_LABEL_BGM_TITLE);
	UninitPressEnter();
	UninitReady();
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateSample(void)
{
	//key入力
	if (!g_bPressReady[0] && 
		(GetKeyboardTrigger(DIK_SPACE) || GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_BUTTON, DIJ_XBOX_A)))
	{
		PlaySound(SOUND_LABEL_SE_PRESS);
		EndPressEnter(0);
		g_bPressReady[0] = true;
		SetReady(0);
	}

	if (!g_bPressReady[1] &&
		(GetKeyboardTrigger(DIK_RETURN) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_BUTTON, DIJ_PS4_×)))
	{
		PlaySound(SOUND_LABEL_SE_PRESS);
		EndPressEnter(1);
		g_bPressReady[1] = true;
		SetReady(1);
	}

	if (g_bPressReady[0] && g_bPressReady[1])
	{
		SetFade(FADE_OUT, MODE_GAME);
	}

	UpdatePressEnter();
	UpdateReady();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawSample(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferSample,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

								   //Textureの設定
	pDevice->SetTexture(0, g_pTextureSample);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//オフセット（頂点数）
		NUM_POLYGON);

	DrawPressEnter();
	DrawReady();
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexSample(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferSample,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferSample->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(SAMPLE_POS_X, SAMPLE_POS_Y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(SAMPLE_POS_X + SCREEN_WIDTH, SAMPLE_POS_Y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(SAMPLE_POS_X, SAMPLE_POS_Y + SCREEN_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(SAMPLE_POS_X + SCREEN_WIDTH, SAMPLE_POS_Y + SCREEN_HEIGHT, 0.0f);

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
	g_pVtxBufferSample->Unlock();

	return S_OK;
}