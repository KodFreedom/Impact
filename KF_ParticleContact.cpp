//--------------------------------------------------------------------------------
//	���q�Փˌv�Z�w�b�_�[
//�@KF_ParticleContact.cpp
//	Author : Xu Wenjie
//	Date   : 2016-07-27
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "KF_ParticleContact.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define NUM_PARTICLE_CONTACT (100)
#define RESTITUTION (0.8f)//��e�W��
#define ITERATION_MAX (300)//�Փˏ����ő���s��
#define GROUND_POS (0.0f)
#define CONTACT_NORMAL_GROUND (D3DXVECTOR3(0.0f,1.0f,0.0f))

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
void ResolveContacts(void);
void Resolve(int nNumContact);
void ResolveVelocity(int nNumContact);
void ResolveInterpenetration(int nNumContact);
float CalculateSeparatingVelocity(int nNumContact);
void ClearParticleContact(int nNumContact);

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
KF_PARTICLE_CONTACT g_aPContact[NUM_PARTICLE_CONTACT];
int g_nNumContacts;

//--------------------------------------------------------------------------------
//  ������
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
//  �I������
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
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateParticleContact(void)
{
	ResolveContacts();
	g_nNumContacts = 0;
}

//--------------------------------------------------------------------------------
//  ���������Փˏ���
//--------------------------------------------------------------------------------
void ResolveContacts(void)
{
	int nCnt;
	//���s��
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
//  �f�X�g���N�^
//--------------------------------------------------------------------------------
void Resolve(int nNumContact)
{
	ResolveVelocity(nNumContact);
	ResolveInterpenetration(nNumContact);
	ClearParticleContact(nNumContact);
}

//--------------------------------------------------------------------------------
//  ���x�v�Z
//--------------------------------------------------------------------------------
void ResolveVelocity(int nNumContact)
{
	//�������x�v�Z
	float fSeparatingVelocity = CalculateSeparatingVelocity(nNumContact);

	//�������x�`�F�b�N
	if (fSeparatingVelocity > 0)
	{
		return;
	}

	//�Ȃ��̏ꍇ�v�Z������
	float fNewSeparatingVelocity = -fSeparatingVelocity * RESTITUTION;

	//�ÓI�Փˏ���
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

	//���x�����v�Z
	float fDeltaVelocity = fNewSeparatingVelocity - fSeparatingVelocity;

	//���q�̎��ʎ擾
	float fTotalInverseMass = g_aPContact[nNumContact].apParticle[0]->fInverseMass;
	if (g_aPContact[nNumContact].apParticle[1])
	{
		fTotalInverseMass += g_aPContact[nNumContact].apParticle[1]->fInverseMass;
	}

	//���ʂ�������̏ꍇ�v�Z���Ȃ�
	if (fTotalInverseMass <= 0) { return; }

	//�Փ˗͌v�Z
	float fImpulse = fDeltaVelocity / fTotalInverseMass;

	//�P�ʋt���ʂ̏Փ˗�
	D3DXVECTOR3 vImpulsePerInverseMass = g_aPContact[nNumContact].vContactNormal * fImpulse;

	//���x�v�Z
	g_aPContact[nNumContact].apParticle[0]->vVelocity =
		g_aPContact[nNumContact].apParticle[0]->vVelocity + vImpulsePerInverseMass * g_aPContact[nNumContact].apParticle[0]->fInverseMass;

	if (g_aPContact[nNumContact].apParticle[1])
	{
		g_aPContact[nNumContact].apParticle[1]->vVelocity =
			g_aPContact[nNumContact].apParticle[1]->vVelocity + vImpulsePerInverseMass * g_aPContact[nNumContact].apParticle[1]->fInverseMass;
	}
}

//--------------------------------------------------------------------------------
//  �Փˌv�Z
//--------------------------------------------------------------------------------
void ResolveInterpenetration(int nNumContact)
{
	//�Փ˂��Ȃ�
	if (g_aPContact[nNumContact].fPenetration <= 0) { return; }

	//�t���ʌv�Z
	float fTotalInverseMass = g_aPContact[nNumContact].apParticle[0]->fInverseMass;
	if (g_aPContact[nNumContact].apParticle[1])
	{
		fTotalInverseMass += g_aPContact[nNumContact].apParticle[1]->fInverseMass;
	}

	//���ʂ�������̏ꍇ�v�Z���Ȃ�
	if (fTotalInverseMass <= 0) { return; }

	//���ʒP�ʖ߂�ʌv�Z
	D3DXVECTOR3 vMovePerInverseMass = g_aPContact[nNumContact].vContactNormal * (-g_aPContact[nNumContact].fPenetration / fTotalInverseMass);

	//�e���q�߂�ʒu�v�Z
	g_aPContact[nNumContact].apParticle[0]->vPosition = g_aPContact[nNumContact].apParticle[0]->vPosition +
		vMovePerInverseMass * g_aPContact[nNumContact].apParticle[0]->fInverseMass;

	if (g_aPContact[nNumContact].apParticle[1])
	{
		g_aPContact[nNumContact].apParticle[1]->vPosition = g_aPContact[nNumContact].apParticle[1]->vPosition +
			vMovePerInverseMass * g_aPContact[nNumContact].apParticle[1]->fInverseMass;
	}
}

//--------------------------------------------------------------------------------
//  �������x�v�Z
//--------------------------------------------------------------------------------
float CalculateSeparatingVelocity(int nNumContact)
{
	if (!g_aPContact[nNumContact].apParticle[0])
	{
		return 0.0f;
	}

	D3DXVECTOR3 vRelativeVelocity = g_aPContact[nNumContact].apParticle[0]->vVelocity;

	//���q�Ɨ��q�̏Փ�
	if (g_aPContact[nNumContact].apParticle[1])
	{
		vRelativeVelocity -= g_aPContact[nNumContact].apParticle[1]->vVelocity;
	}

	//���όv�Z
	return ScalarProduct(vRelativeVelocity, g_aPContact[nNumContact].vContactNormal);
}

//--------------------------------------------------------------------------------
//  �Փːݒ�
//--------------------------------------------------------------------------------
void SetParticleContact(KF_PARTICLE *pParticle1, KF_PARTICLE *pParticle2, float fSetPenetration, D3DXVECTOR3 vSetContactNormal)
{
	if (g_nNumContacts < NUM_PARTICLE_CONTACT)
	{
		g_aPContact[g_nNumContacts].apParticle[0] = pParticle1;
		g_aPContact[g_nNumContacts].apParticle[1] = pParticle2;
		g_aPContact[g_nNumContacts].fPenetration = fSetPenetration;
		g_aPContact[g_nNumContacts].vContactNormal = vSetContactNormal;
		g_nNumContacts++;//�Փˉ񐔃J�E���g
	}
}

//--------------------------------------------------------------------------------
//  �Փ˃N���A����
//--------------------------------------------------------------------------------
void ClearParticleContact(int nNumContact)
{
	g_aPContact[nNumContact].apParticle[0] = NULL;
	g_aPContact[nNumContact].apParticle[1] = NULL;
	g_aPContact[nNumContact].fPenetration = 0.0f;
	g_aPContact[nNumContact].vContactNormal = { 0.0f,0.0f,0.0f };
}

//--------------------------------------------------------------------------------
//  �n�ʏՓˏ���
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