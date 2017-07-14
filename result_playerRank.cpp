//--------------------------------------------------------------------------------
//  PlayerRank.cpp
//  Author : XU WENJIE
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include "main.h"
#include "result_playerRank.h"

//--------------------------------------------------------------------------------
//  �}�N����`
//--------------------------------------------------------------------------------
#define PLAYER_RANK_POS_X (116.0f)
#define PLAYER_RANK_POS_Y (330.0f)
#define ENEMY_RANK_POS_X (984.0f)
#define ENEMY_RANK_POS_Y (330.0f)
#define NUM_PLAYER (2)
#define SCORE_WIDTH (85.0f)
#define SCORE_HEIGHT (150.0f)
#define SCORE_SPACE (14.0f)
#define DEGITAL_MAX (2)//����
#define PLAYER_RANK_TEXTURENAME "data/TEXTURE/num00.png"//�t�@�C����

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPlayerRank(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ��錾
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTexturePlayerRank = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferPlayerRank = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
int g_aPlayerRank[NUM_PLAYER];
int g_aPlayerRankOutPut[NUM_PLAYER];

//--------------------------------------------------------------------------------
//  �������֐�
//--------------------------------------------------------------------------------
void InitPlayerRank(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_aPlayerRank[0] =
		g_aPlayerRank[1] = 0;

	g_aPlayerRankOutPut[0] =
		g_aPlayerRankOutPut[1] = 99;

	if (FAILED(MakeVerTexPlayerRank(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		PLAYER_RANK_TEXTURENAME,
		&g_pTexturePlayerRank)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  ��Еt���֐�
//--------------------------------------------------------------------------------
void UninitPlayerRank(void)
{
	if (g_pVtxBufferPlayerRank != NULL)
	{
		g_pVtxBufferPlayerRank->Release();
		g_pVtxBufferPlayerRank = NULL;
	}

	if (g_pTexturePlayerRank != NULL)
	{
		g_pTexturePlayerRank->Release();
		g_pTexturePlayerRank = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V�֐�
//--------------------------------------------------------------------------------
void UpdatePlayerRank(void)
{
	//�X�V
	int nWork = 0;
	VERTEX_2D *pVtx = NULL;

	g_pVtxBufferPlayerRank->Lock(0, 0, (void**)&pVtx, 0);

	//�X�R�A�v�Z
	for (int nCntPlayer = 0;nCntPlayer < NUM_PLAYER;nCntPlayer++)
	{
		nWork = g_aPlayerRankOutPut[nCntPlayer];
		g_aPlayerRankOutPut[nCntPlayer] = g_aPlayerRankOutPut[nCntPlayer] <= g_aPlayerRank[nCntPlayer] ? g_aPlayerRank[nCntPlayer] : g_aPlayerRankOutPut[nCntPlayer] - 1;

		for (int nCntDig = DEGITAL_MAX - 1;nCntDig >= 0;nCntDig--)
		{
			pVtx[nCntPlayer * 8 + nCntDig * 4 + 0].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 0.0f);
			pVtx[nCntPlayer * 8 + nCntDig * 4 + 1].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 0.0f);
			pVtx[nCntPlayer * 8 + nCntDig * 4 + 2].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 1.0f);
			pVtx[nCntPlayer * 8 + nCntDig * 4 + 3].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 1.0f);
			nWork /= 10;
		}
	}

	g_pVtxBufferPlayerRank->Unlock();
}

//--------------------------------------------------------------------------------
//  �`��֐�
//--------------------------------------------------------------------------------
void DrawPlayerRank(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferPlayerRank,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

								   //score
	for (int nCnt = 0;nCnt < NUM_PLAYER * DEGITAL_MAX;nCnt++)
	{
		//Texture�̐ݒ�
		pDevice->SetTexture(0, g_pTexturePlayerRank);

		//�v���~�e�B�u�`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//�I�t�Z�b�g�i���_���j
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPlayerRank(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NUM_PLAYER * DEGITAL_MAX,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferPlayerRank,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferPlayerRank->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//player
	for (int nCntDig = 0;nCntDig < DEGITAL_MAX;nCntDig++)
	{
		//���_���W�̐ݒ�i2D���W�A�E���j
		pVtx[nCntDig * 4 + 0].pos = D3DXVECTOR3(PLAYER_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), PLAYER_RANK_POS_Y, 0.0f);
		pVtx[nCntDig * 4 + 1].pos = D3DXVECTOR3(PLAYER_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, PLAYER_RANK_POS_Y, 0.0f);
		pVtx[nCntDig * 4 + 2].pos = D3DXVECTOR3(PLAYER_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), PLAYER_RANK_POS_Y + SCORE_HEIGHT, 0.0f);
		pVtx[nCntDig * 4 + 3].pos = D3DXVECTOR3(PLAYER_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, PLAYER_RANK_POS_Y + SCORE_HEIGHT, 0.0f);

		//rhw�̐ݒ�(�K��1.0f)
		pVtx[nCntDig * 4 + 0].rhw = 1.0f;
		pVtx[nCntDig * 4 + 1].rhw = 1.0f;
		pVtx[nCntDig * 4 + 2].rhw = 1.0f;
		pVtx[nCntDig * 4 + 3].rhw = 1.0f;

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[nCntDig * 4 + 0].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 1].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 2].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 3].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);

		//texture���_
		pVtx[nCntDig * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCntDig * 4 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCntDig * 4 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCntDig * 4 + 3].tex = D3DXVECTOR2(0.0f, 0.0f);
	}

	//enemy
	for (int nCntDig = 0;nCntDig < DEGITAL_MAX;nCntDig++)
	{
		//���_���W�̐ݒ�i2D���W�A�E���j
		pVtx[8 + nCntDig * 4 + 0].pos = D3DXVECTOR3(ENEMY_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), ENEMY_RANK_POS_Y, 0.0f);
		pVtx[8 + nCntDig * 4 + 1].pos = D3DXVECTOR3(ENEMY_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, ENEMY_RANK_POS_Y, 0.0f);
		pVtx[8 + nCntDig * 4 + 2].pos = D3DXVECTOR3(ENEMY_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), ENEMY_RANK_POS_Y + SCORE_HEIGHT, 0.0f);
		pVtx[8 + nCntDig * 4 + 3].pos = D3DXVECTOR3(ENEMY_RANK_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, ENEMY_RANK_POS_Y + SCORE_HEIGHT, 0.0f);

		//rhw�̐ݒ�(�K��1.0f)
		pVtx[8 + nCntDig * 4 + 0].rhw = 1.0f;
		pVtx[8 + nCntDig * 4 + 1].rhw = 1.0f;
		pVtx[8 + nCntDig * 4 + 2].rhw = 1.0f;
		pVtx[8 + nCntDig * 4 + 3].rhw = 1.0f;

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[8 + nCntDig * 4 + 0].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[8 + nCntDig * 4 + 1].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[8 + nCntDig * 4 + 2].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[8 + nCntDig * 4 + 3].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		//texture���_
		pVtx[8 + nCntDig * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[8 + nCntDig * 4 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[8 + nCntDig * 4 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[8 + nCntDig * 4 + 3].tex = D3DXVECTOR2(0.0f, 0.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferPlayerRank->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  Player�����N�ݒ�
//--------------------------------------------------------------------------------
void SetPlayerRank(int nRank)
{
	g_aPlayerRank[0] = nRank;
}

//--------------------------------------------------------------------------------
//  Enemy�����N�ݒ�
//--------------------------------------------------------------------------------
void SetEnemyRank(int nRank)
{
	g_aPlayerRank[1] = nRank;
}