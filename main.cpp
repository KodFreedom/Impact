//--------------------------------------------------------------------------------
//
//　ウインドウ表示プログラム
//	Author : Xu Wenjie
//	Date   : 2016-04-26
//--------------------------------------------------------------------------------
//  Update : 2016-07-11
//	
//--------------------------------------------------------------------------------
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "input.h"
#include "joystick.h"
#include "main.h"
#include "KF_Precision.h"
#include "title.h"
#include "sample.h"
#include "game.h"
#include "result.h"
#include "fade.h"
#include "sound.h"
#include "main_close.h"

//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
#define CLASS_NAME "サンプル"//クラスの名前
#define WINDOW_NAME "IMPACT v1.00"//ウインドウの名前

//--------------------------------------------------------------------------------
//  関数ポインタ
//--------------------------------------------------------------------------------
typedef void(*ModeFunc)(void);
ModeFunc g_pInitFunc[] = { InitTitle,InitSample,InitGame,InitResult };
ModeFunc g_pUninitFunc[] = { UninitTitle,UninitSample,UninitGame,UninitResult };
ModeFunc g_pUpdateFunc[] = { UpdateTitle,UpdateSample,UpdateGame,UpdateResult };
ModeFunc g_pDrawFunc[] = { DrawTitle,DrawSample,DrawGame,DrawResult };

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(HWND *pWnd);
void Draw(void);
void Close(HWND *pWnd);

#ifdef _DEBUG
void DrawFPS(void);
#endif//_DEBUG

//--------------------------------------------------------------------------------
//  構造体定義
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//  グローバル変数
//--------------------------------------------------------------------------------
LPDIRECT3D9 g_pD3D = NULL;//Direct3Dインターフェース/オブジェクトへのポインタ
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;//Direct3Dデバイスへのポインタ
MODE g_mode = MODE_TITLE;//ゲームモード

#ifdef _DEBUG
LPD3DXFONT g_pFont = NULL;//フォントへのインターフェース
int g_nCntFPS = 0;//FPS Countor
#endif//_DEBUG

//--------------------------------------------------------------------------------
//  main関数
//--------------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(IpCmdLine);

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);//WNDCLASSEXのメモリサイズを指定
	wcex.style = CS_CLASSDC;//表示するウインドウのスタイルを設定
	wcex.lpfnWndProc = WndProc;//関数ポインタ、ウインドウプロシージャのアドレス（関数名）を指定
	wcex.cbClsExtra = 0;//通常は使用しないので０を指定
	wcex.cbWndExtra = 0;//通常は使用しないので０を指定
	wcex.hInstance = hInstance;//WinMainのパラメータのインスタンスハンドル
	wcex.hIcon = NULL;//自作icon出す、使用するアイコンを指定（Windowsがもっている）
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);//マウスカーソルを指定
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);//ウインドウのクライアント領域の背景色を設定
	wcex.lpszMenuName = NULL;//Windowにつけるメニューを設定（つけると重い）
	wcex.lpszClassName = CLASS_NAME;//ウインドウクラスの名前
	wcex.hIconSm = NULL;//拡張された部分（ミニicon）、小さいアイコンが設定された場合の情報を記述
	RegisterClassEx(&wcex);//ウインドウクラスの登録

	HWND hWnd;
	MSG msg;
	DWORD style = WS_OVERLAPPEDWINDOW ^ (WS_MAXIMIZEBOX | WS_THICKFRAME);
	RECT cr = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	RECT dr;

	//window size
	AdjustWindowRect(&cr, style, false);
	int ww = cr.right - cr.left;
	int wh = cr.bottom - cr.top;

	//window pos
	GetWindowRect(GetDesktopWindow(), &dr);
	int dw = dr.right - dr.left;
	int dh = dr.bottom - dr.top;

	//画面溢出防止
	int wx = ww > dw ? 0 : (dw - ww) / 2;
	int wy = wh > dh ? 0 : (dh - wh) / 2;

	//ウインドウを作成
	hWnd = CreateWindowEx(
		0,//拡張ウインドウスタイル
		CLASS_NAME,//クラスの名前
		WINDOW_NAME,//ウインドウの名前
		style,//**important**window type,ウインドウのスタイル
		wx,//ウインドウ左上座標X
		wy,//ウインドウ左上座標Y
		ww,//幅（ウインドウ全体）
		wh,//高さ（ウインドウ全体）
		NULL,//親ウィンドウのハンドル
		NULL,//メニューハンドルまたは子ウインドウID
		hInstance,//インスタンスハンドル
		NULL);//ウインドウ作成データ

	//ウインドウの表示
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//FPS
	DWORD dwFrameCount;
	DWORD dwCurrentTime;//現時間
	DWORD dwExecLastTime;//実行終了時時間
	DWORD dwFPSLastTime;

	//分解能を設定
	timeBeginPeriod(1);

	UINT nID = 0;//メッセージbox戻り値
	bool bWindow = true;//フルスクリーンフラッグ

	//フルスクリーンモード確認
	nID = MessageBox(hWnd, "フルスクリーンモードで起動しますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

	//押し判定
	if (nID == IDYES) {
		bWindow = false;
	}

	if (FAILED(Init(hInstance, hWnd, bWindow)))
	{
		MessageBox(NULL, "INIT ERROR!!", "エラー", MB_OK | MB_ICONWARNING);
	}

	//各カウンター初期化
	dwCurrentTime = 0;
	dwFrameCount = 0;
	dwExecLastTime =
	dwFPSLastTime = timeGetTime();//システム時刻をミリ秒単位で取得

	//メッセージループ
	for (;;)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			//window処理
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);//仮想キーメッセージを文字メッセージへ変換
				DispatchMessage(&msg);//ウインドウプロシージャへメッセージを送出
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwExecLastTime) >= TIMER_INTERVAL)
			{
				dwExecLastTime = dwCurrentTime;//処理した時間

#ifdef _DEBUG
				if ((dwCurrentTime - dwFPSLastTime) >= 500)//0.5秒ごとに実行
				{
					g_nCntFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);
					dwFPSLastTime = dwCurrentTime;
					dwFrameCount = 0;
				}
#endif//_DEBUG
				Update(&hWnd);
				Draw();

				dwFrameCount++;
			}
		}
	}

	//後片付け
	Uninit();

	//ウインドウクラスの登録お解除
	//第一引数：メクラス名
	//第二引数：アプリケーションインスタン
	UnregisterClass(CLASS_NAME, wcex.hInstance);
	return (int)msg.wParam;
}

//--------------------------------------------------------------------------------
//  ウインドウプロシージャ関数
//  (CALLBACK : Win32API関数を呼び出す時の規約)
//  hWnd ; ウインドウのハンドル
//  uMsg : メッセージの識別子
//  wPara : メッセージの最初のパラメータ
//  iParam : メッセージの二番目のパラメータ
//--------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) {
	case WM_CLOSE:
		Close(&hWnd);
		return 0;
	case WM_DESTROY://保存するかチェックの仕組みを作れる
		PostQuitMessage(0);//WM_QUITというメッセージを呼ぶ
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//--------------------------------------------------------------------------------
//  Init関数
//--------------------------------------------------------------------------------
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DPRESENT_PARAMETERS d3dpp;
	D3DDISPLAYMODE d3ddm;

	//キー初期化
	if (FAILED(InitKeyboard(hInstance,hWnd)))
	{
		return E_FAIL;
	}

	//Joystick初期化
	if (FAILED(InitJoystick(hInstance, hWnd)))
	{
		//return E_FAIL;
		MessageBox(hWnd, "Joystick初期化失敗！", "警告！", MB_ICONWARNING);
	}

	//サウンド初期化
	if (FAILED(InitSound(hWnd)))
	{
		return E_FAIL;
	}

	//Direct3Dインターフェースの取得
	g_pD3D = Direct3DCreate9(D3D9b_SDK_VERSION);

	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}

	//今のディスプレーモードを取得
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&d3ddm)))
	{
		return E_FAIL;
	}

	//deviceのプレゼントパラメーターの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	d3dpp.BackBufferFormat = d3ddm.Format;//fullscreenの時自分で決められる
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;//三年生の時習う！！
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;//三年生の時変える！！
	d3dpp.Windowed = bWindow;//window mode/fullscreen mode(WS_POPUP要る！)
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;//fullscreenのみ(60hz/59hz)
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;//fullscreenのみみたい
	//ほか

	//Direct3Ddevice作成
	//描画、頂点処理もハードウェアで行う
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,//頂点処理をハードウェアに任せ
		&d3dpp,
		&g_pD3DDevice)))//成功/失敗の情報をここに返却
	{
		//描画はハードウェア、頂点処理はCPUで行う
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,//※頂点処理をCPUに任せ
			&d3dpp,
			&g_pD3DDevice)))//成功/失敗の情報をここに返却
		{
			//描画、頂点処理もCPUで行う(3Dほどんど動きません)
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,//※変更
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,//※頂点処理をCPUに任せ
				&d3dpp,
				&g_pD3DDevice)))//成功/失敗の情報をここに返却
			{
				return E_FAIL;
			}
		}
	}

	//描画設定
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);//カリング
	g_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);//塗りつぶし

	//alpha
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);//アルファブレンド有効
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	//texture alpha
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	//サンプラーステート
	g_pD3DDevice->SetSamplerState(0, //layer
		D3DSAMP_MINFILTER,
		D3DTEXF_LINEAR);

	g_pD3DDevice->SetSamplerState(0, //layer
		D3DSAMP_ADDRESSU,
		D3DTADDRESS_WRAP);

	g_pD3DDevice->SetSamplerState(0, //layer
		D3DSAMP_ADDRESSV,
		D3DTADDRESS_WRAP);//WRAP反覆/CLAMP延びる/MIRROR

#ifdef _DEBUG
	//debug
	D3DXCreateFont(
		g_pD3DDevice,
		18,//文字の高さ
		0,//文字の幅（0は高さと合わせる幅）
		0,//太さ
		0,//ミップマップレベル数
		FALSE,//italic
		SHIFTJIS_CHARSET,//CharSet
		OUT_DEFAULT_PRECIS,//OutputPrecision
		DEFAULT_QUALITY,//Quality
		DEFAULT_PITCH,//PitchAndFamily
		"Terminal",//Font Name
		&g_pFont);
#endif//_DEBUG

	//モード設定
	//g_pInitFunc[g_mode]();
	InitMainClose();
	InitFade();
	SetMode(g_mode);
	return S_OK;
}

//--------------------------------------------------------------------------------
//  Uninit関数
//--------------------------------------------------------------------------------
void Uninit(void)
{
	//入力終了処理
	UninitJoystick();
	UninitKeyboard();
	
	UninitMainClose();
	UninitFade();

	for (int nCnt = 0;nCnt < MODE_MAX;nCnt++)
	{
		g_pUninitFunc[nCnt]();
	}

	//サウンド終了処理
	UninitSound();

	if (g_pD3DDevice != NULL)
	{
		g_pD3DDevice->Release();//Direct3Dデバイスの解放
		g_pD3DDevice = NULL;
	}

	if (g_pD3D != NULL)
	{
		g_pD3D->Release();//Direct3Dインターフェース解放
		g_pD3D = NULL;
	}

#ifdef _DEBUG
	if (g_pFont != NULL)
	{
		g_pFont->Release();
		g_pFont = NULL;
	}
#endif//_DEBUG
}

//--------------------------------------------------------------------------------
//  Update関数
//--------------------------------------------------------------------------------
void Update(HWND *pWnd)
{
	UpdateKeyboard();
	UpdateJoystick();

	if (UpdateMainClose(pWnd)) { return; }

	UpdateFade();
	g_pUpdateFunc[g_mode]();
}

//--------------------------------------------------------------------------------
//  Draw関数
//--------------------------------------------------------------------------------
void Draw(void)
{
	//画面(バックバッファ)のクリア
	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);

	//描画開始
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))//1frame once
	{
		g_pDrawFunc[g_mode]();
		DrawFade();
		DrawMainClose();
#ifdef _DEBUG
		DrawFPS();
#endif//_DEBUG
		g_pD3DDevice->EndScene();//描画終了
	}

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);//バックバッファとフロントバッファーのいり替え	
}

//--------------------------------------------------------------------------------
//  GetDevice関数
//--------------------------------------------------------------------------------
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

//--------------------------------------------------------------------------------
//  close確認関数
//--------------------------------------------------------------------------------
void Close(HWND *pWnd)
{
	UINT nID = 0;//メッセージbox戻り値

	//終了確認
	nID = MessageBox(*pWnd, "終了しますか？", "確認", MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON2);

	//押し判定
	if (nID == IDYES) {
		//WM_DESTROYメッセージを送信
		DestroyWindow(*pWnd);
	}
}

//--------------------------------------------------------------------------------
//  DrawFPS関数
//--------------------------------------------------------------------------------
#ifdef _DEBUG
void DrawFPS(void)
{
	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
	char aStr[256];
	DIJOYSTATE2 joyPS = GetJoystickStatus(0);
	DIJOYSTATE2 joyXBOX = GetJoystickStatus(1);

	sprintf(aStr, "FPS:%d\nPS4 X軸:%d\nPS4 Y軸:%d\nXBOX X軸:%d\nXBOX Y軸:%d\n", g_nCntFPS,joyPS.lX,joyPS.lY,joyXBOX.lX,joyXBOX.lY);

	g_pFont->DrawTextA(
		NULL,
		aStr,//文字列
		-1,//文字数-1はNULLまで
		&rect,//範囲
		DT_LEFT,
		D3DCOLOR_RGBA(255, 255, 255, 255));
}
#endif//_DEBUG

//--------------------------------------------------------------------------------
//  SetMode関数
//--------------------------------------------------------------------------------
void SetMode(MODE mode)
{
	//終了処理
	g_pUninitFunc[g_mode]();//初期化されてなくても安全に実行できるように実装する

	//初期化処理
	g_pInitFunc[mode]();

	g_mode = mode;
}

//--------------------------------------------------------------------------------
//  GetMode関数
//--------------------------------------------------------------------------------
MODE GetMode(void)
{
	return g_mode;
}