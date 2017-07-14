//--------------------------------------------------------------------------------
//
//�@�E�C���h�E�\���v���O����
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "input.h"
#include "joystick.h"
#include "player.h"
#include "enemy.h"
#include "ball.h"
#include "fade.h"
#include "sound.h"
#include "tire_effect.h"
#include "NOS_effect.h"
#include "KF_Precision.h"
#include "KF_BodyContact.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define PLAYER_TEXTURENAME "data/TEXTURE/player.png"//�t�@�C����
#define PLAYER_POS (D3DXVECTOR3(40.0f + CAR_WIDTH * 0.5f, (GAME_AREA_TOP + GAME_AREA_BOTTOM) * 0.5f,0.0f))
#define PLAYER_JOYSTICK_NUM (XBOX_CONTROLLER_NUM)

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef struct {
	LPDIRECT3DTEXTURE9 pTexture;//texture�C���^�[�t�F�[�X
	LPDIRECT3DVERTEXBUFFER9 pVtxBuffer;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
	CBodyBox *pBodyBox;
	D3DXVECTOR3 aTirePos[4];//�^�C�A�̈ʒu
	float fAngle;
	float fLength;
	unsigned int nScore;
	int nNOS;//N2O�����V�X�e��
}PLAYER;

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPlayer(LPDIRECT3DDEVICE9 pDevice);
void UpdatePlayerControl(void);
void UpdatePlayerTire(void);

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
PLAYER g_player;
int g_nCntPlaySePlayer;
int g_nCntPlaySeNOSPlayer;

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_player.pTexture = NULL;
	g_player.pVtxBuffer = NULL;
	g_nCntPlaySePlayer = 0;
	g_nCntPlaySeNOSPlayer = 0;
	g_player.nScore = 0;
	g_player.nNOS = NOS_MAX;
	g_player.pBodyBox = new CBodyBox;
	g_player.pBodyBox->Init();
	g_player.pBodyBox->SetPosCenter(PLAYER_POS);
	g_player.pBodyBox->SetRotation(D3DX_PI * 2.0f);
	g_player.pBodyBox->SetHalfSize(D3DXVECTOR2(CAR_WIDTH * 0.5f, CAR_HEIGHT * 0.5f));
	g_player.pBodyBox->SetMass(CAR_MASS);
	g_player.pBodyBox->SetMomentOfInertia(1.0f / 12.0f * CAR_MASS * (CAR_WIDTH * CAR_WIDTH + CAR_HEIGHT * CAR_HEIGHT));

	//�^�C�A�ʒu�p
	g_player.fLength = sqrtf(TIRE_POS_WIDTH * 0.5f * TIRE_POS_WIDTH * 0.5f + TIRE_POS_HEIGHT * 0.5f * TIRE_POS_HEIGHT * 0.5f);
	g_player.fAngle = atan2(TIRE_POS_HEIGHT * 0.5f, TIRE_POS_WIDTH * 0.5f);
	g_player.aTirePos[0] = D3DXVECTOR3(PLAYER_POS.x - TIRE_POS_WIDTH * 0.5f, PLAYER_POS.y - TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_player.aTirePos[1] = D3DXVECTOR3(PLAYER_POS.x + TIRE_POS_WIDTH * 0.5f, PLAYER_POS.y - TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_player.aTirePos[2] = D3DXVECTOR3(PLAYER_POS.x + TIRE_POS_WIDTH * 0.5f, PLAYER_POS.y + TIRE_POS_HEIGHT * 0.5f, 0.0f);
	g_player.aTirePos[3] = D3DXVECTOR3(PLAYER_POS.x - TIRE_POS_WIDTH * 0.5f, PLAYER_POS.y + TIRE_POS_HEIGHT * 0.5f, 0.0f);

	if (FAILED(MakeVerTexPlayer(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexPlayer ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice,
		PLAYER_TEXTURENAME,
		&g_player.pTexture)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitPlayer(void)
{
	if (g_player.pBodyBox != NULL)
	{
		g_player.pBodyBox->Uninit();
		g_player.pBodyBox = NULL;
	}

	if (g_player.pVtxBuffer != NULL)
	{
		g_player.pVtxBuffer->Release();
		g_player.pVtxBuffer = NULL;
	}

	if (g_player.pTexture != NULL)
	{
		g_player.pTexture->Release();
		g_player.pTexture = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
void UpdatePlayer(void)
{
	//�ϐ��錾
	D3DXVECTOR3 vPosToContactPos;//���S�_�ƏՓ˓_�̃x�N�g��
	D3DXVECTOR3 vWork = VEC3_NULL;;//�v�Z�p�x�N�g��

	g_player.pBodyBox->Update();

	if (Vector3Magnitude(g_player.pBodyBox->GetVelocity()) >= FLOAT_MIN && g_nCntPlaySePlayer == 0)
	{
		g_nCntPlaySePlayer = 60;
		PlaySound(SOUND_LABEL_SE_SAND_PLAYER);
	}

	if (g_nCntPlaySePlayer != 0)
	{
		g_nCntPlaySePlayer--;
	}

	if (g_nCntPlaySeNOSPlayer != 0)
	{
		g_nCntPlaySeNOSPlayer--;
	}

	////�ՓˍX�V
	//UpdatePlayerContact();

	UpdatePlayerTire();
	
	VERTEX_2D *pVtx = NULL;

	g_player.pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	//pos
	pVtx[0].pos = g_player.pBodyBox->GetVertex(0);
	pVtx[1].pos = g_player.pBodyBox->GetVertex(1);
	pVtx[2].pos = g_player.pBodyBox->GetVertex(3);
	pVtx[3].pos = g_player.pBodyBox->GetVertex(2);

	g_player.pVtxBuffer->Unlock();

	//UpdatePlayerControl();
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawPlayer(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	//player
	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_player.pVtxBuffer,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	//���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	//Texture�̐ݒ�
	pDevice->SetTexture(0, g_player.pTexture);

	//�v���~�e�B�u�`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
		0,//�I�t�Z�b�g�i���_���j
		NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexPlayer(LPDIRECT3DDEVICE9 pDevice)
{
	//player
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_player.pVtxBuffer,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_player.pVtxBuffer->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	pVtx[0].pos = g_player.pBodyBox->GetVertex(0);
	pVtx[1].pos = g_player.pBodyBox->GetVertex(1);
	pVtx[2].pos = g_player.pBodyBox->GetVertex(3);
	pVtx[3].pos = g_player.pBodyBox->GetVertex(2);


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

	//���z�A�h���X���
	g_player.pVtxBuffer->Unlock();

	return S_OK;
}

//--------------------------------------------------------------------------------
//  player����X�V
//--------------------------------------------------------------------------------
void UpdatePlayerControl(void)
{
	//�ϐ��錾
	D3DXVECTOR3 vNOSPos = VEC3_NULL;
	D3DXVECTOR3 vRotationForce = VEC3_NULL;
	D3DXVECTOR3 vRotationForcePos = VEC3_NULL;
	float fWorldAngle = 0.0f;//�S�̓I�ɂƂ��Ă̊p�x

	if (GetKeyboardPress(DIK_W) || GetJoystickPress(PLAYER_JOYSTICK_NUM, DIJ_BUTTON, DIJ_XBOX_A))//�A�N�Z��
	{
		g_player.pBodyBox->AddForce(D3DXVECTOR3(CAR_ACC * cosf(g_player.pBodyBox->GetRotation()),
			CAR_ACC * sinf(g_player.pBodyBox->GetRotation()), 0.0f));
	}

	if (GetKeyboardPress(DIK_S) || GetJoystickPress(PLAYER_JOYSTICK_NUM, DIJ_BUTTON, DIJ_XBOX_B))//�u���[�L
	{
		g_player.pBodyBox->AddForce(D3DXVECTOR3(-CAR_ACC * 0.3f * cosf(g_player.pBodyBox->GetRotation()),
			-CAR_ACC * 0.3f * sinf(g_player.pBodyBox->GetRotation()), 0.0f));
	}

	if (GetKeyboardPress(DIK_X) || GetJoystickPress(PLAYER_JOYSTICK_NUM, DIJ_BUTTON, DIJ_XBOX_RB))//NOS
	{
		if (g_player.nNOS >= 2)
		{
			g_player.nNOS -= 2;
			g_player.pBodyBox->AddForce(D3DXVECTOR3(CAR_NOS_ACC * cosf(g_player.pBodyBox->GetRotation()),
				CAR_NOS_ACC * sinf(g_player.pBodyBox->GetRotation()), 0.0f));

			vNOSPos = (g_player.pBodyBox->GetVertex(3) + g_player.pBodyBox->GetVertex(0)) * 0.5f;
			SetNOSEffect(vNOSPos, D3DXCOLOR(0.1f, 0.1f, 1.0f, 1.0f));

			if (g_nCntPlaySeNOSPlayer == 0)
			{
				g_nCntPlaySeNOSPlayer = 30;
				PlaySound(SOUND_LABEL_SE_NOS_PLAYER);
			}
		}
	}
	else
	{
		g_player.nNOS++;
		g_player.nNOS = g_player.nNOS >= NOS_MAX ? NOS_MAX : g_player.nNOS;
	}
	
	if (abs(GetJoystickStickStatus(PLAYER_JOYSTICK_NUM, DIJ_L_STICKX)) > STICK_MIN)//��]
	{
		//��p�͂̈ʒu�v�Z
		vRotationForcePos = (g_player.pBodyBox->GetVertex(1) + g_player.pBodyBox->GetVertex(2)) * 0.5f;

		//�x�N�g�������E���ɂƂ��Ă̊p�x
		fWorldAngle = g_player.pBodyBox->GetRotation();

		//��]��p�͌v�Z
		vRotationForce = D3DXVECTOR3(0.0f, CAR_ROTATION_FORCE, 0.0f)
			* ((float)GetJoystickStickStatus(PLAYER_JOYSTICK_NUM, DIJ_L_STICKX) / (float)STICK_MAX);
		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

		g_player.pBodyBox->AddForce(vRotationForce, vRotationForcePos);
	}
	else 
	{
		float fUnrealStick = 0.0f;

		if (GetKeyboardPress(DIK_Q))
		{
			fUnrealStick = -500.0f;
		}
		else if (GetKeyboardPress(DIK_E))
		{
			fUnrealStick = 500.0f;
		}

		//��p�͂̈ʒu�v�Z
		vRotationForcePos = (g_player.pBodyBox->GetVertex(1) + g_player.pBodyBox->GetVertex(2)) * 0.5f;

		//�x�N�g�������E���ɂƂ��Ă̊p�x
		fWorldAngle = g_player.pBodyBox->GetRotation();

		//��]��p�͌v�Z
		vRotationForce = D3DXVECTOR3(0.0f, CAR_ROTATION_FORCE, 0.0f)
			* (fUnrealStick / (float)STICK_MAX);
		vRotationForce = D3DXVECTOR3(vRotationForce.x * cosf(fWorldAngle) - vRotationForce.y * sinf(fWorldAngle),
			vRotationForce.x * sinf(fWorldAngle) + vRotationForce.y * cosf(fWorldAngle), 0.0f);

		g_player.pBodyBox->AddForce(vRotationForce, vRotationForcePos);
	}
}

//--------------------------------------------------------------------------------
//  �^�C�A�ʒu�X�V
//--------------------------------------------------------------------------------
void UpdatePlayerTire(void)
{
	//�ϐ��錾
	D3DXVECTOR3 vPos = VEC3_NULL;
	float fRotation = 0.0f;

	//�^�C�A�ʒu�X�V
	vPos = g_player.pBodyBox->GetPosCenter();
	fRotation = g_player.pBodyBox->GetRotation();

	g_player.aTirePos[0] = D3DXVECTOR3(vPos.x + cosf(fRotation + D3DX_PI + g_player.fAngle) * g_player.fLength, vPos.y + sin(fRotation + D3DX_PI + g_player.fAngle) * g_player.fLength, 0.0f);
	g_player.aTirePos[1] = D3DXVECTOR3(vPos.x + cosf(fRotation - g_player.fAngle) * g_player.fLength, vPos.y + sinf(fRotation - g_player.fAngle) * g_player.fLength, 0.0f);
	g_player.aTirePos[2] = D3DXVECTOR3(vPos.x + cosf(fRotation + g_player.fAngle) * g_player.fLength, vPos.y + sinf(fRotation + g_player.fAngle) * g_player.fLength, 0.0f);
	g_player.aTirePos[3] = D3DXVECTOR3(vPos.x + cosf(fRotation - D3DX_PI - g_player.fAngle) * g_player.fLength, vPos.y + sinf(fRotation - D3DX_PI - g_player.fAngle) * g_player.fLength, 0.0f);

	//effect
	if (Vector3Magnitude(g_player.pBodyBox->GetVelocity()) > FLOAT_MIN ||
		Vector3Magnitude(g_player.pBodyBox->GetRotationSpeed()) > FLOAT_MIN * 0.1f)
	{
		SetTireEffect(g_player.aTirePos[0]);
		SetTireEffect(g_player.aTirePos[1]);
		SetTireEffect(g_player.aTirePos[2]);
		SetTireEffect(g_player.aTirePos[3]);
	}
}

//--------------------------------------------------------------------------------
//  �Փˌ��o
//--------------------------------------------------------------------------------
void UpdatePlayerContact(void)
{
	CBodyBox *pEnemy = NULL;
	CBodyCicle *pBall = NULL;

	//enemy contact
	pEnemy = GetEnemy();
	if (CheckContactBTB(g_player.pBodyBox, pEnemy))
	{
		//StartJoystickShock(PLAYER_JOYSTICK_NUM, 30);
	}

	//ball contact
	pBall = GetBall();
	if (GetBallCnt() == 0)
	{
		CheckContactCTB(pBall, g_player.pBodyBox);
	}
	
	//area contact
	BoxAreaContact(g_player.pBodyBox);
}

//--------------------------------------------------------------------------------
//  �X�R�A���Z
//--------------------------------------------------------------------------------
void AddPlayerScore(int nScore)
{
	g_player.nScore += nScore;
}

//--------------------------------------------------------------------------------
//  �X�R�A�擾
//--------------------------------------------------------------------------------
unsigned int GetPlayerScore(void)
{
	return g_player.nScore;
}

//--------------------------------------------------------------------------------
//  NOS�擾
//--------------------------------------------------------------------------------
int GetPlayerNOS(void)
{
	return g_player.nNOS;
}

//--------------------------------------------------------------------------------
//  �A�h���X�擾
//--------------------------------------------------------------------------------
CBodyBox *GetPlayer(void)
{
	return g_player.pBodyBox;
}

//--------------------------------------------------------------------------------
//  �ʒu�擾
//--------------------------------------------------------------------------------
D3DXVECTOR3 GetPlayerPos(void)
{
	return g_player.pBodyBox->GetPosCenter();
}

//--------------------------------------------------------------------------------
//  �p�x�擾
//--------------------------------------------------------------------------------
float GetPlayerRot(void)
{
	return g_player.pBodyBox->GetRotation();
}