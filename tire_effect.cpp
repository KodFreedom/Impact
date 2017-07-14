//--------------------------------------------------------------------------------
//
//�@effect
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "tire_effect.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define TIRE_EFFECT_TEXTURENAME "data/TEXTURE/circle.png"//�t�@�C����
#define TIRE_EFFECT_RADIUS (6.0f)
#define TIRE_EFFECT_NUM (1000)

#define COLOR_R (0.52f)
#define COLOR_G (0.35f)
#define COLOR_B (0.13f)

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct {
	D3DXVECTOR3 vPosCenter;//���S���W
	float fRadius;//�Ίp������
	float fAlpha;
	int nLife;//����
	bool bUsed;
}TIRE_EFFECT;

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexTireEffect(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureTireEffect = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferTireEffect = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
TIRE_EFFECT g_aEffect[TIRE_EFFECT_NUM];

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitTireEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	//effect������
	for (int nCnt = 0; nCnt < TIRE_EFFECT_NUM; nCnt++)
	{
		g_aEffect[nCnt].vPosCenter = VEC3_NULL;
		g_aEffect[nCnt].fRadius = TIRE_EFFECT_RADIUS;
		g_aEffect[nCnt].fAlpha = 1.0f;
		g_aEffect[nCnt].nLife = 0;
		g_aEffect[nCnt].bUsed = false;
	}

	if (FAILED(MakeVerTexTireEffect(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		TIRE_EFFECT_TEXTURENAME,
		&g_pTextureTireEffect)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitTireEffect(void)
{
	if (g_pVtxBufferTireEffect != NULL)
	{
		g_pVtxBufferTireEffect->Release();
		g_pVtxBufferTireEffect = NULL;
	}

	if (g_pTextureTireEffect != NULL)
	{
		g_pTextureTireEffect->Release();
		g_pTextureTireEffect = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateTireEffect(void)
{
	//for test
	VERTEX_2D *pVtx = NULL;

	g_pVtxBufferTireEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < TIRE_EFFECT_NUM; nCnt++)
	{
		if (g_aEffect[nCnt].bUsed)
		{
			//pos
			pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x - g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y - g_aEffect[nCnt].fRadius, 0.0f);
			pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y - g_aEffect[nCnt].fRadius, 0.0f);
			pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x - g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y + g_aEffect[nCnt].fRadius, 0.0f);
			pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y + g_aEffect[nCnt].fRadius, 0.0f);

			//color
			pVtx[nCnt * 4 + 0].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].fAlpha);
			pVtx[nCnt * 4 + 1].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].fAlpha);
			pVtx[nCnt * 4 + 2].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].fAlpha);
			pVtx[nCnt * 4 + 3].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, g_aEffect[nCnt].fAlpha);

			

			//update
			g_aEffect[nCnt].nLife--;
			g_aEffect[nCnt].fAlpha -= 0.025f;

			if (g_aEffect[nCnt].nLife == 0)
			{
				g_aEffect[nCnt].bUsed = false;
			}
		}
	}
	g_pVtxBufferTireEffect->Unlock();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawTireEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferTireEffect,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

						   //���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	//Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureTireEffect);

	for (int nCnt = 0; nCnt < TIRE_EFFECT_NUM; nCnt++)
	{
		if (g_aEffect[nCnt].bUsed)
		{
			//�v���~�e�B�u�`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCnt * 4,//�I�t�Z�b�g�i���_���j
				NUM_POLYGON);
		}
	}
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexTireEffect(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * TIRE_EFFECT_NUM,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferTireEffect,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferTireEffect->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	for (int nCnt = 0; nCnt < TIRE_EFFECT_NUM; nCnt++)
	{
		//���_���W�̐ݒ�i2D���W�A�E���j
		pVtx[nCnt * 4 + 0].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x - g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y - g_aEffect[nCnt].fRadius, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y - g_aEffect[nCnt].fRadius, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x - g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y + g_aEffect[nCnt].fRadius, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aEffect[nCnt].vPosCenter.x + g_aEffect[nCnt].fRadius, g_aEffect[nCnt].vPosCenter.y + g_aEffect[nCnt].fRadius, 0.0f);

		//rhw�̐ݒ�(�K��1.0f)
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(COLOR_R, COLOR_G, COLOR_B, 1.0f);

		//texture���_
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferTireEffect->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  goal effect�ݒ�
//--------------------------------------------------------------------------------
void SetTireEffect(D3DXVECTOR3 vSetPos)
{
	for (int nCnt = 0;nCnt < TIRE_EFFECT_NUM;nCnt++)
	{
		if (!g_aEffect[nCnt].bUsed)
		{
			g_aEffect[nCnt].bUsed = true;
			g_aEffect[nCnt].nLife = 40;
			g_aEffect[nCnt].fRadius = TIRE_EFFECT_RADIUS;
			g_aEffect[nCnt].fAlpha = 1.0f;
			g_aEffect[nCnt].vPosCenter = vSetPos;
			break;
		}
	}
}