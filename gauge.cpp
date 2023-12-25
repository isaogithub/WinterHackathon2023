//=============================================================================
//
// ゲージ画面処理 [gauge.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "gauge.h"
#include "element.h"
#include "debugproc.h"
#include "ui.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define GAUGE_HEIGHT				(TEXTURE_HEIGHT / 4 * 3 + 10)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//struct ELEMENT
//{
//	XMFLOAT3 pos;
//	XMFLOAT3 rot;
//	XMFLOAT3 scl;
//	BOOL use;
//	int type;
//	float w, h;
//	float spd;
//};
//ELEMENT* GetElement(void) { return &g_Element[0]; }

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/gauge.png",
	"data/TEXTURE/wood.png",
	"data/TEXTURE/metal.png",
	"data/TEXTURE/concrete.png",
};

static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static BOOL						g_Load = FALSE;

static GAUGE g_Gauge[TEXTURE_MAX - 1];

static int g_gaugeType = GAUGE_MAX;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGauge(void)
{
	ID3D11Device* pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;

	// 素材の描画
	for (int i = 0; i < TEXTURE_MAX - 1; i++)
	{
		g_Gauge[i].use = TRUE;
		g_Gauge[i].w = ELEMENT_WIDTH;
		g_Gauge[i].h = ELEMENT_HEIGHT;
		g_Gauge[i].pos = XMFLOAT3(160.0f * i * 2 + 160.0f , GAUGE_HEIGHT, 0.0f);
		g_Gauge[i].texNo = i + 1;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGauge(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGauge(void)
{
	ELEMENT* element = GetElement();
	int elementNo = -1;
	int searchType = ELEMENTYPE_MAX;
	float element_h = -1.0;
	int gaugeType = GAUGE_MAX;

	if (GetKeyboardTrigger(DIK_Q) || GetKeyboardTrigger(DIK_LEFT))
	{
		searchType = WOOD;
	}
	else if (GetKeyboardTrigger(DIK_W) || GetKeyboardTrigger(DIK_DOWN))
	{
		searchType = METAL;
	}
	else if (GetKeyboardTrigger(DIK_E) || GetKeyboardTrigger(DIK_RIGHT))
	{
		searchType = CONCRETE;
	}

	// 素材の探索
	if (searchType != ELEMENTYPE_MAX)
	{
		for (int i = 0; i < ELEMENT_MAX; i++)
		{
			if (!element[i].use) continue;

			// 探索する素材の限定
			if (element[i].type == searchType)
			{
				// 座標が一番高い素材を選択
				if (element_h < element[i].pos.y)
				{
					elementNo = i;
					element_h = element[i].pos.y;
				}
			}
		}
	}

	// 素材が見つかったなら
	if (elementNo != -1 &&
		element[elementNo].type == searchType)
	{
		// 素材の高さを見て判定を決める
		if (GAUGE_HEIGHT - 100.0f < element[elementNo].pos.y &&
			element[elementNo].pos.y < GAUGE_HEIGHT + 100.0f)
		{
			gaugeType = GAUGE_BAD;

			if (GAUGE_HEIGHT - 50.0f < element[elementNo].pos.y &&
				element[elementNo].pos.y < GAUGE_HEIGHT + 50.0f)
			{
				gaugeType = GAUGE_NORMAL;

				if (GAUGE_HEIGHT - 20.0f < element[elementNo].pos.y &&
					element[elementNo].pos.y < GAUGE_HEIGHT + 20.0f)
				{
					gaugeType = GAUGE_GOOD;
				}
			}
		}
	}

	// 素材の画像を消す
	if (gaugeType != GAUGE_MAX)
	{
		g_gaugeType = gaugeType;
		switch (searchType)
		{
		case WOOD:
			PlaySound(SOUND_LABEL_SE_bomb000);
			AddWood(-1);
			break;
		case METAL:
			PlaySound(SOUND_LABEL_SE_bomb000);
			AddMetal(-1);
			break;
		case CONCRETE:
			PlaySound(SOUND_LABEL_SE_bomb000);
			AddConcrete(-1);
			break;
		}
		DeleteElement(elementNo);
	}
#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("gaugeType:%d\n", g_gaugeType);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGauge(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// ゲージを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 材料を描画
	for (int i = 0; i < TEXTURE_MAX - 1; i++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Gauge[i].texNo]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, g_Gauge[i].pos.x, g_Gauge[i].pos.y, g_Gauge[i].w, g_Gauge[i].h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.4f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}





