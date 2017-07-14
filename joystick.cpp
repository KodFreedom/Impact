//--------------------------------------------------------------------------------
//
//　joystick.cpp
//	Author : Xu Wenjie
//	Date   : 2016-09-17
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "input.h"
#include "joystick.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define	NUM_BUTTON_MAX		(128)	// キーの最大数
#define	LIMIT_COUNT_REPEAT	(20)	// リピートカウントリミッター

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
BOOL CALLBACK EnumJoyCallback(const DIDEVICEINSTANCE *pDidInstance, VOID *pContext);
BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef);

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECTINPUTDEVICE8	g_apDevJoystick[NUM_JOYSTICK_MAX];								// 入力デバイス(Joystick)へのポインタ
//DIDEVCAPS				g_adiDevCaps[NUM_JOYSTICK_MAX];									// Joystickの能力情報
bool					g_abActive[NUM_JOYSTICK_MAX];									// ウインドウがアクティブの時trueになる変数
DIJOYSTATE2				g_adiJoystickState[NUM_JOYSTICK_MAX];							// Joystickの入力情報ワーク
DIJOYSTATE2				g_adiJoystickStateTrigger[NUM_JOYSTICK_MAX];					// Joystickのトリガー情報ワーク
DIJOYSTATE2				g_adiJoystickStateRelease[NUM_JOYSTICK_MAX];					// Joystickのリリース情報ワーク
DIJOYSTATE2				g_adiJoystickStateRepeat[NUM_JOYSTICK_MAX];						// Joystickのリピート情報ワーク
int						g_aJoystickStateRepeatCnt[NUM_JOYSTICK_MAX][NUM_BUTTON_MAX];	// Joystickのリピートカウンタ
int						g_nCntJoystick;													// Joystickカウンター

//--------------------------------------------------------------------------------
//  Joystick入力処理の初期化
//--------------------------------------------------------------------------------
HRESULT InitJoystick(HINSTANCE hInstance, HWND hWnd)
{
	LPDIRECTINPUT8 pInput = GetInputObject();// DirectInputオブジェクトへのポインタ

	for (g_nCntJoystick = 0; g_nCntJoystick < NUM_JOYSTICK_MAX;g_nCntJoystick++)
	{
		g_apDevJoystick[g_nCntJoystick] = NULL;  

		//// デバイスの作成
		//if (FAILED(pInput->CreateDevice(GUID_Joystick, &g_apDevJoystick[g_nCntJoystick], NULL)))
		//{ 
		//	MessageBox(hWnd, "Joystickがない！", "警告！", MB_ICONWARNING);
		//	return E_FAIL;
		//}

		// Joystickデバイスを列挙して作成
		if (FAILED(pInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyCallback, NULL, DIEDFL_ATTACHEDONLY)) || g_apDevJoystick[g_nCntJoystick] == NULL)
		{
			MessageBox(hWnd, "Joystickデバイスを列挙して作成に失敗！", "警告！", MB_ICONWARNING);
			return E_FAIL;
		}

		// データフォーマットを設定
		if (FAILED(g_apDevJoystick[g_nCntJoystick]->SetDataFormat(&c_dfDIJoystick2)))
		{
			MessageBox(hWnd, "Joystickのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
			return E_FAIL;
		}

		// 協調モードを設定（フォアグラウンド＆非排他モード）
		if (FAILED(g_apDevJoystick[g_nCntJoystick]->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
		{
			MessageBox(hWnd, "Joystickの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
			return E_FAIL;
		}

		// 軸の値の範囲の設定
		if (FAILED(g_apDevJoystick[g_nCntJoystick]->EnumObjects(EnumAxesCallback, NULL, DIDFT_AXIS)))
		{
			MessageBox(hWnd, "Joystickの軸の値の範囲を設定できませんでした。", "警告！", MB_ICONWARNING);
			return E_FAIL;
		}

		// キーボードへのアクセス権を獲得(入力制御開始)
		g_apDevJoystick[g_nCntJoystick]->Acquire();
	}

	return S_OK;
}

//--------------------------------------------------------------------------------
//  Joystick終了処理
//--------------------------------------------------------------------------------
void UninitJoystick(void)
{
	for (int nCnt = 0; nCnt < NUM_JOYSTICK_MAX;nCnt++)
	{
		if (g_apDevJoystick[nCnt] != NULL)
		{
			g_apDevJoystick[nCnt]->Unacquire();
			g_apDevJoystick[nCnt]->Release();
			g_apDevJoystick[nCnt] = NULL;
		}
	}
}

//--------------------------------------------------------------------------------
//  Joystick更新処理
//--------------------------------------------------------------------------------
void UpdateJoystick(void)
{
	DIJOYSTATE2	adiJoystickState[NUM_JOYSTICK_MAX];

	for (int nCnt = 0; nCnt < NUM_JOYSTICK_MAX;nCnt++)
	{
		if (g_apDevJoystick[nCnt] == NULL) { continue; }

		// 状態を取得する前にデータをポーリング
		if (FAILED(g_apDevJoystick[nCnt]->Poll()))
		{
			while (g_abActive[nCnt] && g_apDevJoystick[nCnt]->Acquire() == DIERR_INPUTLOST)
			{
				g_apDevJoystick[nCnt]->Acquire();
			}
		}

		// デバイスからデータを取得
		if (SUCCEEDED(g_apDevJoystick[nCnt]->GetDeviceState(sizeof(DIJOYSTATE2), &adiJoystickState[nCnt])))
		{
			//x軸の位置
			//キートリガー、リリース情報の取得
			g_adiJoystickStateTrigger[nCnt].lX = (g_adiJoystickState[nCnt].lX != adiJoystickState[nCnt].lX) * adiJoystickState[nCnt].lX;
			g_adiJoystickStateRelease[nCnt].lX = (g_adiJoystickState[nCnt].lX != adiJoystickState[nCnt].lX) * g_adiJoystickState[nCnt].lX;
			// キープレス情報を保存
			g_adiJoystickState[nCnt].lX = adiJoystickState[nCnt].lX;

			//y軸の位置
			//キートリガー、リリース情報の取得
			g_adiJoystickStateTrigger[nCnt].lY = (g_adiJoystickState[nCnt].lY != adiJoystickState[nCnt].lY) * adiJoystickState[nCnt].lY;
			g_adiJoystickStateRelease[nCnt].lY = (g_adiJoystickState[nCnt].lY != adiJoystickState[nCnt].lY) * g_adiJoystickState[nCnt].lY;
			// キープレス情報を保存
			g_adiJoystickState[nCnt].lY = adiJoystickState[nCnt].lY;

			//z軸の位置
			//キートリガー、リリース情報の取得
			g_adiJoystickStateTrigger[nCnt].lZ = (g_adiJoystickState[nCnt].lZ != adiJoystickState[nCnt].lZ) * adiJoystickState[nCnt].lZ;
			g_adiJoystickStateRelease[nCnt].lZ = (g_adiJoystickState[nCnt].lZ != adiJoystickState[nCnt].lZ) * g_adiJoystickState[nCnt].lZ;
			// キープレス情報を保存
			g_adiJoystickState[nCnt].lZ = adiJoystickState[nCnt].lZ;

			//x軸の回転
			//キートリガー、リリース情報の取得
			g_adiJoystickStateTrigger[nCnt].lRx = (g_adiJoystickState[nCnt].lRx != adiJoystickState[nCnt].lRx) * adiJoystickState[nCnt].lRx;
			g_adiJoystickStateRelease[nCnt].lRx = (g_adiJoystickState[nCnt].lRx != adiJoystickState[nCnt].lRx) * g_adiJoystickState[nCnt].lRx;
			// キープレス情報を保存
			g_adiJoystickState[nCnt].lRx = adiJoystickState[nCnt].lRx;

			//y軸の回転
			//キートリガー、リリース情報の取得
			g_adiJoystickStateTrigger[nCnt].lRy = (g_adiJoystickState[nCnt].lRy != adiJoystickState[nCnt].lRy) * adiJoystickState[nCnt].lRy;
			g_adiJoystickStateRelease[nCnt].lRy = (g_adiJoystickState[nCnt].lRy != adiJoystickState[nCnt].lRy) * g_adiJoystickState[nCnt].lRy;
			// キープレス情報を保存
			g_adiJoystickState[nCnt].lRy = adiJoystickState[nCnt].lRy;

			//z軸の回転
			//キートリガー、リリース情報の取得
			g_adiJoystickStateTrigger[nCnt].lRz = (g_adiJoystickState[nCnt].lRz != adiJoystickState[nCnt].lRz) * adiJoystickState[nCnt].lRz;
			g_adiJoystickStateRelease[nCnt].lRz = (g_adiJoystickState[nCnt].lRz != adiJoystickState[nCnt].lRz) * g_adiJoystickState[nCnt].lRz;
			// キープレス情報を保存
			g_adiJoystickState[nCnt].lRz = adiJoystickState[nCnt].lRz;

			//2つの追加軸
			for (int nCntSlider = 0;nCntSlider < 2;nCntSlider++)
			{
				//キートリガー、リリース情報の取得
				g_adiJoystickStateTrigger[nCnt].rglSlider[nCntSlider] = (g_adiJoystickState[nCnt].rglSlider[nCntSlider] != adiJoystickState[nCnt].rglSlider[nCntSlider]) * adiJoystickState[nCnt].rglSlider[nCntSlider];
				g_adiJoystickStateRelease[nCnt].rglSlider[nCntSlider] = (g_adiJoystickState[nCnt].rglSlider[nCntSlider] != adiJoystickState[nCnt].rglSlider[nCntSlider]) * g_adiJoystickState[nCnt].rglSlider[nCntSlider];
				// キープレス情報を保存
				g_adiJoystickState[nCnt].rglSlider[nCntSlider] = adiJoystickState[nCnt].rglSlider[nCntSlider];
			}

			//button
			for (int nCntButton = 0; nCntButton < NUM_BUTTON_MAX; nCntButton++)
			{
				//キートリガー、リリース情報の取得
				g_adiJoystickStateTrigger[nCnt].rgbButtons[nCntButton] = (g_adiJoystickState[nCnt].rgbButtons[nCntButton] ^ adiJoystickState[nCnt].rgbButtons[nCntButton]) & adiJoystickState[nCnt].rgbButtons[nCntButton];
				g_adiJoystickStateRelease[nCnt].rgbButtons[nCntButton] = (g_adiJoystickState[nCnt].rgbButtons[nCntButton] ^ adiJoystickState[nCnt].rgbButtons[nCntButton]) & g_adiJoystickState[nCnt].rgbButtons[nCntButton];

				//repeat
				if (adiJoystickState[nCnt].rgbButtons[nCntButton] & 0x80)
				{
					if (g_aJoystickStateRepeatCnt[nCnt][nCntButton] < LIMIT_COUNT_REPEAT)
					{
						g_aJoystickStateRepeatCnt[nCnt][nCntButton]++;
						if (g_aJoystickStateRepeatCnt[nCnt][nCntButton] == 1 || g_aJoystickStateRepeatCnt[nCnt][nCntButton] >= LIMIT_COUNT_REPEAT)
						{
							g_adiJoystickStateRepeat[nCnt].rgbButtons[nCntButton] = adiJoystickState[nCnt].rgbButtons[nCntButton];
						}
						else
						{
							g_adiJoystickStateRepeat[nCnt].rgbButtons[nCntButton] = 0;
						}
					}
				}
				else
				{
					g_adiJoystickStateRepeat[nCnt].rgbButtons[nCntButton] = 0;
					g_aJoystickStateRepeatCnt[nCnt][nCntButton] = 0;
				}

				// キープレス情報を保存
				g_adiJoystickState[nCnt].rgbButtons[nCntButton] = adiJoystickState[nCnt].rgbButtons[nCntButton];
			}
		}
		else
		{
			// キーボードへのアクセス権を取得
			g_apDevJoystick[nCnt]->Acquire();
		}
	}
}

//--------------------------------------------------------------------------------
//  Joystickのプレス状態を取得
//--------------------------------------------------------------------------------
bool GetJoystickPress(int nNumJoystick, DIJ dij, int nButton)
{
	if (nNumJoystick < 0 || nNumJoystick >= NUM_JOYSTICK_MAX) { return false; }

	switch (dij)
	{
	case DIJ_BUTTON:
		return (g_adiJoystickState[nNumJoystick].rgbButtons[nButton] & 0x80) ? true : false;

	case DIJ_POV:
		return (g_adiJoystickState[nNumJoystick].rgdwPOV[0] == nButton) ? true : false;

	case DIJ_L_STICKX:
		return (g_adiJoystickState[nNumJoystick].lX == nButton) ? true : false;

	case DIJ_L_STICKY:
		return (g_adiJoystickState[nNumJoystick].lY == nButton) ? true : false;

	case DIJ_R_STICKX:
		return (nButton - 10 <= g_adiJoystickState[nNumJoystick].lZ && g_adiJoystickState[nNumJoystick].lZ <= nButton) ? true : false;

	case DIJ_R_STICKY:
		return (nButton - 10 <= g_adiJoystickState[nNumJoystick].lRz && g_adiJoystickState[nNumJoystick].lRz <= nButton) ? true : false;
	}

	return false;
}

//--------------------------------------------------------------------------------
//  Joystickのトリガー状態を取得
//--------------------------------------------------------------------------------
bool GetJoystickTrigger(int nNumJoystick, DIJ dij, int nButton)
{
	switch (dij)
	{
	case DIJ_BUTTON:
		return (g_adiJoystickStateTrigger[nNumJoystick].rgbButtons[nButton] & 0x80) ? true : false;

	case DIJ_POV:
		return (g_adiJoystickStateTrigger[nNumJoystick].rgdwPOV[0] == nButton) ? true : false;

	case DIJ_L_STICKX:
		return (g_adiJoystickStateTrigger[nNumJoystick].lX == nButton) ? true : false;

	case DIJ_L_STICKY:
		return (g_adiJoystickStateTrigger[nNumJoystick].lY == nButton) ? true : false;

	case DIJ_R_STICKX:
		return (nButton - 10 <= g_adiJoystickStateTrigger[nNumJoystick].lZ && g_adiJoystickStateTrigger[nNumJoystick].lZ <= nButton) ? true : false;

	case DIJ_R_STICKY:
		return (nButton - 10 <= g_adiJoystickStateTrigger[nNumJoystick].lRz && g_adiJoystickStateTrigger[nNumJoystick].lRz <= nButton) ? true : false;
	}

	return false;
}

//--------------------------------------------------------------------------------
//  Joystickのリピート状態を取得
//--------------------------------------------------------------------------------
bool GetJoystickRepeat(int nNumJoystick, int nButton)
{
	return (g_adiJoystickStateRepeat[nNumJoystick].rgbButtons[nButton] & 0x80) ? true : false;
}

//--------------------------------------------------------------------------------
//  Joystickのリリ－ス状態を取得
//--------------------------------------------------------------------------------
bool GetJoystickRelease(int nNumJoystick, DIJ dij, int nButton)
{
	switch (dij)
	{
	case DIJ_BUTTON:
		return (g_adiJoystickStateRelease[nNumJoystick].rgbButtons[nButton] & 0x80) ? true : false;

	case DIJ_POV:
		return (g_adiJoystickStateRelease[nNumJoystick].rgdwPOV[0] == nButton) ? true : false;

	case DIJ_L_STICKX:
		return (g_adiJoystickStateRelease[nNumJoystick].lX == nButton) ? true : false;

	case DIJ_L_STICKY:
		return (g_adiJoystickStateRelease[nNumJoystick].lY == nButton) ? true : false;

	case DIJ_R_STICKX:
		return (nButton - 10 <= g_adiJoystickStateRelease[nNumJoystick].lZ && g_adiJoystickStateRelease[nNumJoystick].lZ <= nButton) ? true : false;

	case DIJ_R_STICKY:
		return (nButton - 10 <= g_adiJoystickStateRelease[nNumJoystick].lRz && g_adiJoystickStateRelease[nNumJoystick].lRz <= nButton) ? true : false;
	}

	return false;
}

//--------------------------------------------------------------------------------
//  列挙されたデバイスごとに呼び出されるコールバック関数
//--------------------------------------------------------------------------------
BOOL CALLBACK EnumJoyCallback(const DIDEVICEINSTANCE *pDidInstance, VOID *pContext)
{
	LPDIRECTINPUT8 pInput = GetInputObject();
	static GUID joysticDiscrimination[NUM_JOYSTICK_MAX];//各デバイスの識別子を格納
	DIDEVCAPS diDevCaps;// Joystickの能力情報

						//すでに取得済みのデバイスだった場合はもう一回実行する
	for (int nCnt = 0;nCnt < g_nCntJoystick;nCnt++)
	{
		if (joysticDiscrimination[nCnt] == pDidInstance->guidInstance)
		{
			return DIENUM_CONTINUE;
		}
	}

	//列挙されたJoystickへのインターフェイスを取得する
	if (FAILED(pInput->CreateDevice(pDidInstance->guidInstance, &g_apDevJoystick[g_nCntJoystick], NULL)))
	{
		return DIENUM_CONTINUE;//列挙を続ける
	}

	//Joystickの能力を調べる
	diDevCaps.dwSize = sizeof(DIDEVCAPS);
	if (FAILED(g_apDevJoystick[g_nCntJoystick]->GetCapabilities(&diDevCaps)))
	{
		//Joystick能力の取得に失敗
		g_apDevJoystick[g_nCntJoystick]->Release();
		g_apDevJoystick[g_nCntJoystick] = NULL;
		return DIENUM_CONTINUE;//列挙を続ける
	}

	//Joystickの能力をチェック

	//デバイスの識別子を保存
	joysticDiscrimination[g_nCntJoystick] = pDidInstance->guidInstance;

	//このデバイスを使う
	return DIENUM_STOP;//列挙を終了する場合
					   //return DIENUM_CONTINUE;//他のJoystickも探す場合
}

//--------------------------------------------------------------------------------
//  軸ごとに呼び出されるコールバック関数
//--------------------------------------------------------------------------------
BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
	DIPROPRANGE diprg;

	ZeroMemory(&diprg, sizeof(diprg));
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwObj = lpddoi->dwType;
	diprg.diph.dwHow = DIPH_BYID;
	diprg.lMin = -1000;
	diprg.lMax = +1000;

	if (FAILED(g_apDevJoystick[g_nCntJoystick]->SetProperty(DIPROP_RANGE, &diprg.diph)))
	{
		return DIENUM_STOP;
	}

	return DIENUM_CONTINUE;
}

//--------------------------------------------------------------------------------
//  ジョイスティックのスティック状態を取得
//--------------------------------------------------------------------------------
int GetJoystickStickStatus(int nNumJoystick, DIJ dij)
{
	switch (dij)
	{
	case DIJ_L_STICKX:
		return g_adiJoystickState[nNumJoystick].lX;

	case DIJ_L_STICKY:
		return g_adiJoystickState[nNumJoystick].lY;

	case DIJ_R_STICKX:
		return g_adiJoystickState[nNumJoystick].lRx;

	case DIJ_R_STICKY:
		return g_adiJoystickState[nNumJoystick].lRy;
	}

	return 0;
}

DIJOYSTATE2 GetJoystickStatus(int nCnt)
{
	return g_adiJoystickState[nCnt];
}