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
#include "KF_BodyContact.h"
#include "sound.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define NUM_CONTACT (100)
#define ITERATION_MAX (300)//衝突処理最大実行回数
#define GROUND_POS (GAME_AREA_BOTTOM)
#define LEFT_WALL_POS (GAME_AREA_LEFT)
#define RIGHT_WALL_POS (GAME_AREA_RIGHT)
#define TOP_WALL_POS (GAME_AREA_TOP)
#define CONTACT_NORMAL_GROUND (D3DXVECTOR3(0.0f,-1.0f,0.0f))
#define CONTACT_NORMAL_LEFT_WALL (D3DXVECTOR3(1.0f,0.0f,0.0f))
#define CONTACT_NORMAL_RIGHT_WALL (D3DXVECTOR3(-1.0f,0.0f,0.0f))
#define CONTACT_NORMAL_TOP_WALL (D3DXVECTOR3(0.0f,1.0f,0.0f))

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct {
	D3DXVECTOR3 vContactPos;
	D3DXVECTOR3 vContactNormal;
	float fDistance;
}CROSS_INFO;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void ResolveContacts(void);
void Resolve(int nNumContact);
void ResolveVelocity(int nNumContact);
void ResolveInterpenetration(int nNumContact);
float CalculateSeparatingVelocity(int nNumContact);
void ClearBodyContact(int nNumContact);
CROSS_INFO CheckIntersect(D3DXVECTOR3 vA, D3DXVECTOR3 vB, D3DXVECTOR3 vC, D3DXVECTOR3 vD);

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
KF_CONTACT g_aPContact[NUM_CONTACT];
int g_nNumContacts;
float g_fRestitution;

//--------------------------------------------------------------------------------
//  初期化
//--------------------------------------------------------------------------------
void InitBodyContact(void)
{
	int nCnt;

	g_nNumContacts = 0;
	g_fRestitution = RESTITUTION;

	for (nCnt = 0; nCnt < NUM_CONTACT; nCnt++)
	{
		g_aPContact[nCnt].bRotation = false;
		g_aPContact[nCnt].apBody[0] = NULL;
		g_aPContact[nCnt].apBody[1] = NULL;
		g_aPContact[nCnt].fPenetration = 0.0f;
		g_aPContact[nCnt].vContactPos = VEC3_NULL;
		g_aPContact[nCnt].vContactNormal = VEC3_NULL;
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitBodyContact(void)
{
	int nCnt;

	g_nNumContacts = 0;

	for (nCnt = 0; nCnt < NUM_CONTACT; nCnt++)
	{
		g_aPContact[nCnt].bRotation = false;
		g_aPContact[nCnt].apBody[0] = NULL;
		g_aPContact[nCnt].apBody[1] = NULL;
		g_aPContact[nCnt].fPenetration = 0.0f;
		g_aPContact[nCnt].vContactPos = VEC3_NULL;
		g_aPContact[nCnt].vContactNormal = VEC3_NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateBodyContact(void)
{
	ResolveContacts();
	g_nNumContacts = 0;
}

//--------------------------------------------------------------------------------
//  速さ順次衝突処理
//--------------------------------------------------------------------------------
void ResolveContacts(void)
{
	//実行回数
	int nIterationUsed = 0;

	//while (nIterationUsed < ITERATION_MAX)
	//{
	//	float fMax = 0.0f;
	//	int nMaxIndex = g_nNumContacts - 1;
	//	for (nCnt = 0; nCnt < g_nNumContacts; nCnt++)
	//	{
	//		float fSepVel = CalculateSeparatingVelocity(nCnt);
	//		if (fSepVel < fMax)
	//		{
	//			fMax = fSepVel;
	//			nMaxIndex = nCnt;
	//		}
	//	}

	//	Resolve(nMaxIndex);
	//	nIterationUsed++;

	//	if (nIterationUsed == g_nNumContacts) { break; }
	//}

	for (int nCnt = 0; nCnt < g_nNumContacts; nCnt++)
	{
		Resolve(nCnt);
		nIterationUsed++;
		if (nIterationUsed >= ITERATION_MAX)
		{
			break;
		}
	}
}

//--------------------------------------------------------------------------------
//  デストラクタ
//--------------------------------------------------------------------------------
void Resolve(int nNumContact)
{
	ResolveVelocity(nNumContact);
	ResolveInterpenetration(nNumContact);
	ClearBodyContact(nNumContact);
}

//--------------------------------------------------------------------------------
//  速度計算
//--------------------------------------------------------------------------------
void ResolveVelocity(int nNumContact)
{
	//変数宣言
	D3DXVECTOR3 vAccCausedVelocity;//衝突方向の作用力(加速度)
	D3DXVECTOR3 vImpulsePerInverseMass;//単位逆質量の衝突力
	D3DXVECTOR3 vVelocity;//変化した速度
	D3DXVECTOR3 vForce;//速度あたりの作用力
	D3DXVECTOR3 vPosToContactPos;//中心点と衝突点のベクトル
	D3DXVECTOR3 vWork;//計算用ベクトル
	D3DXVECTOR3 vRotationForce;//回転作用力
	float fSeparatingVelocity;//分離速度
	float fNewSeparatingVelocity;//新しい分離速度
	float fAccCausedSeparatingVelocity;//衝突法線上の加速度
	float fDeltaVelocity;//速度の変化量
	float fTotalInverseMass;//質量の総和
	float fImpulse;//衝突力
	float fWorldAngle;//全体的にとっての角度

	//分離速度計算
	fSeparatingVelocity = CalculateSeparatingVelocity(nNumContact);

	//分離速度チェック
	//衝突法線の逆方向になれば計算しない
	if (fSeparatingVelocity > 0.0f) { return; }

	//ないの場合計算し直す
	fNewSeparatingVelocity = -fSeparatingVelocity * g_fRestitution;

	//衝突方向に作用力を計算する
	vAccCausedVelocity = g_aPContact[nNumContact].apBody[0]->GetAcceleration();
	if (g_aPContact[nNumContact].apBody[1])
	{
		vAccCausedVelocity -= g_aPContact[nNumContact].apBody[1]->GetAcceleration();
	}
	fAccCausedSeparatingVelocity = ScalarProduct(vAccCausedVelocity, g_aPContact[nNumContact].vContactNormal);

	//衝突法線の逆方向になれば
	if (fAccCausedSeparatingVelocity < 0.0f)
	{
		fNewSeparatingVelocity += g_fRestitution * fAccCausedSeparatingVelocity * 1.0f;
		if (fNewSeparatingVelocity < 0.0f) { fNewSeparatingVelocity = 0.0f; }
	}

	//速度差分計算
	fDeltaVelocity = fNewSeparatingVelocity - fSeparatingVelocity;

	//粒子の質量取得
	fTotalInverseMass = g_aPContact[nNumContact].apBody[0]->GetInverseMass();
	if (g_aPContact[nNumContact].apBody[1])
	{
		fTotalInverseMass += g_aPContact[nNumContact].apBody[1]->GetInverseMass();
	}

	//質量が無限大の場合計算しない
	if (fTotalInverseMass <= 0) { return; }

	//衝突力計算
	fImpulse = fDeltaVelocity / fTotalInverseMass;

	//単位逆質量の衝突力
	vImpulsePerInverseMass = g_aPContact[nNumContact].vContactNormal * fImpulse;

	//速度計算
	vVelocity = vImpulsePerInverseMass * g_aPContact[nNumContact].apBody[0]->GetInverseMass();

	g_aPContact[nNumContact].apBody[0]->SetVelocity(g_aPContact[nNumContact].apBody[0]->GetVelocity() + vVelocity);

	if (g_aPContact[nNumContact].bRotation)
	{
		//線速度力積
		vForce = g_aPContact[nNumContact].apBody[0]->GetMass() * vVelocity / DURATION * ROTATION_DAMPING;

		//中心点と衝突点のベクトル
		vPosToContactPos = g_aPContact[nNumContact].apBody[0]->GetPosCenter() - g_aPContact[nNumContact].vContactPos;

		//ベクトルが世界軸にとっての角度
		fWorldAngle = atan2f(vPosToContactPos.y, vPosToContactPos.x);

		//回転作用力計算
		vWork = D3DXVECTOR3(vForce.x * cosf(-fWorldAngle) - vForce.y * sinf(-fWorldAngle),
			vForce.x * sinf(-fWorldAngle) + vForce.y * cosf(-fWorldAngle), 0.0f);
		vRotationForce = D3DXVECTOR3(0.0f, vWork.y, 0.0f);
		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

		g_aPContact[nNumContact].apBody[0]->AddForce(vRotationForce, g_aPContact[nNumContact].vContactPos);
	}
	
	if (g_aPContact[nNumContact].apBody[1])
	{
		vVelocity = -vImpulsePerInverseMass * g_aPContact[nNumContact].apBody[1]->GetInverseMass();
		g_aPContact[nNumContact].apBody[1]->SetVelocity(g_aPContact[nNumContact].apBody[1]->GetVelocity() + vVelocity);

		if (g_aPContact[nNumContact].bRotation)
		{
			//線速度力積
			vForce = g_aPContact[nNumContact].apBody[1]->GetMass() * vVelocity / DURATION * ROTATION_DAMPING;

			//中心点と衝突点のベクトル
			vPosToContactPos = g_aPContact[nNumContact].apBody[1]->GetPosCenter() - g_aPContact[nNumContact].vContactPos;

			//ベクトルが世界軸にとっての角度
			fWorldAngle = atan2f(vPosToContactPos.y, vPosToContactPos.x);

			//回転作用力計算
			vWork = D3DXVECTOR3(vForce.x * cosf(-fWorldAngle) - vForce.y * sinf(-fWorldAngle),
				vForce.x * sinf(-fWorldAngle) + vForce.y * cosf(-fWorldAngle), 0.0f);
			vRotationForce = D3DXVECTOR3(0.0f, vWork.y, 0.0f);
			vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
				vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

			g_aPContact[nNumContact].apBody[1]->AddForce(vRotationForce, g_aPContact[nNumContact].vContactPos);
		}
	}
}
//void ResolveVelocity(int nNumContact)
//{
//	//変数宣言
//	D3DXVECTOR3 vAccCausedVelocity;//衝突方向の作用力(加速度)
//	D3DXVECTOR3 vImpulsePerInverseMass;//単位逆質量の衝突力
//	D3DXVECTOR3 vVelocity;//変化した速度
//	D3DXVECTOR3 vForce;//速度あたりの作用力
//	D3DXVECTOR3 vPosToContactPos;//中心点と衝突点のベクトル
//	D3DXVECTOR3 vWork;//計算用ベクトル
//	D3DXVECTOR3 vRotationForce;//回転作用力
//	float fSeparatingVelocity;//分離速度
//	float fNewSeparatingVelocity;//新しい分離速度
//	float fAccCausedSeparatingVelocity;//衝突法線上の加速度
//	float fDeltaVelocity;//速度の変化量
//	float fTotalInverseMass;//質量の総和
//	float fImpulse;//衝突力
//	float fWorldAngle;//全体的にとっての角度
//
//					  //分離速度計算
//	fSeparatingVelocity = CalculateSeparatingVelocity(nNumContact);
//
//	//分離速度チェック
//	//衝突法線の逆方向になれば計算しない
//	if (fSeparatingVelocity > 0.0f) { return; }
//
//	//ないの場合計算し直す
//	fNewSeparatingVelocity = -fSeparatingVelocity * RESTITUTION;
//
//	//衝突方向に作用力を計算する
//	vAccCausedVelocity = g_aPContact[nNumContact].apBody[0]->GetAcceleration();
//	if (g_aPContact[nNumContact].apBody[1])
//	{
//		vAccCausedVelocity -= g_aPContact[nNumContact].apBody[1]->GetAcceleration();
//	}
//	fAccCausedSeparatingVelocity = ScalarProduct(vAccCausedVelocity, g_aPContact[nNumContact].vContactNormal);
//
//	//衝突法線の逆方向になれば
//	if (fAccCausedSeparatingVelocity < 0.0f)
//	{
//		fNewSeparatingVelocity += RESTITUTION * fAccCausedSeparatingVelocity * 1.0f;
//		if (fNewSeparatingVelocity < 0.0f) { fNewSeparatingVelocity = 0.0f; }
//	}
//
//	//速度差分計算
//	fDeltaVelocity = fNewSeparatingVelocity - fSeparatingVelocity;
//
//	//粒子の質量取得
//	fTotalInverseMass = g_aPContact[nNumContact].apBody[0]->GetInverseMass();
//	if (g_aPContact[nNumContact].apBody[1])
//	{
//		fTotalInverseMass += g_aPContact[nNumContact].apBody[1]->GetInverseMass();
//	}
//
//	//質量が無限大の場合計算しない
//	if (fTotalInverseMass <= 0) { return; }
//
//	//衝突力計算
//	fImpulse = fDeltaVelocity / fTotalInverseMass;
//
//	//単位逆質量の衝突力
//	vImpulsePerInverseMass = g_aPContact[nNumContact].vContactNormal * fImpulse;
//
//	//速度計算
//	vVelocity = vImpulsePerInverseMass * g_aPContact[nNumContact].apBody[0]->GetInverseMass();
//
//	g_aPContact[nNumContact].apBody[0]->SetVelocity(g_aPContact[nNumContact].apBody[0]->GetVelocity() + vVelocity);
//
//	if (g_aPContact[nNumContact].bRotation)
//	{
//		//線速度力積
//		vForce = g_aPContact[nNumContact].apBody[0]->GetMass() * vVelocity / DURATION * ROTATION_DAMPING;
//
//		//中心点と衝突点のベクトル
//		vPosToContactPos = g_aPContact[nNumContact].apBody[0]->GetPosCenter() - g_aPContact[nNumContact].vContactPos;
//
//		//ベクトルが世界軸にとっての角度
//		fWorldAngle = atan2f(vPosToContactPos.y, vPosToContactPos.x);
//
//		//回転作用力計算
//		vWork = D3DXVECTOR3(vForce.x * cosf(-fWorldAngle) - vForce.y * sinf(-fWorldAngle),
//			vForce.x * sinf(-fWorldAngle) + vForce.y * cosf(-fWorldAngle), 0.0f);
//		vRotationForce = D3DXVECTOR3(0.0f, vWork.y, 0.0f);
//		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
//			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);
//
//		g_aPContact[nNumContact].apBody[0]->AddForce(vRotationForce, g_aPContact[nNumContact].vContactPos);
//	}
//
//	if (g_aPContact[nNumContact].apBody[1])
//	{
//		vVelocity = -vImpulsePerInverseMass * g_aPContact[nNumContact].apBody[1]->GetInverseMass();
//		g_aPContact[nNumContact].apBody[1]->SetVelocity(g_aPContact[nNumContact].apBody[1]->GetVelocity() + vVelocity);
//
//		if (g_aPContact[nNumContact].bRotation)
//		{
//			//線速度力積
//			vForce = g_aPContact[nNumContact].apBody[1]->GetMass() * vVelocity / DURATION * ROTATION_DAMPING;
//
//			//中心点と衝突点のベクトル
//			vPosToContactPos = g_aPContact[nNumContact].apBody[1]->GetPosCenter() - g_aPContact[nNumContact].vContactPos;
//
//			//ベクトルが世界軸にとっての角度
//			fWorldAngle = atan2f(vPosToContactPos.y, vPosToContactPos.x);
//
//			//回転作用力計算
//			vWork = D3DXVECTOR3(vForce.x * cosf(-fWorldAngle) - vForce.y * sinf(-fWorldAngle),
//				vForce.x * sinf(-fWorldAngle) + vForce.y * cosf(-fWorldAngle), 0.0f);
//			vRotationForce = D3DXVECTOR3(0.0f, vWork.y, 0.0f);
//			vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
//				vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);
//
//			g_aPContact[nNumContact].apBody[1]->AddForce(vRotationForce, g_aPContact[nNumContact].vContactPos);
//		}
//	}
//}

//--------------------------------------------------------------------------------
//  衝突計算
//--------------------------------------------------------------------------------
void ResolveInterpenetration(int nNumContact)
{
	//衝突しない
	if (g_aPContact[nNumContact].fPenetration <= 0) { return; }

	//逆質量計算
	float fTotalInverseMass = g_aPContact[nNumContact].apBody[0]->GetInverseMass();
	if (g_aPContact[nNumContact].apBody[1])
	{
		fTotalInverseMass += g_aPContact[nNumContact].apBody[1]->GetInverseMass();
	}

	//質量が無限大の場合計算しない
	if (fTotalInverseMass <= 0) { return; }

	//質量単位戻り量計算
	D3DXVECTOR3 vMovePerInverseMass = g_aPContact[nNumContact].vContactNormal * (-g_aPContact[nNumContact].fPenetration / fTotalInverseMass);

	//各粒子戻り位置計算
	g_aPContact[nNumContact].apBody[0]->SetPosCenter(g_aPContact[nNumContact].apBody[0]->GetPosCenter() -
		vMovePerInverseMass * g_aPContact[nNumContact].apBody[0]->GetInverseMass());

	g_aPContact[nNumContact].apBody[0]->SetMovement(g_aPContact[nNumContact].apBody[0]->GetMovement() -
		vMovePerInverseMass * g_aPContact[nNumContact].apBody[0]->GetInverseMass());

	if (g_aPContact[nNumContact].apBody[1])
	{
		g_aPContact[nNumContact].apBody[1]->SetPosCenter(g_aPContact[nNumContact].apBody[1]->GetPosCenter() +
		vMovePerInverseMass * g_aPContact[nNumContact].apBody[1]->GetInverseMass());

		g_aPContact[nNumContact].apBody[1]->SetMovement(g_aPContact[nNumContact].apBody[1]->GetMovement() +
			vMovePerInverseMass * g_aPContact[nNumContact].apBody[1]->GetInverseMass());
	}
}

//--------------------------------------------------------------------------------
//  分離速度計算
//--------------------------------------------------------------------------------
float CalculateSeparatingVelocity(int nNumContact)
{
	if (!g_aPContact[nNumContact].apBody[0])
	{
		return 0.0f;
	}

	D3DXVECTOR3 vRelativeVelocity = g_aPContact[nNumContact].apBody[0]->GetVelocity();

	//粒子と粒子の衝突
	if (g_aPContact[nNumContact].apBody[1])
	{
		vRelativeVelocity -= g_aPContact[nNumContact].apBody[1]->GetVelocity();
	}

	//内積計算
	return ScalarProduct(vRelativeVelocity, g_aPContact[nNumContact].vContactNormal);
}

//--------------------------------------------------------------------------------
//  衝突設定
//--------------------------------------------------------------------------------
void SetBodyContact(bool bRotation, CBody *pBody1, CBody *pBody2, float fSetPenetration, D3DXVECTOR3 vSetContactNormal, D3DXVECTOR3 vSetContactPos)
{
	if (g_nNumContacts < NUM_CONTACT)
	{
		g_aPContact[g_nNumContacts].bRotation = bRotation;
		g_aPContact[g_nNumContacts].apBody[0] = pBody1;
		g_aPContact[g_nNumContacts].apBody[1] = pBody2;
		g_aPContact[g_nNumContacts].fPenetration = fSetPenetration;
		g_aPContact[g_nNumContacts].vContactNormal = vSetContactNormal;
		g_aPContact[g_nNumContacts].vContactPos = vSetContactPos;
		g_nNumContacts++;//衝突回数カウント
	}
}

//--------------------------------------------------------------------------------
//  衝突クリア処理
//--------------------------------------------------------------------------------
void ClearBodyContact(int nNumContact)
{
	g_aPContact[nNumContact].bRotation = false;
	g_aPContact[nNumContact].apBody[0] = NULL;
	g_aPContact[nNumContact].apBody[1] = NULL;
	g_aPContact[nNumContact].fPenetration = 0.0f;
	g_aPContact[nNumContact].vContactPos = VEC3_NULL;
	g_aPContact[nNumContact].vContactNormal = VEC3_NULL;
}

//--------------------------------------------------------------------------------
//  衝突検出処理
//--------------------------------------------------------------------------------
bool CheckContactBTB(CBodyBox *pBody1, CBodyBox *pBody2)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return false; }

	//変数宣言
	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	int nCntVertex = 0;
	bool bTransform = true;
	D3DXVECTOR3 vPos = VEC3_NULL;
	D3DXVECTOR3 vVertex = VEC3_NULL;
	D3DXVECTOR3 vSide = VEC3_NULL;
	D3DXVECTOR3 vNormal = VEC3_NULL;
	D3DXVECTOR3 vContactNormal = VEC3_NULL;
	D3DXVECTOR3 vContactPos = VEC3_NULL;
	CBodyBox *apBody[2] = { pBody1,pBody2 };

	//頂点と四辺との衝突を検出
	for (int nCntBody = 0; nCntBody < 1;nCntBody++)
	{
		for (int nCntPos = 0;nCntPos < 4;nCntPos++)
		{
			vPos = apBody[nCntBody]->GetVertex(nCntPos);

			for (nCntVertex = 0;nCntVertex < 4;nCntVertex++)
			{
				vVertex = apBody[(nCntBody + 1) % 2]->GetVertex(nCntVertex);
				vSide = apBody[(nCntBody + 1) % 2]->GetSide(nCntVertex);
				fDistance = Vector3Cross2D(vSide, (vPos - vVertex));

				//辺の外にあれば
				if (fDistance < 0.0f) { break; }
				if (fDistance == 0.0f) { continue; }
				//if (fDistance == 0.0f) { bTransform = false; }

				//fDistance = -fDistance;

				//衝突深さとほかの辺との衝突深さを比べて
				//一番浅いのを洗い出す
				if (fContactDistance > 0.0f && fDistance >= fContactDistance) { continue; }

				fContactDistance = fDistance;
				vContactNormal = -VerticalVector3(vSide);
				vContactPos = vPos;
			} 

			if (nCntVertex == 4)
			{
				PlaySound(SOUND_LABEL_SE_MEDIUM_CRASH);
				SetBodyContact(
					bTransform,
					apBody[nCntBody],
					apBody[(nCntBody + 1) % 2],
					fContactDistance,
					vContactNormal,
					vContactPos);
				return true;
			}
		}
	}

	return false;
}
//void CheckContactBTB(CBodyBox *pBody1, CBodyBox *pBody2)
//{
//	//衝突回数が最大数に超えれば検出しない
//	if (g_nNumContacts > NUM_CONTACT) { return; }
//
//	//変数宣言
//	float fContactDistance = 0.0f;
//	int nCntVertex = 0;
//	bool bTransform = true;
//	D3DXVECTOR3 vPos = VEC3_NULL;
//	D3DXVECTOR3 vPosOld = VEC3_NULL;
//	D3DXVECTOR3 aVertex[2] = {};
//	D3DXVECTOR3 vSide = VEC3_NULL;
//	D3DXVECTOR3 vNormal = VEC3_NULL;
//	D3DXVECTOR3 vContactNormal = VEC3_NULL;
//	D3DXVECTOR3 vContactPos = VEC3_NULL;
//	CBodyBox *apBody[2] = { pBody1,pBody2 };
//	CROSS_INFO crossInfo = {};
//
//	//頂点と四辺との衝突を検出
//	for (int nCntBody = 0; nCntBody < 2;nCntBody++)
//	{
//		for (int nCntPos = 0;nCntPos < 4;nCntPos++)
//		{
//			vPos = apBody[nCntBody]->GetVertex(nCntPos);
//			vPosOld = apBody[nCntBody]->GetVertexOld(nCntPos);
//
//			for (nCntVertex = 0;nCntVertex < 4;nCntVertex++)
//			{
//				aVertex[0] = apBody[(nCntBody + 1) % 2]->GetVertex(nCntVertex);
//				aVertex[1] = apBody[(nCntBody + 1) % 2]->GetVertex((nCntVertex + 1) % 4);
//				crossInfo = CheckIntersect(vPosOld, vPos, aVertex[0], aVertex[1]);
//				
//				//辺と交差しない
//				if (crossInfo.fDistance == 0.0f) { continue; }
//				//if (fDistance == 0.0f) { bTransform = false; }
//
//				//衝突深さとほかの辺との衝突深さを比べて
//				//一番深いのを洗い出す
//				if (fContactDistance > 0.0f && crossInfo.fDistance <= fContactDistance) { continue; }
//
//				fContactDistance = crossInfo.fDistance;
//				vContactNormal = crossInfo.vContactNormal;
//				vContactPos = crossInfo.vContactPos;
//			}
//
//			if (nCntVertex == 4 && fContactDistance != 0.0f)
//			{
//				PlaySound(SOUND_LABEL_SE_MEDIUM_CRASH);
//				SetBodyContact(
//					bTransform,
//					apBody[nCntBody],
//					apBody[(nCntBody + 1) % 2],
//					fContactDistance,
//					vContactNormal,
//					vContactPos);
//				return;
//			}
//		}
//	}
//}

//--------------------------------------------------------------------------------
//  衝突検出処理(円と円)
//--------------------------------------------------------------------------------
void CheckContactCTC(CBodyCicle *pBody1, CBodyCicle *pBody2)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	//円心の距離を計算する
	D3DXVECTOR3 vMidLine = pBody1->GetPosCenter() - pBody2->GetPosCenter();
	float fMidLineDistance = Vector3Magnitude(vMidLine);

	//距離と半径の和と比べる
	if (fMidLineDistance <= 0.0f || fMidLineDistance >= (pBody1->GetRadius() + pBody2->GetRadius()))
	{
		return;
	}

	//衝突法線を計算する
	D3DXVECTOR3 vContactNormal = vMidLine / fMidLineDistance;

	//衝突保存
	SetBodyContact(
		false,
		pBody1,
		pBody2, 
		pBody1->GetRadius() + pBody2->GetRadius() - fMidLineDistance, 
		vContactNormal,
		VEC3_NULL);
}

//--------------------------------------------------------------------------------
//  衝突検出処理(circle to box)
//--------------------------------------------------------------------------------
void CheckContactCTB(CBodyCicle *pBody1, CBodyBox *pBody2)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	float fRadius = pBody1->GetRadius();//円の半径を取得
	int nCnt = 0;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vSide = VEC3_NULL;
	D3DXVECTOR3 vNormal = VEC3_NULL;
	D3DXVECTOR3 vContactNormal = VEC3_NULL;
	D3DXVECTOR3 vCiclePos = pBody1->GetPosCenter();

	//四辺との衝突を検出
	for (nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody2->GetVertex(nCnt);
		vSide = pBody2->GetSide(nCnt);
		fDistance = -Vector3Cross2D(vSide, (vCiclePos - vVector));

		//辺の外にあれば
		if (fDistance >= 0.0f + fRadius) { break; }

		//衝突深度の計算
		fDistance = -fDistance + fRadius;

		//衝突深さとほかの辺との衝突深さを比べて
		//一番浅いのを洗い出す
		if (fContactDistance > 0.0f && fDistance >= fContactDistance) { continue; }

		fContactDistance = fDistance;
		vContactNormal = -VerticalVector3(vSide);
	}

	if (nCnt == 4)
	{
		SetBodyContact(
			false,
			pBody1,
			pBody2,
			fContactDistance,
			vContactNormal,
			VEC3_NULL);
	}
}

//--------------------------------------------------------------------------------
//  衝突検出処理(circle to box)
//--------------------------------------------------------------------------------
void CheckContactCTQ(CBodyCicle *pBody1, CBodyQuadrangle *pBody2)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	float fRadius = pBody1->GetRadius();//円の半径を取得
	int nCnt = 0;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vSide = VEC3_NULL;
	D3DXVECTOR3 vNormal = VEC3_NULL;
	D3DXVECTOR3 vContactNormal = VEC3_NULL;
	D3DXVECTOR3 vCiclePos = pBody1->GetPosCenter();

	//四辺との衝突を検出
	for (nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody2->GetVertex(nCnt);
		vSide = pBody2->GetSide(nCnt);
		fDistance = -Vector3Cross2D(vSide, (vCiclePos - vVector));

		//辺の外にあれば
		if (fDistance >= 0.0f + fRadius)
		{
			break;
		}

		//衝突深度の計算
		fDistance = -fDistance + fRadius;

		//衝突深さとほかの辺との衝突深さを比べて
		//一番浅いのを洗い出す
		if (fContactDistance > 0.0f && fDistance >= fContactDistance)
		{
			continue;
		}

		fContactDistance = fDistance;
		vContactNormal = -VerticalVector3(vSide);
	}

	if (nCnt == 4)
	{
		SetBodyContact(
			false,
			pBody1,
			pBody2,
			fContactDistance,
			vContactNormal,
			VEC3_NULL);
	}
}

//--------------------------------------------------------------------------------
//  エリア衝突処理
//--------------------------------------------------------------------------------
void AreaContact(CBody *pBody, float fRadius)
{
	UpdateGroundContact(pBody, fRadius);
	//UpdateLeftWallContact(pBody, fRadius);
	//UpdateRightWallContact(pBody, fRadius);
	UpdateTopWallContact(pBody, fRadius);
}

//--------------------------------------------------------------------------------
//  地面衝突処理
//--------------------------------------------------------------------------------
void UpdateGroundContact(CBody *pBody, float fRadius)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	if (pBody)
	{
		if (pBody->GetPosCenter().y + fRadius >= GROUND_POS)
		{
			SetBodyContact(
				false,
				pBody,
				NULL,
				pBody->GetPosCenter().y + fRadius - GROUND_POS,
				CONTACT_NORMAL_GROUND,
				VEC3_NULL
			);
		}
	}
}

//--------------------------------------------------------------------------------
//  左衝突処理
//--------------------------------------------------------------------------------
void UpdateLeftWallContact(CBody *pBody, float fRadius)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	if (pBody)
	{
		if (pBody->GetPosCenter().x - fRadius <= LEFT_WALL_POS)
		{
			SetBodyContact(
				false,
				pBody,
				NULL,
				LEFT_WALL_POS - pBody->GetPosCenter().x + fRadius,
				CONTACT_NORMAL_LEFT_WALL,
				VEC3_NULL
			);
		}
	}
}

//--------------------------------------------------------------------------------
//  右衝突処理
//--------------------------------------------------------------------------------
void UpdateRightWallContact(CBody *pBody, float fRadius)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	if (pBody)
	{
		if (pBody->GetPosCenter().x + fRadius >= RIGHT_WALL_POS)
		{
			SetBodyContact(
				false,
				pBody,
				NULL,
				pBody->GetPosCenter().x + fRadius - RIGHT_WALL_POS,
				CONTACT_NORMAL_RIGHT_WALL,
				VEC3_NULL
			);
		}
	}
}

//--------------------------------------------------------------------------------
//  上衝突処理
//--------------------------------------------------------------------------------
void UpdateTopWallContact(CBody *pBody, float fRadius)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	if (pBody)
	{
		if (pBody->GetPosCenter().y - fRadius <= TOP_WALL_POS)
		{
			SetBodyContact(
				false,
				pBody,
				NULL,
				TOP_WALL_POS + fRadius - pBody->GetPosCenter().y,
				CONTACT_NORMAL_TOP_WALL,
				VEC3_NULL
			);
		}
	}
}

//--------------------------------------------------------------------------------
//  箱の空間衝突
//--------------------------------------------------------------------------------
void BoxAreaContact(CBodyBox *pBody)
{
	BoxUpdateGroundContact(pBody);
	BoxUpdateLeftWallContact(pBody);
	BoxUpdateRightWallContact(pBody);
	BoxUpdateTopWallContact(pBody);
}

//--------------------------------------------------------------------------------
//  箱の空間衝突（地面）
//--------------------------------------------------------------------------------
void BoxUpdateGroundContact(CBodyBox *pBody)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vContactPos = VEC3_NULL;

	//四辺との衝突を検出
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody->GetVertex(nCnt);
		fDistance = vVector.y - GROUND_POS;

		//辺の外にあれば
		if (fDistance <= 0.0f) { continue; }

		//衝突深さとほかの辺との衝突深さを比べて
		//一番深いのを洗い出す
		if (fContactDistance > 0.0f && fDistance <= fContactDistance) { continue; }

		fContactDistance = fDistance;
		vContactPos = vVector;
	}

	if (fContactDistance > 0.0f)
	{
		PlaySound(SOUND_LABEL_SE_SMALL_CRASH);
		SetBodyContact(
			true,
			pBody,
			NULL,
			fContactDistance,
			CONTACT_NORMAL_GROUND,
			vContactPos);
	}
}

//--------------------------------------------------------------------------------
//  箱の空間衝突（左）
//--------------------------------------------------------------------------------
void BoxUpdateLeftWallContact(CBodyBox *pBody)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vContactPos = VEC3_NULL;

	//四辺との衝突を検出
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody->GetVertex(nCnt);
		fDistance = LEFT_WALL_POS - vVector.x;

		//辺の外にあれば
		if (fDistance <= 0.0f) { continue; }

		//衝突深さとほかの辺との衝突深さを比べて
		//一番深いのを洗い出す
		if (fContactDistance > 0.0f && fDistance <= fContactDistance) { continue; }

		fContactDistance = fDistance;
		vContactPos = vVector;
	}

	if (fContactDistance > 0.0f)
	{
		PlaySound(SOUND_LABEL_SE_SMALL_CRASH);
		SetBodyContact(
			true,
			pBody,
			NULL,
			fContactDistance,
			CONTACT_NORMAL_LEFT_WALL,
			vContactPos);
	}
}

//--------------------------------------------------------------------------------
//  箱の空間衝突（右）
//--------------------------------------------------------------------------------
void BoxUpdateRightWallContact(CBodyBox *pBody)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vContactPos = VEC3_NULL;

	//四辺との衝突を検出
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody->GetVertex(nCnt);
		fDistance = vVector.x - RIGHT_WALL_POS;

		//辺の外にあれば
		if (fDistance <= 0.0f) { continue; }

		//衝突深さとほかの辺との衝突深さを比べて
		//一番深いのを洗い出す
		if (fContactDistance > 0.0f && fDistance <= fContactDistance) { continue; }

		fContactDistance = fDistance;
		vContactPos = vVector;
		
	}

	if (fContactDistance > 0.0f)
	{
		PlaySound(SOUND_LABEL_SE_SMALL_CRASH);
		SetBodyContact(
			true,
			pBody,
			NULL,
			fContactDistance,
			CONTACT_NORMAL_RIGHT_WALL,
			vContactPos);
	}
}

//--------------------------------------------------------------------------------
//  箱の空間衝突（上）
//--------------------------------------------------------------------------------
void BoxUpdateTopWallContact(CBodyBox *pBody)
{
	//衝突回数が最大数に超えれば検出しない
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vContactPos = VEC3_NULL;

	//四辺との衝突を検出
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody->GetVertex(nCnt);
		fDistance = TOP_WALL_POS - vVector.y;

		//辺の外にあれば
		if (fDistance <= 0.0f) { continue; }

		//衝突深さとほかの辺との衝突深さを比べて
		//一番深いのを洗い出す
		if (fContactDistance > 0.0f && fDistance <= fContactDistance) { continue; }

		fContactDistance = fDistance;
		vContactPos = vVector;
		
	}

	if (fContactDistance > 0.0f)
	{
		PlaySound(SOUND_LABEL_SE_SMALL_CRASH);
		SetBodyContact(
			true,
			pBody,
			NULL,
			fContactDistance,
			CONTACT_NORMAL_TOP_WALL,
			vContactPos);
	}
}

//--------------------------------------------------------------------------------
//  線ABと線CDの交差を求める
//  交差しないの場合0を返す、交差の場合点Bと線CDの距離と交差点を返す
//--------------------------------------------------------------------------------
CROSS_INFO CheckIntersect(D3DXVECTOR3 vA, D3DXVECTOR3 vB, D3DXVECTOR3 vC, D3DXVECTOR3 vD)
{
	//変数宣言
	float fDistanceA = 0.0f;
	float fDistanceB = 0.0f;
	float fDistanceC = 0.0f;
	float fDistanceD = 0.0f;
	float fWorkA = 0.0f;
	float fWorkB = 0.0f;
	float fWorkC = 0.0f;
	float fWorkD = 0.0f;
	D3DXVECTOR3 vAB = VEC3_NULL;
	D3DXVECTOR3 vCD = VEC3_NULL;
	CROSS_INFO crossInfo;

	//ベクトルCDを求める
	vCD = vD - vC;
	D3DXVec3Normalize(&vCD, &vCD);

	//点AとベクトルCDの距離を求める
	fDistanceA = Vector3Cross2D(vCD, (vA - vC));

	//点BとベクトルCDの距離を求める
	fDistanceB = Vector3Cross2D(vCD, (vB - vC));

	//ベクトルABを求める
	vAB = vB - vA;
	D3DXVec3Normalize(&vAB, &vAB);

	//点CとベクトルCDの距離を求める
	fDistanceC = Vector3Cross2D(vAB, (vC - vA));

	//点SとベクトルCDの距離を求める
	fDistanceD = Vector3Cross2D(vAB, (vD - vA));

	//点Aと点BはベクトルCDの同じ側にある場合
	//もしくは点Cと点DはベクトルABの同じ側にある場合0を返す
	if (fDistanceA * fDistanceB >= 0.0f || fDistanceC * fDistanceD >= 0.0f)
	{
		crossInfo.fDistance = 0.0f;
		crossInfo.vContactPos = VEC3_NULL;
		crossInfo.vContactNormal = VEC3_NULL;
		return crossInfo;
	}

	//衝突深さ
	crossInfo.fDistance = fDistanceB;

	//直線方程を算出
	//直線方程式y = ax + b、点A(x,y)と点B(x,y)がわかるの場合
	//a = ( Ay - By ) / ( Ax - Bx )
	//b = ( AxBy - AyBx ) / ( Ax - Bx )
	//Ax = Bxの時 方程式はx = Axになる
	//Ay = Byの時 方程式はy = Ayになる
	//線AB算出
	fWorkA = (vA.y - vB.y) / (vA.x - vB.x);
	fWorkB = (vA.x * vB.y - vA.y * vB.x) / (vA.x - vB.x);

	//線CD算出
	fWorkC = (vC.y - vD.y) / (vC.x - vD.x);
	fWorkD = (vC.x * vD.y - vC.y * vD.x) / (vC.x - vD.x);

	//交差点を算出
	//両直線方程式y = ax + b, y = cx + bの場合
	//x = ( d - b ) / ( a - c )
	//y = a * ( d - b ) / ( a - c ) + b 
	crossInfo.vContactPos.x = (fWorkD - fWorkB) / (fWorkA - fWorkC);
	crossInfo.vContactPos.y = fWorkA * (fWorkD - fWorkB) / (fWorkA - fWorkC) + fWorkB;
	crossInfo.vContactPos.z = 0.0f;

	//衝突法線算出
	crossInfo.vContactNormal = -VerticalVector3(vCD);

	return crossInfo;
}

//--------------------------------------------------------------------------------
//  回弾係数を設定する（event用）
//--------------------------------------------------------------------------------
void SetRestitution(float fRestitution)
{
	g_fRestitution = fRestitution;
}