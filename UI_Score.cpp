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
#include "enemy.h"
#include "player.h"
#include "UI_Score.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define PLAYER_SCORE_POS_X (59.0f - 0.5f)//�|���S���̕\���ʒuX
#define PLAYER_SCORE_POS_Y (42.0f - 0.5f)//�|���S���̕\���ʒuY
#define ENEMY_SCORE_POS_X (824.0f - 0.5f)//�|���S���̕\���ʒuX
#define ENEMY_SCORE_POS_Y (42.0f - 0.5f)//�|���S���̕\���ʒuY
#define UI_SCORE_WIDTH (74.0f)
#define UI_SCORE_HEIGHT (124.0f)
#define UI_SCORE_SPACE (7.0f)
#define SCORE_TEXTURENAME "data/TEXTURE/num00.png"//�t�@�C����
#define DEGITAL_MAX (5)//����

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexUIScore(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct {
	unsigned int nScore;
	unsigned int nScoreOutput;
}UI_SCORE;

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureUIScore = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferUIScore = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
UI_SCORE g_aUIScore[2];

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitUIScore(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_pTextureUIScore = NULL;

	//result score init
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		g_aUIScore[nCnt].nScore = 0;
		g_aUIScore[nCnt].nScoreOutput = 0;
	}

	if (FAILED(MakeVerTexUIScore(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexUIScore ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, SCORE_TEXTURENAME, &g_pTextureUIScore)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitUIScore(void)
{
	//safe release
	if (g_pVtxBufferUIScore != NULL)
	{
		g_pVtxBufferUIScore->Release();
		g_pVtxBufferUIScore = NULL;
	}

	if (g_pTextureUIScore != NULL)
	{
		g_pTextureUIScore->Release();
		g_pTextureUIScore = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateUIScore(void)
{
	VERTEX_2D *pVtx = NULL;
	int nWork;

	//�X�R�A�擾
	g_aUIScore[0].nScore = GetPlayerScore();
	g_aUIScore[1].nScore = GetEnemyScore();

	g_pVtxBufferUIScore->Lock(0, 0, (void**)&pVtx, 0);

	//�X�R�A�v�Z
	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		nWork = g_aUIScore[nCnt].nScoreOutput;
		g_aUIScore[nCnt].nScoreOutput = g_aUIScore[nCnt].nScoreOutput >= g_aUIScore[nCnt].nScore ? g_aUIScore[nCnt].nScore : g_aUIScore[nCnt].nScoreOutput + 2;

		for (int nCntDig = DEGITAL_MAX - 1;nCntDig >= 0;nCntDig--)
		{
			pVtx[nCnt * 20 + nCntDig * 4 + 0].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 0.0f);
			pVtx[nCnt * 20 + nCntDig * 4 + 1].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 0.0f);
			pVtx[nCnt * 20 + nCntDig * 4 + 2].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 1.0f);
			pVtx[nCnt * 20 + nCntDig * 4 + 3].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 1.0f);
			nWork /= 10;

			if (nWork == 0) { break; }
		}
	}
	g_pVtxBufferUIScore->Unlock();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawUIScore(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferUIScore,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

	//score
	for (int nCnt = 0;nCnt < 2 * DEGITAL_MAX;nCnt++)
	{
		//Texture�̐ݒ�
		pDevice->SetTexture(0, g_pTextureUIScore);

		//�v���~�e�B�u�`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//�I�t�Z�b�g�i���_���j
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexUIScore(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * DEGITAL_MAX * 2,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferUIScore,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferUIScore->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	//player
	for (int nCntDig = 0;nCntDig < DEGITAL_MAX; nCntDig++)
	{
		pVtx[nCntDig * 4 + 0].pos = D3DXVECTOR3(PLAYER_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE), PLAYER_SCORE_POS_Y, 0.0f);
		pVtx[nCntDig * 4 + 1].pos = D3DXVECTOR3(PLAYER_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE) + UI_SCORE_WIDTH, PLAYER_SCORE_POS_Y, 0.0f);
		pVtx[nCntDig * 4 + 2].pos = D3DXVECTOR3(PLAYER_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE), PLAYER_SCORE_POS_Y + UI_SCORE_HEIGHT, 0.0f);
		pVtx[nCntDig * 4 + 3].pos = D3DXVECTOR3(PLAYER_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE) + UI_SCORE_WIDTH, PLAYER_SCORE_POS_Y + UI_SCORE_HEIGHT, 0.0f);

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[nCntDig * 4 + 0].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 1].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 2].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
		pVtx[nCntDig * 4 + 3].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	}

	//enemy
	for (int nCntDig = 0;nCntDig < DEGITAL_MAX; nCntDig++)
	{
		pVtx[20 + nCntDig * 4 + 0].pos = D3DXVECTOR3(ENEMY_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE), ENEMY_SCORE_POS_Y, 0.0f);
		pVtx[20 + nCntDig * 4 + 1].pos = D3DXVECTOR3(ENEMY_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE) + UI_SCORE_WIDTH, ENEMY_SCORE_POS_Y, 0.0f);
		pVtx[20 + nCntDig * 4 + 2].pos = D3DXVECTOR3(ENEMY_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE), ENEMY_SCORE_POS_Y + UI_SCORE_HEIGHT, 0.0f);
		pVtx[20 + nCntDig * 4 + 3].pos = D3DXVECTOR3(ENEMY_SCORE_POS_X + nCntDig * (UI_SCORE_WIDTH + UI_SCORE_SPACE) + UI_SCORE_WIDTH, ENEMY_SCORE_POS_Y + UI_SCORE_HEIGHT, 0.0f);

		pVtx[20 + nCntDig * 4 + 0].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[20 + nCntDig * 4 + 1].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[20 + nCntDig * 4 + 2].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[20 + nCntDig * 4 + 3].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	}

	//rhw�̐ݒ�(�K��1.0f)
	for (int nCnt = 0;nCnt < DEGITAL_MAX * 2;nCnt++)
	{
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//texture���_
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(0.0f, 0.0f);
	}
	
	//���z�A�h���X���
	g_pVtxBufferUIScore->Unlock();

	return S_OK;
}