//--------------------------------------------------------------------------------
//  ranking.cpp
//  Author : XU WENJIE
//--------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS

//--------------------------------------------------------------------------------
//  �C���N���[�h�t�@�C��
//--------------------------------------------------------------------------------
#include <stdio.h>
#include "main.h"
#include "ranking.h"
#include "result_playerRank.h"
#include "player.h"
#include "enemy.h"

//--------------------------------------------------------------------------------
//  �}�N����`
//--------------------------------------------------------------------------------
#define NUM_PLAYER (100)         //�v���C���[�̐�
#define NUM_NAME (20)
#define SCORE_POS_X (520.0f)
#define SCORE_POS_Y (66.0f)
#define SCORE_WIDTH (58.0f)
#define SCORE_HEIGHT (100.0f)
#define SCORE_SPACE (6.5f)
#define SCORE_SPACE_HEIGHT (23.0f)
#define RANKING_MAX (5)
#define DEGITAL_MAX (5)//����
#define RANKING_TEXTURENAME "data/TEXTURE/num00.png"//�t�@�C����

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct
{
	//char aName[NUM_NAME];
	unsigned int nScore;
}PLAYERDATA;

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexRanking(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ��錾
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureRanking = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferRanking = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
bool g_bUpdated;

//--------------------------------------------------------------------------------
//  �������֐�
//--------------------------------------------------------------------------------
void InitRanking(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_bUpdated = false;

	if (FAILED(MakeVerTexRanking(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		RANKING_TEXTURENAME,
		&g_pTextureRanking)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  ��Еt���֐�
//--------------------------------------------------------------------------------
void UninitRanking(void)
{
	if (g_pVtxBufferRanking != NULL)
	{
		g_pVtxBufferRanking->Release();
		g_pVtxBufferRanking = NULL;
	}

	if (g_pTextureRanking != NULL)
	{
		g_pTextureRanking->Release();
		g_pTextureRanking = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V�֐�
//--------------------------------------------------------------------------------
void UpdateRanking(void)
{
	if (g_bUpdated) { return; }

	//�X�V
	PLAYERDATA aRankedData[NUM_PLAYER] = { 0 };
	PLAYERDATA aPlayerData[2] = { 0 };
	PLAYERDATA enemyData = { 0 };
	bool bPlayerFirst = false;
	FILE *pRanking = NULL;
	int nNumRanked = 0;
	int nCntPlayer = 0;
	int aPlayerRank[2] = { 0 };
	unsigned int nWork = 0;
	VERTEX_2D *pVtx = NULL;

	//data�擾
	aPlayerData[0].nScore = GetPlayerScore();
	aPlayerData[1].nScore = GetEnemyScore();

	//�����L���O���Ԃ����߂�
	bPlayerFirst = aPlayerData[0].nScore >= aPlayerData[1].nScore ? true : false;


	//data���邩�`�F�b�N
	//if (strlen(pPlayerData->playerInfo.aName) != 0)//copy playerdata
	//{
	//	for (nCntName = 0;nCntName < strlen(pPlayerData->playerInfo.aName);nCntName++)
	//	{
	//		g_playerData.aName[nCntName] = pPlayerData->playerInfo.aName[nCntName];
	//	}
	//	g_playerData.aName[nCntName] = NULL;
	//	g_playerData.fScore = pPlayerData->playerInfo.fScore;
	//	g_bPlayerData = true;
	//}

	//

	//load ranking
	pRanking = fopen("ranking.bin", "rb");
	if (pRanking != NULL)
	{
		fread(&nNumRanked, sizeof(nNumRanked), 1, pRanking);
		fread(aRankedData, sizeof(PLAYERDATA), nNumRanked, pRanking);
		fclose(pRanking);
	}

	if (nNumRanked == 0)//�t�@�C���������
	{
		for (int nCnt = 0;nCnt < 2;nCnt++)
		{
			if (bPlayerFirst)
			{
				nCntPlayer = nCnt;
			}
			else
			{
				nCntPlayer = (nCnt + 1) % 2;
			}
			//copy playerdata
			//for (int nCntName = 0;nCntName < strlen(aPlayerData[0].aName);nCntName++)
			//{
			//	aRankedData[nCnt].aName[nCntName] = playerData.aName[nCntName];
			//}
			//aRankedData[0].aName[nCntName] = NULL;

			aRankedData[nCnt].nScore = aPlayerData[nCntPlayer].nScore;
			nNumRanked += 1;
			aPlayerRank[nCntPlayer] = nCnt + 1;
		}
	}
	else
	{
		for (int nCnt = 0;nCnt < 2;nCnt++)
		{
			if (bPlayerFirst)
			{
				nCntPlayer = nCnt;
			}
			else
			{
				nCntPlayer = (nCnt + 1) % 2;
			}

			//�����L���O
			for (int nCntY = nNumRanked - 1;nCntY >= 0;nCntY--)
			{
				//����
				if (aPlayerData[nCntPlayer].nScore > aRankedData[nCntY].nScore)
				{
					////copy ranked
					//for (nCntName = 0;nCntName < strlen(aRankedData[nCntY].aName);nCntName++)
					//{
					//	aRankedData[nCntY + 1].aName[nCntName] = aRankedData[nCntY].aName[nCntName];
					//}
					//aRankedData[nCntY + 1].aName[nCntName] = NULL;
					aRankedData[nCntY + 1].nScore = aRankedData[nCntY].nScore;

					if (nCntY == 0)
					{
						//copy playerdata
						//for (nCntName = 0;nCntName < strlen(g_playerData.aName);nCntName++)
						//{
						//	aRankedData[nCntY].aName[nCntName] = g_playerData.aName[nCntName];
						//}
						//aRankedData[nCntY].aName[nCntName] = NULL;
						aRankedData[nCntY].nScore = aPlayerData[nCntPlayer].nScore;
						nNumRanked += 1;
						aPlayerRank[nCntPlayer] = 1;
						break;
					}
				}
				else
				{
					//copy playerdata
					//for (nCntName = 0;nCntName < strlen(g_playerData.aName);nCntName++)
					//{
					//	aRankedData[nCntY + 1].aName[nCntName] = g_playerData.aName[nCntName];
					//}
					//aRankedData[nCntY + 1].aName[nCntName] = NULL;
					aRankedData[nCntY + 1].nScore = aPlayerData[nCntPlayer].nScore;
					nNumRanked += 1;
					aPlayerRank[nCntPlayer] = nCntY + 2;
					break;
				}
			}
		}
	}

	//save ranking
	pRanking = fopen("ranking.bin", "wb");
	if (pRanking != NULL)
	{
		fwrite(&nNumRanked, sizeof(nNumRanked), 1, pRanking);
		fwrite(aRankedData, sizeof(PLAYERDATA), nNumRanked, pRanking);
		fclose(pRanking);
	}

	//�X�V�t���b�O
	g_bUpdated = true;

	//rank�ݒ�
	SetPlayerRank(aPlayerRank[0]);
	SetEnemyRank(aPlayerRank[1]);

	g_pVtxBufferRanking->Lock(0, 0, (void**)&pVtx, 0);

	//�X�R�A�v�Z
	for (int nCntRanking = 0;nCntRanking < RANKING_MAX;nCntRanking++)
	{
		nWork = aRankedData[nCntRanking].nScore;

		for (int nCntDig = DEGITAL_MAX - 1;nCntDig >= 0;nCntDig--)
		{
			pVtx[nCntRanking * 20 + nCntDig * 4 + 0].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 1].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 2].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10), 1.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 3].tex = D3DXVECTOR2(0.1f * (float)(nWork % 10) + 0.1f, 1.0f);
			nWork /= 10;

			if (aPlayerRank[0] == nCntRanking + 1)
			{
				pVtx[nCntRanking * 20 + nCntDig * 4 + 0].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
				pVtx[nCntRanking * 20 + nCntDig * 4 + 1].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
				pVtx[nCntRanking * 20 + nCntDig * 4 + 2].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
				pVtx[nCntRanking * 20 + nCntDig * 4 + 3].color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
			}

			if (aPlayerRank[1] == nCntRanking + 1)
			{
				pVtx[nCntRanking * 20 + nCntDig * 4 + 0].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
				pVtx[nCntRanking * 20 + nCntDig * 4 + 1].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
				pVtx[nCntRanking * 20 + nCntDig * 4 + 2].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
				pVtx[nCntRanking * 20 + nCntDig * 4 + 3].color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			}

			if (nWork == 0) { break; }
		}
	}

	g_pVtxBufferRanking->Unlock();
}

//--------------------------------------------------------------------------------
//  �`��֐�
//--------------------------------------------------------------------------------
void DrawRanking(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾z

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferRanking,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

	//score
	for (int nCnt = 0;nCnt < RANKING_MAX * DEGITAL_MAX;nCnt++)
	{
		//Texture�̐ݒ�
		pDevice->SetTexture(0, g_pTextureRanking);

		//�v���~�e�B�u�`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//�I�t�Z�b�g�i���_���j
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexRanking(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * RANKING_MAX * DEGITAL_MAX,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferRanking,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferRanking->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	for (int nCntRanking = 0; nCntRanking < RANKING_MAX; nCntRanking++)
	{
		for (int nCntDig = 0;nCntDig < DEGITAL_MAX;nCntDig++)
		{
			//���_���W�̐ݒ�i2D���W�A�E���j
			pVtx[nCntRanking * 20 + nCntDig * 4 + 0].pos = D3DXVECTOR3(SCORE_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), SCORE_POS_Y + nCntRanking * (SCORE_HEIGHT + SCORE_SPACE_HEIGHT), 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 1].pos = D3DXVECTOR3(SCORE_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, SCORE_POS_Y + nCntRanking * (SCORE_HEIGHT + SCORE_SPACE_HEIGHT), 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 2].pos = D3DXVECTOR3(SCORE_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), SCORE_POS_Y + nCntRanking * (SCORE_HEIGHT + SCORE_SPACE_HEIGHT) + SCORE_HEIGHT, 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 3].pos = D3DXVECTOR3(SCORE_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, SCORE_POS_Y + nCntRanking * (SCORE_HEIGHT + SCORE_SPACE_HEIGHT) + SCORE_HEIGHT, 0.0f);

			//rhw�̐ݒ�(�K��1.0f)
			pVtx[nCntRanking * 20 + nCntDig * 4 + 0].rhw = 1.0f;
			pVtx[nCntRanking * 20 + nCntDig * 4 + 1].rhw = 1.0f;
			pVtx[nCntRanking * 20 + nCntDig * 4 + 2].rhw = 1.0f;
			pVtx[nCntRanking * 20 + nCntDig * 4 + 3].rhw = 1.0f;

			//���_�J���[�̐ݒ�(0�`255�̐����l)
			pVtx[nCntRanking * 20 + nCntDig * 4 + 0].color = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 1].color = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 2].color = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 3].color = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);

			//texture���_
			pVtx[nCntRanking * 20 + nCntDig * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 3].tex = D3DXVECTOR2(0.0f, 0.0f);
		}
	}

	//���z�A�h���X���
	g_pVtxBufferRanking->Unlock();

	return S_OK;
}