//--------------------------------------------------------------------------------
//
//　ウインドウ表示プログラム
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "input.h"
#include "joystick.h"
#include "player.h"
#include "enemy.h"
#include "ball.h"
#include "fade.h"
#include "sound.h"
#include "tire_effect.h"
#include "NOS_effect.h"
#include "KF_Precision.h"
#include "KF_BodyContact.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define PLAYER_TEXTURENAME "data/TEXTURE/player.png"//ファイル名
#define PLAYER_POS (D3DXVECTOR3(40.0f + CAR_WIDTH * 0.5f, (GAME_AREA_TOP + GAME_AREA_BOTTOM) * 0.5f,0.0f))
#define PLAYER_JOYSTICK_NUM (XBOX_CONTROLLER_NUM)

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct {
	LPDIRECT3DTEXTURE9 pTexture;//textureインターフェース
	LPDIRECT3DVERTEXBUFFER9 pVtxBuffer;//頂点バッファ管理インターフェースポインタ
	CBodyBox *pBodyBox;
	D3DXVECTOR3 aTirePos[4];//タイアの位置
	float fAngle;
	float fLength;
	unsigned int nScore;
	int nNOS;//N2O加速システム
}PLAYER;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPlayer(LPDIRECT3DDEVICE9 pDevice);
void UpdatePlayerControl(void);
void UpdatePlayerTire(void);

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
PLAYER g_player;
int g_nCntPlaySePlayer;
int g_nCntPlaySeNOSPlayer;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_player.pTexture = NULL;
	g_player.pVtxBuffer = NULL;
	g_nCntPlaySePlayer = 0;
	g_nCntPlaySeNOSPlayer = 0;
	g_player.nScore = 0;
	g_player.nNOS = NOS_MAX;
	g_player.pBodyBox = new CBodyBox;
	g_player.pBodyBox->Init();
	g_player.pBodyBox->SetPosCenter(PLAYER_POS);
	g_player.pBodyBox->SetRotation(D3DX_PI * 2.0f);
	g_player.pBodyBox->SetHalfSize(D3DXVECTOR2(CAR_WIDTH * 0.5f, CAR_HEIGHT * 0.5f));
	g_player.pBodyBox->SetMass(CAR_MASS);
	g_player.pBodyBox->SetMomentOfInertia(1.0f / 12.0f * CAR_MASS * (CAR_WIDTH * CAR_WIDTH + CAR_HEIGHT * CAR_HEIGHT));

	//タイア位置用
	g_player.fLength = sqrtf(TIRE_POS_WIDTH * 0.5f * TIRE_POS_WIDTH * 0.5f + TIRE_POS_HEIGHT * 0.5f * TIRE_POS_HEIGHT * 0.5f);
	g_player.fAngle = atan2(TIRE_POS_HEIGHT * 0.5f, TIRE_POS_WIDTH * 0.5f);
	g_player.aTirePos[0] = D3DXVECTOR3(PLAYER_POS.x - TIRE_POS_WIDTH * 0.5f, PLAYER_POS.y - TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_player.aTirePos[1] = D3DXVECTOR3(PLAYER_POS.x + TIRE_POS_WIDTH * 0.5f, PLAYER_POS.y - TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_player.aTirePos[2] = D3DXVECTOR3(PLAYER_POS.x + TIRE_POS_WIDTH * 0.5f, PLAYER_POS.y + TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_player.aTirePos[3] = D3DXVECTOR3(PLAYER_POS.x - TIRE_POS_WIDTH * 0.5f, PLAYER_POS.y + TIRE_POS_HEIGHT * 0.5f, 0.0f);

	if (FAILED(MakeVerTexPlayer(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		PLAYER_TEXTURENAME,
		&g_player.pTexture)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitPlayer(void)
{
	if (g_player.pBodyBox != NULL)
	{
		g_player.pBodyBox->Uninit();
		g_player.pBodyBox = NULL;
	}

	if (g_player.pVtxBuffer != NULL)
	{
		g_player.pVtxBuffer->Release();
		g_player.pVtxBuffer = NULL;
	}

	if (g_player.pTexture != NULL)
	{
		g_player.pTexture->Release();
		g_player.pTexture = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdatePlayer(void)
{
	//変数宣言
	D3DXVECTOR3 vPosToContactPos;//中心点と衝突点のベクトル
	D3DXVECTOR3 vWork = VEC3_NULL;;//計算用ベクトル

	g_player.pBodyBox->Update();

	if (Vector3Magnitude(g_player.pBodyBox->GetVelocity()) >= FLOAT_MIN && g_nCntPlaySePlayer == 0)
	{
		g_nCntPlaySePlayer = 60;
		PlaySound(SOUND_LABEL_SE_SAND_PLAYER);
	}

	if (g_nCntPlaySePlayer != 0)
	{
		g_nCntPlaySePlayer--;
	}

	if (g_nCntPlaySeNOSPlayer != 0)
	{
		g_nCntPlaySeNOSPlayer--;
	}

	////衝突更新
	//UpdatePlayerContact();

	UpdatePlayerTire();
	
	VERTEX_2D *pVtx = NULL;

	g_player.pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	//pos
	pVtx[0].pos = g_player.pBodyBox->GetVertex(0);
	pVtx[1].pos = g_player.pBodyBox->GetVertex(1);
	pVtx[2].pos = g_player.pBodyBox->GetVertex(3);
	pVtx[3].pos = g_player.pBodyBox->GetVertex(2);

	g_player.pVtxBuffer->Unlock();

	//UpdatePlayerControl();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	//player
	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_player.pVtxBuffer,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//Textureの設定
	pDevice->SetTexture(0, g_player.pTexture);

	//プリミティブ描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//オフセット（頂点数）
		NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPlayer(LPDIRECT3DDEVICE9 pDevice)
{
	//player
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_player.pVtxBuffer,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_player.pVtxBuffer->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	//頂点座標の設定（2D座標、右回り）
	pVtx[0].pos = g_player.pBodyBox->GetVertex(0);
	pVtx[1].pos = g_player.pBodyBox->GetVertex(1);
	pVtx[2].pos = g_player.pBodyBox->GetVertex(3);
	pVtx[3].pos = g_player.pBodyBox->GetVertex(2);


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
	g_player.pVtxBuffer->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  player操作更新
//--------------------------------------------------------------------------------
void UpdatePlayerControl(void)
{
	//変数宣言
	D3DXVECTOR3 vNOSPos = VEC3_NULL;
	D3DXVECTOR3 vRotationForce = VEC3_NULL;
	D3DXVECTOR3 vRotationForcePos = VEC3_NULL;
	float fWorldAngle = 0.0f;//全体的にとっての角度

	if (GetKeyboardPress(DIK_W) || GetJoystickPress(PLAYER_JOYSTICK_NUM, DIJ_BUTTON, DIJ_XBOX_A))//アクセル
	{
		g_player.pBodyBox->AddForce(D3DXVECTOR3(CAR_ACC * cosf(g_player.pBodyBox->GetRotation()),
			CAR_ACC * sinf(g_player.pBodyBox->GetRotation()), 0.0f));
	}

	if (GetKeyboardPress(DIK_S) || GetJoystickPress(PLAYER_JOYSTICK_NUM, DIJ_BUTTON, DIJ_XBOX_B))//ブレーキ
	{
		g_player.pBodyBox->AddForce(D3DXVECTOR3(-CAR_ACC * 0.3f * cosf(g_player.pBodyBox->GetRotation()),
			-CAR_ACC * 0.3f * sinf(g_player.pBodyBox->GetRotation()), 0.0f));
	}

	if (GetKeyboardPress(DIK_X) || GetJoystickPress(PLAYER_JOYSTICK_NUM, DIJ_BUTTON, DIJ_XBOX_RB))//NOS
	{
		if (g_player.nNOS >= 2)
		{
			g_player.nNOS -= 2;
			g_player.pBodyBox->AddForce(D3DXVECTOR3(CAR_NOS_ACC * cosf(g_player.pBodyBox->GetRotation()),
				CAR_NOS_ACC * sinf(g_player.pBodyBox->GetRotation()), 0.0f));

			vNOSPos = (g_player.pBodyBox->GetVertex(3) + g_player.pBodyBox->GetVertex(0)) * 0.5f;
			SetNOSEffect(vNOSPos, D3DXCOLOR(0.1f, 0.1f, 1.0f, 1.0f));

			if (g_nCntPlaySeNOSPlayer == 0)
			{
				g_nCntPlaySeNOSPlayer = 30;
				PlaySound(SOUND_LABEL_SE_NOS_PLAYER);
			}
		}
	}
	else
	{
		g_player.nNOS++;
		g_player.nNOS = g_player.nNOS >= NOS_MAX ? NOS_MAX : g_player.nNOS;
	}
	
	if (abs(GetJoystickStickStatus(PLAYER_JOYSTICK_NUM, DIJ_L_STICKX)) > STICK_MIN)//回転
	{
		//作用力の位置計算
		vRotationForcePos = (g_player.pBodyBox->GetVertex(1) + g_player.pBodyBox->GetVertex(2)) * 0.5f;

		//ベクトルが世界軸にとっての角度
		fWorldAngle = g_player.pBodyBox->GetRotation();

		//回転作用力計算
		vRotationForce = D3DXVECTOR3(0.0f, CAR_ROTATION_FORCE, 0.0f)
			* ((float)GetJoystickStickStatus(PLAYER_JOYSTICK_NUM, DIJ_L_STICKX) / (float)STICK_MAX);
		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

		g_player.pBodyBox->AddForce(vRotationForce, vRotationForcePos);
	}
	else 
	{
		float fUnrealStick = 0.0f;

		if (GetKeyboardPress(DIK_Q))
		{
			fUnrealStick = -500.0f;
		}
		else if (GetKeyboardPress(DIK_E))
		{
			fUnrealStick = 500.0f;
		}

		//作用力の位置計算
		vRotationForcePos = (g_player.pBodyBox->GetVertex(1) + g_player.pBodyBox->GetVertex(2)) * 0.5f;

		//ベクトルが世界軸にとっての角度
		fWorldAngle = g_player.pBodyBox->GetRotation();

		//回転作用力計算
		vRotationForce = D3DXVECTOR3(0.0f, CAR_ROTATION_FORCE, 0.0f)
			* (fUnrealStick / (float)STICK_MAX);
		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

		g_player.pBodyBox->AddForce(vRotationForce, vRotationForcePos);
	}
}

//--------------------------------------------------------------------------------
//  タイア位置更新
//--------------------------------------------------------------------------------
void UpdatePlayerTire(void)
{
	//変数宣言
	D3DXVECTOR3 vPos = VEC3_NULL;
	float fRotation = 0.0f;

	//タイア位置更新
	vPos = g_player.pBodyBox->GetPosCenter();
	fRotation = g_player.pBodyBox->GetRotation();

	g_player.aTirePos[0] = D3DXVECTOR3(vPos.x + cosf(fRotation + D3DX_PI + g_player.fAngle) * g_player.fLength, vPos.y + sin(fRotation + D3DX_PI + g_player.fAngle) * g_player.fLength, 0.0f);
	g_player.aTirePos[1] = D3DXVECTOR3(vPos.x + cosf(fRotation - g_player.fAngle) * g_player.fLength, vPos.y + sinf(fRotation - g_player.fAngle) * g_player.fLength, 0.0f);
	g_player.aTirePos[2] = D3DXVECTOR3(vPos.x + cosf(fRotation + g_player.fAngle) * g_player.fLength, vPos.y + sinf(fRotation + g_player.fAngle) * g_player.fLength, 0.0f);
	g_player.aTirePos[3] = D3DXVECTOR3(vPos.x + cosf(fRotation - D3DX_PI - g_player.fAngle) * g_player.fLength, vPos.y + sinf(fRotation - D3DX_PI - g_player.fAngle) * g_player.fLength, 0.0f);

	//effect
	if (Vector3Magnitude(g_player.pBodyBox->GetVelocity()) > FLOAT_MIN ||
		Vector3Magnitude(g_player.pBodyBox->GetRotationSpeed()) > FLOAT_MIN * 0.1f)
	{
		SetTireEffect(g_player.aTirePos[0]);
		SetTireEffect(g_player.aTirePos[1]);
		SetTireEffect(g_player.aTirePos[2]);
		SetTireEffect(g_player.aTirePos[3]);
	}
}

//--------------------------------------------------------------------------------
//  衝突検出
//--------------------------------------------------------------------------------
void UpdatePlayerContact(void)
{
	CBodyBox *pEnemy = NULL;
	CBodyCicle *pBall = NULL;

	//enemy contact
	pEnemy = GetEnemy();
	if (CheckContactBTB(g_player.pBodyBox, pEnemy))
	{
		//StartJoystickShock(PLAYER_JOYSTICK_NUM, 30);
	}

	//ball contact
	pBall = GetBall();
	if (GetBallCnt() == 0)
	{
		CheckContactCTB(pBall, g_player.pBodyBox);
	}
	
	//area contact
	BoxAreaContact(g_player.pBodyBox);
}

//--------------------------------------------------------------------------------
//  スコア加算
//--------------------------------------------------------------------------------
void AddPlayerScore(int nScore)
{
	g_player.nScore += nScore;
}

//--------------------------------------------------------------------------------
//  スコア取得
//--------------------------------------------------------------------------------
unsigned int GetPlayerScore(void)
{
	return g_player.nScore;
}

//--------------------------------------------------------------------------------
//  NOS取得
//--------------------------------------------------------------------------------
int GetPlayerNOS(void)
{
	return g_player.nNOS;
}

//--------------------------------------------------------------------------------
//  アドレス取得
//--------------------------------------------------------------------------------
CBodyBox *GetPlayer(void)
{
	return g_player.pBodyBox;
}

//--------------------------------------------------------------------------------
//  位置取得
//--------------------------------------------------------------------------------
D3DXVECTOR3 GetPlayerPos(void)
{
	return g_player.pBodyBox->GetPosCenter();
}

//--------------------------------------------------------------------------------
//  角度取得
//--------------------------------------------------------------------------------
float GetPlayerRot(void)
{
	return g_player.pBodyBox->GetRotation();
}