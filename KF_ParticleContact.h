//--------------------------------------------------------------------------------
//	���q�Փˌv�Z�w�b�_�[
//�@KF_ParticleContact.h
//	Author : Xu Wenjie
//	Date   : 2016-07-27
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _KF_PARTICLE_CONTACT_H_
#define _KF_PARTICLE_CONTACT_H_

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "KF_Particle.h"
//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
typedef struct {
	KF_PARTICLE *apParticle[2];//�Փ˂���̗��q
	D3DXVECTOR3 vContactNormal;//�Փ˖@��
	float fPenetration;//�Փː[�� �}�C�i�X�ɂȂ�Ƃ͏Փ˂��Ȃ�
}KF_PARTICLE_CONTACT;

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
void InitParticleContact(void);
void UninitParticleContact(void);
void UpdateParticleContact(void);
void SetParticleContact(KF_PARTICLE *pParticle1, KF_PARTICLE *pParticle2, float fSetPenetration, D3DXVECTOR3 vSetContactNormal);
void UpdateGroundContact(KF_PARTICLE *pParticle);
#endif