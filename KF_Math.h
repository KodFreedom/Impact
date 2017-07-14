//--------------------------------------------------------------------------------
//	物理演算用計算式ヘッダー
//　KF_Math.h
//	Author : Xu Wenjie
//	Date   : 2016-07-24
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _KF_MATH_H_
#define _KF_MATH_H_

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include <math.h>
#include "main.h"
#include "KF_Precision.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  クラス定義
//--------------------------------------------------------------------------------
//class CKFVector3
//{
//	//ベクトル計算
//	void operator*=(const real rValue);
//	CKFVector3 operator*(const real rValue) const;
//
//	void operator+=(const CKFVector3 &vValue);
//	CKFVector3 operator+(const CKFVector3 &vValue) const;
//
//	void operator-=(const CKFVector3 &vValue);
//	CKFVector3 operator-(const CKFVector3 &vValue) const;
//
//	void componentProductUpdate(const CKFVector3 &vVector);
//	CKFVector3 componentProduct(const CKFVector3 &vVector) const;
//
//	real scalarProduct(const CKFVector3 &vVector) const;
//
//	CKFVector3 vectorProduct3D(const CKFVector3 &vVector) const;
//	void vectorProductUpdate3D(const CKFVector3 &vVector);
//
//	real vectorProduct2D(const CKFVector3 &vVector) const;
//
//private:
//	real rPad;//3bitの値を4bitにするための変数(処理速度を上げるため)
//};

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void Vector3Invert(D3DXVECTOR3 *pVector);
void Vector3Clear(D3DXVECTOR3 *pVector);

//ベクトル規範化
float Vector3Magnitude(D3DXVECTOR3 vVector);
float Vector3SquareMagnitude(D3DXVECTOR3 vVector);
void Vector3Normalize(D3DXVECTOR3 *pVector);
float ScalarProduct(D3DXVECTOR3 vVector1, D3DXVECTOR3 vVector2);
float Vector3Cross2D(D3DXVECTOR3 vVector1, D3DXVECTOR3 vVector2);
//D3DXVECTOR3 Vector3Cross3D(D3DXVECTOR3 vVector1, D3DXVECTOR3 vVector2);
D3DXVECTOR3 VerticalVector3(D3DXVECTOR3 vVector);
#endif