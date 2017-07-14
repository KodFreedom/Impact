//--------------------------------------------------------------------------------
//	粒子演算ヘッダー
//　KF_Particle.h
//	Author : Xu Wenjie
//	Date   : 2016-07-25
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _KF_PARTICLE_H_
#define _KF_PARTICLE_H_

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
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct {
	D3DXVECTOR3 vPosition;//粒子の位置
	D3DXVECTOR3 vVelocity;//粒子の速度
	D3DXVECTOR3 vAcceleration;//粒子の加速度
	D3DXVECTOR3 vGravity;//重力
	D3DXVECTOR3 vForceAccum;//合わせた作用力
	float fInverseMass;//質量の逆数
	float fK1;//抵抗力係数(速度)
	float fK2;//抵抗力係数(平方速度)
	bool bUse;
}KF_PARTICLE;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);
void SetParticle(float rSetInverseMass, D3DXVECTOR3 vSetPosition, D3DXVECTOR3 vSetVelocity, D3DXVECTOR3 vSetAcceleration);
void AddForce(int nNumParticle, D3DXVECTOR3 vForce);//作用力を増加する
#endif