//--------------------------------------------------------------------------------
//
//�@title.cpp
//	Author : Xu Wenjie
//	Date   : 2016-07-01
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "title_word.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define TITLE_WORD_POS_X (SCREEN_WIDTH * 0.5f - 0.5f)//�|���S���̕\���ʒuX
#define TITLE_WORD_POS_Y (SCREEN_HEIGHT * 0.5f - 0.5f)//�|���S���̕\���ʒuY
#define TITLE_WORD_WIDTH (913.0f)
#define TITLE_WORD_HEIGHT (212.0f)
#define TITLE_WORD_TEXTURENAME "data/TEXTURE/title_word.png"//�t�@�C����
#define TITLE_WORD_UPDATE_TIME (42)

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexTitleWord(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureTitleWord = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferTitleWord = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
int g_nCntTitleWord;

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitTitleWord(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_nCntTitleWord = 0;

	if (FAILED(MakeVerTexTitleWord(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexTitleWord ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, TITLE_WORD_TEXTURENAME, &g_pTextureTitleWord)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitTitleWord(void)
{
	//safe release
	if (g_pVtxBufferTitleWord != NULL)
	{
		g_pVtxBufferTitleWord->Release();
		g_pVtxBufferTitleWord = NULL;
	}

	if (g_pTextureTitleWord != NULL)
	{
		g_pTextureTitleWord->Release();
		g_pTextureTitleWord = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateTitleWord(void)
{
	VERTEX_2D *pVtx;
	float fWidth;
	float fHeight;
	float fColor;

	g_nCntTitleWord = (g_nCntTitleWord + 1) % TITLE_WORD_UPDATE_TIME;

	if (g_nCntTitleWord < TITLE_WORD_UPDATE_TIME / 4)
	{
		fWidth = TITLE_WORD_WIDTH * 0.05f * ((float)g_nCntTitleWord / ((float)TITLE_WORD_UPDATE_TIME / 4.0f));
		fHeight = TITLE_WORD_HEIGHT * 0.05f * ((float)g_nCntTitleWord / ((float)TITLE_WORD_UPDATE_TIME / 4.0f));
		fColor = 0.3f * ((float)g_nCntTitleWord / ((float)TITLE_WORD_UPDATE_TIME / 4.0f)) + 0.7f;
	}
	else
	{
		fWidth = TITLE_WORD_WIDTH * 0.05f * ((float)(TITLE_WORD_UPDATE_TIME - g_nCntTitleWord) / ((float)TITLE_WORD_UPDATE_TIME * 3.0f / 4.0f));
		fHeight = TITLE_WORD_HEIGHT * 0.05f * ((float)(TITLE_WORD_UPDATE_TIME - g_nCntTitleWord) / ((float)TITLE_WORD_UPDATE_TIME * 3.0f / 4.0f));
		fColor = 0.3f * ((float)(TITLE_WORD_UPDATE_TIME - g_nCntTitleWord) / ((float)TITLE_WORD_UPDATE_TIME * 3.0f / 4.0f)) + 0.7f;
	}

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferTitleWord->Lock(0,0,(void**)&pVtx,0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	pVtx[0].pos = D3DXVECTOR3(TITLE_WORD_POS_X - TITLE_WORD_WIDTH * 0.5f - fWidth, TITLE_WORD_POS_Y - TITLE_WORD_HEIGHT * 0.5f - fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TITLE_WORD_POS_X + TITLE_WORD_WIDTH * 0.5f + fWidth, TITLE_WORD_POS_Y - TITLE_WORD_HEIGHT * 0.5f - fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TITLE_WORD_POS_X - TITLE_WORD_WIDTH * 0.5f - fWidth, TITLE_WORD_POS_Y + TITLE_WORD_HEIGHT * 0.5f + fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TITLE_WORD_POS_X + TITLE_WORD_WIDTH * 0.5f + fWidth, TITLE_WORD_POS_Y + TITLE_WORD_HEIGHT * 0.5f + fHeight, 0.0f);

	pVtx[0].color = D3DXCOLOR(fColor, fColor, fColor, 1.0f);
	pVtx[1].color = D3DXCOLOR(fColor, fColor, fColor, 1.0f);
	pVtx[2].color = D3DXCOLOR(fColor, fColor, fColor, 1.0f);
	pVtx[3].color = D3DXCOLOR(fColor, fColor, fColor, 1.0f);

	//���z�A�h���X���
	g_pVtxBufferTitleWord->Unlock();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawTitleWord(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferTitleWord,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

	//Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureTitleWord);

	//�v���~�e�B�u�`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//�I�t�Z�b�g�i���_���j
		NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexTitleWord(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferTitleWord,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferTitleWord->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	pVtx[0].pos = D3DXVECTOR3(TITLE_WORD_POS_X - TITLE_WORD_WIDTH * 0.5f, TITLE_WORD_POS_Y - TITLE_WORD_HEIGHT * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(TITLE_WORD_POS_X + TITLE_WORD_WIDTH * 0.5f, TITLE_WORD_POS_Y - TITLE_WORD_HEIGHT * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(TITLE_WORD_POS_X - TITLE_WORD_WIDTH * 0.5f, TITLE_WORD_POS_Y + TITLE_WORD_HEIGHT * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(TITLE_WORD_POS_X + TITLE_WORD_WIDTH * 0.5f, TITLE_WORD_POS_Y + TITLE_WORD_HEIGHT * 0.5f, 0.0f);

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
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	//���z�A�h���X���
	g_pVtxBufferTitleWord->Unlock();

	return S_OK;
}