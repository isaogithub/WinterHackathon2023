//=============================================================================
//
// �X�R�A���� [score.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define ELEMENT_MAX			(50)
#define ELEMENT_WIDTH		(150.0f) 
#define ELEMENT_HEIGHT		(150.0f)

enum ELEMENTYPE
{
	WOOD,
	METAL,
	CONCRETE,
	ELEMENTYPE_MAX,
};

struct ELEMENT
{
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scl;
	BOOL use;
	int type;
	float w , h;
	float spd;
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitElement(void);
void UninitElement(void);
void UpdateElement(void);
void DrawElement(void);

void SetElement(int type, float initspd);
ELEMENT* GetElement(void);
void DeleteElement(int i);
