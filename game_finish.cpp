//--------------------------------------------------------------------------------
//
//�@GameFinish.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "game_start.h"
#include "fade.h"
#include "sound.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define GAME_FINISH_POS_X (SCREEN_WIDTH * 0.5f)//�|���S���̕\���ʒuX
#define GAME_FINISH_POS_Y (SCREEN_HEIGHT * 0.5f)//�|���S���̕\���ʒuY
#define GAME_FINISH_WIDTH (1265.0f * 8.0f)
#define GAME_FINISH_HEIGHT (380.0f * 8.0f)
#define GAME_FINISH_SPEED (0.90f)
#define GAME_FINISH_TEXTURENAME "data/TEXTURE/time.png"//�t�@�C����

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGameFinish(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureGameFinish = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferGameFinish = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
D3DXVECTOR3 g_vHalfSize;
int g_nCntFinish;
float g_fFinishAlpha;

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitGameFinish(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_nCntFinish = 0;
	g_fFinishAlpha = 0.0f;
	g_vHalfSize = D3DXVECTOR3(GAME_FINISH_WIDTH * 0.5f, GAME_FINISH_HEIGHT * 0.5f, 0.0f);

	if (FAILED(MakeVerTexGameFinish(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexGameFinish ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, GAME_FINISH_TEXTURENAME, &g_pTextureGameFinish)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitGameFinish(void)
{
	StopSound(SOUND_LABEL_BGM_GAME);

	//safe release
	if (g_pVtxBufferGameFinish != NULL)
	{
		g_pVtxBufferGameFinish->Release();
		g_pVtxBufferGameFinish = NULL;
	}

	if (g_pTextureGameFinish != NULL)
	{
		g_pTextureGameFinish->Release();
		g_pTextureGameFinish = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
bool UpdateGameFinish(void)
{
	if (g_nCntFinish == 0)
	{
		return false;
	}

	if (g_nCntFinish == 60)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

	g_nCntFinish--;

	if (g_nCntFinish >= 2 * 60 + 30)
	{
		g_vHalfSize *= GAME_FINISH_SPEED;
		g_fFinishAlpha += 0.05f;
	}
	else if(g_nCntFinish <= 1 * 60)
	{
		g_vHalfSize /= GAME_FINISH_SPEED;
		g_fFinishAlpha -= 0.05f;
	}
	
	g_fFinishAlpha = g_fFinishAlpha >= 1.0f ? 1.0f : g_fFinishAlpha <= 0.0f ? 0.0f : g_fFinishAlpha;

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferGameFinish->Lock(0, 0, (void**)&pVtx, 0);

	//texture���_
	pVtx[0].pos = D3DXVECTOR3(GAME_FINISH_POS_X - g_vHalfSize.x, GAME_FINISH_POS_Y - g_vHalfSize.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(GAME_FINISH_POS_X + g_vHalfSize.x, GAME_FINISH_POS_Y - g_vHalfSize.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(GAME_FINISH_POS_X - g_vHalfSize.x, GAME_FINISH_POS_Y + g_vHalfSize.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(GAME_FINISH_POS_X + g_vHalfSize.x, GAME_FINISH_POS_Y + g_vHalfSize.y, 0.0f);

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[0].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, g_fFinishAlpha);
	pVtx[1].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, g_fFinishAlpha);
	pVtx[2].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, g_fFinishAlpha);
	pVtx[3].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, g_fFinishAlpha);

	//���z�A�h���X���
	g_pVtxBufferGameFinish->Unlock();

	return true;
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawGameFinish(void)
{
	if (g_nCntFinish == 0) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferGameFinish,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

								   //Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureGameFinish);

	//�v���~�e�B�u�`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//�I�t�Z�b�g�i���_���j
		NUM_POLYGON);

}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGameFinish(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferGameFinish,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferGameFinish->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	pVtx[0].pos = D3DXVECTOR3(GAME_FINISH_POS_X - g_vHalfSize.x, GAME_FINISH_POS_Y - g_vHalfSize.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(GAME_FINISH_POS_X + g_vHalfSize.x, GAME_FINISH_POS_Y - g_vHalfSize.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(GAME_FINISH_POS_X - g_vHalfSize.x, GAME_FINISH_POS_Y + g_vHalfSize.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(GAME_FINISH_POS_X + g_vHalfSize.x, GAME_FINISH_POS_Y + g_vHalfSize.y, 0.0f);

	//rhw�̐ݒ�(�K��1.0f)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[0].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
	pVtx[1].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
	pVtx[2].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
	pVtx[3].color = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);

	//texture���_
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//���z�A�h���X���
	g_pVtxBufferGameFinish->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  GameFinish�J�n
//--------------------------------------------------------------------------------
void StartGameFinish(void)
{
	if (g_nCntFinish != 0) { return; }

	PlaySound(SOUND_LABEL_SE_TIME);
	StopSound(SOUND_LABEL_BGM_GAME);
	g_nCntFinish = 3 * 60;
}