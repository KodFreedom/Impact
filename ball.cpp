//--------------------------------------------------------------------------------
//
//�@�E�C���h�E�\���v���O����
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include <time.h>
#include "ball.h"
#include "goal.h"
#include "effect.h"
#include "tire_effect.h"
#include "KF_Precision.h"
#include "KF_BodyContact.h"
#include "sound.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define BALL_TEXTURENAME "data/TEXTURE/ball.png"//�t�@�C����
#define BALL_COLOR_WHITE (D3DXCOLOR(1.0f,1.0f,1.0f,1.0f))
#define BALL_COLOR_BLACK (D3DXCOLOR(0.2f,0.2f,0.2f,1.0f))
#define BALL_COLOR_GOLDEN (D3DXCOLOR(1.0f,0.84f,0.0f,1.0f))
#define BALL_GOLDEN_RATE (5)//���F�̊m��
#define BALL_BLACK_RATE (4)//���F�̊m��
#define BALL_DAMPING (0.98f)

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexBall(LPDIRECT3DDEVICE9 pDevice);
void SetBallType(void);

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureBall = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferBall = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
CBodyCicle *g_pBall = NULL;
int g_nCntBall;
BALL_TYPE g_type;

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitBall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	srand((unsigned)time(NULL));

	g_pBall = new CBodyCicle;
	g_pBall->Init();
	g_pBall->SetRadius(BALL_RADIUS);
	g_pBall->SetPosCenter(BALL_POS);
	g_pBall->SetMass(BALL_MASS);
	g_pBall->SetGravity(GRAVITY);
	g_pBall->SetDamping(BALL_DAMPING);
	g_type = BALL_WHITE;

	g_nCntBall = 0;

	if (FAILED(MakeVerTexBall(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		BALL_TEXTURENAME,
		&g_pTextureBall)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitBall(void)
{
	if (g_pBall != NULL)//�A�h���X�̒��g�m�F
	{
		g_pBall->Uninit();//�I������
		delete g_pBall;//���g���폜
		g_pBall = NULL;//�A�h���X����ɂ���
	}

	if (g_pVtxBufferBall != NULL)
	{
		g_pVtxBufferBall->Release();
		g_pVtxBufferBall = NULL;
	}

	if (g_pTextureBall != NULL)
	{
		g_pTextureBall->Release();
		g_pTextureBall = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateBall(void)
{
	if (g_nCntBall != 0)
	{
		g_nCntBall--;
		if (g_nCntBall == 40)
		{
			SetRebornEffect(g_pBall->GetPosCenter());
		}
		return;
	}

	VERTEX_2D *pVtx = NULL;
	D3DXVECTOR3 vBallPos;
	D3DXCOLOR aColor[(int)BALL_MAX];

	aColor[(int)BALL_WHITE] = BALL_COLOR_WHITE;
	aColor[(int)BALL_BLACK] = BALL_COLOR_BLACK;
	aColor[(int)BALL_GOLDEN] = BALL_COLOR_GOLDEN;

	g_pBall->Update();
	AreaContact(g_pBall, BALL_RADIUS);

	g_pVtxBufferBall->Lock(0, 0, (void**)&pVtx, 0);

	vBallPos = g_pBall->GetPosCenter();

	//pos
	pVtx[0].pos = D3DXVECTOR3(vBallPos.x - BALL_RADIUS, vBallPos.y - BALL_RADIUS, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(vBallPos.x + BALL_RADIUS, vBallPos.y - BALL_RADIUS, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(vBallPos.x - BALL_RADIUS, vBallPos.y + BALL_RADIUS, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(vBallPos.x + BALL_RADIUS, vBallPos.y + BALL_RADIUS, 0.0f);

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[0].color = aColor[(int)g_type];
	pVtx[1].color = aColor[(int)g_type];
	pVtx[2].color = aColor[(int)g_type];
	pVtx[3].color = aColor[(int)g_type];

	g_pVtxBufferBall->Unlock();

	//effect
	if (Vector3Magnitude(g_pBall->GetVelocity()) > FLOAT_MIN)
	{
		SetTireEffect(g_pBall->GetPosCenter());
	}

	//goal
	if (GoalContact(g_pBall->GetPosCenter(),g_type))
	{
		PlaySound(SOUND_LABEL_SE_GOAL);
		SetGoalEffect(g_pBall->GetPosCenter());
		g_nCntBall = 200;
		g_pBall->SetPosCenter(BALL_POS);
		g_pBall->SetVelocity(VEC3_NULL);
		g_pBall->SetAcceleration(VEC3_NULL);
		g_pBall->ClearAccumulator();

		SetBallType();//�{�[���̃^�C�v�������_���Őݒ肷��

		switch (g_type)
		{
		case BALL_WHITE:
			g_pBall->SetMass(BALL_MASS);
			g_pBall->SetDamping(BALL_DAMPING);
			break;
		case BALL_BLACK:
			g_pBall->SetMass(BALL_MASS * 5.0f);
			g_pBall->SetDamping(BALL_DAMPING * BALL_DAMPING);
			break;
		case BALL_GOLDEN:
			g_pBall->SetMass(BALL_MASS);
			g_pBall->SetDamping(BALL_DAMPING);
			break;
		}
	}
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawBall(void)
{
	if (g_nCntBall != 0) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferBall,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

						   //���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	//Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureBall);

	//�v���~�e�B�u�`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//�I�t�Z�b�g�i���_���j
		NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexBall(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferBall,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;
	D3DXVECTOR3 vBallPos;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferBall->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	vBallPos = g_pBall->GetPosCenter();
	//���_���W�̐ݒ�i2D���W�A�E���j
	pVtx[0].pos = D3DXVECTOR3(vBallPos.x - BALL_RADIUS, vBallPos.y - BALL_RADIUS, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(vBallPos.x + BALL_RADIUS, vBallPos.y - BALL_RADIUS, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(vBallPos.x - BALL_RADIUS, vBallPos.y + BALL_RADIUS, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(vBallPos.x + BALL_RADIUS, vBallPos.y + BALL_RADIUS, 0.0f);

	//rhw�̐ݒ�(�K��1.0f)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[0].color = BALL_COLOR_WHITE;
	pVtx[1].color = BALL_COLOR_WHITE;
	pVtx[2].color = BALL_COLOR_WHITE;
	pVtx[3].color = BALL_COLOR_WHITE;

	//texture���_
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//���z�A�h���X���
	g_pVtxBufferBall->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  �{�[���̃^�C�v��ݒ肷��
//--------------------------------------------------------------------------------
void SetBallType(void)
{
	if (rand() % BALL_BLACK_RATE == 0)
	{
		g_type = BALL_BLACK;
		return;
	}

	if (rand() % BALL_GOLDEN_RATE == 0)
	{
		g_type = BALL_GOLDEN;
		return;
	}
	
	g_type = BALL_WHITE;
}
//--------------------------------------------------------------------------------
//  �{�[���Փˌ��o
//--------------------------------------------------------------------------------
//void UpdateBallContact(void)
//{
//	//check table contact
//	CBodyQuadrangle *pTable = NULL;
//
//	for (int nCntTable = 0;nCntTable < NUM_TABLE;nCntTable++)
//	{
//		pTable = GetTable(nCntTable);
//		CheckContactCTQ(g_pBall, pTable);
//		pTable = NULL;
//	}
//
//	AreaContact(&g_pBall->cBody, g_pBall->GetRadius());
//
//	//check contact
//	CBodyCicle *pBall = NULL;
//
//	for (int nCnt = 0; nCnt < NUM_BALL; nCnt++)
//	{
//		if (!GetBallUsing(nCnt)) { continue; }
//
//		pBall = GetBall(nCnt);
//		CheckContactCTC(pBall, g_pBall);
//		pBall = NULL;
//	}
//}

//--------------------------------------------------------------------------------
//  �G�A�h���X�擾
//--------------------------------------------------------------------------------
CBodyCicle *GetBall(void)
{
	return g_pBall;
}

D3DXVECTOR3 GetBallPos(void)
{
	return g_pBall->GetPosCenter();
}

D3DXVECTOR3 GetBallVelocity(void)
{
	return g_pBall->GetVelocity();
}

int GetBallCnt(void)
{
	return g_nCntBall;
}