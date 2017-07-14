//--------------------------------------------------------------------------------
//  ranking.cpp
//  Author : XU WENJIE
//--------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include <stdio.h>
#include "main.h"
#include "ranking.h"
#include "result_playerRank.h"
#include "player.h"
#include "enemy.h"

//--------------------------------------------------------------------------------
//  マクロ定義
//--------------------------------------------------------------------------------
#define NUM_PLAYER (100)         //プレイヤーの数
#define NUM_NAME (20)
#define SCORE_POS_X (520.0f)
#define SCORE_POS_Y (66.0f)
#define SCORE_WIDTH (58.0f)
#define SCORE_HEIGHT (100.0f)
#define SCORE_SPACE (6.5f)
#define SCORE_SPACE_HEIGHT (23.0f)
#define RANKING_MAX (5)
#define DEGITAL_MAX (5)//桁数
#define RANKING_TEXTURENAME "data/TEXTURE/num00.png"//ファイル名

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------
typedef struct
{
	//char aName[NUM_NAME];
	unsigned int nScore;
}PLAYERDATA;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexRanking(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  グローバル変数宣言
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureRanking = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferRanking = NULL;//頂点バッファ管理インターフェースポインタ
bool g_bUpdated;

//--------------------------------------------------------------------------------
//  初期化関数
//--------------------------------------------------------------------------------
void InitRanking(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得

	g_bUpdated = false;

	if (FAILED(MakeVerTexRanking(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		RANKING_TEXTURENAME,
		&g_pTextureRanking)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  後片付け関数
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
//  更新関数
//--------------------------------------------------------------------------------
void UpdateRanking(void)
{
	if (g_bUpdated) { return; }

	//更新
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

	//data取得
	aPlayerData[0].nScore = GetPlayerScore();
	aPlayerData[1].nScore = GetEnemyScore();

	//ランキング順番を決める
	bPlayerFirst = aPlayerData[0].nScore >= aPlayerData[1].nScore ? true : false;


	//dataあるかチェック
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

	if (nNumRanked == 0)//ファイルが空っぽ
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

			//ランキング
			for (int nCntY = nNumRanked - 1;nCntY >= 0;nCntY--)
			{
				//判定
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

	//更新フラッグ
	g_bUpdated = true;

	//rank設定
	SetPlayerRank(aPlayerRank[0]);
	SetEnemyRank(aPlayerRank[1]);

	g_pVtxBufferRanking->Lock(0, 0, (void**)&pVtx, 0);

	//スコア計算
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
//  描画関数
//--------------------------------------------------------------------------------
void DrawRanking(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得z

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferRanking,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定

	//score
	for (int nCnt = 0;nCnt < RANKING_MAX * DEGITAL_MAX;nCnt++)
	{
		//Textureの設定
		pDevice->SetTexture(0, g_pTextureRanking);

		//プリミティブ描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
			nCnt * 4,//オフセット（頂点数）
			NUM_POLYGON);
	}
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexRanking(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * RANKING_MAX * DEGITAL_MAX,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferRanking,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferRanking->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	for (int nCntRanking = 0; nCntRanking < RANKING_MAX; nCntRanking++)
	{
		for (int nCntDig = 0;nCntDig < DEGITAL_MAX;nCntDig++)
		{
			//頂点座標の設定（2D座標、右回り）
			pVtx[nCntRanking * 20 + nCntDig * 4 + 0].pos = D3DXVECTOR3(SCORE_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), SCORE_POS_Y + nCntRanking * (SCORE_HEIGHT + SCORE_SPACE_HEIGHT), 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 1].pos = D3DXVECTOR3(SCORE_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, SCORE_POS_Y + nCntRanking * (SCORE_HEIGHT + SCORE_SPACE_HEIGHT), 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 2].pos = D3DXVECTOR3(SCORE_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE), SCORE_POS_Y + nCntRanking * (SCORE_HEIGHT + SCORE_SPACE_HEIGHT) + SCORE_HEIGHT, 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 3].pos = D3DXVECTOR3(SCORE_POS_X + nCntDig * (SCORE_WIDTH + SCORE_SPACE) + SCORE_WIDTH, SCORE_POS_Y + nCntRanking * (SCORE_HEIGHT + SCORE_SPACE_HEIGHT) + SCORE_HEIGHT, 0.0f);

			//rhwの設定(必ず1.0f)
			pVtx[nCntRanking * 20 + nCntDig * 4 + 0].rhw = 1.0f;
			pVtx[nCntRanking * 20 + nCntDig * 4 + 1].rhw = 1.0f;
			pVtx[nCntRanking * 20 + nCntDig * 4 + 2].rhw = 1.0f;
			pVtx[nCntRanking * 20 + nCntDig * 4 + 3].rhw = 1.0f;

			//頂点カラーの設定(0～255の整数値)
			pVtx[nCntRanking * 20 + nCntDig * 4 + 0].color = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 1].color = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 2].color = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 3].color = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);

			//texture頂点
			pVtx[nCntRanking * 20 + nCntDig * 4 + 0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 1].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 2].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[nCntRanking * 20 + nCntDig * 4 + 3].tex = D3DXVECTOR2(0.0f, 0.0f);
		}
	}

	//仮想アドレス解放
	g_pVtxBufferRanking->Unlock();

	return S_OK;
}