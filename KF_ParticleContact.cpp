//--------------------------------------------------------------------------------
//	粒子衝突計算ヘッダー
//　KF_ParticleContact.cpp
//	Author : Xu Wenjie
//	Date   : 2016-07-27
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "KF_ParticleContact.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define NUM_PARTICLE_CONTACT (100)
#define RESTITUTION (0.8f)//回弾係数
#define ITERATION_MAX (300)//衝突処理最大実行回数
#define GROUND_POS (0.0f)
#define CONTACT_NORMAL_GROUND (D3DXVECTOR3(0.0f,1.0f,0.0f))

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void ResolveContacts(void);
void Resolve(int nNumContact);
void ResolveVelocity(int nNumContact);
void ResolveInterpenetration(int nNumContact);
float CalculateSeparatingVelocity(int nNumContact);
void ClearParticleContact(int nNumContact);

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
KF_PARTICLE_CONTACT g_aPContact[NUM_PARTICLE_CONTACT];
int g_nNumContacts;

//--------------------------------------------------------------------------------
//  初期化
//--------------------------------------------------------------------------------
void InitParticleContact(void)
{
	int nCnt;

	g_nNumContacts = 0;
	for (nCnt = 0; nCnt < NUM_PARTICLE_CONTACT; nCnt++)
	{
		g_aPContact[nCnt].apParticle[0] = NULL;
		g_aPContact[nCnt].apParticle[1] = NULL;
		g_aPContact[nCnt].fPenetration = 0.0f;
		g_aPContact[nCnt].vContactNormal = { 0.0f,0.0f,0.0f };
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitParticleContact(void)
{
	int nCnt;

	for (nCnt = 0; nCnt < NUM_PARTICLE_CONTACT; nCnt++)
	{
		g_aPContact[nCnt].apParticle[0] = NULL;
		g_aPContact[nCnt].apParticle[1] = NULL;
		g_aPContact[nCnt].fPenetration = 0.0f;
		g_aPContact[nCnt].vContactNormal = { 0.0f,0.0f,0.0f };
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateParticleContact(void)
{
	ResolveContacts();
	g_nNumContacts = 0;
}

//--------------------------------------------------------------------------------
//  速さ順次衝突処理
//--------------------------------------------------------------------------------
void ResolveContacts(void)
{
	int nCnt;
	//実行回数
	int nIterationUsed = 0;

	while (nIterationUsed < ITERATION_MAX)
	{
		float fMax = 0.0f;
		int nMaxIndex = g_nNumContacts;
		for (nCnt = 0; nCnt < g_nNumContacts; nCnt++)
		{
			float fSepVel = CalculateSeparatingVelocity(nCnt);
			if (fSepVel < fMax)
			{
				fMax = fSepVel;
				nMaxIndex = nCnt;
			}
		}

		Resolve(nMaxIndex);
		nIterationUsed++;
	}
}

//--------------------------------------------------------------------------------
//  デストラクタ
//--------------------------------------------------------------------------------
void Resolve(int nNumContact)
{
	ResolveVelocity(nNumContact);
	ResolveInterpenetration(nNumContact);
	ClearParticleContact(nNumContact);
}

//--------------------------------------------------------------------------------
//  速度計算
//--------------------------------------------------------------------------------
void ResolveVelocity(int nNumContact)
{
	//分離速度計算
	float fSeparatingVelocity = CalculateSeparatingVelocity(nNumContact);

	//分離速度チェック
	if (fSeparatingVelocity > 0)
	{
		return;
	}

	//ないの場合計算し直す
	float fNewSeparatingVelocity = -fSeparatingVelocity * RESTITUTION;

	//静的衝突処理
	D3DXVECTOR3 vAccCausedVelocity = g_aPContact[nNumContact].apParticle[0]->vAcceleration;
	if (g_aPContact[nNumContact].apParticle[1])
	{
		vAccCausedVelocity -= g_aPContact[nNumContact].apParticle[1]->vAcceleration;
	}
	float fAccCausedSeparatingVelocity = ScalarProduct(vAccCausedVelocity, g_aPContact[nNumContact].vContactNormal) * DURATION;

	//
	if (fAccCausedSeparatingVelocity < 0)
	{
		fNewSeparatingVelocity += RESTITUTION * fAccCausedSeparatingVelocity;
		if (fNewSeparatingVelocity < 0) { fNewSeparatingVelocity = 0; }
	}

	//速度差分計算
	float fDeltaVelocity = fNewSeparatingVelocity - fSeparatingVelocity;

	//粒子の質量取得
	float fTotalInverseMass = g_aPContact[nNumContact].apParticle[0]->fInverseMass;
	if (g_aPContact[nNumContact].apParticle[1])
	{
		fTotalInverseMass += g_aPContact[nNumContact].apParticle[1]->fInverseMass;
	}

	//質量が無限大の場合計算しない
	if (fTotalInverseMass <= 0) { return; }

	//衝突力計算
	float fImpulse = fDeltaVelocity / fTotalInverseMass;

	//単位逆質量の衝突力
	D3DXVECTOR3 vImpulsePerInverseMass = g_aPContact[nNumContact].vContactNormal * fImpulse;

	//速度計算
	g_aPContact[nNumContact].apParticle[0]->vVelocity =
		g_aPContact[nNumContact].apParticle[0]->vVelocity + vImpulsePerInverseMass * g_aPContact[nNumContact].apParticle[0]->fInverseMass;

	if (g_aPContact[nNumContact].apParticle[1])
	{
		g_aPContact[nNumContact].apParticle[1]->vVelocity =
			g_aPContact[nNumContact].apParticle[1]->vVelocity + vImpulsePerInverseMass * g_aPContact[nNumContact].apParticle[1]->fInverseMass;
	}
}

//--------------------------------------------------------------------------------
//  衝突計算
//--------------------------------------------------------------------------------
void ResolveInterpenetration(int nNumContact)
{
	//衝突しない
	if (g_aPContact[nNumContact].fPenetration <= 0) { return; }

	//逆質量計算
	float fTotalInverseMass = g_aPContact[nNumContact].apParticle[0]->fInverseMass;
	if (g_aPContact[nNumContact].apParticle[1])
	{
		fTotalInverseMass += g_aPContact[nNumContact].apParticle[1]->fInverseMass;
	}

	//質量が無限大の場合計算しない
	if (fTotalInverseMass <= 0) { return; }

	//質量単位戻り量計算
	D3DXVECTOR3 vMovePerInverseMass = g_aPContact[nNumContact].vContactNormal * (-g_aPContact[nNumContact].fPenetration / fTotalInverseMass);

	//各粒子戻り位置計算
	g_aPContact[nNumContact].apParticle[0]->vPosition = g_aPContact[nNumContact].apParticle[0]->vPosition +
		vMovePerInverseMass * g_aPContact[nNumContact].apParticle[0]->fInverseMass;

	if (g_aPContact[nNumContact].apParticle[1])
	{
		g_aPContact[nNumContact].apParticle[1]->vPosition = g_aPContact[nNumContact].apParticle[1]->vPosition +
			vMovePerInverseMass * g_aPContact[nNumContact].apParticle[1]->fInverseMass;
	}
}

//--------------------------------------------------------------------------------
//  分離速度計算
//--------------------------------------------------------------------------------
float CalculateSeparatingVelocity(int nNumContact)
{
	if (!g_aPContact[nNumContact].apParticle[0])
	{
		return 0.0f;
	}

	D3DXVECTOR3 vRelativeVelocity = g_aPContact[nNumContact].apParticle[0]->vVelocity;

	//粒子と粒子の衝突
	if (g_aPContact[nNumContact].apParticle[1])
	{
		vRelativeVelocity -= g_aPContact[nNumContact].apParticle[1]->vVelocity;
	}

	//内積計算
	return ScalarProduct(vRelativeVelocity, g_aPContact[nNumContact].vContactNormal);
}

//--------------------------------------------------------------------------------
//  衝突設定
//--------------------------------------------------------------------------------
void SetParticleContact(KF_PARTICLE *pParticle1, KF_PARTICLE *pParticle2, float fSetPenetration, D3DXVECTOR3 vSetContactNormal)
{
	if (g_nNumContacts < NUM_PARTICLE_CONTACT)
	{
		g_aPContact[g_nNumContacts].apParticle[0] = pParticle1;
		g_aPContact[g_nNumContacts].apParticle[1] = pParticle2;
		g_aPContact[g_nNumContacts].fPenetration = fSetPenetration;
		g_aPContact[g_nNumContacts].vContactNormal = vSetContactNormal;
		g_nNumContacts++;//衝突回数カウント
	}
}

//--------------------------------------------------------------------------------
//  衝突クリア処理
//--------------------------------------------------------------------------------
void ClearParticleContact(int nNumContact)
{
	g_aPContact[nNumContact].apParticle[0] = NULL;
	g_aPContact[nNumContact].apParticle[1] = NULL;
	g_aPContact[nNumContact].fPenetration = 0.0f;
	g_aPContact[nNumContact].vContactNormal = { 0.0f,0.0f,0.0f };
}

//--------------------------------------------------------------------------------
//  地面衝突処理
//--------------------------------------------------------------------------------
void UpdateGroundContact(KF_PARTICLE *pParticle)
{
	if (pParticle)
	{
		if (pParticle->vPosition.y >= GROUND_POS)
		{
			SetParticleContact(
				pParticle,
				NULL,
				pParticle->vPosition.y - GROUND_POS,
				CONTACT_NORMAL_GROUND
			);
		}
	}
}