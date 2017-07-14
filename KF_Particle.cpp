//--------------------------------------------------------------------------------
//	粒子演算ヘッダー
//　KF_Particle.h
//	Author : Xu Wenjie
//	Date   : 2016-07-25
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "KF_Particle.h"
#include "KF_ParticleContact.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define NUM_PARTICLE (100)//作用力の最大数
#define PARTICLE_LENGTH (10.0f)
#define PARTICLE_TEXTURENAME "data/TEXTURE/bullet.png"//ファイル名


//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
HRESULT MakeVerTexParticle(LPDIRECT3DDEVICE9 pDevice);
void UpdateIntegrate(int nNumParticle);
void ClearAccumulator(int nNumParticle);
void UpdateGravity(int nNumParticle);
void UpdateDrag(int nNumParticle);
//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureParticle = NULL;//textureインターフェース
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferParticle = NULL;//頂点バッファ管理インターフェースポインタ
KF_PARTICLE g_aParticle[NUM_PARTICLE];

//--------------------------------------------------------------------------------
//  初期化処理
//--------------------------------------------------------------------------------
void InitParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得
	int nCnt;

	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		g_aParticle[nCnt].bUse = false;
		g_aParticle[nCnt].vPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].vVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].vAcceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].vForceAccum = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].vGravity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCnt].fInverseMass = 0.0f;
		g_aParticle[nCnt].fK1 = 0.0f;
		g_aParticle[nCnt].fK2 = 0.0f;
	}

	if (FAILED(MakeVerTexParticle(pDevice)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "MakeVerTexParticle ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//ハードディスクからTextureの読み込み
	//※エラーチェック必須
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		PARTICLE_TEXTURENAME,
		&g_pTextureParticle)))//textureポインタへのポインタ
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  終了処理
//--------------------------------------------------------------------------------
void UninitParticle(void)
{
	if (g_pTextureParticle != NULL)
	{
		g_pTextureParticle->Release();
		g_pTextureParticle = NULL;
	}
	
	if (g_pVtxBufferParticle != NULL)
	{
		g_pVtxBufferParticle->Release();
		g_pVtxBufferParticle = NULL;
	}
}

//--------------------------------------------------------------------------------
//  更新処理
//--------------------------------------------------------------------------------
void UpdateParticle(void)
{
	VERTEX_2D *pVtx = NULL;
	int nCnt;

	g_pVtxBufferParticle->Lock(0, 0, (void**)&pVtx, 0);
	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		if (g_aParticle[nCnt].bUse)
		{
			UpdateGravity(nCnt);
			UpdateDrag(nCnt);
			
			//速度計算
			UpdateIntegrate(nCnt);

			//位置計算
			g_aParticle[nCnt].vPosition = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x + g_aParticle[nCnt].vVelocity.x,
				g_aParticle[nCnt].vPosition.y + g_aParticle[nCnt].vVelocity.y,
				g_aParticle[nCnt].vPosition.z + g_aParticle[nCnt].vVelocity.z);

			ClearAccumulator(nCnt);

			//衝突計算（地面）
			UpdateGroundContact(&g_aParticle[nCnt]);
		}
	}
	g_pVtxBufferParticle->Unlock();
}

//--------------------------------------------------------------------------------
//  描画処理
//--------------------------------------------------------------------------------
void DrawParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//デバイス取得
	int nCnt;

	pDevice->SetStreamSource(
		0,//ストリーム番号
		g_pVtxBufferParticle,
		0,//オフセット（開始位置）
		sizeof(VERTEX_2D));//ストライド量

	pDevice->SetFVF(FVF_VERTEX_2D);//頂点フォーマットの設定
	pDevice->SetTexture(0, g_pTextureParticle);//Textureの設定

	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		//プリミティブ描画
		if (g_aParticle[nCnt].bUse)
		{
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCnt * 4, NUM_POLYGON);
		}
	}
}

//--------------------------------------------------------------------------------
//  頂点の作成
//--------------------------------------------------------------------------------
HRESULT MakeVerTexParticle(LPDIRECT3DDEVICE9 pDevice)
{
	int nCnt;

	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NUM_PARTICLE,//作成したい頂点バッファのサイズ
		D3DUSAGE_WRITEONLY,//頂点バッファの使用方法
		FVF_VERTEX_2D,//書かなくても大丈夫
		D3DPOOL_MANAGED,//メモリ管理方法(managed：デバイスにお任せ)
		&g_pVtxBufferParticle,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//仮想アドレスを取得するためのポインタ
	VERTEX_2D *pVtx;

	//頂点バッファをロックして、仮想アドレスを取得する
	g_pVtxBufferParticle->Lock(
		0,//範囲
		0,//範囲
		(void**)&pVtx,//アドレスが書かれたメモ帳のアドレス
		0);

	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		//頂点座標の設定（2D座標、右回り）
		pVtx[nCnt * 4].pos = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x - PARTICLE_LENGTH, g_aParticle[nCnt].vPosition.y - PARTICLE_LENGTH, 0.0f);
		pVtx[nCnt * 4 + 1].pos = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x + PARTICLE_LENGTH, g_aParticle[nCnt].vPosition.y - PARTICLE_LENGTH, 0.0f);
		pVtx[nCnt * 4 + 2].pos = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x - PARTICLE_LENGTH, g_aParticle[nCnt].vPosition.y + PARTICLE_LENGTH, 0.0f);
		pVtx[nCnt * 4 + 3].pos = D3DXVECTOR3(g_aParticle[nCnt].vPosition.x + PARTICLE_LENGTH, g_aParticle[nCnt].vPosition.y + PARTICLE_LENGTH, 0.0f);

		//rhwの設定(必ず1.0f)
		pVtx[nCnt * 4].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//頂点カラーの設定(0～255の整数値)
		pVtx[nCnt * 4].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		//texture頂点
		pVtx[nCnt * 4].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//仮想アドレス解放
	g_pVtxBufferParticle->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  積分計算（属性更新）
//
//  引数
//	rDuration：更新間隔
//--------------------------------------------------------------------------------
void UpdateIntegrate(int nNumParticle)
{
	//計算済み加速度
	D3DXVECTOR3 vResultAcc = g_aParticle[nNumParticle].vAcceleration;

	//位置更新
	g_aParticle[nNumParticle].vPosition += g_aParticle[nNumParticle].vVelocity * DURATION;

	//力によって加速度を更新
	vResultAcc += g_aParticle[nNumParticle].vForceAccum * g_aParticle[nNumParticle].fInverseMass;

	//速度更新
	g_aParticle[nNumParticle].vVelocity += vResultAcc * DURATION;

	//抵抗力(計算間隔によって抵抗力が変わる)
	//vVelocity *= REAL_POW(rDamping, rDuration);
}

//--------------------------------------------------------------------------------
//  作用力をクリアする
//--------------------------------------------------------------------------------
void ClearAccumulator(int nNumParticle)
{
	g_aParticle[nNumParticle].vForceAccum.x = 0.0f;
	g_aParticle[nNumParticle].vForceAccum.y = 0.0f;
	g_aParticle[nNumParticle].vForceAccum.z = 0.0f;
}

//--------------------------------------------------------------------------------
//  作用力を増加する
//--------------------------------------------------------------------------------
void AddForce(int nNumParticle, D3DXVECTOR3 vForce)
{
	g_aParticle[nNumParticle].vForceAccum += vForce;
}

//--------------------------------------------------------------------------------
//  重力更新
//--------------------------------------------------------------------------------
void UpdateGravity(int nNumParticle)
{
	//重力を作用力に加算
	AddForce(nNumParticle, g_aParticle[nNumParticle].vGravity / g_aParticle[nNumParticle].fInverseMass);
}

//--------------------------------------------------------------------------------
//  抵抗力更新
//--------------------------------------------------------------------------------
void UpdateDrag(int nNumParticle)
{
	D3DXVECTOR3 vForce = g_aParticle[nNumParticle].vVelocity;

	//抵抗力係数計算
	float fDragCoeff = Vector3Magnitude(vForce);//速度のスカラ値
	fDragCoeff = g_aParticle[nNumParticle].fK1 * fDragCoeff + g_aParticle[nNumParticle].fK2 * fDragCoeff * fDragCoeff;

	//抵抗力計算
	Vector3Normalize(&vForce);
	vForce *= -fDragCoeff;

	//抵抗力を作用力に加算
	AddForce(nNumParticle, vForce);
}

//--------------------------------------------------------------------------------
//  粒子設定
//--------------------------------------------------------------------------------
void SetParticle(float rSetInverseMass, D3DXVECTOR3 vSetPosition, D3DXVECTOR3 vSetVelocity, D3DXVECTOR3 vSetAcceleration)
{
	int nCnt;

	for (nCnt = 0; nCnt < NUM_PARTICLE; nCnt++)
	{
		if (g_aParticle[nCnt].bUse == false)
		{
			g_aParticle[nCnt].fInverseMass = rSetInverseMass;
			g_aParticle[nCnt].vPosition = vSetPosition;
			g_aParticle[nCnt].vVelocity = vSetVelocity;
			g_aParticle[nCnt].vAcceleration = vSetAcceleration;
			g_aParticle[nCnt].bUse = true;
			break;
		}
	}
}
