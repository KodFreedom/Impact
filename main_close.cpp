//--------------------------------------------------------------------------------
//
//�@MainClose.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "main_close.h"
#include "input.h"
#include "joystick.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define MAIN_CLOSE_POS_X (SCREEN_WIDTH * 0.5f)//�|���S���̕\���ʒuX
#define MAIN_CLOSE_POS_Y (SCREEN_HEIGHT * 0.5f)//�|���S���̕\���ʒuY
#define MAIN_CLOSE_WIDTH (400.0f)
#define MAIN_CLOSE_HEIGHT (200.0f)
#define MAIN_CLOSE_NO_POS_X (MAIN_CLOSE_POS_X + 58.0f)//�|���S���̕\���ʒuX
#define MAIN_CLOSE_NO_POS_Y (MAIN_CLOSE_POS_Y + 30.0f)//�|���S���̕\���ʒuY
#define MAIN_CLOSE_NO_WIDTH (50.0f)
#define MAIN_CLOSE_NO_HEIGHT (35.0f)
#define MAIN_CLOSE_YES_POS_X (MAIN_CLOSE_POS_X - 58.0f)//�|���S���̕\���ʒuX
#define MAIN_CLOSE_YES_POS_Y (MAIN_CLOSE_POS_Y + 30.0f)//�|���S���̕\���ʒuY
#define MAIN_CLOSE_YES_WIDTH (70.0f)
#define MAIN_CLOSE_YES_HEIGHT (35.0f)
#define MAIN_CLOSE_TEXTURENAME "data/TEXTURE/mainclose.png"//�t�@�C����

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexMainClose(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef enum {
	MAIN_CLOSE_NONE,
	MAIN_CLOSE_NO,
	MAIN_CLOSE_YES,
	MAIN_CLOSE_MAX
}CLOSE;

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureMainClose = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferMainClose = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
CLOSE g_close;

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitMainClose(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_close = MAIN_CLOSE_NONE;

	if (FAILED(MakeVerTexMainClose(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexMainClose ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, MAIN_CLOSE_TEXTURENAME, &g_pTextureMainClose)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitMainClose(void)
{
	//safe release
	if (g_pVtxBufferMainClose != NULL)
	{
		g_pVtxBufferMainClose->Release();
		g_pVtxBufferMainClose = NULL;
	}

	if (g_pTextureMainClose != NULL)
	{
		g_pTextureMainClose->Release();
		g_pTextureMainClose = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
bool UpdateMainClose(HWND *pWnd)
{
	//key����
	if (GetKeyboardTrigger(DIK_ESCAPE))//Pause
	{
		g_close = MAIN_CLOSE_NO;
	}

	if (g_close == MAIN_CLOSE_NONE) { return false; }

	//key����
	if (GetKeyboardTrigger(DIK_LEFT) || GetKeyboardTrigger(DIK_RIGHT))
	{
		g_close = g_close == MAIN_CLOSE_NO ? MAIN_CLOSE_YES : MAIN_CLOSE_NO;
	}

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		switch (g_close)
		{
		case MAIN_CLOSE_NO://resume
			g_close = MAIN_CLOSE_NONE;
			return false;
			break;
		case MAIN_CLOSE_YES://close window
			//WM_DESTROY���b�Z�[�W�𑗐M
			DestroyWindow(*pWnd);
			return true;
			break;
		default:
			break;
		}
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;
	float fWork;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferMainClose->Lock(0, 0, (void**)&pVtx, 0);

	//texture���_
	for (int nCnt = 0;nCnt < MAIN_CLOSE_MAX - 1;nCnt++)
	{
		fWork = (g_close - nCnt) % 2 * 0.5f + 0.5f;
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferMainClose->Unlock();

	return true;
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawMainClose(void)
{
	if (g_close == MAIN_CLOSE_NONE) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferMainClose,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

	//Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureMainClose);

	//�v���~�e�B�u�`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexMainClose(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * 3,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferMainClose,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferMainClose->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	
	pVtx[0].pos = D3DXVECTOR3(MAIN_CLOSE_NO_POS_X - MAIN_CLOSE_NO_WIDTH * 0.5f, MAIN_CLOSE_NO_POS_Y - MAIN_CLOSE_NO_HEIGHT * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(MAIN_CLOSE_NO_POS_X + MAIN_CLOSE_NO_WIDTH * 0.5f, MAIN_CLOSE_NO_POS_Y - MAIN_CLOSE_NO_HEIGHT * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(MAIN_CLOSE_NO_POS_X - MAIN_CLOSE_NO_WIDTH * 0.5f, MAIN_CLOSE_NO_POS_Y + MAIN_CLOSE_NO_HEIGHT * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(MAIN_CLOSE_NO_POS_X + MAIN_CLOSE_NO_WIDTH * 0.5f, MAIN_CLOSE_NO_POS_Y + MAIN_CLOSE_NO_HEIGHT * 0.5f, 0.0f);
	
	pVtx[4].pos = D3DXVECTOR3(MAIN_CLOSE_YES_POS_X - MAIN_CLOSE_YES_WIDTH * 0.5f, MAIN_CLOSE_YES_POS_Y - MAIN_CLOSE_YES_HEIGHT * 0.5f, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(MAIN_CLOSE_YES_POS_X + MAIN_CLOSE_YES_WIDTH * 0.5f, MAIN_CLOSE_YES_POS_Y - MAIN_CLOSE_YES_HEIGHT * 0.5f, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(MAIN_CLOSE_YES_POS_X - MAIN_CLOSE_YES_WIDTH * 0.5f, MAIN_CLOSE_YES_POS_Y + MAIN_CLOSE_YES_HEIGHT * 0.5f, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(MAIN_CLOSE_YES_POS_X + MAIN_CLOSE_YES_WIDTH * 0.5f, MAIN_CLOSE_YES_POS_Y + MAIN_CLOSE_YES_HEIGHT * 0.5f, 0.0f);

	pVtx[8].pos = D3DXVECTOR3(MAIN_CLOSE_POS_X - MAIN_CLOSE_WIDTH * 0.5f, MAIN_CLOSE_POS_Y - MAIN_CLOSE_HEIGHT * 0.5f, 0.0f);
	pVtx[9].pos = D3DXVECTOR3(MAIN_CLOSE_POS_X + MAIN_CLOSE_WIDTH * 0.5f, MAIN_CLOSE_POS_Y - MAIN_CLOSE_HEIGHT * 0.5f, 0.0f);
	pVtx[10].pos = D3DXVECTOR3(MAIN_CLOSE_POS_X - MAIN_CLOSE_WIDTH * 0.5f, MAIN_CLOSE_POS_Y + MAIN_CLOSE_HEIGHT * 0.5f, 0.0f);
	pVtx[11].pos = D3DXVECTOR3(MAIN_CLOSE_POS_X + MAIN_CLOSE_WIDTH * 0.5f, MAIN_CLOSE_POS_Y + MAIN_CLOSE_HEIGHT * 0.5f, 0.0f);

	//rhw�̐ݒ�(�K��1.0f)
	for (int nCnt = 0;nCnt < MAIN_CLOSE_MAX - 1;nCnt++)
	{
		float fWork = (g_close - nCnt) % 2 * 0.5f + 0.5f;
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
	}

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[8 + 0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[8 + 1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[8 + 2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[8 + 3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	for (int nCnt = 0;nCnt < MAIN_CLOSE_MAX;nCnt++)
	{
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//texture���_
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 1.0f / 3.0f * nCnt);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 1.0f / 3.0f * nCnt);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f / 3.0f * nCnt + 1.0f / 3.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f / 3.0f * nCnt + 1.0f / 3.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferMainClose->Unlock();

	return S_OK;
}