//=============================================================================
//
// エネミー処理 [enemy.cpp]
// 作成者 : GP11A132 11 蔡文権
// 作成日 : 2023 / 12 / 06
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
// マクロ定義
//*****************************************************************************

#define	VALUE_MOVE			(2.5f)						// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// 回転量

static char* g_ObjectName[MAX_TATE] = {
	"data/MODEL/TokyoTowerPart1.obj",
	"data/MODEL/TokyoTowerPart2.obj",
	"data/MODEL/TokyoTowerPart3.obj",
	"data/MODEL/TokyoTowerPart4.obj",
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static TATE			g_Tate[MAX_TATE];				// エネミー
static int g_Level;

//=============================================================================
// 初期化処理
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

		g_Tate[i].spd  = 1.0f;	// 移動スピードクリア
		g_Tate[i].size = 10.0f;	// 当たり判定の大きさ
		g_Tate[i].count = 0;

		// モデルのディフューズを保存しておく。色変え対応の為。
		GetModelDiffuse(&g_Tate[i].model, &g_Tate[i].diffuse[0]);

		
		g_Tate[i].use = FALSE;
	}
	
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
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
// 描画処理
//=============================================================================
void DrawTate(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_TATE; i++)
	{
		if (g_Tate[i].use == FALSE) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Tate[i].scl.x, g_Tate[i].scl.y, g_Tate[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Tate[i].rot.x, g_Tate[i].rot.y + XM_PI, g_Tate[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Tate[i].pos.x, g_Tate[i].pos.y*i*0.7f, g_Tate[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Tate[i].mtxWorld, mtxWorld);


		// モデル描画
		DrawModel(&g_Tate[i].model);
	}

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// エネミーの取得
//=============================================================================
TATE *GetTate()
{
	return &g_Tate[0];
}

int GetLevel()
{
	return g_Level;
}
