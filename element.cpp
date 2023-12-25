//=============================================================================
//
// スコア処理 [score.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"
#include "element.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(80)	// 
#define TEXTURE_HEIGHT				(80)	// 
#define TEXTURE_MAX					(ELEMENTYPE_MAX)	// テクスチャの数


#define ELEMENT_BASESPEED			(1.0f)
#define ELEMENT_SPEED_MAX			(4.0f)
#define ELEMENT_SPACE				(100.0f)
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

};


static ELEMENT g_Element[ELEMENT_MAX];

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitElement(void)
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


	// 初期化
	for (int i = 0; i < ELEMENT_MAX; i++)
	{
		g_Element[i].pos = XMFLOAT3(0.0f,0.0f,0.0f);
		g_Element[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Element[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Element[i].w = ELEMENT_WIDTH;
		g_Element[i].h = ELEMENT_HEIGHT;
		g_Element[i].type = WOOD;
		g_Element[i].use = FALSE;
		g_Element[i].spd = ELEMENT_BASESPEED;
	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitElement(void)
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
void UpdateElement(void)
{

	for (int i = 0; i < ELEMENT_MAX; i++)
	{
		if (g_Element[i].use == FALSE)continue;

		XMVECTOR v_spd = { 0.0f,g_Element[i].spd,0.0f };
		XMStoreFloat3(&g_Element[i].pos, XMLoadFloat3(&g_Element[i].pos) + v_spd);

		//一番下に落ちたら消す
		if (g_Element[i].pos.y > SCREEN_HEIGHT)g_Element[i].use = FALSE;
	}

	//エレメントの生成処理

	if (rand() % 50 == 1)
	{
		int tempType = rand() % ELEMENTYPE_MAX;

		//→
		//int curLevel = GetLevel();
		//float spd = ELEMENT_BASESPEED * curLevel;

		float spd = ELEMENT_BASESPEED;
		spd = ELEMENT_BASESPEED * 3;
		SetElement(tempType, spd);
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawElement(void)
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

	// テクスチャ設定

	// 桁数分処理する
	for (int i = 0; i < ELEMENT_MAX; i++)
	{
		if (!g_Element[i].use)continue;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Element[i].type]);

		// 位置やテクスチャー座標を反映
		float px = g_Element[i].pos.x;	// スコアの表示位置X
		float py = g_Element[i].pos.y;			// スコアの表示位置Y
		float pw = g_Element[i].w;				// スコアの表示幅
		float ph = g_Element[i].h;				// スコアの表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;			// テクスチャの左上X座標
		float ty = 0.0f;			// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// スコアを加算する
//=============================================================================

void SetElement(int type,float initspd)
{
	for (int i = 0; i < ELEMENT_MAX; i++)
	{
		if (g_Element[i].use == TRUE)continue;
		g_Element[i].use = TRUE;

		g_Element[i].type = type;

		//g_Element[i].pos = XMFLOAT3(type * ELEMENT_SPACE  + 40.0f, 0.0f, 0.0f);
		g_Element[i].pos = XMFLOAT3(160.0f * type * 2 + 160.0f, 0.0f, 0.0f);
		g_Element[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Element[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Element[i].spd = initspd;

		return;
	}
}


ELEMENT* GetElement(void)
{
	return &g_Element[0];
}

void DeleteElement(int i)
{
	g_Element[i].use = FALSE;
}