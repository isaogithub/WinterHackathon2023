//=============================================================================
//
// UI���� [score.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define UI_DIGIT			(2)			// ����


//*****************************************************************************
// UI�\����
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
// �v���g�^�C�v�錾
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

