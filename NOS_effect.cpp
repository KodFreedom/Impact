//--------------------------------------------------------------------------------
//
//�@�E�C���h�E�\���v���O����
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "NOS_effect.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define NOS_EFFECT_TEXTURENAME "data/TEXTURE/effect000.jpg"//�t�@�C����
#define NOS_EFFECT_RADIUS (10.0f)
#define NOS_EFFECT_RADIUS_VALUE (0.9f)
#define NOS_EFFECT_NUM (500)
#define NOS_EFFECT_LIFE (20)

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexNOSEffect(LPDIRECT3DDEVICE9 pDevice);
void SetColorEffect(int nCnt, D3DXCOLOR cColor);
void SetVertexEffect(int nCnt, D3DXVECTOR3 vPos, float fRadius);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct
{
	bool bUse;
	D3DXVECTOR3 vPos;//���W
	D3DXCOLOR cColor;//�F
	float fColorAlphaValue;//�A���t�@�ω���
	float fRadius;//���a
	float fRadiusValue;//���a�ω���
	int nLife;//����
}NOS_EFFECT;

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureNOSEffect = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferNOSEffect = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
NOS_EFFECT g_aNOSEffect[NOS_EFFECT_NUM];

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitNOSEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
	{
		g_aNOSEffect[nCnt].bUse = false;
		g_aNOSEffect[nCnt].vPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aNOSEffect[nCnt].cColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		g_aNOSEffect[nCnt].fRadius = 0.0f;
		g_aNOSEffect[nCnt].nLife = 0;
	}

	if (FAILED(MakeVerTexNOSEffect(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexNOSEffect ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		NOS_EFFECT_TEXTURENAME,
		&g_pTextureNOSEffect)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitNOSEffect(void)
{
	if (g_pVtxBufferNOSEffect != NULL)
	{
		g_pVtxBufferNOSEffect->Release();
		g_pVtxBufferNOSEffect = NULL;
	}

	if (g_pTextureNOSEffect != NULL)
	{
		g_pTextureNOSEffect->Release();
		g_pTextureNOSEffect = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateNOSEffect(void)
{
	for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
	{
		if (!g_aNOSEffect[nCnt].bUse) { continue; }

		g_aNOSEffect[nCnt].nLife--;

		if (g_aNOSEffect[nCnt].nLife <= 0)
		{
			g_aNOSEffect[nCnt].bUse = false;
		}
		else
		{
			//color
			g_aNOSEffect[nCnt].cColor.a -= g_aNOSEffect[nCnt].fColorAlphaValue;
			if (g_aNOSEffect[nCnt].cColor.a < 0.0f)
			{
				g_aNOSEffect[nCnt].cColor.a = 0.0f;
			}

			SetColorEffect(nCnt, g_aNOSEffect[nCnt].cColor);

			//���a�̐ݒ�
			g_aNOSEffect[nCnt].fRadius -= g_aNOSEffect[nCnt].fRadiusValue;
			if (g_aNOSEffect[nCnt].fRadius < 0.0f)
			{
				g_aNOSEffect[nCnt].fRadius = 0.0f;
			}
			SetVertexEffect(nCnt, g_aNOSEffect[nCnt].vPos, g_aNOSEffect[nCnt].fRadius);
		}

	}
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawNOSEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	//���Z����
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferNOSEffect,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

								   //Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureNOSEffect);

	for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
	{
		//�v���~�e�B�u�`��
		if (g_aNOSEffect[nCnt].bUse)
		{
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
				nCnt * 4,//�I�t�Z�b�g�i���_���j
				NUM_POLYGON);
		}
	}

	//��Z����
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexNOSEffect(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NOS_EFFECT_NUM,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferNOSEffect,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferNOSEffect->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
	{
		//���_���W�̐ݒ�i2D���W�A�E���j
		pVtx[nCnt * 4].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x - NOS_EFFECT_RADIUS, g_aNOSEffect[nCnt].vPos.y - NOS_EFFECT_RADIUS, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x + NOS_EFFECT_RADIUS, g_aNOSEffect[nCnt].vPos.y - NOS_EFFECT_RADIUS, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x - NOS_EFFECT_RADIUS, g_aNOSEffect[nCnt].vPos.y + NOS_EFFECT_RADIUS, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x + NOS_EFFECT_RADIUS, g_aNOSEffect[nCnt].vPos.y + NOS_EFFECT_RADIUS, 0.0f);

		//rhw�̐ݒ�(�K��1.0f)
		pVtx[nCnt * 4].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[nCnt * 4].color = g_aNOSEffect[nCnt].cColor;
		pVtx[nCnt * 4 + 1].color = g_aNOSEffect[nCnt].cColor;
		pVtx[nCnt * 4 + 2].color = g_aNOSEffect[nCnt].cColor;
		pVtx[nCnt * 4 + 3].color = g_aNOSEffect[nCnt].cColor;

		//texture���_
		pVtx[nCnt * 4].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferNOSEffect->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  setEffect
//--------------------------------------------------------------------------------
void SetNOSEffect(D3DXVECTOR3 vPos, D3DXCOLOR cColor)
{
	for (int nCntNum = 0;nCntNum < 2;nCntNum++)
	{
		for (int nCnt = 0;nCnt < NOS_EFFECT_NUM;nCnt++)
		{
			if (!g_aNOSEffect[nCnt].bUse)
			{
				g_aNOSEffect[nCnt].bUse = true;
				g_aNOSEffect[nCnt].vPos = vPos;
				g_aNOSEffect[nCnt].nLife = NOS_EFFECT_LIFE;
				g_aNOSEffect[nCnt].cColor = cColor;
				g_aNOSEffect[nCnt].fRadius = NOS_EFFECT_RADIUS;
				g_aNOSEffect[nCnt].fRadiusValue = NOS_EFFECT_RADIUS * NOS_EFFECT_RADIUS_VALUE / NOS_EFFECT_LIFE;
				g_aNOSEffect[nCnt].fColorAlphaValue = 1.0f / NOS_EFFECT_LIFE;
				break;
			}
		}
	}
}

//--------------------------------------------------------------------------------
//  SetColorEffect
//--------------------------------------------------------------------------------
void SetColorEffect(int nCnt, D3DXCOLOR cColor)
{
	VERTEX_2D *pVtx = NULL;

	g_pVtxBufferNOSEffect->Lock(0, 0, (void**)&pVtx, 0);

	//���_�J���[�̐ݒ�
	pVtx[nCnt * 4].color = cColor;
	pVtx[nCnt * 4 + 1].color = cColor;
	pVtx[nCnt * 4 + 2].color = cColor;
	pVtx[nCnt * 4 + 3].color = cColor;

	g_pVtxBufferNOSEffect->Unlock();
}

//--------------------------------------------------------------------------------
//  SetVertexEffect
//--------------------------------------------------------------------------------
void SetVertexEffect(int nCnt, D3DXVECTOR3 vPos, float fRadius)
{
	VERTEX_2D *pVtx = NULL;

	g_pVtxBufferNOSEffect->Lock(0, 0, (void**)&pVtx, 0);

	//���_�J���[�̐ݒ�
	pVtx[nCnt * 4].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x - g_aNOSEffect[nCnt].fRadius, g_aNOSEffect[nCnt].vPos.y - g_aNOSEffect[nCnt].fRadius, 0.0f);
	pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x + g_aNOSEffect[nCnt].fRadius, g_aNOSEffect[nCnt].vPos.y - g_aNOSEffect[nCnt].fRadius, 0.0f);
	pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x - g_aNOSEffect[nCnt].fRadius, g_aNOSEffect[nCnt].vPos.y + g_aNOSEffect[nCnt].fRadius, 0.0f);
	pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aNOSEffect[nCnt].vPos.x + g_aNOSEffect[nCnt].fRadius, g_aNOSEffect[nCnt].vPos.y + g_aNOSEffect[nCnt].fRadius, 0.0f);

	g_pVtxBufferNOSEffect->Unlock();
}