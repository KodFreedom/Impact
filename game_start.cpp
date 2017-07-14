//--------------------------------------------------------------------------------
//
//�@GameStart.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "game_start.h"
#include "sound.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define GAME_START_POS_X (SCREEN_WIDTH * 0.5f)//�|���S���̕\���ʒuX
#define GAME_START_POS_Y (SCREEN_HEIGHT * 0.5f)//�|���S���̕\���ʒuY
#define GAME_START_WIDTH (474.0f)
#define GAME_START_HEIGHT (164.0f)
#define GAME_START_TEXTURENAME "data/TEXTURE/gamestart.png"//�t�@�C����

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGameStart(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureGameStart = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferGameStart = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
int g_nCntStart;

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitGameStart(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_nCntStart = 4 * 60;

	if (FAILED(MakeVerTexGameStart(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexGameStart ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, GAME_START_TEXTURENAME, &g_pTextureGameStart)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitGameStart(void)
{
	//safe release
	if (g_pVtxBufferGameStart != NULL)
	{
		g_pVtxBufferGameStart->Release();
		g_pVtxBufferGameStart = NULL;
	}

	if (g_pTextureGameStart != NULL)
	{
		g_pTextureGameStart->Release();
		g_pTextureGameStart = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
bool UpdateGameStart(void)
{
	if (g_nCntStart == 0) { return false; }

	if (g_nCntStart == 1)
	{
		PlaySound(SOUND_LABEL_BGM_GAME);
	}

	if (g_nCntStart == 4 * 60)
	{
		PlaySound(SOUND_LABEL_SE_SIGNAL);
	}

	g_nCntStart--;
	int nWork = 3 - g_nCntStart / 60;
	

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferGameStart->Lock(0,0,(void**)&pVtx,0);

	//texture���_
	pVtx[0].tex = D3DXVECTOR2(0.0f, nWork * 0.25f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, nWork * 0.25f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, nWork * 0.25f + 0.25f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, nWork * 0.25f + 0.25f);

	//���z�A�h���X���
	g_pVtxBufferGameStart->Unlock();

	return true;
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawGameStart(void)
{
	if (g_nCntStart == 0) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferGameStart,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

								   //Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureGameStart);

	//�v���~�e�B�u�`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//�I�t�Z�b�g�i���_���j
		NUM_POLYGON);

}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGameStart(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferGameStart,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferGameStart->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	pVtx[0].pos = D3DXVECTOR3(GAME_START_POS_X - GAME_START_WIDTH * 0.5f, GAME_START_POS_Y - GAME_START_HEIGHT * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(GAME_START_POS_X + GAME_START_WIDTH * 0.5f, GAME_START_POS_Y - GAME_START_HEIGHT * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(GAME_START_POS_X - GAME_START_WIDTH * 0.5f, GAME_START_POS_Y + GAME_START_HEIGHT * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(GAME_START_POS_X + GAME_START_WIDTH * 0.5f, GAME_START_POS_Y + GAME_START_HEIGHT * 0.5f, 0.0f);

	//rhw�̐ݒ�(�K��1.0f)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//texture���_
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 0.25f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 0.25f);

	//���z�A�h���X���
	g_pVtxBufferGameStart->Unlock();

	return S_OK;
}