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
#include "KF_BodyContact.h"
#include "sound.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define NUM_CONTACT (100)
#define ITERATION_MAX (300)//�Փˏ����ő���s��
#define GROUND_POS (GAME_AREA_BOTTOM)
#define LEFT_WALL_POS (GAME_AREA_LEFT)
#define RIGHT_WALL_POS (GAME_AREA_RIGHT)
#define TOP_WALL_POS (GAME_AREA_TOP)
#define CONTACT_NORMAL_GROUND (D3DXVECTOR3(0.0f,-1.0f,0.0f))
#define CONTACT_NORMAL_LEFT_WALL (D3DXVECTOR3(1.0f,0.0f,0.0f))
#define CONTACT_NORMAL_RIGHT_WALL (D3DXVECTOR3(-1.0f,0.0f,0.0f))
#define CONTACT_NORMAL_TOP_WALL (D3DXVECTOR3(0.0f,1.0f,0.0f))

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct {
	D3DXVECTOR3 vContactPos;
	D3DXVECTOR3 vContactNormal;
	float fDistance;
}CROSS_INFO;

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
void ResolveContacts(void);
void Resolve(int nNumContact);
void ResolveVelocity(int nNumContact);
void ResolveInterpenetration(int nNumContact);
float CalculateSeparatingVelocity(int nNumContact);
void ClearBodyContact(int nNumContact);
CROSS_INFO CheckIntersect(D3DXVECTOR3 vA, D3DXVECTOR3 vB, D3DXVECTOR3 vC, D3DXVECTOR3 vD);

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
KF_CONTACT g_aPContact[NUM_CONTACT];
int g_nNumContacts;
float g_fRestitution;

//--------------------------------------------------------------------------------
//  ������
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
//  �I������
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
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateBodyContact(void)
{
	ResolveContacts();
	g_nNumContacts = 0;
}

//--------------------------------------------------------------------------------
//  ���������Փˏ���
//--------------------------------------------------------------------------------
void ResolveContacts(void)
{
	//���s��
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
//  �f�X�g���N�^
//--------------------------------------------------------------------------------
void Resolve(int nNumContact)
{
	ResolveVelocity(nNumContact);
	ResolveInterpenetration(nNumContact);
	ClearBodyContact(nNumContact);
}

//--------------------------------------------------------------------------------
//  ���x�v�Z
//--------------------------------------------------------------------------------
void ResolveVelocity(int nNumContact)
{
	//�ϐ��錾
	D3DXVECTOR3 vAccCausedVelocity;//�Փ˕����̍�p��(�����x)
	D3DXVECTOR3 vImpulsePerInverseMass;//�P�ʋt���ʂ̏Փ˗�
	D3DXVECTOR3 vVelocity;//�ω��������x
	D3DXVECTOR3 vForce;//���x������̍�p��
	D3DXVECTOR3 vPosToContactPos;//���S�_�ƏՓ˓_�̃x�N�g��
	D3DXVECTOR3 vWork;//�v�Z�p�x�N�g��
	D3DXVECTOR3 vRotationForce;//��]��p��
	float fSeparatingVelocity;//�������x
	float fNewSeparatingVelocity;//�V�����������x
	float fAccCausedSeparatingVelocity;//�Փ˖@����̉����x
	float fDeltaVelocity;//���x�̕ω���
	float fTotalInverseMass;//���ʂ̑��a
	float fImpulse;//�Փ˗�
	float fWorldAngle;//�S�̓I�ɂƂ��Ă̊p�x

	//�������x�v�Z
	fSeparatingVelocity = CalculateSeparatingVelocity(nNumContact);

	//�������x�`�F�b�N
	//�Փ˖@���̋t�����ɂȂ�Όv�Z���Ȃ�
	if (fSeparatingVelocity > 0.0f) { return; }

	//�Ȃ��̏ꍇ�v�Z������
	fNewSeparatingVelocity = -fSeparatingVelocity * g_fRestitution;

	//�Փ˕����ɍ�p�͂��v�Z����
	vAccCausedVelocity = g_aPContact[nNumContact].apBody[0]->GetAcceleration();
	if (g_aPContact[nNumContact].apBody[1])
	{
		vAccCausedVelocity -= g_aPContact[nNumContact].apBody[1]->GetAcceleration();
	}
	fAccCausedSeparatingVelocity = ScalarProduct(vAccCausedVelocity, g_aPContact[nNumContact].vContactNormal);

	//�Փ˖@���̋t�����ɂȂ��
	if (fAccCausedSeparatingVelocity < 0.0f)
	{
		fNewSeparatingVelocity += g_fRestitution * fAccCausedSeparatingVelocity * 1.0f;
		if (fNewSeparatingVelocity < 0.0f) { fNewSeparatingVelocity = 0.0f; }
	}

	//���x�����v�Z
	fDeltaVelocity = fNewSeparatingVelocity - fSeparatingVelocity;

	//���q�̎��ʎ擾
	fTotalInverseMass = g_aPContact[nNumContact].apBody[0]->GetInverseMass();
	if (g_aPContact[nNumContact].apBody[1])
	{
		fTotalInverseMass += g_aPContact[nNumContact].apBody[1]->GetInverseMass();
	}

	//���ʂ�������̏ꍇ�v�Z���Ȃ�
	if (fTotalInverseMass <= 0) { return; }

	//�Փ˗͌v�Z
	fImpulse = fDeltaVelocity / fTotalInverseMass;

	//�P�ʋt���ʂ̏Փ˗�
	vImpulsePerInverseMass = g_aPContact[nNumContact].vContactNormal * fImpulse;

	//���x�v�Z
	vVelocity = vImpulsePerInverseMass * g_aPContact[nNumContact].apBody[0]->GetInverseMass();

	g_aPContact[nNumContact].apBody[0]->SetVelocity(g_aPContact[nNumContact].apBody[0]->GetVelocity() + vVelocity);

	if (g_aPContact[nNumContact].bRotation)
	{
		//�����x�͐�
		vForce = g_aPContact[nNumContact].apBody[0]->GetMass() * vVelocity / DURATION * ROTATION_DAMPING;

		//���S�_�ƏՓ˓_�̃x�N�g��
		vPosToContactPos = g_aPContact[nNumContact].apBody[0]->GetPosCenter() - g_aPContact[nNumContact].vContactPos;

		//�x�N�g�������E���ɂƂ��Ă̊p�x
		fWorldAngle = atan2f(vPosToContactPos.y, vPosToContactPos.x);

		//��]��p�͌v�Z
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
			//�����x�͐�
			vForce = g_aPContact[nNumContact].apBody[1]->GetMass() * vVelocity / DURATION * ROTATION_DAMPING;

			//���S�_�ƏՓ˓_�̃x�N�g��
			vPosToContactPos = g_aPContact[nNumContact].apBody[1]->GetPosCenter() - g_aPContact[nNumContact].vContactPos;

			//�x�N�g�������E���ɂƂ��Ă̊p�x
			fWorldAngle = atan2f(vPosToContactPos.y, vPosToContactPos.x);

			//��]��p�͌v�Z
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
//	//�ϐ��錾
//	D3DXVECTOR3 vAccCausedVelocity;//�Փ˕����̍�p��(�����x)
//	D3DXVECTOR3 vImpulsePerInverseMass;//�P�ʋt���ʂ̏Փ˗�
//	D3DXVECTOR3 vVelocity;//�ω��������x
//	D3DXVECTOR3 vForce;//���x������̍�p��
//	D3DXVECTOR3 vPosToContactPos;//���S�_�ƏՓ˓_�̃x�N�g��
//	D3DXVECTOR3 vWork;//�v�Z�p�x�N�g��
//	D3DXVECTOR3 vRotationForce;//��]��p��
//	float fSeparatingVelocity;//�������x
//	float fNewSeparatingVelocity;//�V�����������x
//	float fAccCausedSeparatingVelocity;//�Փ˖@����̉����x
//	float fDeltaVelocity;//���x�̕ω���
//	float fTotalInverseMass;//���ʂ̑��a
//	float fImpulse;//�Փ˗�
//	float fWorldAngle;//�S�̓I�ɂƂ��Ă̊p�x
//
//					  //�������x�v�Z
//	fSeparatingVelocity = CalculateSeparatingVelocity(nNumContact);
//
//	//�������x�`�F�b�N
//	//�Փ˖@���̋t�����ɂȂ�Όv�Z���Ȃ�
//	if (fSeparatingVelocity > 0.0f) { return; }
//
//	//�Ȃ��̏ꍇ�v�Z������
//	fNewSeparatingVelocity = -fSeparatingVelocity * RESTITUTION;
//
//	//�Փ˕����ɍ�p�͂��v�Z����
//	vAccCausedVelocity = g_aPContact[nNumContact].apBody[0]->GetAcceleration();
//	if (g_aPContact[nNumContact].apBody[1])
//	{
//		vAccCausedVelocity -= g_aPContact[nNumContact].apBody[1]->GetAcceleration();
//	}
//	fAccCausedSeparatingVelocity = ScalarProduct(vAccCausedVelocity, g_aPContact[nNumContact].vContactNormal);
//
//	//�Փ˖@���̋t�����ɂȂ��
//	if (fAccCausedSeparatingVelocity < 0.0f)
//	{
//		fNewSeparatingVelocity += RESTITUTION * fAccCausedSeparatingVelocity * 1.0f;
//		if (fNewSeparatingVelocity < 0.0f) { fNewSeparatingVelocity = 0.0f; }
//	}
//
//	//���x�����v�Z
//	fDeltaVelocity = fNewSeparatingVelocity - fSeparatingVelocity;
//
//	//���q�̎��ʎ擾
//	fTotalInverseMass = g_aPContact[nNumContact].apBody[0]->GetInverseMass();
//	if (g_aPContact[nNumContact].apBody[1])
//	{
//		fTotalInverseMass += g_aPContact[nNumContact].apBody[1]->GetInverseMass();
//	}
//
//	//���ʂ�������̏ꍇ�v�Z���Ȃ�
//	if (fTotalInverseMass <= 0) { return; }
//
//	//�Փ˗͌v�Z
//	fImpulse = fDeltaVelocity / fTotalInverseMass;
//
//	//�P�ʋt���ʂ̏Փ˗�
//	vImpulsePerInverseMass = g_aPContact[nNumContact].vContactNormal * fImpulse;
//
//	//���x�v�Z
//	vVelocity = vImpulsePerInverseMass * g_aPContact[nNumContact].apBody[0]->GetInverseMass();
//
//	g_aPContact[nNumContact].apBody[0]->SetVelocity(g_aPContact[nNumContact].apBody[0]->GetVelocity() + vVelocity);
//
//	if (g_aPContact[nNumContact].bRotation)
//	{
//		//�����x�͐�
//		vForce = g_aPContact[nNumContact].apBody[0]->GetMass() * vVelocity / DURATION * ROTATION_DAMPING;
//
//		//���S�_�ƏՓ˓_�̃x�N�g��
//		vPosToContactPos = g_aPContact[nNumContact].apBody[0]->GetPosCenter() - g_aPContact[nNumContact].vContactPos;
//
//		//�x�N�g�������E���ɂƂ��Ă̊p�x
//		fWorldAngle = atan2f(vPosToContactPos.y, vPosToContactPos.x);
//
//		//��]��p�͌v�Z
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
//			//�����x�͐�
//			vForce = g_aPContact[nNumContact].apBody[1]->GetMass() * vVelocity / DURATION * ROTATION_DAMPING;
//
//			//���S�_�ƏՓ˓_�̃x�N�g��
//			vPosToContactPos = g_aPContact[nNumContact].apBody[1]->GetPosCenter() - g_aPContact[nNumContact].vContactPos;
//
//			//�x�N�g�������E���ɂƂ��Ă̊p�x
//			fWorldAngle = atan2f(vPosToContactPos.y, vPosToContactPos.x);
//
//			//��]��p�͌v�Z
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
//  �Փˌv�Z
//--------------------------------------------------------------------------------
void ResolveInterpenetration(int nNumContact)
{
	//�Փ˂��Ȃ�
	if (g_aPContact[nNumContact].fPenetration <= 0) { return; }

	//�t���ʌv�Z
	float fTotalInverseMass = g_aPContact[nNumContact].apBody[0]->GetInverseMass();
	if (g_aPContact[nNumContact].apBody[1])
	{
		fTotalInverseMass += g_aPContact[nNumContact].apBody[1]->GetInverseMass();
	}

	//���ʂ�������̏ꍇ�v�Z���Ȃ�
	if (fTotalInverseMass <= 0) { return; }

	//���ʒP�ʖ߂�ʌv�Z
	D3DXVECTOR3 vMovePerInverseMass = g_aPContact[nNumContact].vContactNormal * (-g_aPContact[nNumContact].fPenetration / fTotalInverseMass);

	//�e���q�߂�ʒu�v�Z
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
//  �������x�v�Z
//--------------------------------------------------------------------------------
float CalculateSeparatingVelocity(int nNumContact)
{
	if (!g_aPContact[nNumContact].apBody[0])
	{
		return 0.0f;
	}

	D3DXVECTOR3 vRelativeVelocity = g_aPContact[nNumContact].apBody[0]->GetVelocity();

	//���q�Ɨ��q�̏Փ�
	if (g_aPContact[nNumContact].apBody[1])
	{
		vRelativeVelocity -= g_aPContact[nNumContact].apBody[1]->GetVelocity();
	}

	//���όv�Z
	return ScalarProduct(vRelativeVelocity, g_aPContact[nNumContact].vContactNormal);
}

//--------------------------------------------------------------------------------
//  �Փːݒ�
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
		g_nNumContacts++;//�Փˉ񐔃J�E���g
	}
}

//--------------------------------------------------------------------------------
//  �Փ˃N���A����
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
//  �Փˌ��o����
//--------------------------------------------------------------------------------
bool CheckContactBTB(CBodyBox *pBody1, CBodyBox *pBody2)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
	if (g_nNumContacts > NUM_CONTACT) { return false; }

	//�ϐ��錾
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

	//���_�Ǝl�ӂƂ̏Փ˂����o
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

				//�ӂ̊O�ɂ����
				if (fDistance < 0.0f) { break; }
				if (fDistance == 0.0f) { continue; }
				//if (fDistance == 0.0f) { bTransform = false; }

				//fDistance = -fDistance;

				//�Փː[���Ƃق��̕ӂƂ̏Փː[�����ׂ�
				//��Ԑ󂢂̂�􂢏o��
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
//	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
//	if (g_nNumContacts > NUM_CONTACT) { return; }
//
//	//�ϐ��錾
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
//	//���_�Ǝl�ӂƂ̏Փ˂����o
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
//				//�ӂƌ������Ȃ�
//				if (crossInfo.fDistance == 0.0f) { continue; }
//				//if (fDistance == 0.0f) { bTransform = false; }
//
//				//�Փː[���Ƃق��̕ӂƂ̏Փː[�����ׂ�
//				//��Ԑ[���̂�􂢏o��
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
//  �Փˌ��o����(�~�Ɖ~)
//--------------------------------------------------------------------------------
void CheckContactCTC(CBodyCicle *pBody1, CBodyCicle *pBody2)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
	if (g_nNumContacts > NUM_CONTACT) { return; }

	//�~�S�̋������v�Z����
	D3DXVECTOR3 vMidLine = pBody1->GetPosCenter() - pBody2->GetPosCenter();
	float fMidLineDistance = Vector3Magnitude(vMidLine);

	//�����Ɣ��a�̘a�Ɣ�ׂ�
	if (fMidLineDistance <= 0.0f || fMidLineDistance >= (pBody1->GetRadius() + pBody2->GetRadius()))
	{
		return;
	}

	//�Փ˖@�����v�Z����
	D3DXVECTOR3 vContactNormal = vMidLine / fMidLineDistance;

	//�Փ˕ۑ�
	SetBodyContact(
		false,
		pBody1,
		pBody2, 
		pBody1->GetRadius() + pBody2->GetRadius() - fMidLineDistance, 
		vContactNormal,
		VEC3_NULL);
}

//--------------------------------------------------------------------------------
//  �Փˌ��o����(circle to box)
//--------------------------------------------------------------------------------
void CheckContactCTB(CBodyCicle *pBody1, CBodyBox *pBody2)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	float fRadius = pBody1->GetRadius();//�~�̔��a���擾
	int nCnt = 0;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vSide = VEC3_NULL;
	D3DXVECTOR3 vNormal = VEC3_NULL;
	D3DXVECTOR3 vContactNormal = VEC3_NULL;
	D3DXVECTOR3 vCiclePos = pBody1->GetPosCenter();

	//�l�ӂƂ̏Փ˂����o
	for (nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody2->GetVertex(nCnt);
		vSide = pBody2->GetSide(nCnt);
		fDistance = -Vector3Cross2D(vSide, (vCiclePos - vVector));

		//�ӂ̊O�ɂ����
		if (fDistance >= 0.0f + fRadius) { break; }

		//�Փː[�x�̌v�Z
		fDistance = -fDistance + fRadius;

		//�Փː[���Ƃق��̕ӂƂ̏Փː[�����ׂ�
		//��Ԑ󂢂̂�􂢏o��
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
//  �Փˌ��o����(circle to box)
//--------------------------------------------------------------------------------
void CheckContactCTQ(CBodyCicle *pBody1, CBodyQuadrangle *pBody2)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	float fRadius = pBody1->GetRadius();//�~�̔��a���擾
	int nCnt = 0;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vSide = VEC3_NULL;
	D3DXVECTOR3 vNormal = VEC3_NULL;
	D3DXVECTOR3 vContactNormal = VEC3_NULL;
	D3DXVECTOR3 vCiclePos = pBody1->GetPosCenter();

	//�l�ӂƂ̏Փ˂����o
	for (nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody2->GetVertex(nCnt);
		vSide = pBody2->GetSide(nCnt);
		fDistance = -Vector3Cross2D(vSide, (vCiclePos - vVector));

		//�ӂ̊O�ɂ����
		if (fDistance >= 0.0f + fRadius)
		{
			break;
		}

		//�Փː[�x�̌v�Z
		fDistance = -fDistance + fRadius;

		//�Փː[���Ƃق��̕ӂƂ̏Փː[�����ׂ�
		//��Ԑ󂢂̂�􂢏o��
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
//  �G���A�Փˏ���
//--------------------------------------------------------------------------------
void AreaContact(CBody *pBody, float fRadius)
{
	UpdateGroundContact(pBody, fRadius);
	//UpdateLeftWallContact(pBody, fRadius);
	//UpdateRightWallContact(pBody, fRadius);
	UpdateTopWallContact(pBody, fRadius);
}

//--------------------------------------------------------------------------------
//  �n�ʏՓˏ���
//--------------------------------------------------------------------------------
void UpdateGroundContact(CBody *pBody, float fRadius)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
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
//  ���Փˏ���
//--------------------------------------------------------------------------------
void UpdateLeftWallContact(CBody *pBody, float fRadius)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
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
//  �E�Փˏ���
//--------------------------------------------------------------------------------
void UpdateRightWallContact(CBody *pBody, float fRadius)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
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
//  ��Փˏ���
//--------------------------------------------------------------------------------
void UpdateTopWallContact(CBody *pBody, float fRadius)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
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
//  ���̋�ԏՓ�
//--------------------------------------------------------------------------------
void BoxAreaContact(CBodyBox *pBody)
{
	BoxUpdateGroundContact(pBody);
	BoxUpdateLeftWallContact(pBody);
	BoxUpdateRightWallContact(pBody);
	BoxUpdateTopWallContact(pBody);
}

//--------------------------------------------------------------------------------
//  ���̋�ԏՓˁi�n�ʁj
//--------------------------------------------------------------------------------
void BoxUpdateGroundContact(CBodyBox *pBody)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vContactPos = VEC3_NULL;

	//�l�ӂƂ̏Փ˂����o
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody->GetVertex(nCnt);
		fDistance = vVector.y - GROUND_POS;

		//�ӂ̊O�ɂ����
		if (fDistance <= 0.0f) { continue; }

		//�Փː[���Ƃق��̕ӂƂ̏Փː[�����ׂ�
		//��Ԑ[���̂�􂢏o��
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
//  ���̋�ԏՓˁi���j
//--------------------------------------------------------------------------------
void BoxUpdateLeftWallContact(CBodyBox *pBody)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vContactPos = VEC3_NULL;

	//�l�ӂƂ̏Փ˂����o
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody->GetVertex(nCnt);
		fDistance = LEFT_WALL_POS - vVector.x;

		//�ӂ̊O�ɂ����
		if (fDistance <= 0.0f) { continue; }

		//�Փː[���Ƃق��̕ӂƂ̏Փː[�����ׂ�
		//��Ԑ[���̂�􂢏o��
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
//  ���̋�ԏՓˁi�E�j
//--------------------------------------------------------------------------------
void BoxUpdateRightWallContact(CBodyBox *pBody)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vContactPos = VEC3_NULL;

	//�l�ӂƂ̏Փ˂����o
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody->GetVertex(nCnt);
		fDistance = vVector.x - RIGHT_WALL_POS;

		//�ӂ̊O�ɂ����
		if (fDistance <= 0.0f) { continue; }

		//�Փː[���Ƃق��̕ӂƂ̏Փː[�����ׂ�
		//��Ԑ[���̂�􂢏o��
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
//  ���̋�ԏՓˁi��j
//--------------------------------------------------------------------------------
void BoxUpdateTopWallContact(CBodyBox *pBody)
{
	//�Փˉ񐔂��ő吔�ɒ�����Ό��o���Ȃ�
	if (g_nNumContacts > NUM_CONTACT) { return; }

	float fDistance = 0.0f;
	float fContactDistance = 0.0f;
	D3DXVECTOR3 vVector = VEC3_NULL;
	D3DXVECTOR3 vContactPos = VEC3_NULL;

	//�l�ӂƂ̏Փ˂����o
	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		vVector = pBody->GetVertex(nCnt);
		fDistance = TOP_WALL_POS - vVector.y;

		//�ӂ̊O�ɂ����
		if (fDistance <= 0.0f) { continue; }

		//�Փː[���Ƃق��̕ӂƂ̏Փː[�����ׂ�
		//��Ԑ[���̂�􂢏o��
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
//  ��AB�Ɛ�CD�̌��������߂�
//  �������Ȃ��̏ꍇ0��Ԃ��A�����̏ꍇ�_B�Ɛ�CD�̋����ƌ����_��Ԃ�
//--------------------------------------------------------------------------------
CROSS_INFO CheckIntersect(D3DXVECTOR3 vA, D3DXVECTOR3 vB, D3DXVECTOR3 vC, D3DXVECTOR3 vD)
{
	//�ϐ��錾
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

	//�x�N�g��CD�����߂�
	vCD = vD - vC;
	D3DXVec3Normalize(&vCD, &vCD);

	//�_A�ƃx�N�g��CD�̋��������߂�
	fDistanceA = Vector3Cross2D(vCD, (vA - vC));

	//�_B�ƃx�N�g��CD�̋��������߂�
	fDistanceB = Vector3Cross2D(vCD, (vB - vC));

	//�x�N�g��AB�����߂�
	vAB = vB - vA;
	D3DXVec3Normalize(&vAB, &vAB);

	//�_C�ƃx�N�g��CD�̋��������߂�
	fDistanceC = Vector3Cross2D(vAB, (vC - vA));

	//�_S�ƃx�N�g��CD�̋��������߂�
	fDistanceD = Vector3Cross2D(vAB, (vD - vA));

	//�_A�Ɠ_B�̓x�N�g��CD�̓������ɂ���ꍇ
	//�������͓_C�Ɠ_D�̓x�N�g��AB�̓������ɂ���ꍇ0��Ԃ�
	if (fDistanceA * fDistanceB >= 0.0f || fDistanceC * fDistanceD >= 0.0f)
	{
		crossInfo.fDistance = 0.0f;
		crossInfo.vContactPos = VEC3_NULL;
		crossInfo.vContactNormal = VEC3_NULL;
		return crossInfo;
	}

	//�Փː[��
	crossInfo.fDistance = fDistanceB;

	//�����������Z�o
	//����������y = ax + b�A�_A(x,y)�Ɠ_B(x,y)���킩��̏ꍇ
	//a = ( Ay - By ) / ( Ax - Bx )
	//b = ( AxBy - AyBx ) / ( Ax - Bx )
	//Ax = Bx�̎� ��������x = Ax�ɂȂ�
	//Ay = By�̎� ��������y = Ay�ɂȂ�
	//��AB�Z�o
	fWorkA = (vA.y - vB.y) / (vA.x - vB.x);
	fWorkB = (vA.x * vB.y - vA.y * vB.x) / (vA.x - vB.x);

	//��CD�Z�o
	fWorkC = (vC.y - vD.y) / (vC.x - vD.x);
	fWorkD = (vC.x * vD.y - vC.y * vD.x) / (vC.x - vD.x);

	//�����_���Z�o
	//������������y = ax + b, y = cx + b�̏ꍇ
	//x = ( d - b ) / ( a - c )
	//y = a * ( d - b ) / ( a - c ) + b 
	crossInfo.vContactPos.x = (fWorkD - fWorkB) / (fWorkA - fWorkC);
	crossInfo.vContactPos.y = fWorkA * (fWorkD - fWorkB) / (fWorkA - fWorkC) + fWorkB;
	crossInfo.vContactPos.z = 0.0f;

	//�Փ˖@���Z�o
	crossInfo.vContactNormal = -VerticalVector3(vCD);

	return crossInfo;
}

//--------------------------------------------------------------------------------
//  ��e�W����ݒ肷��ievent�p�j
//--------------------------------------------------------------------------------
void SetRestitution(float fRestitution)
{
	g_fRestitution = fRestitution;
}