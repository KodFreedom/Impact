//--------------------------------------------------------------------------------
//	物理演算用計算式ヘッダー
//　KF_Math.cpp
//	Author : Xu Wenjie
//	Date   : 2016-07-24
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "KF_Math.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  クラス定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  正負転換
//--------------------------------------------------------------------------------
void Vector3Invert(D3DXVECTOR3 *pVector)
{
	pVector->x = -pVector->x;
	pVector->y = -pVector->y;
	pVector->z = -pVector->z;
}

//--------------------------------------------------------------------------------
//  値を初期化する
//--------------------------------------------------------------------------------
void Vector3Clear(D3DXVECTOR3 *pVector)
{
	pVector->x = 0.0f;
	pVector->y = 0.0f;
	pVector->z = 0.0f;
}

//--------------------------------------------------------------------------------
//  ベクトルの模
//--------------------------------------------------------------------------------
float Vector3Magnitude(D3DXVECTOR3 vVector)
{
	return sqrtf(vVector.x * vVector.x + vVector.y * vVector.y + vVector.z * vVector.z);
}

//--------------------------------------------------------------------------------
//  ベクトルの模(平方)
//--------------------------------------------------------------------------------
float Vector3SquareMagnitude(D3DXVECTOR3 vVector)
{
	return (vVector.x * vVector.x + vVector.y * vVector.y + vVector.z * vVector.z);
}

//--------------------------------------------------------------------------------
//  規範化
//--------------------------------------------------------------------------------
void Vector3Normalize(D3DXVECTOR3 *pVector)
{
	float fWork = Vector3Magnitude(*pVector);

	if (fWork > 0)
	{
		//(*this) *= ((real)rWork) / rWork;
		pVector->x /= fWork;
		pVector->y /= fWork;
		pVector->z /= fWork;
	}
}

//--------------------------------------------------------------------------------
//  ベクトルとベクトルの乗算(結果はスカラ)(内積)
//--------------------------------------------------------------------------------
float ScalarProduct(D3DXVECTOR3 vVector1, D3DXVECTOR3 vVector2)
{
	return (vVector1.x * vVector2.x + vVector1.y * vVector2.y + vVector1.z * vVector2.z);
}

//--------------------------------------------------------------------------------
//  ベクトルとベクトルの乗算(結果はスカラ)(2D外積)
//--------------------------------------------------------------------------------
float Vector3Cross2D(D3DXVECTOR3 vVector1, D3DXVECTOR3 vVector2)
{
	return (vVector1.x * vVector2.y - vVector1.y * vVector2.x);
}

//--------------------------------------------------------------------------------
//  ベクトルとベクトルの乗算(3D外積)
//--------------------------------------------------------------------------------
//D3DXVECTOR3 Vector3Cross3D(D3DXVECTOR3 vVector1, D3DXVECTOR3 vVector2)
//{
//}

//--------------------------------------------------------------------------------
//  ベクトルとの垂直のベクトルの計算
//--------------------------------------------------------------------------------
D3DXVECTOR3 VerticalVector3(D3DXVECTOR3 vVector)
{
	vVector = D3DXVECTOR3(
		cosf(D3DX_PI * 0.5f) * vVector.x - sinf(D3DX_PI * 0.5f) * vVector.y,
		sinf(D3DX_PI * 0.5f) * vVector.x + cosf(D3DX_PI * 0.5f) * vVector.y,
		0.0f);

	D3DXVec3Normalize(&vVector,&vVector);

	return vVector;
}
//
////--------------------------------------------------------------------------------
////  スカラとベクトルの乗算
////--------------------------------------------------------------------------------
//void CKFVector3::operator*=(const real rValue)
//{
//	m_rX *= rValue;
//	m_rY *= rValue;
//	m_rZ *= rValue;
//}
//
//CKFVector3 CKFVector3::operator*(const real rValue) const
//{
//	return CKFVector3(m_rX * rValue, m_rY * rValue, m_rZ * rValue);
//}
//
////--------------------------------------------------------------------------------
////  ベクトルとベクトルの加算
////--------------------------------------------------------------------------------
//void CKFVector3::operator+=(const CKFVector3 &vValue)
//{
//	m_rX += vValue.m_rX;
//	m_rY += vValue.m_rY;
//	m_rZ += vValue.m_rZ;
//}
//
//CKFVector3 CKFVector3::operator+(const CKFVector3 &vValue) const
//{
//	return CKFVector3(m_rX + vValue.m_rX, m_rY + vValue.m_rY, m_rZ + vValue.m_rZ);
//}
//
////--------------------------------------------------------------------------------
////  ベクトルとベクトルの減算
////--------------------------------------------------------------------------------
//void CKFVector3::operator-=(const CKFVector3 &vValue)
//{
//	m_rX -= vValue.m_rX;
//	m_rY -= vValue.m_rY;
//	m_rZ -= vValue.m_rZ;
//}
//
//CKFVector3 CKFVector3::operator-(const CKFVector3 &vValue) const
//{
//	return CKFVector3(m_rX - vValue.m_rX, m_rY - vValue.m_rY, m_rZ - vValue.m_rZ);
//}
//
////--------------------------------------------------------------------------------
////  ベクトルとベクトルの各軸の乗算
////--------------------------------------------------------------------------------
//void CKFVector3::componentProductUpdate(const CKFVector3 &vVector)
//{
//	m_rX *= vVector.m_rX;
//	m_rY *= vVector.m_rY;
//	m_rZ *= vVector.m_rZ;
//}
//
//CKFVector3 CKFVector3::componentProduct(const CKFVector3 &vVector) const
//{
//	return CKFVector3(m_rX * vVector.m_rX, m_rY * vVector.m_rY, m_rZ * vVector.m_rZ);
//}
//

////--------------------------------------------------------------------------------
////  ベクトルとベクトルの乗算(結果はベクトル)(3D外積)
////--------------------------------------------------------------------------------
//void CKFVector3::vectorProductUpdate3D(const CKFVector3 &vVector)
//{
//	m_rX = m_rY * vVector.m_rZ - m_rZ * vVector.m_rY;
//	m_rY = m_rZ * vVector.m_rX - m_rX * vVector.m_rZ;
//	m_rZ = m_rX * vVector.m_rY - m_rY * vVector.m_rX;
//}
//
//CKFVector3 CKFVector3::vectorProduct3D(const CKFVector3 &vVector) const
//{
//	return CKFVector3(m_rY * vVector.m_rZ - m_rZ * vVector.m_rY,
//		m_rZ * vVector.m_rX - m_rX * vVector.m_rZ,
//		m_rX * vVector.m_rY - m_rY * vVector.m_rX);
//}
//
////--------------------------------------------------------------------------------
////  ベクトルとベクトルの乗算(結果はスカラ)(2D外積)
////--------------------------------------------------------------------------------
//real CKFVector3::vectorProduct2D(const CKFVector3 &vVector) const
//{
//	return (m_rX * vVector.m_rY - m_rY * vVector.m_rX);
//}