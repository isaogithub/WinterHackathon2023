//=============================================================================
//
// UI処理 [ui.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"
#include "ui.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// キャラサイズ
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(UI_MAX)		// テクスチャの数

#define LIFE_MAX					(3)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/wood.png",
	"data/TEXTURE/metal.png",
	"data/TEXTURE/concrete.png",
	"data/TEXTURE/life.png",
	"data/TEXTURE/number16x32.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static BOOL						g_Load = FALSE;
static UI						g_UI[UI_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUI(void)
{
	ID3D11Device *pDevice = GetDevice();

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


	// プレイヤーの初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 500.0f, 20.0f, 0.0f };
	g_TexNo = 0;

	for (int i = 0; i < UI_MAX; i++)
	{
		g_UI[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_UI[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_UI[i].scl= XMFLOAT3(1.0f, 1.0f, 0.0f);

		g_UI[i].texNo = i;
		g_UI[i].w= TEXTURE_WIDTH;
		g_UI[i].h= TEXTURE_HEIGHT;

		g_UI[i].count = 0;
		g_UI[i].use = FALSE;
	}

	float base = 700.0f;
	float space = 70.0f;
	g_UI[UI_WOOD].pos = XMFLOAT3(base + space * UI_WOOD, space, 0.0f);
	g_UI[UI_WOOD].count = MATERIAL_COUNT;
	g_UI[UI_WOOD].use = TRUE;

	g_UI[UI_METAL].pos = XMFLOAT3(base + space * UI_METAL, space, 0.0f);
	g_UI[UI_METAL].count = MATERIAL_COUNT;
	g_UI[UI_METAL].use = TRUE;

	g_UI[UI_CONCRETE].pos = XMFLOAT3(base + space * UI_CONCRETE, space, 0.0f);
	g_UI[UI_CONCRETE].count = MATERIAL_COUNT;
	g_UI[UI_CONCRETE].use = TRUE;

	g_UI[UI_LIFE].pos = XMFLOAT3(base + space * UI_LIFE, space, 0.0f);
	g_UI[UI_LIFE].count = LIFE_MAX;
	g_UI[UI_LIFE].use = TRUE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitUI(void)
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
void UpdateUI(void)
{


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUI(void)
{
	if (GetMode() != MODE_GAME)return;
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

	for (int i = 0; i < UI_NUMBER; i++)
	{
		if (g_UI[i].use == FALSE)continue;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UI[i].texNo]);

		// スコアの位置やテクスチャー座標を反映
			// １枚のポリゴンの頂点とテクスチャ座標を設定
		float px = g_UI[i].pos.x;
		float py = g_UI[i].pos.y;
		float pw = g_UI[i].w;
		float ph = g_UI[i].h;

		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

	//UIの数字表示
	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_NUMBER]);
	for (int j = 0; j< UI_NUMBER; j++)
	{
		if (g_UI[j].use == FALSE)continue;

		// 桁数分処理する
		int number = g_UI[j].count;
		for (int i = 0; i < UI_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = (float)(number % 10);

			// スコアの位置やテクスチャー座標を反映
			float px = g_UI[j].pos.x - 20.0f* i;	// スコアの表示位置X
			float py = g_UI[j].pos.y + 50.0f;			// スコアの表示位置Y
			float pw = 16.0f;				// スコアの表示幅
			float ph = 32.0f;				// スコアの表示高さ

			float tw = 1.0f / 10;		// テクスチャの幅
			float th = 1.0f / 1;		// テクスチャの高さ
			float tx = x * tw;			// テクスチャの左上X座標
			float ty = 0.0f;			// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

			// 次の桁へ
			number /= 10;
		}
	}
	
}


//=============================================================================
// スコアを加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================


UI * GetUI(void)
{
	return &g_UI[0];
}

//=============================================================================
// WOOD
//=============================================================================

void AddWood(int add)
{
	g_UI[UI_WOOD].count+=add;
	if (g_UI[UI_WOOD].count <= 0)
	{
		g_UI[UI_WOOD].count = 0;
	}

}
void GetWood()
{

}

//=============================================================================
// METAL
//=============================================================================

void AddMetal(int add)
{
	g_UI[UI_METAL].count += add;
	if (g_UI[UI_METAL].count <= 0)
	{
		g_UI[UI_METAL].count = 0;
	}

}
void GetMetal(int add)
{

}

//=============================================================================
// コンクリート
//=============================================================================

void AddConcrete(int add)
{
	g_UI[UI_CONCRETE].count += add;
	if (g_UI[UI_CONCRETE].count <= 0)
	{
		g_UI[UI_CONCRETE].count = 0;
	}
}
void GetConcrete(int add)
{

}


void SetMaterialReset(void)
{
	g_UI[UI_WOOD].count = MATERIAL_COUNT;
	g_UI[UI_METAL].count = MATERIAL_COUNT;
	g_UI[UI_CONCRETE].count = MATERIAL_COUNT;
}