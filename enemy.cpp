//--------------------------------------------------------------------------------
//
//�@�E�C���h�E�\���v���O����
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "enemy.h"
#include "player.h"
#include "ball.h"
#include "input.h"
#include "joystick.h"
#include "sound.h"
#include "tire_effect.h"
#include "NOS_effect.h"
#include "KF_Precision.h"
#include "KF_BodyContact.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define ENEMY_TEXTURENAME "data/TEXTURE/enemy.png"//�t�@�C����
#define ENEMY_POS (D3DXVECTOR3(GAME_AREA_RIGHT - 40.0f - CAR_WIDTH * 0.5f, (GAME_AREA_TOP + GAME_AREA_BOTTOM) * 0.5f,0.0f))
#define ENEMY_JOYSTICK_NUM (PS4_CONTROLLER_NUM)

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct {
	CBodyBox *pBodyBox;
	unsigned int nScore;
	D3DXVECTOR3 aTirePos[4];//�^�C�A�̈ʒu
	float fAngle;
	float fLength;
	int nNOS;//N2O�����V�X�e��
}ENEMY;

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexEnemy(LPDIRECT3DDEVICE9 pDevice);
void UpdateEnemyControl(void);
void UpdateEnemyTire(void);

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureEnemy = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferEnemy = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
ENEMY g_enemy;
int g_nCntPlaySeEnemy;
int g_nCntPlaySeNOSEnemy;

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitEnemy(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾
	
	g_nCntPlaySeEnemy = 0;
	g_enemy.nScore = 0;
	g_enemy.nNOS = NOS_MAX;
	g_nCntPlaySeNOSEnemy = 0;
	g_enemy.pBodyBox = new CBodyBox;
	g_enemy.pBodyBox->Init();
	g_enemy.pBodyBox->SetPosCenter(ENEMY_POS);
	g_enemy.pBodyBox->SetRotation(-D3DX_PI);
	g_enemy.pBodyBox->SetHalfSize(D3DXVECTOR2(CAR_WIDTH * 0.5f, CAR_HEIGHT * 0.5f));
	g_enemy.pBodyBox->SetMass(CAR_MASS);
	g_enemy.pBodyBox->SetMomentOfInertia(1.0f / 12.0f * CAR_MASS * (CAR_WIDTH * CAR_WIDTH + CAR_HEIGHT * CAR_HEIGHT));
	g_enemy.pBodyBox->Update();

	//�^�C�A�ʒu�p
	g_enemy.fLength = sqrtf(TIRE_POS_WIDTH * 0.5f * TIRE_POS_WIDTH * 0.5f + TIRE_POS_HEIGHT * 0.5f * TIRE_POS_HEIGHT * 0.5f);
	g_enemy.fAngle = atan2(TIRE_POS_HEIGHT * 0.5f, TIRE_POS_WIDTH * 0.5f);
	g_enemy.aTirePos[0] = D3DXVECTOR3(ENEMY_POS.x - TIRE_POS_WIDTH * 0.5f, ENEMY_POS.y - TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_enemy.aTirePos[1] = D3DXVECTOR3(ENEMY_POS.x + TIRE_POS_WIDTH * 0.5f, ENEMY_POS.y - TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_enemy.aTirePos[2] = D3DXVECTOR3(ENEMY_POS.x + TIRE_POS_WIDTH * 0.5f, ENEMY_POS.y + TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_enemy.aTirePos[3] = D3DXVECTOR3(ENEMY_POS.x - TIRE_POS_WIDTH * 0.5f, ENEMY_POS.y + TIRE_POS_HEIGHT * 0.5f, 0.0f);

	if (FAILED(MakeVerTexEnemy(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		ENEMY_TEXTURENAME,
		&g_pTextureEnemy)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitEnemy(void)
{
	if (g_enemy.pBodyBox != NULL)//�A�h���X�̒��g�m�F
	{
		g_enemy.pBodyBox->Uninit();//�I������
		delete g_enemy.pBodyBox;//���g���폜
		g_enemy.pBodyBox = NULL;//�A�h���X����ɂ���
	}

	if (g_pVtxBufferEnemy != NULL)
	{
		g_pVtxBufferEnemy->Release();
		g_pVtxBufferEnemy = NULL;
	}

	if (g_pTextureEnemy != NULL)
	{
		g_pTextureEnemy->Release();
		g_pTextureEnemy = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdateEnemy(void)
{
	D3DXVECTOR3 vPosToContactPos = VEC3_NULL;//���S�_�ƏՓ˓_�̃x�N�g��
	D3DXVECTOR3 vWork = VEC3_NULL;//�v�Z�p�x�N�g��
	
	VERTEX_2D *pVtx = NULL;

	g_enemy.pBodyBox->Update();

	if (Vector3Magnitude(g_enemy.pBodyBox->GetVelocity()) >= FLOAT_MIN && g_nCntPlaySeEnemy == 0)
	{
		g_nCntPlaySeEnemy = 60;
		PlaySound(SOUND_LABEL_SE_SAND_ENEMY);
	}

	if (g_nCntPlaySeEnemy != 0)
	{
		g_nCntPlaySeEnemy--;
	}

	if (g_nCntPlaySeNOSEnemy != 0)
	{
		g_nCntPlaySeNOSEnemy--;
	}

	//UpdateEnemyContact();

	UpdateEnemyTire();

	g_pVtxBufferEnemy->Lock(0, 0, (void**)&pVtx, 0);

	//pos
	pVtx[0].pos = g_enemy.pBodyBox->GetVertex(0);
	pVtx[1].pos = g_enemy.pBodyBox->GetVertex(1);
	pVtx[2].pos = g_enemy.pBodyBox->GetVertex(3);
	pVtx[3].pos = g_enemy.pBodyBox->GetVertex(2);

	g_pVtxBufferEnemy->Unlock();

	//UpdateEnemyControl();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawEnemy(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferEnemy,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	//Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureEnemy);

	for (int nCnt = 0; nCnt < NUM_ENEMY; nCnt++)
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
HRESULT MakeVerTexEnemy(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * NUM_ENEMY,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferEnemy,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferEnemy->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	for (int nCnt = 0; nCnt < NUM_ENEMY; nCnt++)
	{
	
		//���_���W�̐ݒ�i2D���W�A�E���j
		pVtx[0].pos = g_enemy.pBodyBox->GetVertex(0);
		pVtx[1].pos = g_enemy.pBodyBox->GetVertex(1);
		pVtx[2].pos = g_enemy.pBodyBox->GetVertex(3);
		pVtx[3].pos = g_enemy.pBodyBox->GetVertex(2);

		//rhw�̐ݒ�(�K��1.0f)
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		//���_�J���[�̐ݒ�(0�`255�̐����l)
		pVtx[0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		//texture���_
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferEnemy->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  �G�Փˌ��o
//--------------------------------------------------------------------------------
void UpdateEnemyContact(void)
{
	CBodyBox *pPlayer = NULL;
	CBodyCicle *pBall = GetBall();

	//player contact
	pPlayer = GetPlayer();
	if (CheckContactBTB(g_enemy.pBodyBox, pPlayer))
	{
		//StartJoystickShock(ENEMY_JOYSTICK_NUM, 30);
	}

	if (GetBallCnt() == 0)
	{
		CheckContactCTB(pBall, g_enemy.pBodyBox);
	}
	
	BoxAreaContact(g_enemy.pBodyBox);
}

//--------------------------------------------------------------------------------
//  enemy����X�V
//--------------------------------------------------------------------------------
void UpdateEnemyControl(void)
{
	D3DXVECTOR3 vNOSPos = VEC3_NULL;
	D3DXVECTOR3 vRotationForce = VEC3_NULL;
	D3DXVECTOR3 vRotationForcePos = VEC3_NULL;
	float fWorldAngle = 0.0f;//�S�̓I�ɂƂ��Ă̊p�x

	if (GetKeyboardPress(DIK_I) || GetJoystickPress(ENEMY_JOYSTICK_NUM, DIJ_BUTTON, DIJ_PS4_�~))//
	{
		g_enemy.pBodyBox->AddForce(D3DXVECTOR3(CAR_ACC * cosf(g_enemy.pBodyBox->GetRotation()),
			CAR_ACC * sinf(g_enemy.pBodyBox->GetRotation()), 0.0f));
	}

	if (GetKeyboardPress(DIK_K) || GetJoystickPress(ENEMY_JOYSTICK_NUM, DIJ_BUTTON, DIJ_PS4_��))//
	{
		g_enemy.pBodyBox->AddForce(D3DXVECTOR3(-CAR_ACC * 0.3f * cosf(g_enemy.pBodyBox->GetRotation()),
			-CAR_ACC * 0.5f * sinf(g_enemy.pBodyBox->GetRotation()), 0.0f));
	}

	if ((GetKeyboardPress(DIK_N) || GetJoystickPress(ENEMY_JOYSTICK_NUM, DIJ_BUTTON, DIJ_PS4_R1)))//NOS
	{
		if (g_enemy.nNOS >= 2)
		{
			g_enemy.nNOS -= 2;
			g_enemy.pBodyBox->AddForce(D3DXVECTOR3(CAR_NOS_ACC * cosf(g_enemy.pBodyBox->GetRotation()),
				CAR_NOS_ACC * sinf(g_enemy.pBodyBox->GetRotation()), 0.0f));
			vNOSPos = (g_enemy.pBodyBox->GetVertex(3) + g_enemy.pBodyBox->GetVertex(0)) * 0.5f;
			SetNOSEffect(vNOSPos, D3DXCOLOR(1.0f, 0.1f, 0.1f, 1.0f));

			if (g_nCntPlaySeNOSEnemy == 0)
			{
				g_nCntPlaySeNOSEnemy = 30;
				PlaySound(SOUND_LABEL_SE_NOS_ENEMY);
			}
		}
	}
	else
	{
		g_enemy.nNOS++;
		g_enemy.nNOS = g_enemy.nNOS >= NOS_MAX ? NOS_MAX : g_enemy.nNOS;
	}

	if (abs(GetJoystickStickStatus(ENEMY_JOYSTICK_NUM, DIJ_L_STICKX)) > STICK_MIN)//��]
	{
		vRotationForcePos = (g_enemy.pBodyBox->GetVertex(1) + g_enemy.pBodyBox->GetVertex(2)) * 0.5f;

		//�x�N�g�������E���ɂƂ��Ă̊p�x
		fWorldAngle = g_enemy.pBodyBox->GetRotation();

		//��]��p�͌v�Z
		vRotationForce = D3DXVECTOR3(0.0f, CAR_ROTATION_FORCE, 0.0f)
			* ((float)GetJoystickStickStatus(ENEMY_JOYSTICK_NUM, DIJ_L_STICKX) / (float)STICK_MAX);
		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

		g_enemy.pBodyBox->AddForce(vRotationForce, vRotationForcePos);
	}
	else
	{
		float fUnrealStick = 0.0f;

		if (GetKeyboardPress(DIK_U))
		{
			fUnrealStick = -500.0f;
		}
		else if (GetKeyboardPress(DIK_O))
		{
			fUnrealStick = 500.0f;
		}

		vRotationForcePos = (g_enemy.pBodyBox->GetVertex(1) + g_enemy.pBodyBox->GetVertex(2)) * 0.5f;

		//�x�N�g�������E���ɂƂ��Ă̊p�x
		fWorldAngle = g_enemy.pBodyBox->GetRotation();

		//��]��p�͌v�Z
		vRotationForce = D3DXVECTOR3(0.0f, CAR_ROTATION_FORCE, 0.0f)
			* (fUnrealStick / (float)STICK_MAX);
		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

		g_enemy.pBodyBox->AddForce(vRotationForce, vRotationForcePos);
	}
}

//--------------------------------------------------------------------------------
//  �^�C�A�ʒu�X�V
//--------------------------------------------------------------------------------
void UpdateEnemyTire(void)
{
	D3DXVECTOR3 vPos = VEC3_NULL;
	float fRotation = 0.0f;

	//�^�C�A�ʒu�X�V
	vPos = g_enemy.pBodyBox->GetPosCenter();
	fRotation = g_enemy.pBodyBox->GetRotation();

	g_enemy.aTirePos[0] = D3DXVECTOR3(vPos.x + cosf(fRotation + D3DX_PI + g_enemy.fAngle) * g_enemy.fLength, vPos.y + sin(fRotation + D3DX_PI + g_enemy.fAngle) * g_enemy.fLength, 0.0f);
	g_enemy.aTirePos[1] = D3DXVECTOR3(vPos.x + cosf(fRotation - g_enemy.fAngle) * g_enemy.fLength, vPos.y + sinf(fRotation - g_enemy.fAngle) * g_enemy.fLength, 0.0f);
	g_enemy.aTirePos[2] = D3DXVECTOR3(vPos.x + cosf(fRotation + g_enemy.fAngle) * g_enemy.fLength, vPos.y + sinf(fRotation + g_enemy.fAngle) * g_enemy.fLength, 0.0f);
	g_enemy.aTirePos[3] = D3DXVECTOR3(vPos.x + cosf(fRotation - D3DX_PI - g_enemy.fAngle) * g_enemy.fLength, vPos.y + sinf(fRotation - D3DX_PI - g_enemy.fAngle) * g_enemy.fLength, 0.0f);

	//effect
	if (Vector3Magnitude(g_enemy.pBodyBox->GetVelocity()) > FLOAT_MIN ||
		Vector3Magnitude(g_enemy.pBodyBox->GetRotationSpeed()) > FLOAT_MIN * 0.1f)
	{
		SetTireEffect(g_enemy.aTirePos[0]);
		SetTireEffect(g_enemy.aTirePos[1]);
		SetTireEffect(g_enemy.aTirePos[2]);
		SetTireEffect(g_enemy.aTirePos[3]);
	}
}

//--------------------------------------------------------------------------------
//  �G�A�h���X�擾
//--------------------------------------------------------------------------------
CBodyBox *GetEnemy(void)
{
	return g_enemy.pBodyBox;
}

//--------------------------------------------------------------------------------
//  �X�R�A���Z
//--------------------------------------------------------------------------------
void AddEnemyScore(int nScore)
{
	g_enemy.nScore += nScore;
}

//--------------------------------------------------------------------------------
//  �X�R�A�擾
//--------------------------------------------------------------------------------
unsigned int GetEnemyScore(void)
{
	return g_enemy.nScore;
}

//--------------------------------------------------------------------------------
//  NOS�擾
//--------------------------------------------------------------------------------
int GetEnemyNOS(void)
{
	return g_enemy.nNOS;
}

//--------------------------------------------------------------------------------
//  �ʒu�擾
//--------------------------------------------------------------------------------
D3DXVECTOR3 GetEnemyPos(void)
{
	return g_enemy.pBodyBox->GetPosCenter();
}

//--------------------------------------------------------------------------------
//  �p�x�擾
//--------------------------------------------------------------------------------
float GetEnemyRot(void)
{
	return g_enemy.pBodyBox->GetRotation();
}