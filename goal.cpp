//--------------------------------------------------------------------------------
//
//�@Goal.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "goal.h"
#include "player.h"
#include "enemy.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define PLAYER_GOAL_POS_X (GAME_AREA_LEFT - 0.5f)//�|���S���̕\���ʒuX
#define PLAYER_GOAL_POS_Y (GAME_AREA_TOP - 0.5f)//�|���S���̕\���ʒuY
#define PLAYER_GOAL_WIDTH (39.0f)
#define PLAYER_GOAL_HEIGHT (GAME_AREA_BOTTOM - GAME_AREA_TOP)
#define ENEMY_GOAL_WIDTH (39.0f)
#define ENEMY_GOAL_HEIGHT (GAME_AREA_BOTTOM - GAME_AREA_TOP)
#define ENEMY_GOAL_POS_X (GAME_AREA_RIGHT - ENEMY_GOAL_WIDTH - 0.5f)//�|���S���̕\���ʒuX
#define ENEMY_GOAL_POS_Y (GAME_AREA_TOP - 0.5f)//�|���S���̕\���ʒuY
#define GOAL_TEXTURENAME "data/TEXTURE/fade.png"//�t�@�C����

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGoal(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct {
	float fAlpha;
	int nLife;
}GOAL;

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureGoal = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferGoal = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
GOAL g_aGoal[2];

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitGoal(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		g_aGoal[nCnt].nLife = 0;
		g_aGoal[nCnt].fAlpha = 0.5f;
	}

	if (FAILED(MakeVerTexGoal(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexGoal ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, GOAL_TEXTURENAME, &g_pTextureGoal)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitGoal(void)
{
	//safe release
	if (g_pVtxBufferGoal != NULL)
	{
		g_pVtxBufferGoal->Release();
		g_pVtxBufferGoal = NULL;
	}

	if (g_pTextureGoal != NULL)
	{
		g_pTextureGoal->Release();
		g_pTextureGoal = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateGoal(void)
{
	VERTEX_2D *pVtx;
	g_pVtxBufferGoal->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aGoal[nCnt].fAlpha);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aGoal[nCnt].fAlpha);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aGoal[nCnt].fAlpha);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, g_aGoal[nCnt].fAlpha);

		if (g_aGoal[nCnt].nLife % 10 < 5)
		{
			g_aGoal[nCnt].fAlpha = 0.5f;
		}
		else
		{
			g_aGoal[nCnt].fAlpha = 0.0f;
		}

		g_aGoal[nCnt].nLife = g_aGoal[nCnt].nLife == 0 ? 0 : g_aGoal[nCnt].nLife - 1;
	}

	g_pVtxBufferGoal->Unlock();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawGoal(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferGoal,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

								   //Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureGoal);

	for (int nCnt = 0;nCnt < 2;nCnt++)
	{
		//�v���~�e�B�u�`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//�I�t�Z�b�g�i���_���j
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGoal(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * 2,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferGoal,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferGoal->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	pVtx[0].pos = D3DXVECTOR3(PLAYER_GOAL_POS_X, PLAYER_GOAL_POS_Y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PLAYER_GOAL_POS_X + PLAYER_GOAL_WIDTH, PLAYER_GOAL_POS_Y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PLAYER_GOAL_POS_X, PLAYER_GOAL_POS_Y + PLAYER_GOAL_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PLAYER_GOAL_POS_X + PLAYER_GOAL_WIDTH, PLAYER_GOAL_POS_Y + PLAYER_GOAL_HEIGHT, 0.0f);

	pVtx[4].pos = D3DXVECTOR3(ENEMY_GOAL_POS_X, ENEMY_GOAL_POS_Y, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(ENEMY_GOAL_POS_X + ENEMY_GOAL_WIDTH, ENEMY_GOAL_POS_Y, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(ENEMY_GOAL_POS_X, ENEMY_GOAL_POS_Y + ENEMY_GOAL_HEIGHT, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(ENEMY_GOAL_POS_X + ENEMY_GOAL_WIDTH, ENEMY_GOAL_POS_Y + ENEMY_GOAL_HEIGHT, 0.0f);

	//rhw�̐ݒ�(�K��1.0f)
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;
	pVtx[4].rhw = 1.0f;
	pVtx[5].rhw = 1.0f;
	pVtx[6].rhw = 1.0f;
	pVtx[7].rhw = 1.0f;

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

	pVtx[4].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[5].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[6].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[7].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

	//texture���_
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	pVtx[4].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[5].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[6].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[7].tex = D3DXVECTOR2(1.0f, 1.0f);

	//���z�A�h���X���
	g_pVtxBufferGoal->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  goal check
//--------------------------------------------------------------------------------
bool GoalContact(D3DXVECTOR3 vBallPos, BALL_TYPE type)
{
	//player goal
	if (vBallPos.x <= PLAYER_GOAL_POS_X + PLAYER_GOAL_WIDTH)
	{
		g_aGoal[0].nLife = 60;
		AddEnemyScore((int)type * 150 + 100);
		return true;
	}

	//enemy goal
	if (vBallPos.x >= ENEMY_GOAL_POS_X)
	{
		g_aGoal[1].nLife = 60;
		AddPlayerScore((int)type * 150 + 100);
		return true;
	}

	return false;
}