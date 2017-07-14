//--------------------------------------------------------------------------------
//	衝突計算ヘッダー
//　KFBodyContact.h
//	Author : Xu Wenjie
//	Date   : 2016-07-27
//--------------------------------------------------------------------------------
//  Update : 
//	
//--------------------------------------------------------------------------------
#ifndef _KF_BODY_CONTACT_H_
#define _KF_BODY_CONTACT_H_

//--------------------------------------------------------------------------------
//  インクルードファイル
//--------------------------------------------------------------------------------
#include "KF_Body.h"
//--------------------------------------------------------------------------------
//  定数定義
//--------------------------------------------------------------------------------
//-------------------------------------------
//  回転しないタイプ
//-------------------------------------------
typedef struct {
	bool bRotation;
	CBody *apBody[2];
	D3DXVECTOR3 vContactNormal;//衝突法線
	D3DXVECTOR3 vContactPos;//衝突点
	float fPenetration;//衝突深さ マイナスになるとは衝突しない
}KF_CONTACT;

//-------------------------------------------
//  回転しないタイプ
//-------------------------------------------
//typedef struct {
//	CBody *apBody[2];
//	D3DXVECTOR3 vContactNormal;//衝突法線
//	float fPenetration;//衝突深さ マイナスになるとは衝突しない
//}KF_CONTACT_ROTATION;
////-------------------------------------------
////  円と円
////-------------------------------------------
//typedef struct {
//	CBodyCicle *apBodyCicle[2];
//	D3DXVECTOR3 vContactNormal;//衝突法線
//	float fPenetration;//衝突深さ マイナスになるとは衝突しない
//}KF_CONTACT_CICLE_TO_CICLE;
//
////-------------------------------------------
////  円と円
////-------------------------------------------
//typedef struct {
//	CBodyCicle *pBodyCicle;
//	CBodyBox *pBodyBox;
//	D3DXVECTOR3 vContactNormal;//衝突法線
//	float fPenetration;//衝突深さ マイナスになるとは衝突しない
//}KF_CONTACT_CICLE_TO_BOX;
//
////-------------------------------------------
////  円と円
////-------------------------------------------
//typedef struct {
//	CBodyBox *apBodyBox[2];
//	D3DXVECTOR3 vContactNormal;//衝突法線
//	float fPenetration;//衝突深さ マイナスになるとは衝突しない
//}KF_CONTACT_BOX_TO_BOX;

//--------------------------------------------------------------------------------
//  プロトタイプ宣言
//--------------------------------------------------------------------------------
void InitBodyContact(void);
void UninitBodyContact(void);
void UpdateBodyContact(void);
void SetBodyContact(bool bRotation, CBody *pBody1, CBody *pBody2, float fSetPenetration, D3DXVECTOR3 vSetContactNormal, D3DXVECTOR3 vSetContactPos);
bool CheckContactBTB(CBodyBox *pBody1, CBodyBox *pBody2);
void CheckContactCTC(CBodyCicle *pBody1, CBodyCicle *pBody2);
void CheckContactCTB(CBodyCicle *pBody1, CBodyBox *pBody2);
void CheckContactCTQ(CBodyCicle *pBody1, CBodyQuadrangle *pBody2);
void AreaContact(CBody *pBody, float fRadius);
void UpdateGroundContact(CBody *pBody, float fRadius);
void UpdateLeftWallContact(CBody *pBody, float fRadius);
void UpdateRightWallContact(CBody *pBody, float fRadius);
void UpdateTopWallContact(CBody *pBody, float fRadius);


void BoxAreaContact(CBodyBox *pBody);
void BoxUpdateGroundContact(CBodyBox *pBody);
void BoxUpdateLeftWallContact(CBodyBox *pBody);
void BoxUpdateRightWallContact(CBodyBox *pBody);
void BoxUpdateTopWallContact(CBodyBox *pBody);

void SetRestitution(float fRestitution);
#endif