//=============================================================================
//
// �G�l�~�[���� [enemy.cpp]
// �쐬�� : GP11A132 11 ���
// �쐬�� : 2023 / 12 / 06
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "debugproc.h"
#include "tatemono.h"
#include "collision.h"
#include "input.h"
#include "ui.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define	VALUE_MOVE			(2.5f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

static char* g_ObjectName[MAX_TATE] = {
	"data/MODEL/TokyoTowerPart1.obj",
	"data/MODEL/TokyoTowerPart2.obj",
	"data/MODEL/TokyoTowerPart3.obj",
	"data/MODEL/TokyoTowerPart4.obj",
};
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static TATE			g_Tate[MAX_TATE];				// �G�l�~�[
static int g_Level;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTate(void)
{
	g_Level = 0;
	for (int i = 0; i < MAX_TATE; i++)
	{
		LoadModel(g_ObjectName[i], &g_Tate[i].model);
		g_Tate[i].load = TRUE;

		g_Tate[i].pos = XMFLOAT3(0.0f, i*TATE_SIZE+10.0f, 0.0f);
		g_Tate[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Tate[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Tate[i].spd  = 1.0f;	// �ړ��X�s�[�h�N���A
		g_Tate[i].size = 10.0f;	// �����蔻��̑傫��
		g_Tate[i].count = 0;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Tate[i].model, &g_Tate[i].diffuse[0]);

		
		g_Tate[i].use = FALSE;
	}
	
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTate(void)
{

	for (int i = 0; i < MAX_TATE; i++)
	{
		if (g_Tate[i].load)
		{
			UnloadModel(&g_Tate[i].model);
			g_Tate[i].load = FALSE;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTate(void)
{
	UI* ui = GetUI();
	if (ui[UI_WOOD].count == 0 &&
		ui[UI_METAL].count == 0 &&
		ui[UI_CONCRETE].count == 0)
	{
		int tempLevel = 0;
		for (int i = 0; i < MAX_TATE; i++)
		{
			if (!g_Tate[i].use)
			{
				g_Tate[i].use = TRUE;
				SetMaterialReset();

				tempLevel++;
				break;
			}
		}

		g_Level = tempLevel;
		if (g_Level == 4)g_Level = 3;
	}


	for (int i = 0; i < MAX_TATE; i++)
	{
		if (g_Tate[i].use)
		{
			if (g_Tate[i].pos.y > (i * TATE_SIZE + 10.0f))
			{
				g_Tate[i].pos.y += VALUE_MOVE;
			}
		}
	}
#ifdef _DEBUG

#endif


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTate(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_TATE; i++)
	{
		if (g_Tate[i].use == FALSE) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Tate[i].scl.x, g_Tate[i].scl.y, g_Tate[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Tate[i].rot.x, g_Tate[i].rot.y + XM_PI, g_Tate[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Tate[i].pos.x, g_Tate[i].pos.y*i*0.7f, g_Tate[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Tate[i].mtxWorld, mtxWorld);


		// ���f���`��
		DrawModel(&g_Tate[i].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
TATE *GetTate()
{
	return &g_Tate[0];
}

int GetLevel()
{
	return g_Level;
}
