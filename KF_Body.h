//--------------------------------------------------------------------------------
//	衝突物ヘッダー
//　KF_Body.h
//	Author : Xu Wenjie
//	Date   : 2016-07-25
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _KF_BODY_H_
#define _KF_BODY_H_

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "main.h"
#include "KF_Precision.h"
#include "KF_Math.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
//#define DAMPING_MAX ((float)0.0)//最大抵抗力
//#define DAMPING_MIN ((float)0.995)//最小抵抗力

//--------------------------------------------------------------------------------
//  クラス定義
//--------------------------------------------------------------------------------
//class CBody
//{
//public:
//	typedef struct {
//		D3DXVECTOR3 vVertexPos;
//		D3DXVECTOR3 vLengthToCenter;
//		D3DXVECTOR3 vDirection;
//	}KF_VERTEX;
//
//	CBody();//コンストラクタ
//
//	//仮想関数
//	virtual ~CBody();//デストラクタ
//	virtual void Init(void);
//	virtual void Update(void);
//	virtual void Uninit(void);
//
//	//メンバー変数
//	void SetPosCenter(D3DXVECTOR3 vSetPosCenter);
//	D3DXVECTOR3 GetPosCenter(void);
//	void SetGravityCenter(D3DXVECTOR3 vGravityCenter);
//	D3DXVECTOR3 GetGravityCenter(void);
//	void SetVelocity(D3DXVECTOR3 vSetVelocity);
//	D3DXVECTOR3 GetVelocity(void);
//	void SetAcceleration(D3DXVECTOR3 vSetAcceleration);
//	D3DXVECTOR3 GetAcceleration(void);
//	void SetMass(float fMass);
//	float GetMass(void);
//	void SetGravity(D3DXVECTOR3 vSetGravity);
//	void SetDrag(float fSetK1, float fSetK2);
//	void ClearAccumulator(void);//毎フレーム作用力をクリアする
//	void AddForce(D3DXVECTOR3 vForce);//作用力を増加する
//
//	//作用力計算
//	void UpdateGravity(void);
//	void UpdateDrag(void);
//private:
//	D3DXVECTOR3 m_vPositionCenter;//中心位置
//	D3DXVECTOR3 m_vGravityCenter;//重心位置
//	D3DXVECTOR3 m_vVelocity;//速度
//	D3DXVECTOR3 m_vAcceleration;//粒子の加速度
//	D3DXVECTOR3 m_vGravity;//重力
//	D3DXVECTOR3 m_vForceAccum;//合わせた作用力
//	float m_fInverseMass;//質量の逆数
//	float m_fK1;//抵抗力係数(速度)
//	float m_fK2;//抵抗力係数(平方速度)
//};

class CBody
{
public:
	CBody();//コンストラクタ
	virtual ~CBody();//デストラクタ
	virtual void Init(void) = 0;
	virtual void Update(void) = 0;
	virtual void Uninit(void) = 0;

	//メンバー変数
	void SetPosCenter(D3DXVECTOR3 vSetPosCenter);
	D3DXVECTOR3 GetPosCenter(void);
	void SetGravityCenter(D3DXVECTOR3 vGravityCenter);
	D3DXVECTOR3 GetGravityCenter(void);
	void SetVelocity(D3DXVECTOR3 vSetVelocity);
	D3DXVECTOR3 GetVelocity(void);
	void SetMovement(D3DXVECTOR3 vSetMovement);
	D3DXVECTOR3 GetMovement(void);
	void SetAcceleration(D3DXVECTOR3 vSetAcceleration);
	D3DXVECTOR3 GetAcceleration(void);
	void SetMass(float fMass);
	float GetMass(void);
	void SetInverseMass(float fInverseMass);
	float GetInverseMass(void);
	void SetGravity(D3DXVECTOR3 vSetGravity);
	void SetDamping(float fSetDamping);
	void SetRotation(float fSetRotation);
	void SetRotation(D3DXVECTOR3 vSetRotation);
	float GetRotation(void);
	D3DXVECTOR3 GetRotationSpeed(void);
	void SetMomentOfInertia(float fSetMomentOfInertia);
	float GetMomentOfInertia(void);
	//void SetDrag(float fSetK1, float fSetK2);
	void ClearAccumulator(void);//毎フレーム作用力をクリアする
	void AddForce(D3DXVECTOR3 vForce);//作用力を増加する
	void AddForce(D3DXVECTOR3 vForce, D3DXVECTOR3 vPos);
	//作用力計算
	void UpdateGravity(void);
	//void UpdateDrag(void);
private:
	D3DXVECTOR3 m_vPositionCenter;//中心位置
	D3DXVECTOR3 m_vGravityCenter;//重心位置
	D3DXVECTOR3 m_vVelocity;//速度
	D3DXVECTOR3 m_vMovement;//1franeの移動量
	D3DXVECTOR3 m_vAcceleration;//粒子の加速度
	D3DXVECTOR3 m_vGravity;//重力
	D3DXVECTOR3 m_vForceAccum;//合わせた作用力
	D3DXVECTOR3 m_vRotation;
	D3DXVECTOR3 m_vRotationSpeed;
	D3DXVECTOR3 m_vAccRotation;
	D3DXVECTOR3 m_vRotationForceAccum;
	float m_fMomentOfInertia;
	float m_fInverseMass;//質量の逆数
	//float m_fRotation;
	float m_fDamping;
	//float m_fK1;//抵抗力係数(速度)
	//float m_fK2;//抵抗力係数(平方速度)
};

class CBodyCicle : public CBody
{
public:
	CBodyCicle();//コンストラクタ
	~CBodyCicle();//デストラクタ

	void Init(void);
	void Update(void);
	void Uninit(void);

	void SetRadius(float fSetRadius);
	float GetRadius(void);

private:
	float m_fRadius;
};

class CBodyBox : public CBody
{
public:
	CBodyBox();//コンストラクタ
	~CBodyBox();//デストラクタ

	void Init(void);
	void Update(void);
	void Uninit(void);

	void SetHalfSize(D3DXVECTOR2 vSetHalfSize);
	D3DXVECTOR2 GetHalfSize(void);
	D3DXVECTOR3 GetVertexOld(int nNumVertex);
	D3DXVECTOR3 GetVertex(int nNumVertex);
	D3DXVECTOR3 GetSide(int nNumSide);
	void SetAngle(float fAngle);
	float GetAngle(void);
	float GetLength(void);
private:
	D3DXVECTOR2 m_vHalfSize;//
	D3DXVECTOR3 m_aVertex[4];
	D3DXVECTOR3 m_aSide[4];
	D3DXVECTOR3 m_aVertexOld[4];
	float m_fAngle;
	float m_fLength;

	void InitVertex(void);
	void UpdateVertex(void);
	void UpdateSide(void);
};

class CBodyQuadrangle : public CBody
{
public:
	CBodyQuadrangle();//コンストラクタ
	~CBodyQuadrangle();//デストラクタ

	void Init(void);
	void Update(void);
	void Uninit(void);

	void SetVertex(int nNumVertex, D3DXVECTOR3 vSetVertex);
	D3DXVECTOR3 GetVertex(int nNumVertex);
	D3DXVECTOR3 GetSide(int nNumSide);
private:
	D3DXVECTOR3 m_aVertex[4];
	D3DXVECTOR3 m_aSide[4];

	void UpdateSide(void);
};

//class CBodyCicle : public CBody
//{
//public:
//	CBodyCicle();//コンストラクタ
//	~CBodyCicle();//デストラクタ
//
//	void Init(void);
//	void Update(void);
//	void Uninit(void);
//
//	void SetRadius(float fSetRadius);
//	float GetRadius(void);
//private:
//	float m_fRadius;
//};
//
//class CBodyBox : public CBody
//{
//public:
//	CBodyBox();//コンストラクタ
//	~CBodyBox();//デストラクタ
//
//	void Init(void);
//	void Update(void);
//	void Uninit(void);
//
//	void SetHalfSize(D3DXVECTOR2 vSetHalfSize);
//	D3DXVECTOR2 GetHalfSize(void);
//	void SetRotation(float fSetRotation);
//	float GetRotation(void);
//private:
//	D3DXVECTOR2 m_vHalfSize;//
//	float m_fRotation;
//	KF_VERTEX m_aVertex[4];
//
//	void InitVertex(void);
//	void UpdateVertex(void);
//};

//class CBodyTriangle : public CBody
//{
//public:
//	CBodyTriangle();//コンストラクタ
//	~CBodyTriangle();//デストラクタ
//
//	void Init(void);
//	void Update(void);
//	void Uninit(void);
//
//	void SetHalfSize(D3DXVECTOR2 vSetHalfSize);
//
//private:
//	D3DXVECTOR2 vHalfSize;//
//	float fAngle;
//	KF_VERTEX aVertex[3];
//};

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
#endif