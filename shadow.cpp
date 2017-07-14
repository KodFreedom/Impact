//--------------------------------------------------------------------------------
//
//�@Shadow.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "shadow.h"
#include "player.h"
#include "enemy.h"
#include "ball.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define SHADOW_CAR_TEXTURENAME "data/TEXTURE/shadow_car.png"//�t�@�C����
#define SHADOW_BALL_TEXTURENAME "data/TEXTURE/shadow_ball.png"//�t�@�C����
#define NUM_SHADOW (3)
#define SHADOW_CAR_WIDTH (CAR_WIDTH * 1.0f)
#define SHADOW_CAR_HEIGHT (CAR_HEIGHT * 1.0f)
#define SHADOW_BALL_RADIUS (BALL_RADIUS * 1.2f)

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexShadow(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureShadowCar = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DTEXTURE9 g_pTextureShadowBall = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferShadow = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitShadow(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	if (FAILED(MakeVerTexShadow(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexShadow ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, SHADOW_CAR_TEXTURENAME, &g_pTextureShadowCar))
		|| FAILED(D3DXCreateTextureFromFile(pDevice, SHADOW_BALL_TEXTURENAME, &g_pTextureShadowBall)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitShadow(void)
{
	//safe release
	if (g_pVtxBufferShadow != NULL)
	{
		g_pVtxBufferShadow->Release();
		g_pVtxBufferShadow = NULL;
	}

	if (g_pTextureShadowCar != NULL)
	{
		g_pTextureShadowCar->Release();
		g_pTextureShadowCar = NULL;
	}

	if (g_pTextureShadowBall != NULL)
	{
		g_pTextureShadowBall->Release();
		g_pTextureShadowBall = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateShadow(void)
{
	D3DXVECTOR3 vPos;
	float fRot;
	float fAngle = atan2f(SHADOW_CAR_HEIGHT, SHADOW_CAR_WIDTH);
	float fLength = sqrtf(SHADOW_CAR_WIDTH * SHADOW_CAR_WIDTH + SHADOW_CAR_HEIGHT * SHADOW_CAR_HEIGHT) * 0.5f;
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferShadow->Lock(0, 0, (void**)&pVtx, 0);

	//player
	vPos = GetPlayerPos();
	fRot = GetPlayerRot();
	pVtx[0].pos = D3DXVECTOR3(vPos.x + cosf(fRot + D3DX_PI + fAngle) * fLength, vPos.y + sin(fRot + D3DX_PI + fAngle) * fLength, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(vPos.x + cosf(fRot - fAngle) * fLength, vPos.y + sinf(fRot - fAngle) * fLength, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(vPos.x + cosf(fRot - D3DX_PI - fAngle) * fLength, vPos.y + sinf(fRot - D3DX_PI - fAngle) * fLength, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(vPos.x + cosf(fRot + fAngle) * fLength, vPos.y + sinf(fRot + fAngle) * fLength, 0.0f);

	//enemy
	vPos = GetEnemyPos();
	fRot = GetEnemyRot();
	pVtx[4].pos = D3DXVECTOR3(vPos.x + cosf(fRot + D3DX_PI + fAngle) * fLength, vPos.y + sin(fRot + D3DX_PI + fAngle) * fLength, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(vPos.x + cosf(fRot - fAngle) * fLength, vPos.y + sinf(fRot - fAngle) * fLength, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(vPos.x + cosf(fRot - D3DX_PI - fAngle) * fLength, vPos.y + sinf(fRot - D3DX_PI - fAngle) * fLength, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(vPos.x + cosf(fRot + fAngle) * fLength, vPos.y + sinf(fRot + fAngle) * fLength, 0.0f);

	//ball
	vPos = GetBallPos();
	pVtx[8].pos = D3DXVECTOR3(vPos.x - SHADOW_BALL_RADIUS, vPos.y - SHADOW_BALL_RADIUS, 0.0f);
	pVtx[9].pos = D3DXVECTOR3(vPos.x + SHADOW_BALL_RADIUS, vPos.y - SHADOW_BALL_RADIUS, 0.0f);
	pVtx[10].pos = D3DXVECTOR3(vPos.x - SHADOW_BALL_RADIUS, vPos.y + SHADOW_BALL_RADIUS, 0.0f);
	pVtx[11].pos = D3DXVECTOR3(vPos.x + SHADOW_BALL_RADIUS, vPos.y + SHADOW_BALL_RADIUS, 0.0f);

	//���z�A�h���X���
	g_pVtxBufferShadow->Unlock();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawShadow(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferShadow,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

	//Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureShadowCar);

	//�v���~�e�B�u�`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, NUM_POLYGON);

	if (GetBallCnt() != 0) { return; }

	//Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureShadowBall);

	//�v���~�e�B�u�`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexShadow(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NUM_SHADOW,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferShadow,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferShadow->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	D3DXVECTOR3 vPos;
	float fRot;
	float fAngle = atan2f(SHADOW_CAR_HEIGHT, SHADOW_CAR_WIDTH);
	float fLength = sqrtf(SHADOW_CAR_WIDTH * SHADOW_CAR_WIDTH + SHADOW_CAR_HEIGHT * SHADOW_CAR_HEIGHT) * 0.5f;

	//player
	vPos = GetPlayerPos();
	fRot = GetPlayerRot();
	pVtx[0].pos = D3DXVECTOR3(vPos.x + cosf(fRot + D3DX_PI + fAngle) * fLength, vPos.y + sin(fRot + D3DX_PI + fAngle) * fLength, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(vPos.x + cosf(fRot - fAngle) * fLength, vPos.y + sinf(fRot - fAngle) * fLength, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(vPos.x + cosf(fRot - D3DX_PI - fAngle) * fLength, vPos.y + sinf(fRot - D3DX_PI - fAngle) * fLength, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(vPos.x + cosf(fRot + fAngle) * fLength, vPos.y + sinf(fRot + fAngle) * fLength, 0.0f);

	//enemy
	vPos = GetEnemyPos();
	fRot = GetEnemyRot();
	pVtx[4].pos = D3DXVECTOR3(vPos.x + cosf(fRot + D3DX_PI + fAngle) * fLength, vPos.y + sin(fRot + D3DX_PI + fAngle) * fLength, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(vPos.x + cosf(fRot - fAngle) * fLength, vPos.y + sinf(fRot - fAngle) * fLength, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(vPos.x + cosf(fRot - D3DX_PI - fAngle) * fLength, vPos.y + sinf(fRot - D3DX_PI - fAngle) * fLength, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(vPos.x + cosf(fRot + fAngle) * fLength, vPos.y + sinf(fRot + fAngle) * fLength, 0.0f);

	//ball
	vPos = GetBallPos();
	pVtx[8].pos = D3DXVECTOR3(vPos.x - SHADOW_BALL_RADIUS, vPos.y - SHADOW_BALL_RADIUS, 0.0f);
	pVtx[9].pos = D3DXVECTOR3(vPos.x + SHADOW_BALL_RADIUS, vPos.y - SHADOW_BALL_RADIUS, 0.0f);
	pVtx[10].pos = D3DXVECTOR3(vPos.x - SHADOW_BALL_RADIUS, vPos.y + SHADOW_BALL_RADIUS, 0.0f);
	pVtx[11].pos = D3DXVECTOR3(vPos.x + SHADOW_BALL_RADIUS, vPos.y + SHADOW_BALL_RADIUS, 0.0f);

	//���_���W�̐ݒ�i2D���W�A�E���j
	for (int nCnt = 0;nCnt < NUM_SHADOW;nCnt++)
	{
		//pVtx[nCnt * 4 + 0].pos = VEC3_NULL;
		//pVtx[nCnt * 4 + 1].pos = VEC3_NULL;
		//pVtx[nCnt * 4 + 2].pos = VEC3_NULL;
		//pVtx[nCnt * 4 + 3].pos = VEC3_NULL;

		//rhw�̐ݒ�(�K��1.0f)
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.6f);

		//texture���_
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}
	
	//���z�A�h���X���
	g_pVtxBufferShadow->Unlock();

	return S_OK;
}