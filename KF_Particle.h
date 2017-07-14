//--------------------------------------------------------------------------------
//	���q���Z�w�b�_�[
//�@KF_Particle.h
//	Author : Xu Wenjie
//	Date   : 2016-07-25
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _KF_PARTICLE_H_
#define _KF_PARTICLE_H_

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "main.h"
#include "KF_Precision.h"
#include "KF_Math.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
//#define DAMPING_MAX ((float)0.0)//�ő��R��
//#define DAMPING_MIN ((float)0.995)//�ŏ���R��

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct {
	D3DXVECTOR3 vPosition;//���q�̈ʒu
	D3DXVECTOR3 vVelocity;//���q�̑��x
	D3DXVECTOR3 vAcceleration;//���q�̉����x
	D3DXVECTOR3 vGravity;//�d��
	D3DXVECTOR3 vForceAccum;//���킹����p��
	float fInverseMass;//���ʂ̋t��
	float fK1;//��R�͌W��(���x)
	float fK2;//��R�͌W��(�������x)
	bool bUse;
}KF_PARTICLE;

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
void InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);
void SetParticle(float rSetInverseMass, D3DXVECTOR3 vSetPosition, D3DXVECTOR3 vSetVelocity, D3DXVECTOR3 vSetAcceleration);
void AddForce(int nNumParticle, D3DXVECTOR3 vForce);//��p�͂𑝉�����
#endif