//--------------------------------------------------------------------------------
//
//　ウインドウ表示プログラム
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "enemy.h"
#include "player.h"
#include "ball.h"
#include "input.h"
#include "joystick.h"
#include "sound.h"
#include "tire_effect.h"
#include "NOS_effect.h"
#include "KF_Precision.h"
#include "KF_BodyContact.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define ENEMY_TEXTURENAME "data/TEXTURE/enemy.png"//ファイル名
#define ENEMY_POS (D3DXVECTOR3(GAME_AREA_RIGHT - 40.0f - CAR_WIDTH * 0.5f, (GAME_AREA_TOP + GAME_AREA_BOTTOM) * 0.5f,0.0f))
#define ENEMY_JOYSTICK_NUM (PS4_CONTROLLER_NUM)

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct {
	CBodyBox *pBodyBox;
	unsigned int nScore;
	D3DXVECTOR3 aTirePos[4];//タイアの位置
	float fAngle;
	float fLength;
	int nNOS;//N2O加速システム
}ENEMY;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexEnemy(LPDIRECT3DDEVICE9 pDevice);
void UpdateEnemyControl(void);
void UpdateEnemyTire(void);

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureEnemy = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferEnemy = NULL;//頂点バッファ管理インターフェースポインタ
ENEMY g_enemy;
int g_nCntPlaySeEnemy;
int g_nCntPlaySeNOSEnemy;

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitEnemy(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得
	
	g_nCntPlaySeEnemy = 0;
	g_enemy.nScore = 0;
	g_enemy.nNOS = NOS_MAX;
	g_nCntPlaySeNOSEnemy = 0;
	g_enemy.pBodyBox = new CBodyBox;
	g_enemy.pBodyBox->Init();
	g_enemy.pBodyBox->SetPosCenter(ENEMY_POS);
	g_enemy.pBodyBox->SetRotation(-D3DX_PI);
	g_enemy.pBodyBox->SetHalfSize(D3DXVECTOR2(CAR_WIDTH * 0.5f, CAR_HEIGHT * 0.5f));
	g_enemy.pBodyBox->SetMass(CAR_MASS);
	g_enemy.pBodyBox->SetMomentOfInertia(1.0f / 12.0f * CAR_MASS * (CAR_WIDTH * CAR_WIDTH + CAR_HEIGHT * CAR_HEIGHT));
	g_enemy.pBodyBox->Update();

	//タイア位置用
	g_enemy.fLength = sqrtf(TIRE_POS_WIDTH * 0.5f * TIRE_POS_WIDTH * 0.5f + TIRE_POS_HEIGHT * 0.5f * TIRE_POS_HEIGHT * 0.5f);
	g_enemy.fAngle = atan2(TIRE_POS_HEIGHT * 0.5f, TIRE_POS_WIDTH * 0.5f);
	g_enemy.aTirePos[0] = D3DXVECTOR3(ENEMY_POS.x - TIRE_POS_WIDTH * 0.5f, ENEMY_POS.y - TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_enemy.aTirePos[1] = D3DXVECTOR3(ENEMY_POS.x + TIRE_POS_WIDTH * 0.5f, ENEMY_POS.y - TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_enemy.aTirePos[2] = D3DXVECTOR3(ENEMY_POS.x + TIRE_POS_WIDTH * 0.5f, ENEMY_POS.y + TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_enemy.aTirePos[3] = D3DXVECTOR3(ENEMY_POS.x - TIRE_POS_WIDTH * 0.5f, ENEMY_POS.y + TIRE_POS_HEIGHT * 0.5f, 0.0f);

	if (FAILED(MakeVerTexEnemy(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		ENEMY_TEXTURENAME,
		&g_pTextureEnemy)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitEnemy(void)
{
	if (g_enemy.pBodyBox != NULL)//アドレスの中身確認
	{
		g_enemy.pBodyBox->Uninit();//終了処理
		delete g_enemy.pBodyBox;//中身を削除
		g_enemy.pBodyBox = NULL;//アドレスを空にする
	}

	if (g_pVtxBufferEnemy != NULL)
	{
		g_pVtxBufferEnemy->Release();
		g_pVtxBufferEnemy = NULL;
	}

	if (g_pTextureEnemy != NULL)
	{
		g_pTextureEnemy->Release();
		g_pTextureEnemy = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateEnemy(void)
{
	D3DXVECTOR3 vPosToContactPos = VEC3_NULL;//中心点と衝突点のベクトル
	D3DXVECTOR3 vWork = VEC3_NULL;//計算用ベクトル
	
	VERTEX_2D *pVtx = NULL;

	g_enemy.pBodyBox->Update();

	if (Vector3Magnitude(g_enemy.pBodyBox->GetVelocity()) >= FLOAT_MIN && g_nCntPlaySeEnemy == 0)
	{
		g_nCntPlaySeEnemy = 60;
		PlaySound(SOUND_LABEL_SE_SAND_ENEMY);
	}

	if (g_nCntPlaySeEnemy != 0)
	{
		g_nCntPlaySeEnemy--;
	}

	if (g_nCntPlaySeNOSEnemy != 0)
	{
		g_nCntPlaySeNOSEnemy--;
	}

	//UpdateEnemyContact();

	UpdateEnemyTire();

	g_pVtxBufferEnemy->Lock(0, 0, (void**)&pVtx, 0);

	//pos
	pVtx[0].pos = g_enemy.pBodyBox->GetVertex(0);
	pVtx[1].pos = g_enemy.pBodyBox->GetVertex(1);
	pVtx[2].pos = g_enemy.pBodyBox->GetVertex(3);
	pVtx[3].pos = g_enemy.pBodyBox->GetVertex(2);

	g_pVtxBufferEnemy->Unlock();

	//UpdateEnemyControl();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawEnemy(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferEnemy,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	//Textureの設定
	pDevice->SetTexture(0, g_pTextureEnemy);

	for (int nCnt = 0; nCnt < NUM_ENEMY; nCnt++)
	{
		//プリミティブ描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//オフセット（頂点数）
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexEnemy(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NUM_ENEMY,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferEnemy,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferEnemy->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	for (int nCnt = 0; nCnt < NUM_ENEMY; nCnt++)
	{
	
		//頂点座標の設定（2D座標、右回り）
		pVtx[0].pos = g_enemy.pBodyBox->GetVertex(0);
		pVtx[1].pos = g_enemy.pBodyBox->GetVertex(1);
		pVtx[2].pos = g_enemy.pBodyBox->GetVertex(3);
		pVtx[3].pos = g_enemy.pBodyBox->GetVertex(2);

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
	}

	//仮想アドレス解放
	g_pVtxBufferEnemy->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  敵衝突検出
//--------------------------------------------------------------------------------
void UpdateEnemyContact(void)
{
	CBodyBox *pPlayer = NULL;
	CBodyCicle *pBall = GetBall();

	//player contact
	pPlayer = GetPlayer();
	if (CheckContactBTB(g_enemy.pBodyBox, pPlayer))
	{
		//StartJoystickShock(ENEMY_JOYSTICK_NUM, 30);
	}

	if (GetBallCnt() == 0)
	{
		CheckContactCTB(pBall, g_enemy.pBodyBox);
	}
	
	BoxAreaContact(g_enemy.pBodyBox);
}

//--------------------------------------------------------------------------------
//  enemy操作更新
//--------------------------------------------------------------------------------
void UpdateEnemyControl(void)
{
	D3DXVECTOR3 vNOSPos = VEC3_NULL;
	D3DXVECTOR3 vRotationForce = VEC3_NULL;
	D3DXVECTOR3 vRotationForcePos = VEC3_NULL;
	float fWorldAngle = 0.0f;//全体的にとっての角度

	if (GetKeyboardPress(DIK_I) || GetJoystickPress(ENEMY_JOYSTICK_NUM, DIJ_BUTTON, DIJ_PS4_×))//
	{
		g_enemy.pBodyBox->AddForce(D3DXVECTOR3(CAR_ACC * cosf(g_enemy.pBodyBox->GetRotation()),
			CAR_ACC * sinf(g_enemy.pBodyBox->GetRotation()), 0.0f));
	}

	if (GetKeyboardPress(DIK_K) || GetJoystickPress(ENEMY_JOYSTICK_NUM, DIJ_BUTTON, DIJ_PS4_◯))//
	{
		g_enemy.pBodyBox->AddForce(D3DXVECTOR3(-CAR_ACC * 0.3f * cosf(g_enemy.pBodyBox->GetRotation()),
			-CAR_ACC * 0.5f * sinf(g_enemy.pBodyBox->GetRotation()), 0.0f));
	}

	if ((GetKeyboardPress(DIK_N) || GetJoystickPress(ENEMY_JOYSTICK_NUM, DIJ_BUTTON, DIJ_PS4_R1)))//NOS
	{
		if (g_enemy.nNOS >= 2)
		{
			g_enemy.nNOS -= 2;
			g_enemy.pBodyBox->AddForce(D3DXVECTOR3(CAR_NOS_ACC * cosf(g_enemy.pBodyBox->GetRotation()),
				CAR_NOS_ACC * sinf(g_enemy.pBodyBox->GetRotation()), 0.0f));
			vNOSPos = (g_enemy.pBodyBox->GetVertex(3) + g_enemy.pBodyBox->GetVertex(0)) * 0.5f;
			SetNOSEffect(vNOSPos, D3DXCOLOR(1.0f, 0.1f, 0.1f, 1.0f));

			if (g_nCntPlaySeNOSEnemy == 0)
			{
				g_nCntPlaySeNOSEnemy = 30;
				PlaySound(SOUND_LABEL_SE_NOS_ENEMY);
			}
		}
	}
	else
	{
		g_enemy.nNOS++;
		g_enemy.nNOS = g_enemy.nNOS >= NOS_MAX ? NOS_MAX : g_enemy.nNOS;
	}

	if (abs(GetJoystickStickStatus(ENEMY_JOYSTICK_NUM, DIJ_L_STICKX)) > STICK_MIN)//回転
	{
		vRotationForcePos = (g_enemy.pBodyBox->GetVertex(1) + g_enemy.pBodyBox->GetVertex(2)) * 0.5f;

		//ベクトルが世界軸にとっての角度
		fWorldAngle = g_enemy.pBodyBox->GetRotation();

		//回転作用力計算
		vRotationForce = D3DXVECTOR3(0.0f, CAR_ROTATION_FORCE, 0.0f)
			* ((float)GetJoystickStickStatus(ENEMY_JOYSTICK_NUM, DIJ_L_STICKX) / (float)STICK_MAX);
		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

		g_enemy.pBodyBox->AddForce(vRotationForce, vRotationForcePos);
	}
	else
	{
		float fUnrealStick = 0.0f;

		if (GetKeyboardPress(DIK_U))
		{
			fUnrealStick = -500.0f;
		}
		else if (GetKeyboardPress(DIK_O))
		{
			fUnrealStick = 500.0f;
		}

		vRotationForcePos = (g_enemy.pBodyBox->GetVertex(1) + g_enemy.pBodyBox->GetVertex(2)) * 0.5f;

		//ベクトルが世界軸にとっての角度
		fWorldAngle = g_enemy.pBodyBox->GetRotation();

		//回転作用力計算
		vRotationForce = D3DXVECTOR3(0.0f, CAR_ROTATION_FORCE, 0.0f)
			* (fUnrealStick / (float)STICK_MAX);
		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

		g_enemy.pBodyBox->AddForce(vRotationForce, vRotationForcePos);
	}
}

//--------------------------------------------------------------------------------
//  タイア位置更新
//--------------------------------------------------------------------------------
void UpdateEnemyTire(void)
{
	D3DXVECTOR3 vPos = VEC3_NULL;
	float fRotation = 0.0f;

	//タイア位置更新
	vPos = g_enemy.pBodyBox->GetPosCenter();
	fRotation = g_enemy.pBodyBox->GetRotation();

	g_enemy.aTirePos[0] = D3DXVECTOR3(vPos.x + cosf(fRotation + D3DX_PI + g_enemy.fAngle) * g_enemy.fLength, vPos.y + sin(fRotation + D3DX_PI + g_enemy.fAngle) * g_enemy.fLength, 0.0f);
	g_enemy.aTirePos[1] = D3DXVECTOR3(vPos.x + cosf(fRotation - g_enemy.fAngle) * g_enemy.fLength, vPos.y + sinf(fRotation - g_enemy.fAngle) * g_enemy.fLength, 0.0f);
	g_enemy.aTirePos[2] = D3DXVECTOR3(vPos.x + cosf(fRotation + g_enemy.fAngle) * g_enemy.fLength, vPos.y + sinf(fRotation + g_enemy.fAngle) * g_enemy.fLength, 0.0f);
	g_enemy.aTirePos[3] = D3DXVECTOR3(vPos.x + cosf(fRotation - D3DX_PI - g_enemy.fAngle) * g_enemy.fLength, vPos.y + sinf(fRotation - D3DX_PI - g_enemy.fAngle) * g_enemy.fLength, 0.0f);

	//effect
	if (Vector3Magnitude(g_enemy.pBodyBox->GetVelocity()) > FLOAT_MIN ||
		Vector3Magnitude(g_enemy.pBodyBox->GetRotationSpeed()) > FLOAT_MIN * 0.1f)
	{
		SetTireEffect(g_enemy.aTirePos[0]);
		SetTireEffect(g_enemy.aTirePos[1]);
		SetTireEffect(g_enemy.aTirePos[2]);
		SetTireEffect(g_enemy.aTirePos[3]);
	}
}

//--------------------------------------------------------------------------------
//  敵アドレス取得
//--------------------------------------------------------------------------------
CBodyBox *GetEnemy(void)
{
	return g_enemy.pBodyBox;
}

//--------------------------------------------------------------------------------
//  スコア加算
//--------------------------------------------------------------------------------
void AddEnemyScore(int nScore)
{
	g_enemy.nScore += nScore;
}

//--------------------------------------------------------------------------------
//  スコア取得
//--------------------------------------------------------------------------------
unsigned int GetEnemyScore(void)
{
	return g_enemy.nScore;
}

//--------------------------------------------------------------------------------
//  NOS取得
//--------------------------------------------------------------------------------
int GetEnemyNOS(void)
{
	return g_enemy.nNOS;
}

//--------------------------------------------------------------------------------
//  位置取得
//--------------------------------------------------------------------------------
D3DXVECTOR3 GetEnemyPos(void)
{
	return g_enemy.pBodyBox->GetPosCenter();
}

//--------------------------------------------------------------------------------
//  角度取得
//--------------------------------------------------------------------------------
float GetEnemyRot(void)
{
	return g_enemy.pBodyBox->GetRotation();
}