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
#include "ready.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define READY_WIDTH (207.0f)//�|���S���̕\���ʒuY
#define READY_HEIGHT (51.0f)//�|���S���̕\���ʒuY
#define READY_TEXTURENAME "data/TEXTURE/ready.png"//�t�@�C����
#define READY_NUM (2)

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexReady(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureReady = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferReady = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
bool g_bReady[READY_NUM];

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitReady(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	for (int nCnt = 0;nCnt < READY_NUM;nCnt++)
	{
		g_bReady[nCnt] = false;
	}

	if (FAILED(MakeVerTexReady(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexReady ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, READY_TEXTURENAME, &g_pTextureReady)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitReady(void)
{
	//safe release
	if (g_pVtxBufferReady != NULL)
	{
		g_pVtxBufferReady->Release();
		g_pVtxBufferReady = NULL;
	}

	if (g_pTextureReady != NULL)
	{
		g_pTextureReady->Release();
		g_pTextureReady = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateReady(void)
{
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawReady(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferReady,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

								   //Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureReady);

	//�v���~�e�B�u�`��
	for (int nCnt = 0;nCnt < READY_NUM;nCnt++)
	{
		if (g_bReady[nCnt])
		{
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCnt * 4,//�I�t�Z�b�g�i���_���j
				NUM_POLYGON);
		}
	}
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexReady(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * READY_NUM,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferReady,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferReady->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	for (int nCnt = 0;nCnt < READY_NUM;nCnt++)
	{
		//���_���W�̐ݒ�i2D���W�A�E���j
		pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(SCREEN_WIDTH * (0.205f + nCnt * 0.59f) - READY_WIDTH * 0.5f, SCREEN_HEIGHT * 0.92f - READY_HEIGHT * 0.5f, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(SCREEN_WIDTH * (0.205f + nCnt * 0.59f) + READY_WIDTH * 0.5f, SCREEN_HEIGHT * 0.92f - READY_HEIGHT * 0.5f, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(SCREEN_WIDTH * (0.205f + nCnt * 0.59f) - READY_WIDTH * 0.5f, SCREEN_HEIGHT * 0.92f + READY_HEIGHT * 0.5f, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(SCREEN_WIDTH * (0.205f + nCnt * 0.59f) + READY_WIDTH * 0.5f, SCREEN_HEIGHT * 0.92f + READY_HEIGHT * 0.5f, 0.0f);

		//rhw�̐ݒ�(�K��1.0f)
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(1.0f * nCnt, 0.0f, 1.0f*(1 - nCnt), 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(1.0f * nCnt, 0.0f, 1.0f*(1 - nCnt), 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(1.0f * nCnt, 0.0f, 1.0f*(1 - nCnt), 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(1.0f * nCnt, 0.0f, 1.0f*(1 - nCnt), 1.0f);

		//texture���_
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferReady->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  �ʒu�ƐF��ݒ�
//--------------------------------------------------------------------------------
void SetReady(int nNumReady)
{
	g_bReady[nNumReady] = true;
}