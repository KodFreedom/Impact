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
#include "press_enter.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define PRESS_ENTER_WIDTH (298.0f)//�|���S���̕\���ʒuY
#define PRESS_ENTER_HEIGHT (30.0f)//�|���S���̕\���ʒuY
#define PRESS_ENTER_TEXTURENAME "data/TEXTURE/press.png"//�t�@�C����
#define PRESS_NUM (2)

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPressEnter(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct {
	D3DXCOLOR cColor;//�����F
	float fAlpha;//�A���t�@�l
	float fChange;//�A���t�@�l�ω������]��
	bool bUse;
}PRESS;

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTexturePressEnter = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferPressEnter = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
PRESS g_aPress[PRESS_NUM];

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitPressEnter(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		g_aPress[nCnt].cColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		g_aPress[nCnt].fAlpha = 0.0f;
		g_aPress[nCnt].fChange = 1.0f;
		g_aPress[nCnt].bUse = false;
	}
	
	if (FAILED(MakeVerTexPressEnter(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexPressEnter ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, PRESS_ENTER_TEXTURENAME, &g_pTexturePressEnter)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitPressEnter(void)
{
	//safe release
	if (g_pVtxBufferPressEnter != NULL)
	{
		g_pVtxBufferPressEnter->Release();
		g_pVtxBufferPressEnter = NULL;
	}

	if (g_pTexturePressEnter != NULL)
	{
		g_pTexturePressEnter->Release();
		g_pTexturePressEnter = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdatePressEnter(void)
{
	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferPressEnter->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		if (g_aPress[nCnt].bUse)
		{
			g_aPress[nCnt].fAlpha += 0.025f * g_aPress[nCnt].fChange;

			if (g_aPress[nCnt].fAlpha >= 1.0f)
			{
				g_aPress[nCnt].fAlpha = 1.0f;
				g_aPress[nCnt].fChange = -1.0f;
			}

			if (g_aPress[nCnt].fAlpha <= 0.0f)
			{
				g_aPress[nCnt].fAlpha = 0.0f;
				g_aPress[nCnt].fChange = 1.0f;
			}

			g_aPress[nCnt].cColor.a = g_aPress[nCnt].fAlpha;

			//���_�J���[�̐ݒ�(0�`255�̐����l)
			pVtx[nCnt * 4 + 0].color = g_aPress[nCnt].cColor;
			pVtx[nCnt * 4 + 1].color = g_aPress[nCnt].cColor;
			pVtx[nCnt * 4 + 2].color = g_aPress[nCnt].cColor;
			pVtx[nCnt * 4 + 3].color = g_aPress[nCnt].cColor;
		}
	}
	
	//���z�A�h���X���
	g_pVtxBufferPressEnter->Unlock();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawPressEnter(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferPressEnter,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

								   //Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTexturePressEnter);

	//�v���~�e�B�u�`��
	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		if (g_aPress[nCnt].bUse)
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
HRESULT MakeVerTexPressEnter(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * PRESS_NUM,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferPressEnter,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferPressEnter->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);
	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		//���_���W�̐ݒ�i2D���W�A�E���j
		pVtx[nCnt * 4 + 0].pos = VEC3_NULL;
		pVtx[nCnt * 4 + 1].pos = VEC3_NULL;
		pVtx[nCnt * 4 + 2].pos = VEC3_NULL;
		pVtx[nCnt * 4 + 3].pos = VEC3_NULL;

		//rhw�̐ݒ�(�K��1.0f)
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[nCnt * 4 + 0].color = g_aPress[nCnt].cColor;
		pVtx[nCnt * 4 + 1].color = g_aPress[nCnt].cColor;
		pVtx[nCnt * 4 + 2].color = g_aPress[nCnt].cColor;
		pVtx[nCnt * 4 + 3].color = g_aPress[nCnt].cColor;

		//texture���_
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferPressEnter->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  �ʒu�ƐF��ݒ�
//--------------------------------------------------------------------------------
void SetPressEnter(D3DXVECTOR3 vPos, D3DXCOLOR cColor)
{
	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferPressEnter->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0;nCnt < PRESS_NUM;nCnt++)
	{
		if (!g_aPress[nCnt].bUse)
		{
			g_aPress[nCnt].bUse = true;
			g_aPress[nCnt].cColor = cColor;

			//���_���W�̐ݒ�i2D���W�A�E���j
			pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(vPos.x - PRESS_ENTER_WIDTH * 0.5f, vPos.y - PRESS_ENTER_HEIGHT * 0.5f, 0.0f);
			pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(vPos.x + PRESS_ENTER_WIDTH * 0.5f, vPos.y - PRESS_ENTER_HEIGHT * 0.5f, 0.0f);
			pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(vPos.x - PRESS_ENTER_WIDTH * 0.5f, vPos.y + PRESS_ENTER_HEIGHT * 0.5f, 0.0f);
			pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(vPos.x + PRESS_ENTER_WIDTH * 0.5f, vPos.y + PRESS_ENTER_HEIGHT * 0.5f, 0.0f);

			//���_�J���[�̐ݒ�(0�`255�̐����l)
			pVtx[nCnt * 4 + 0].color = cColor;
			pVtx[nCnt * 4 + 1].color = cColor;
			pVtx[nCnt * 4 + 2].color = cColor;
			pVtx[nCnt * 4 + 3].color = cColor;

			break;
		}
	}

	//���z�A�h���X���
	g_pVtxBufferPressEnter->Unlock();
}

//--------------------------------------------------------------------------------
//  ��\���ɂ���
//--------------------------------------------------------------------------------
void EndPressEnter(int nNumEnter)
{
	g_aPress[nNumEnter].bUse = false;
}