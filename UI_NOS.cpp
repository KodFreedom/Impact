//--------------------------------------------------------------------------------
//
//�@Result.cpp
//	Author : Xu Wenjie
//	Date   : 2016-07-01
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "enemy.h"
#include "player.h"
#include "UI_NOS.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define PLAYER_NOS_POS_X (163.0f - 0.5f)//�|���S���̕\���ʒuX
#define PLAYER_NOS_POS_Y (185.0f - 0.5f)//�|���S���̕\���ʒuY
#define ENEMY_NOS_POS_X (930.0f - 0.5f)//�|���S���̕\���ʒuX
#define ENEMY_NOS_POS_Y (185.0f - 0.5f)//�|���S���̕\���ʒuY
#define UI_NOS_WIDTH (265.0f)
#define UI_NOS_HEIGHT (30.0f)
#define NOS_TEXTURENAME "data/TEXTURE/NOS.png"//�t�@�C����

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexUINOS(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureUINOS = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferUINOS = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitUINOS(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_pTextureUINOS = NULL;

	if (FAILED(MakeVerTexUINOS(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexUINOS ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, NOS_TEXTURENAME, &g_pTextureUINOS)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitUINOS(void)
{
	//safe release
	if (g_pVtxBufferUINOS != NULL)
	{
		g_pVtxBufferUINOS->Release();
		g_pVtxBufferUINOS = NULL;
	}

	if (g_pTextureUINOS != NULL)
	{
		g_pTextureUINOS->Release();
		g_pTextureUINOS = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateUINOS(void)
{
	VERTEX_2D *pVtx = NULL;
	int aNOS[2];
	float aPosX[2];
	float aPosY[2];

	aNOS[0] = GetPlayerNOS();
	aNOS[1] = GetEnemyNOS();
	aPosX[0] = PLAYER_NOS_POS_X;
	aPosX[1] = ENEMY_NOS_POS_X;
	aPosY[0] = PLAYER_NOS_POS_Y;
	aPosY[1] = ENEMY_NOS_POS_Y;

	g_pVtxBufferUINOS->Lock(0, 0, (void**)&pVtx, 0);

	//�X�R�A�v�Z
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(aPosX[nCnt] + UI_NOS_WIDTH * (float)(NOS_MAX - aNOS[nCnt]) / (float)NOS_MAX, aPosY[nCnt], 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(aPosX[nCnt] + UI_NOS_WIDTH, aPosY[nCnt], 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(aPosX[nCnt] + UI_NOS_WIDTH * (float)(NOS_MAX - aNOS[nCnt]) / (float)NOS_MAX, aPosY[nCnt] + UI_NOS_HEIGHT, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(aPosX[nCnt] + UI_NOS_WIDTH, aPosY[nCnt] + UI_NOS_HEIGHT, 0.0f);

		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(1.0f * (float)(NOS_MAX - aNOS[nCnt]) / (float)NOS_MAX, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(1.0f * (float)(NOS_MAX - aNOS[nCnt]) / (float)NOS_MAX, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}
	g_pVtxBufferUINOS->Unlock();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawUINOS(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferUINOS,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

	//NOS
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		//Texture�̐ݒ�
		pDevice->SetTexture(0, g_pTextureUINOS);

		//�v���~�e�B�u�`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//�I�t�Z�b�g�i���_���j
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexUINOS(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * 2,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferUINOS,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferUINOS->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	pVtx[0].pos = D3DXVECTOR3(PLAYER_NOS_POS_X , PLAYER_NOS_POS_Y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PLAYER_NOS_POS_X + UI_NOS_WIDTH, PLAYER_NOS_POS_Y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PLAYER_NOS_POS_X, PLAYER_NOS_POS_Y + UI_NOS_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PLAYER_NOS_POS_X + UI_NOS_WIDTH, PLAYER_NOS_POS_Y + UI_NOS_HEIGHT, 0.0f);

	pVtx[4].pos = D3DXVECTOR3(ENEMY_NOS_POS_X, ENEMY_NOS_POS_Y, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(ENEMY_NOS_POS_X + UI_NOS_WIDTH, ENEMY_NOS_POS_Y, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(ENEMY_NOS_POS_X, ENEMY_NOS_POS_Y + UI_NOS_HEIGHT, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(ENEMY_NOS_POS_X + UI_NOS_WIDTH, ENEMY_NOS_POS_Y + UI_NOS_HEIGHT, 0.0f);

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[0].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pVtx[1].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pVtx[2].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	pVtx[3].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

	pVtx[4].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[5].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[6].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	pVtx[7].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	//rhw�̐ݒ�(�K��1.0f)
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//texture���_
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferUINOS->Unlock();

	return S_OK;
}