//=============================================================================
//
// UI���� [ui.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"
#include "ui.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(50)	// 
#define TEXTURE_MAX					(UI_MAX)		// �e�N�X�`���̐�

#define LIFE_MAX					(3)
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/wood.png",
	"data/TEXTURE/metal.png",
	"data/TEXTURE/concrete.png",
	"data/TEXTURE/life.png",
	"data/TEXTURE/number16x32.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static BOOL						g_Load = FALSE;
static UI						g_UI[UI_MAX];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �v���C���[�̏�����
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateUI(void)
{


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUI(void)
{
	if (GetMode() != MODE_GAME)return;
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	for (int i = 0; i < UI_NUMBER; i++)
	{
		if (g_UI[i].use == FALSE)continue;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_UI[i].texNo]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		float px = g_UI[i].pos.x;
		float py = g_UI[i].pos.y;
		float pw = g_UI[i].w;
		float ph = g_UI[i].h;

		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

	//UI�̐����\��
	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[UI_NUMBER]);
	for (int j = 0; j< UI_NUMBER; j++)
	{
		if (g_UI[j].use == FALSE)continue;

		// ��������������
		int number = g_UI[j].count;
		for (int i = 0; i < UI_DIGIT; i++)
		{
			// ����\�����錅�̐���
			float x = (float)(number % 10);

			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_UI[j].pos.x - 20.0f* i;	// �X�R�A�̕\���ʒuX
			float py = g_UI[j].pos.y + 50.0f;			// �X�R�A�̕\���ʒuY
			float pw = 16.0f;				// �X�R�A�̕\����
			float ph = 32.0f;				// �X�R�A�̕\������

			float tw = 1.0f / 10;		// �e�N�X�`���̕�
			float th = 1.0f / 1;		// �e�N�X�`���̍���
			float tx = x * tw;			// �e�N�X�`���̍���X���W
			float ty = 0.0f;			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			// ���̌���
			number /= 10;
		}
	}
	
}


//=============================================================================
// �X�R�A�����Z����
// ����:add :�ǉ�����_���B�}�C�i�X���\
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
// �R���N���[�g
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