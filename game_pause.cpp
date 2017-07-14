//--------------------------------------------------------------------------------
//
//　GamePause.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "game_pause.h"
#include "sound.h"
#include "fade.h"
#include "input.h"
#include "joystick.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define PAUSE_POS_X (SCREEN_WIDTH * 0.5f)//ポリゴンの表示位置X
#define PAUSE_POS_Y (SCREEN_HEIGHT * 0.6f)//ポリゴンの表示位置Y
#define PAUSE_WIDTH (400.0f)
#define PAUSE_HEIGHT (200.0f)
#define PAUSE_RESUME_POS_X (SCREEN_WIDTH * 0.5f)//ポリゴンの表示位置X
#define PAUSE_RESUME_POS_Y (SCREEN_HEIGHT * 0.6f - 30.0f)//ポリゴンの表示位置Y
#define PAUSE_RESUME_WIDTH (251.0f)
#define PAUSE_RESUME_HEIGHT (50.0f)
#define PAUSE_RETURN_POS_X (SCREEN_WIDTH * 0.5f)//ポリゴンの表示位置X
#define PAUSE_RETURN_POS_Y (SCREEN_HEIGHT * 0.6f + 30.0f)//ポリゴンの表示位置Y
#define PAUSE_RETURN_WIDTH (146.0f)
#define PAUSE_RETURN_HEIGHT (50.0f)
#define PAUSE_BG_POS_X (-0.5f)//ポリゴンの表示位置X
#define PAUSE_BG_POS_Y (-0.5f)//ポリゴンの表示位置Y
#define PAUSE_BG_WIDTH (SCREEN_WIDTH)
#define PAUSE_BG_HEIGHT (SCREEN_HEIGHT)
#define PAUSE_TEXTURENAME "data/TEXTURE/gamepause.png"//ファイル名

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGamePause(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef enum {
	PAUSE_NONE,
	PAUSE_RESUME,
	PAUSE_RETURN_TO_MENU,
	PAUSE_MAX
}PAUSE;

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureGamePause = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferGamePause = NULL;//頂点バッファ管理インターフェースポインタ
PAUSE g_pause;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitGamePause(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_pause = PAUSE_NONE;

	if (FAILED(MakeVerTexGamePause(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexGamePause ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice, PAUSE_TEXTURENAME, &g_pTextureGamePause)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitGamePause(void)
{
	//safe release
	if (g_pVtxBufferGamePause != NULL)
	{
		g_pVtxBufferGamePause->Release();
		g_pVtxBufferGamePause = NULL;
	}

	if (g_pTextureGamePause != NULL)
	{
		g_pTextureGamePause->Release();
		g_pTextureGamePause = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
bool UpdateGamePause(void)
{
	//key入力
	if (GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_BUTTON, DIJ_XBOX_MENU) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_BUTTON, DIJ_PS4_OPTIONS))//Pause
	{
		g_pause = PAUSE_RESUME;
	}

	if (g_pause == PAUSE_NONE) { return false; }

	//key入力
	if (GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_L_STICKY, DIJ_XBOX_L_UP) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_L_STICKY, DIJ_PS4_L_UP) ||
		GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_L_STICKY, DIJ_XBOX_L_DOWN) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_L_STICKY, DIJ_PS4_L_DOWN))
	{
		g_pause = g_pause == PAUSE_RESUME ? PAUSE_RETURN_TO_MENU : PAUSE_RESUME;
	}


	if (GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_BUTTON, DIJ_XBOX_B) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_BUTTON, DIJ_PS4_◯))
	{
		g_pause = PAUSE_NONE;
		return false;
	}

	if (GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_BUTTON, DIJ_XBOX_A) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_BUTTON, DIJ_PS4_×))
	{
		PlaySound(SOUND_LABEL_SE_PRESS);

		switch (g_pause)
		{
		case PAUSE_RESUME://refuse
			g_pause = PAUSE_NONE;
			return false;
			break;
		case PAUSE_RETURN_TO_MENU://return to menu
			SetFade(FADE_OUT, MODE_TITLE);
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
	g_pVtxBufferGamePause->Lock(0, 0, (void**)&pVtx, 0);

	//texture頂点
	for (int nCnt = 0;nCnt < PAUSE_MAX - 1;nCnt++)
	{
		fWork = (g_pause - nCnt) % 2 * 0.5f + 0.5f;
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferGamePause->Unlock();

	return true;
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawGamePause(void)
{
	if (g_pause == PAUSE_NONE) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferGamePause,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureGamePause);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGamePause(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * 4,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferGamePause,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferGamePause->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = D3DXVECTOR3(PAUSE_RESUME_POS_X - PAUSE_RESUME_WIDTH * 0.5f, PAUSE_RESUME_POS_Y - PAUSE_RESUME_HEIGHT * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PAUSE_RESUME_POS_X + PAUSE_RESUME_WIDTH * 0.5f, PAUSE_RESUME_POS_Y - PAUSE_RESUME_HEIGHT * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PAUSE_RESUME_POS_X - PAUSE_RESUME_WIDTH * 0.5f, PAUSE_RESUME_POS_Y + PAUSE_RESUME_HEIGHT * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PAUSE_RESUME_POS_X + PAUSE_RESUME_WIDTH * 0.5f, PAUSE_RESUME_POS_Y + PAUSE_RESUME_HEIGHT * 0.5f, 0.0f);

	pVtx[4].pos = D3DXVECTOR3(PAUSE_RETURN_POS_X - PAUSE_RETURN_WIDTH * 0.5f, PAUSE_RETURN_POS_Y - PAUSE_RETURN_HEIGHT * 0.5f, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(PAUSE_RETURN_POS_X + PAUSE_RETURN_WIDTH * 0.5f, PAUSE_RETURN_POS_Y - PAUSE_RETURN_HEIGHT * 0.5f, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(PAUSE_RETURN_POS_X - PAUSE_RETURN_WIDTH * 0.5f, PAUSE_RETURN_POS_Y + PAUSE_RETURN_HEIGHT * 0.5f, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(PAUSE_RETURN_POS_X + PAUSE_RETURN_WIDTH * 0.5f, PAUSE_RETURN_POS_Y + PAUSE_RETURN_HEIGHT * 0.5f, 0.0f);

	pVtx[8].pos = D3DXVECTOR3(PAUSE_POS_X - PAUSE_WIDTH * 0.5f, PAUSE_POS_Y - PAUSE_HEIGHT * 0.5f, 0.0f);
	pVtx[9].pos = D3DXVECTOR3(PAUSE_POS_X + PAUSE_WIDTH * 0.5f, PAUSE_POS_Y - PAUSE_HEIGHT * 0.5f, 0.0f);
	pVtx[10].pos = D3DXVECTOR3(PAUSE_POS_X - PAUSE_WIDTH * 0.5f, PAUSE_POS_Y + PAUSE_HEIGHT * 0.5f, 0.0f);
	pVtx[11].pos = D3DXVECTOR3(PAUSE_POS_X + PAUSE_WIDTH * 0.5f, PAUSE_POS_Y + PAUSE_HEIGHT * 0.5f, 0.0f);

	//rhwの設定(必ず1.0f)
	for (int nCnt = 0;nCnt < PAUSE_MAX - 1;nCnt++)
	{
		float fWork = (g_pause - nCnt) % 2 * 0.5f + 0.5f;
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

	for (int nCnt = 0;nCnt < PAUSE_MAX;nCnt++)
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

	pVtx[12].pos = D3DXVECTOR3(PAUSE_BG_POS_X, PAUSE_BG_POS_Y, 0.0f);
	pVtx[13].pos = D3DXVECTOR3(PAUSE_BG_POS_X + PAUSE_BG_WIDTH, PAUSE_BG_POS_Y, 0.0f);
	pVtx[14].pos = D3DXVECTOR3(PAUSE_BG_POS_X, PAUSE_BG_POS_Y + PAUSE_BG_HEIGHT, 0.0f);
	pVtx[15].pos = D3DXVECTOR3(PAUSE_BG_POS_X + PAUSE_BG_WIDTH, PAUSE_BG_POS_Y + PAUSE_BG_HEIGHT, 0.0f);

	//頂点カラーの設定(0～255の整数値)
	pVtx[12].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[13].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[14].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[15].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

	pVtx[12].rhw = 1.0f;
	pVtx[13].rhw = 1.0f;
	pVtx[14].rhw = 1.0f;
	pVtx[15].rhw = 1.0f;

	//texture頂点
	pVtx[12].tex = D3DXVECTOR2(0.98f, 0.98f);
	pVtx[13].tex = D3DXVECTOR2(0.99f, 0.98f);
	pVtx[14].tex = D3DXVECTOR2(0.98f, 0.99f);
	pVtx[15].tex = D3DXVECTOR2(0.99f, 0.99f);

	//仮想アドレス解放
	g_pVtxBufferGamePause->Unlock();

	return S_OK;
}