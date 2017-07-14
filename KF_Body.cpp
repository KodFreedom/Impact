//--------------------------------------------------------------------------------
//	衝突物ヘッダー
//　KF_Body.h
//	Author : Xu Wenjie
//	Date   : 2016-07-25
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"
#include "KF_Body.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  静的メンバ変数
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  Body
//--------------------------------------------------------------------------------
//---------------------------------------
//  コンストラクタ
//---------------------------------------
CBody::CBody()
{
	m_fInverseMass = 0.0f;
	m_fDamping = 0.0f;
	m_fMomentOfInertia = 0.0f;
	//m_fRotation = 0.0f;
	//m_fK1 = 0.0f;
	//m_fK2 = 0.0f;
	m_vRotationSpeed = VEC3_NULL;
	m_vAccRotation = VEC3_NULL;
	m_vRotationForceAccum = VEC3_NULL;
	m_vRotation = VEC3_NULL;
	m_vAcceleration = VEC3_NULL;
	m_vForceAccum = VEC3_NULL;
	m_vGravity = VEC3_NULL;
	m_vGravityCenter = VEC3_NULL;
	m_vPositionCenter = VEC3_NULL;
	m_vVelocity = VEC3_NULL;
	m_vMovement = VEC3_NULL;
}

//---------------------------------------
//  デストラクタ
//---------------------------------------
CBody::~CBody()
{
	m_fInverseMass = 0.0f;
	m_fDamping = 0.0f;
	m_fMomentOfInertia = 0.0f;
	//m_fRotation = 0.0f;
	//m_fK1 = 0.0f;
	//m_fK2 = 0.0f;
	m_vRotationSpeed = VEC3_NULL;
	m_vAccRotation = VEC3_NULL;
	m_vRotationForceAccum = VEC3_NULL;
	m_vRotation = VEC3_NULL;
	m_vAcceleration = VEC3_NULL;
	m_vForceAccum = VEC3_NULL;
	m_vGravity = VEC3_NULL;
	m_vGravityCenter = VEC3_NULL;
	m_vPositionCenter = VEC3_NULL;
	m_vVelocity = VEC3_NULL;
	m_vMovement = VEC3_NULL;
}

//---------------------------------------
//  初期化
//---------------------------------------
void CBody::Init(void)
{
	m_fInverseMass = 0.0f;
	m_fDamping = DAMPING;
	m_fMomentOfInertia = 0.0f;
	//m_fRotation = 0.0f;
	//m_fK1 = 0.0f;
	//m_fK2 = 0.0f;
	m_vRotationSpeed = VEC3_NULL;
	m_vAccRotation = VEC3_NULL;
	m_vRotationForceAccum = VEC3_NULL;
	m_vRotation = VEC3_NULL;
	m_vAcceleration = VEC3_NULL;
	m_vForceAccum = VEC3_NULL;
	m_vGravity = VEC3_NULL;
	m_vGravityCenter = VEC3_NULL;
	m_vPositionCenter = VEC3_NULL;
	m_vVelocity = VEC3_NULL;
	m_vMovement = VEC3_NULL;
}

//---------------------------------------
//  終了処理
//---------------------------------------
void CBody::Uninit(void)
{
	m_fInverseMass = 0.0f;
	m_fDamping = 0.0f;
	m_fMomentOfInertia = 0.0f;
	//m_fRotation = 0.0f;
	//m_fK1 = 0.0f;
	//m_fK2 = 0.0f;
	m_vRotationSpeed = VEC3_NULL;
	m_vAccRotation = VEC3_NULL;
	m_vRotationForceAccum = VEC3_NULL;
	m_vRotation = VEC3_NULL;
	m_vAcceleration = VEC3_NULL;
	m_vForceAccum = VEC3_NULL;
	m_vGravity = VEC3_NULL;
	m_vGravityCenter = VEC3_NULL;
	m_vPositionCenter = VEC3_NULL;
	m_vVelocity = VEC3_NULL;
	m_vMovement = VEC3_NULL;
}

//---------------------------------------
//  更新
//---------------------------------------
void CBody::Update(void)
{
	m_vAcceleration = VEC3_NULL;
	m_vAccRotation = VEC3_NULL;

	UpdateGravity();

	//計算済み加速度
	//D3DXVECTOR3 vResultAcc = m_vAcceleration;

	//位置更新
	m_vPositionCenter += m_vVelocity;
	m_vMovement += m_vVelocity;

	//回転
	m_vAccRotation += m_vRotationForceAccum * DURATION;
	m_vRotationSpeed += m_vAccRotation;
	m_vRotation += m_vRotationSpeed;
	Vector3Normalize(&m_vRotation);

	//力によって加速度を更新
	m_vAcceleration += m_vForceAccum * m_fInverseMass;
	

	//速度更新
	m_vVelocity += m_vAcceleration;

	//抵抗力
	m_vVelocity *= m_fDamping;
	m_vRotationSpeed *= m_fDamping * m_fDamping;

	ClearAccumulator();
}

//---------------------------------------
//  中心設定
//---------------------------------------
void CBody::SetPosCenter(D3DXVECTOR3 vSetPosCenter)
{
	m_vPositionCenter = vSetPosCenter;
}

//---------------------------------------
//  中心取得
//---------------------------------------
D3DXVECTOR3 CBody::GetPosCenter(void)
{
	return m_vPositionCenter;
}

//---------------------------------------
//  重心設定
//---------------------------------------
void CBody::SetGravityCenter(D3DXVECTOR3 vGravityCenter)
{
	m_vGravityCenter = vGravityCenter;
}

//---------------------------------------
//  重心取得
//---------------------------------------
D3DXVECTOR3 CBody::GetGravityCenter(void)
{
	return m_vGravityCenter;
}

//---------------------------------------
//  速度設定
//---------------------------------------
void CBody::SetVelocity(D3DXVECTOR3 vSetVelocity)
{
	m_vVelocity = vSetVelocity;
}

//---------------------------------------
//  速度取得
//---------------------------------------
D3DXVECTOR3 CBody::GetVelocity(void)
{
	return m_vVelocity;
}

//---------------------------------------
//  移動量取得
//---------------------------------------
void CBody::SetMovement(D3DXVECTOR3 vSetMovement)
{
	m_vMovement = vSetMovement;
}

//---------------------------------------
//  移動量取得
//---------------------------------------
D3DXVECTOR3 CBody::GetMovement(void)
{
	return m_vMovement;
}

//---------------------------------------
//  加速度設定
//---------------------------------------
void CBody::SetAcceleration(D3DXVECTOR3 vSetAcceleration)
{
	m_vAcceleration = vSetAcceleration;
}

//---------------------------------------
//  加速度取得
//---------------------------------------
D3DXVECTOR3 CBody::GetAcceleration(void)
{
	return m_vAcceleration;
}

//---------------------------------------
//  質量設定
//---------------------------------------
void CBody::SetMass(float fMass)
{
	if (fMass == 0.0f) { return; }
	m_fInverseMass = 1.0f / fMass;
}

//---------------------------------------
//  質量取得
//---------------------------------------
float CBody::GetMass(void)
{
	if (m_fInverseMass == 0.0f) { return FLOAT_MAX; }
	return (1.0f / m_fInverseMass);
}

//---------------------------------------
//  逆質量設定
//---------------------------------------
void CBody::SetInverseMass(float fInverseMass)
{
	m_fInverseMass = fInverseMass;
}

//---------------------------------------
//  逆質量取得
//---------------------------------------
float CBody::GetInverseMass(void)
{
	return m_fInverseMass;
}

//---------------------------------------
//  重力設定
//---------------------------------------
void CBody::SetGravity(D3DXVECTOR3 vSetGravity)
{
	m_vGravity = vSetGravity;
}

//---------------------------------------
//  抵抗力係数設定
//---------------------------------------
void CBody::SetDamping(float fSetDamping)
{
	m_fDamping = fSetDamping;
}

//---------------------------------------
//  角度設定
//---------------------------------------
void CBody::SetRotation(float fSetRotation)
{
	//m_fRotation = fSetRotation;
	m_vRotation = D3DXVECTOR3(cosf(fSetRotation), sinf(fSetRotation), 0.0f);
	Vector3Normalize(&m_vRotation);
}

void CBody::SetRotation(D3DXVECTOR3 vSetRotation)
{
	//m_fRotation = fSetRotation;
	m_vRotation = vSetRotation;
	Vector3Normalize(&m_vRotation);
}
//---------------------------------------
//  角度取得
//---------------------------------------
float CBody::GetRotation(void)
{
	D3DXVECTOR3 vRotation = m_vRotation;
	//if (vRotation.x < FLOAT_MIN && vRotation.x > -FLOAT_MIN) { vRotation.x = 0.0f; }
	//if (vRotation.y < FLOAT_MIN && vRotation.y > -FLOAT_MIN) { vRotation.y = 0.0f; }
	float fRotation = atan2f(vRotation.y, vRotation.x);
	return fRotation;
}

//---------------------------------------
//  回転速度取得
//---------------------------------------
D3DXVECTOR3 CBody::GetRotationSpeed(void)
{
	return m_vRotationSpeed;
}

//---------------------------------------
//  角度設定
//---------------------------------------
void CBody::SetMomentOfInertia(float fSetMomentOfInertia)
{
	m_fMomentOfInertia = fSetMomentOfInertia;
}

//---------------------------------------
//  角度取得
//---------------------------------------
float CBody::GetMomentOfInertia(void)
{
	return m_fMomentOfInertia;
}
//void CBody::SetDrag(float fSetK1, float fSetK2)
//{
//	m_fK1 = fSetK1;
//	m_fK2 = fSetK2;
//}

//---------------------------------------
//  作用力をクリアする
//---------------------------------------
void CBody::ClearAccumulator(void)
{
	m_vForceAccum = VEC3_NULL;
	m_vRotationForceAccum = VEC3_NULL;
}

//---------------------------------------
//  作用力を足す
//---------------------------------------
void CBody::AddForce(D3DXVECTOR3 vForce)
{
	m_vForceAccum += vForce;
}

void CBody::AddForce(D3DXVECTOR3 vForce, D3DXVECTOR3 vPos)
{
	float fArmOfForce = Vector3Magnitude(vPos - m_vPositionCenter);
	D3DXVECTOR3 vMomentOfForce = vForce * fArmOfForce;
	m_vRotationForceAccum += (vMomentOfForce / m_fMomentOfInertia);
	//m_vForceAccum += vForce;

}

//---------------------------------------
//  重力更新
//---------------------------------------
void CBody::UpdateGravity(void)
{
	AddForce(m_vGravity * GetMass() * DURATION);
}

////---------------------------------------
////  抵抗力更新
////---------------------------------------
//void CBody::UpdateDrag(void)
//{
//	D3DXVECTOR3 vForce = GetVelocity();
//
//	//抵抗力係数計算
//	float fDragCoeff = Vector3Magnitude(vForce);//速度のスカラ値
//	fDragCoeff = m_fK1 * fDragCoeff + m_fK2 * fDragCoeff * fDragCoeff;
//
//	//抵抗力計算
//	Vector3Normalize(&vForce);
//	vForce *= -fDragCoeff;
//
//	//抵抗力を作用力に加算
//	AddForce(vForce);
//}

//--------------------------------------------------------------------------------
//  BodyCicle
//--------------------------------------------------------------------------------
//---------------------------------------
//  コンストラクタ
//---------------------------------------
CBodyCicle::CBodyCicle()
{
	m_fRadius = 0.0f;
}

//---------------------------------------
//  デストラクタ
//---------------------------------------
CBodyCicle::~CBodyCicle()
{
	m_fRadius = 0.0f;
}

//---------------------------------------
//  初期化
//---------------------------------------
void CBodyCicle::Init(void)
{
	CBody::Init();
	m_fRadius = 0.0f;
}

//---------------------------------------
//  終了処理
//---------------------------------------
void CBodyCicle::Uninit(void)
{
	CBody::Uninit();
	m_fRadius = 0.0f;
}

//---------------------------------------
//  更新処理
//---------------------------------------
void CBodyCicle::Update(void)
{
	CBody::Update();
}

//---------------------------------------
//  半径設定
//---------------------------------------
void CBodyCicle::SetRadius(float fSetRadius)
{
	m_fRadius = fSetRadius;
}

//---------------------------------------
//  半径取得
//---------------------------------------
float CBodyCicle::GetRadius(void)
{
	return m_fRadius;
}

//--------------------------------------------------------------------------------
//  BodyBox
//--------------------------------------------------------------------------------
//---------------------------------------
//  コンストラクタ
//---------------------------------------
CBodyBox::CBodyBox()
{
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aVertex[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aSide[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	m_vHalfSize = D3DXVECTOR2(0.0f, 0.0f);
}

//---------------------------------------
//  デストラクタ
//---------------------------------------
CBodyBox::~CBodyBox()
{
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aVertex[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aSide[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	m_vHalfSize = D3DXVECTOR2(0.0f, 0.0f);
}

//---------------------------------------
//  初期化
//---------------------------------------
void CBodyBox::Init(void)
{
	CBody::Init();
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aVertex[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aVertexOld[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aSide[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	m_vHalfSize = D3DXVECTOR2(0.0f, 0.0f);
}

//---------------------------------------
//  終了処理
//---------------------------------------
void CBodyBox::Uninit(void)
{
	CBody::Uninit();
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aVertex[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aVertexOld[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aSide[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	m_vHalfSize = D3DXVECTOR2(0.0f, 0.0f);
}

//---------------------------------------
//  更新処理
//---------------------------------------
void CBodyBox::Update(void)
{
	CBody::Update();

	UpdateVertex();
	UpdateSide();
}

//---------------------------------------
//  BoxSize設定
//---------------------------------------
void CBodyBox::SetHalfSize(D3DXVECTOR2 vSetHalfSize)
{
	m_vHalfSize = vSetHalfSize;
	m_fLength = sqrtf(m_vHalfSize.x * m_vHalfSize.x + m_vHalfSize.y * m_vHalfSize.y);
	m_fAngle = atan2(m_vHalfSize.y, m_vHalfSize.x);
	InitVertex();
	UpdateSide();
}

//---------------------------------------
//  BoxSize取得
//---------------------------------------
D3DXVECTOR2 CBodyBox::GetHalfSize(void)
{
	return m_vHalfSize;
}

//---------------------------------------
//  頂点初期化
//---------------------------------------
void CBodyBox::InitVertex(void)
{
	D3DXVECTOR3 vPosCenter = CBody::GetPosCenter();

	//pos
	m_aVertex[0] = D3DXVECTOR3(vPosCenter.x - m_vHalfSize.x, vPosCenter.y - m_vHalfSize.y, 0.0f);
	m_aVertex[1] = D3DXVECTOR3(vPosCenter.x + m_vHalfSize.x, vPosCenter.y - m_vHalfSize.y, 0.0f);
	m_aVertex[2] = D3DXVECTOR3(vPosCenter.x + m_vHalfSize.x, vPosCenter.y + m_vHalfSize.y, 0.0f);
	m_aVertex[3] = D3DXVECTOR3(vPosCenter.x - m_vHalfSize.x, vPosCenter.y + m_vHalfSize.y, 0.0f);

}

//---------------------------------------
//  頂点更新
//---------------------------------------
void CBodyBox::UpdateVertex(void)
{
	float fRotation = CBody::GetRotation();

	////rotation
	//for (nCnt = 0; nCnt < 4; nCnt++)
	//{
	//	m_aVertex[nCnt] = D3DXVECTOR3(
	//		cosf(fRotation) * m_aVertex[nCnt].x - sinf(fRotation) * m_aVertex[nCnt].y,
	//		sinf(fRotation) * m_aVertex[nCnt].x + cosf(fRotation) * m_aVertex[nCnt].y,
	//		0.0f);
	//}

	//D3DXVECTOR3 vMovement = CBody::GetMovement();
	//CBody::SetMovement(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	////pos
	//m_aVertex[0] += vMovement;
	//m_aVertex[1] += vMovement;
	//m_aVertex[2] += vMovement;
	//m_aVertex[3] += vMovement;

	D3DXVECTOR3 vPos = CBody::GetPosCenter();

	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aVertexOld[nCnt] = m_aVertex[nCnt];
	}

	m_aVertex[0] = D3DXVECTOR3(vPos.x + cosf(fRotation + D3DX_PI + m_fAngle) * m_fLength, vPos.y + sin(fRotation + D3DX_PI + m_fAngle) * m_fLength, 0.0f);
	m_aVertex[1] = D3DXVECTOR3(vPos.x + cosf(fRotation - m_fAngle) * m_fLength, vPos.y + sinf(fRotation - m_fAngle) * m_fLength, 0.0f);
	m_aVertex[2] = D3DXVECTOR3(vPos.x + cosf(fRotation + m_fAngle) * m_fLength, vPos.y + sinf(fRotation + m_fAngle) * m_fLength, 0.0f);
	m_aVertex[3] = D3DXVECTOR3(vPos.x + cosf(fRotation - D3DX_PI - m_fAngle) * m_fLength, vPos.y + sinf(fRotation - D3DX_PI - m_fAngle) * m_fLength, 0.0f);
}

//---------------------------------------
//  四辺更新
//---------------------------------------
void CBodyBox::UpdateSide(void)
{
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aSide[nCnt] = m_aVertex[(nCnt + 1) % 4] - m_aVertex[nCnt];
		D3DXVec3Normalize(&m_aSide[nCnt], &m_aSide[nCnt]);
	}
}

//---------------------------------------
//  頂点取得
//---------------------------------------
D3DXVECTOR3 CBodyBox::GetVertex(int nNumVertex)
{
	if (nNumVertex < 0 || nNumVertex > 3) 
	{ 
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f); 
	}

	return m_aVertex[nNumVertex];
}

//---------------------------------------
//  前フレーム頂点取得
//---------------------------------------
D3DXVECTOR3 CBodyBox::GetVertexOld(int nNumVertex)
{
	if (nNumVertex < 0 || nNumVertex > 3)
	{
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return m_aVertexOld[nNumVertex];
}

//---------------------------------------
//  四辺取得
//---------------------------------------
D3DXVECTOR3 CBodyBox::GetSide(int nNumSide)
{
	if (nNumSide < 0 || nNumSide > 3) 
	{
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return m_aSide[nNumSide];
}

//---------------------------------------
//  角度設定
//---------------------------------------
void CBodyBox::SetAngle(float fAngle)
{
	m_fAngle = fAngle;
}

//---------------------------------------
//  角度取得
//---------------------------------------
float CBodyBox::GetAngle(void)
{
	return m_fAngle;
}

//---------------------------------------
//  対角線取得
//---------------------------------------
float CBodyBox::GetLength(void)
{
	return m_fLength;
}

//--------------------------------------------------------------------------------
//  BodyQuadrangle
//--------------------------------------------------------------------------------
//---------------------------------------
//  コンストラクタ
//---------------------------------------
CBodyQuadrangle::CBodyQuadrangle()
{
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aVertex[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aSide[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
}

//---------------------------------------
//  デストラクタ
//---------------------------------------
CBodyQuadrangle::~CBodyQuadrangle()
{
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aVertex[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aSide[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
}

//---------------------------------------
//  初期化
//---------------------------------------
void CBodyQuadrangle::Init(void)
{
	CBody::Init();
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aVertex[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aSide[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
}

//---------------------------------------
//  終了処理
//---------------------------------------
void CBodyQuadrangle::Uninit(void)
{
	CBody::Uninit();
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aVertex[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_aSide[nCnt] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
}

//---------------------------------------
//  更新処理
//---------------------------------------
void CBodyQuadrangle::Update(void)
{
	CBody::Update();

	UpdateSide();
}

//---------------------------------------
//  四辺更新
//---------------------------------------
void CBodyQuadrangle::UpdateSide(void)
{
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_aSide[nCnt] = m_aVertex[(nCnt + 1) % 4] - m_aVertex[nCnt];
		D3DXVec3Normalize(&m_aSide[nCnt], &m_aSide[nCnt]);
	}
}

//---------------------------------------
//  頂点設定
//---------------------------------------
void CBodyQuadrangle::SetVertex(int nNumVertex, D3DXVECTOR3 vSetVertex)
{
	if (nNumVertex < 0 || nNumVertex > 3) { return; }

	m_aVertex[nNumVertex] = vSetVertex;
}

//---------------------------------------
//  頂点取得
//---------------------------------------
D3DXVECTOR3 CBodyQuadrangle::GetVertex(int nNumVertex)
{
	if (nNumVertex < 0 || nNumVertex > 3)
	{
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return m_aVertex[nNumVertex];
}

//---------------------------------------
//  四辺取得
//---------------------------------------
D3DXVECTOR3 CBodyQuadrangle::GetSide(int nNumSide)
{
	if (nNumSide < 0 || nNumSide > 3)
	{
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return m_aSide[nNumSide];
}