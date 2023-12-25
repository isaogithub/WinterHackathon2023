//=============================================================================
//
// UI処理 [score.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define UI_DIGIT			(2)			// 桁数


//*****************************************************************************
// UI構造体
//*****************************************************************************
enum UI_TYPE
{
	UI_WOOD,
	UI_METAL,
	UI_CONCRETE,
	UI_LIFE,
	UI_NUMBER,
	UI_MAX,
};

struct UI
{
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scl;

	int texNo;
	float w, h;
	int		count;
	BOOL use;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitUI(void);
void   UninitUI(void);
void   UpdateUI(void);
void   DrawUI(void);

void AddWood(int add);
void GetWood();

void AddMetal(int add);
void GetMetal(int add);

void AddConcrete(int add);
void GetConcrete(int add);

