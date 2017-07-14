//--------------------------------------------------------------------------------
//	���q���Z�w�b�_�[
//�@KF_Particle.h
//	Author : Xu Wenjie
//	Date   : 2016-07-25
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "KF_Particle.h"
#include "KF_ParticleContact.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define NUM_PARTICLE (100)//��p�͂̍ő吔
#define PARTICLE_LENGTH (10.0f)
#define PARTICLE_TEXTURENAME "data/TEXTURE/bullet.png"//�t�@�C����


//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexParticle(LPDIRECT3DDEVICE9 pDevice);
void UpdateIntegrate(int nNumParticle);
void ClearAccumulator(int nNumParticle);
void UpdateGravity(int nNumParticle);
void UpdateDrag(int nNumParticle);
//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureParticle = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferParticle = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
KF_PARTICLE g_aParticle[NUM_PARTICLE];

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾
	int nCnt;

	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		g_aParticle[nCnt].bUse = false;
		g_aParticle[nCnt].vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].vAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].vForceAccum = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].vGravity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].fInverseMass = 0.0f;
		g_aParticle[nCnt].fK1 = 0.0f;
		g_aParticle[nCnt].fK2 = 0.0f;
	}

	if (FAILED(MakeVerTexParticle(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexParticle ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		PARTICLE_TEXTURENAME,
		&g_pTextureParticle)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitParticle(void)
{
	if (g_pTextureParticle != NULL)
	{
		g_pTextureParticle->Release();
		g_pTextureParticle = NULL;
	}
	
	if (g_pVtxBufferParticle != NULL)
	{
		g_pVtxBufferParticle->Release();
		g_pVtxBufferParticle = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateParticle(void)
{
	VERTEX_2D *pVtx = NULL;
	int nCnt;

	g_pVtxBufferParticle->Lock(0, 0, (void**)&pVtx, 0);
	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		if (g_aParticle[nCnt].bUse)
		{
			UpdateGravity(nCnt);
			UpdateDrag(nCnt);
			
			//���x�v�Z
			UpdateIntegrate(nCnt);

			//�ʒu�v�Z
			g_aParticle[nCnt].vPosition = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x + g_aParticle[nCnt].vVelocity.x,
				g_aParticle[nCnt].vPosition.y + g_aParticle[nCnt].vVelocity.y,
				g_aParticle[nCnt].vPosition.z + g_aParticle[nCnt].vVelocity.z);

			ClearAccumulator(nCnt);

			//�Փˌv�Z�i�n�ʁj
			UpdateGroundContact(&g_aParticle[nCnt]);
		}
	}
	g_pVtxBufferParticle->Unlock();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾
	int nCnt;

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferParticle,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetTexture(0, g_pTextureParticle);//Texture�̐ݒ�

	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		//�v���~�e�B�u�`��
		if (g_aParticle[nCnt].bUse)
		{
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4, NUM_POLYGON);
		}
	}
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexParticle(LPDIRECT3DDEVICE9 pDevice)
{
	int nCnt;

	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NUM_PARTICLE,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferParticle,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferParticle->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		//���_���W�̐ݒ�i2D���W�A�E���j
		pVtx[nCnt * 4].pos = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x - PARTICLE_LENGTH, g_aParticle[nCnt].vPosition.y - PARTICLE_LENGTH, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x + PARTICLE_LENGTH, g_aParticle[nCnt].vPosition.y - PARTICLE_LENGTH, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x - PARTICLE_LENGTH, g_aParticle[nCnt].vPosition.y + PARTICLE_LENGTH, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x + PARTICLE_LENGTH, g_aParticle[nCnt].vPosition.y + PARTICLE_LENGTH, 0.0f);

		//rhw�̐ݒ�(�K��1.0f)
		pVtx[nCnt * 4].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[nCnt * 4].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		//texture���_
		pVtx[nCnt * 4].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferParticle->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  �ϕ��v�Z�i�����X�V�j
//
//  ����
//	rDuration�F�X�V�Ԋu
//--------------------------------------------------------------------------------
void UpdateIntegrate(int nNumParticle)
{
	//�v�Z�ς݉����x
	D3DXVECTOR3 vResultAcc = g_aParticle[nNumParticle].vAcceleration;

	//�ʒu�X�V
	g_aParticle[nNumParticle].vPosition += g_aParticle[nNumParticle].vVelocity * DURATION;

	//�͂ɂ���ĉ����x���X�V
	vResultAcc += g_aParticle[nNumParticle].vForceAccum * g_aParticle[nNumParticle].fInverseMass;

	//���x�X�V
	g_aParticle[nNumParticle].vVelocity += vResultAcc * DURATION;

	//��R��(�v�Z�Ԋu�ɂ���Ē�R�͂��ς��)
	//vVelocity *= REAL_POW(rDamping, rDuration);
}

//--------------------------------------------------------------------------------
//  ��p�͂��N���A����
//--------------------------------------------------------------------------------
void ClearAccumulator(int nNumParticle)
{
	g_aParticle[nNumParticle].vForceAccum.x = 0.0f;
	g_aParticle[nNumParticle].vForceAccum.y = 0.0f;
	g_aParticle[nNumParticle].vForceAccum.z = 0.0f;
}

//--------------------------------------------------------------------------------
//  ��p�͂𑝉�����
//--------------------------------------------------------------------------------
void AddForce(int nNumParticle, D3DXVECTOR3 vForce)
{
	g_aParticle[nNumParticle].vForceAccum += vForce;
}

//--------------------------------------------------------------------------------
//  �d�͍X�V
//--------------------------------------------------------------------------------
void UpdateGravity(int nNumParticle)
{
	//�d�͂���p�͂ɉ��Z
	AddForce(nNumParticle, g_aParticle[nNumParticle].vGravity / g_aParticle[nNumParticle].fInverseMass);
}

//--------------------------------------------------------------------------------
//  ��R�͍X�V
//--------------------------------------------------------------------------------
void UpdateDrag(int nNumParticle)
{
	D3DXVECTOR3 vForce = g_aParticle[nNumParticle].vVelocity;

	//��R�͌W���v�Z
	float fDragCoeff = Vector3Magnitude(vForce);//���x�̃X�J���l
	fDragCoeff = g_aParticle[nNumParticle].fK1 * fDragCoeff + g_aParticle[nNumParticle].fK2 * fDragCoeff * fDragCoeff;

	//��R�͌v�Z
	Vector3Normalize(&vForce);
	vForce *= -fDragCoeff;

	//��R�͂���p�͂ɉ��Z
	AddForce(nNumParticle, vForce);
}

//--------------------------------------------------------------------------------
//  ���q�ݒ�
//--------------------------------------------------------------------------------
void SetParticle(float rSetInverseMass, D3DXVECTOR3 vSetPosition, D3DXVECTOR3 vSetVelocity, D3DXVECTOR3 vSetAcceleration)
{
	int nCnt;

	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		if (g_aParticle[nCnt].bUse == false)
		{
			g_aParticle[nCnt].fInverseMass = rSetInverseMass;
			g_aParticle[nCnt].vPosition = vSetPosition;
			g_aParticle[nCnt].vVelocity = vSetVelocity;
			g_aParticle[nCnt].vAcceleration = vSetAcceleration;
			g_aParticle[nCnt].bUse = true;
			break;
		}
	}
}
