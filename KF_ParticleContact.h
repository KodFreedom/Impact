//--------------------------------------------------------------------------------
//	粒子衝突計算ヘッダー
//　KF_ParticleContact.h
//	Author : Xu Wenjie
//	Date   : 2016-07-27
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _KF_PARTICLE_CONTACT_H_
#define _KF_PARTICLE_CONTACT_H_

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "KF_Particle.h"
//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
typedef struct {
	KF_PARTICLE *apParticle[2];//衝突するの粒子
	D3DXVECTOR3 vContactNormal;//衝突法線
	float fPenetration;//衝突深さ マイナスになるとは衝突しない
}KF_PARTICLE_CONTACT;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void InitParticleContact(void);
void UninitParticleContact(void);
void UpdateParticleContact(void);
void SetParticleContact(KF_PARTICLE *pParticle1, KF_PARTICLE *pParticle2, float fSetPenetration, D3DXVECTOR3 vSetContactNormal);
void UpdateGroundContact(KF_PARTICLE *pParticle);
#endif