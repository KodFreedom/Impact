//--------------------------------------------------------------------------------
//
//�@GamePause.cpp
//	Author : Xu Wenjie
//	Date   : 2016-06-07
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#include "main.h"
#include "game_pause.h"
#include "sound.h"
#include "fade.h"
#include "input.h"
#include "joystick.h"

//--------------------------------------------------------------------------------
//  �萔��`
//--------------------------------------------------------------------------------
#define PAUSE_POS_X (SCREEN_WIDTH * 0.5f)//�|���S���̕\���ʒuX
#define PAUSE_POS_Y (SCREEN_HEIGHT * 0.6f)//�|���S���̕\���ʒuY
#define PAUSE_WIDTH (400.0f)
#define PAUSE_HEIGHT (200.0f)
#define PAUSE_RESUME_POS_X (SCREEN_WIDTH * 0.5f)//�|���S���̕\���ʒuX
#define PAUSE_RESUME_POS_Y (SCREEN_HEIGHT * 0.6f - 30.0f)//�|���S���̕\���ʒuY
#define PAUSE_RESUME_WIDTH (251.0f)
#define PAUSE_RESUME_HEIGHT (50.0f)
#define PAUSE_RETURN_POS_X (SCREEN_WIDTH * 0.5f)//�|���S���̕\���ʒuX
#define PAUSE_RETURN_POS_Y (SCREEN_HEIGHT * 0.6f + 30.0f)//�|���S���̕\���ʒuY
#define PAUSE_RETURN_WIDTH (146.0f)
#define PAUSE_RETURN_HEIGHT (50.0f)
#define PAUSE_BG_POS_X (-0.5f)//�|���S���̕\���ʒuX
#define PAUSE_BG_POS_Y (-0.5f)//�|���S���̕\���ʒuY
#define PAUSE_BG_WIDTH (SCREEN_WIDTH)
#define PAUSE_BG_HEIGHT (SCREEN_HEIGHT)
#define PAUSE_TEXTURENAME "data/TEXTURE/gamepause.png"//�t�@�C����

//--------------------------------------------------------------------------------
//  �v���g�^�C�v�錾
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGamePause(LPDIRECT3DDEVICE9 pDevice);

//--------------------------------------------------------------------------------
//  �\���̒�`
//--------------------------------------------------------------------------------
typedef enum {
	PAUSE_NONE,
	PAUSE_RESUME,
	PAUSE_RETURN_TO_MENU,
	PAUSE_MAX
}PAUSE;

//--------------------------------------------------------------------------------
//  �O���[�o���ϐ�
//--------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9 g_pTextureGamePause = NULL;//texture�C���^�[�t�F�[�X
LPDIRECT3DVERTEXBUFFER9 g_pVtxBufferGamePause = NULL;//���_�o�b�t�@�Ǘ��C���^�[�t�F�[�X�|�C���^
PAUSE g_pause;

//--------------------------------------------------------------------------------
//  ����������
//--------------------------------------------------------------------------------
void InitGamePause(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	g_pause = PAUSE_NONE;

	if (FAILED(MakeVerTexGamePause(pDevice)))//texture�|�C���^�ւ̃|�C���^
	{
		MessageBox(NULL, "MakeVerTexGamePause ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}

	//�n�[�h�f�B�X�N����Texture�̓ǂݍ���
	//���G���[�`�F�b�N�K�{
	if (FAILED(D3DXCreateTextureFromFile(pDevice, PAUSE_TEXTURENAME, &g_pTextureGamePause)))
	{
		MessageBox(NULL, "D3DXCreateTextureFromFile ERROR!!", "�G���[", MB_OK | MB_ICONWARNING);
	}
}

//--------------------------------------------------------------------------------
//  �I������
//--------------------------------------------------------------------------------
void UninitGamePause(void)
{
	//safe release
	if (g_pVtxBufferGamePause != NULL)
	{
		g_pVtxBufferGamePause->Release();
		g_pVtxBufferGamePause = NULL;
	}

	if (g_pTextureGamePause != NULL)
	{
		g_pTextureGamePause->Release();
		g_pTextureGamePause = NULL;
	}
}

//--------------------------------------------------------------------------------
//  �X�V����
//--------------------------------------------------------------------------------
bool UpdateGamePause(void)
{
	//key����
	if (GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_BUTTON, DIJ_XBOX_MENU) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_BUTTON, DIJ_PS4_OPTIONS))//Pause
	{
		g_pause = PAUSE_RESUME;
	}

	if (g_pause == PAUSE_NONE) { return false; }

	//key����
	if (GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_L_STICKY, DIJ_XBOX_L_UP) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_L_STICKY, DIJ_PS4_L_UP) ||
		GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_L_STICKY, DIJ_XBOX_L_DOWN) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_L_STICKY, DIJ_PS4_L_DOWN))
	{
		g_pause = g_pause == PAUSE_RESUME ? PAUSE_RETURN_TO_MENU : PAUSE_RESUME;
	}


	if (GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_BUTTON, DIJ_XBOX_B) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_BUTTON, DIJ_PS4_��))
	{
		g_pause = PAUSE_NONE;
		return false;
	}

	if (GetJoystickTrigger(XBOX_CONTROLLER_NUM, DIJ_BUTTON, DIJ_XBOX_A) || GetJoystickTrigger(PS4_CONTROLLER_NUM, DIJ_BUTTON, DIJ_PS4_�~))
	{
		PlaySound(SOUND_LABEL_SE_PRESS);

		switch (g_pause)
		{
		case PAUSE_RESUME://refuse
			g_pause = PAUSE_NONE;
			return false;
			break;
		case PAUSE_RETURN_TO_MENU://return to menu
			SetFade(FADE_OUT, MODE_TITLE);
			return true;
			break;
		default:
			break;
		}
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;
	float fWork;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferGamePause->Lock(0, 0, (void**)&pVtx, 0);

	//texture���_
	for (int nCnt = 0;nCnt < PAUSE_MAX - 1;nCnt++)
	{
		fWork = (g_pause - nCnt) % 2 * 0.5f + 0.5f;
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
	}

	//���z�A�h���X���
	g_pVtxBufferGamePause->Unlock();

	return true;
}

//--------------------------------------------------------------------------------
//  �`�揈��
//--------------------------------------------------------------------------------
void DrawGamePause(void)
{
	if (g_pause == PAUSE_NONE) { return; }

	LPDIRECT3DDEVICE9 pDevice = GetDevice();//�f�o�C�X�擾

	pDevice->SetStreamSource(
		0,//�X�g���[���ԍ�
		g_pVtxBufferGamePause,
		0,//�I�t�Z�b�g�i�J�n�ʒu�j
		sizeof(VERTEX_2D));//�X�g���C�h��

	pDevice->SetFVF(FVF_VERTEX_2D);//���_�t�H�[�}�b�g�̐ݒ�

	//Texture�̐ݒ�
	pDevice->SetTexture(0, g_pTextureGamePause);

	//�v���~�e�B�u�`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 8, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4, NUM_POLYGON);
}

//--------------------------------------------------------------------------------
//  ���_�̍쐬
//--------------------------------------------------------------------------------
HRESULT MakeVerTexGamePause(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(
		sizeof(VERTEX_2D) * NUM_VERTEX * 4,//�쐬���������_�o�b�t�@�̃T�C�Y
		D3DUSAGE_WRITEONLY,//���_�o�b�t�@�̎g�p���@
		FVF_VERTEX_2D,//�����Ȃ��Ă����v
		D3DPOOL_MANAGED,//�������Ǘ����@(managed�F�f�o�C�X�ɂ��C��)
		&g_pVtxBufferGamePause,//
		NULL//
	)))
	{
		return E_FAIL;
	}

	//���z�A�h���X���擾���邽�߂̃|�C���^
	VERTEX_2D *pVtx;

	//���_�o�b�t�@�����b�N���āA���z�A�h���X���擾����
	g_pVtxBufferGamePause->Lock(
		0,//�͈�
		0,//�͈�
		(void**)&pVtx,//�A�h���X�������ꂽ�������̃A�h���X
		0);

	//���_���W�̐ݒ�i2D���W�A�E���j
	pVtx[0].pos = D3DXVECTOR3(PAUSE_RESUME_POS_X - PAUSE_RESUME_WIDTH * 0.5f, PAUSE_RESUME_POS_Y - PAUSE_RESUME_HEIGHT * 0.5f, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(PAUSE_RESUME_POS_X + PAUSE_RESUME_WIDTH * 0.5f, PAUSE_RESUME_POS_Y - PAUSE_RESUME_HEIGHT * 0.5f, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(PAUSE_RESUME_POS_X - PAUSE_RESUME_WIDTH * 0.5f, PAUSE_RESUME_POS_Y + PAUSE_RESUME_HEIGHT * 0.5f, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(PAUSE_RESUME_POS_X + PAUSE_RESUME_WIDTH * 0.5f, PAUSE_RESUME_POS_Y + PAUSE_RESUME_HEIGHT * 0.5f, 0.0f);

	pVtx[4].pos = D3DXVECTOR3(PAUSE_RETURN_POS_X - PAUSE_RETURN_WIDTH * 0.5f, PAUSE_RETURN_POS_Y - PAUSE_RETURN_HEIGHT * 0.5f, 0.0f);
	pVtx[5].pos = D3DXVECTOR3(PAUSE_RETURN_POS_X + PAUSE_RETURN_WIDTH * 0.5f, PAUSE_RETURN_POS_Y - PAUSE_RETURN_HEIGHT * 0.5f, 0.0f);
	pVtx[6].pos = D3DXVECTOR3(PAUSE_RETURN_POS_X - PAUSE_RETURN_WIDTH * 0.5f, PAUSE_RETURN_POS_Y + PAUSE_RETURN_HEIGHT * 0.5f, 0.0f);
	pVtx[7].pos = D3DXVECTOR3(PAUSE_RETURN_POS_X + PAUSE_RETURN_WIDTH * 0.5f, PAUSE_RETURN_POS_Y + PAUSE_RETURN_HEIGHT * 0.5f, 0.0f);

	pVtx[8].pos = D3DXVECTOR3(PAUSE_POS_X - PAUSE_WIDTH * 0.5f, PAUSE_POS_Y - PAUSE_HEIGHT * 0.5f, 0.0f);
	pVtx[9].pos = D3DXVECTOR3(PAUSE_POS_X + PAUSE_WIDTH * 0.5f, PAUSE_POS_Y - PAUSE_HEIGHT * 0.5f, 0.0f);
	pVtx[10].pos = D3DXVECTOR3(PAUSE_POS_X - PAUSE_WIDTH * 0.5f, PAUSE_POS_Y + PAUSE_HEIGHT * 0.5f, 0.0f);
	pVtx[11].pos = D3DXVECTOR3(PAUSE_POS_X + PAUSE_WIDTH * 0.5f, PAUSE_POS_Y + PAUSE_HEIGHT * 0.5f, 0.0f);

	//rhw�̐ݒ�(�K��1.0f)
	for (int nCnt = 0;nCnt < PAUSE_MAX - 1;nCnt++)
	{
		float fWork = (g_pause - nCnt) % 2 * 0.5f + 0.5f;
		pVtx[nCnt * 4 + 0].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 1].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 2].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
		pVtx[nCnt * 4 + 3].color = D3DXCOLOR(fWork, fWork, fWork, 1.0f);
	}

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[8 + 0].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[8 + 1].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[8 + 2].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[8 + 3].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	for (int nCnt = 0;nCnt < PAUSE_MAX;nCnt++)
	{
		pVtx[nCnt * 4 + 0].rhw = 1.0f;
		pVtx[nCnt * 4 + 1].rhw = 1.0f;
		pVtx[nCnt * 4 + 2].rhw = 1.0f;
		pVtx[nCnt * 4 + 3].rhw = 1.0f;

		//texture���_
		pVtx[nCnt * 4 + 0].tex = D3DXVECTOR2(0.0f, 1.0f / 3.0f * nCnt);
		pVtx[nCnt * 4 + 1].tex = D3DXVECTOR2(1.0f, 1.0f / 3.0f * nCnt);
		pVtx[nCnt * 4 + 2].tex = D3DXVECTOR2(0.0f, 1.0f / 3.0f * nCnt + 1.0f / 3.0f);
		pVtx[nCnt * 4 + 3].tex = D3DXVECTOR2(1.0f, 1.0f / 3.0f * nCnt + 1.0f / 3.0f);
	}

	pVtx[12].pos = D3DXVECTOR3(PAUSE_BG_POS_X, PAUSE_BG_POS_Y, 0.0f);
	pVtx[13].pos = D3DXVECTOR3(PAUSE_BG_POS_X + PAUSE_BG_WIDTH, PAUSE_BG_POS_Y, 0.0f);
	pVtx[14].pos = D3DXVECTOR3(PAUSE_BG_POS_X, PAUSE_BG_POS_Y + PAUSE_BG_HEIGHT, 0.0f);
	pVtx[15].pos = D3DXVECTOR3(PAUSE_BG_POS_X + PAUSE_BG_WIDTH, PAUSE_BG_POS_Y + PAUSE_BG_HEIGHT, 0.0f);

	//���_�J���[�̐ݒ�(0�`255�̐����l)
	pVtx[12].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[13].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[14].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	pVtx[15].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

	pVtx[12].rhw = 1.0f;
	pVtx[13].rhw = 1.0f;
	pVtx[14].rhw = 1.0f;
	pVtx[15].rhw = 1.0f;

	//texture���_
	pVtx[12].tex = D3DXVECTOR2(0.98f, 0.98f);
	pVtx[13].tex = D3DXVECTOR2(0.99f, 0.98f);
	pVtx[14].tex = D3DXVECTOR2(0.98f, 0.99f);
	pVtx[15].tex = D3DXVECTOR2(0.99f, 0.99f);

	//���z�A�h���X���
	g_pVtxBufferGamePause->Unlock();

	return S_OK;
}