//=============================================================================
//
// �Q�[�W��ʏ��� [gauge.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(80)			// 

#define GAUGE_HEIGHT				(TEXTURE_HEIGHT / 4 * 3 + 10)

//*****************************************************************************
// �v���g�^�C�v�錾
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
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/gauge.png",
	"data/TEXTURE/wood.png",
	"data/TEXTURE/metal.png",
	"data/TEXTURE/concrete.png",
};

static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static BOOL						g_Load = FALSE;

static GAUGE g_Gauge[TEXTURE_MAX - 1];

static int g_gaugeType = GAUGE_MAX;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGauge(void)
{
	ID3D11Device* pDevice = GetDevice();

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


	// �ϐ��̏�����
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;

	// �f�ނ̕`��
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
// �I������
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
// �X�V����
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

	// �f�ނ̒T��
	if (searchType != ELEMENTYPE_MAX)
	{
		for (int i = 0; i < ELEMENT_MAX; i++)
		{
			if (!element[i].use) continue;

			// �T������f�ނ̌���
			if (element[i].type == searchType)
			{
				// ���W����ԍ����f�ނ�I��
				if (element_h < element[i].pos.y)
				{
					elementNo = i;
					element_h = element[i].pos.y;
				}
			}
		}
	}

	// �f�ނ����������Ȃ�
	if (elementNo != -1 &&
		element[elementNo].type == searchType)
	{
		// �f�ނ̍��������Ĕ�������߂�
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

	// �f�ނ̉摜������
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
#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("gaugeType:%d\n", g_gaugeType);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGauge(void)
{
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

	// �Q�[�W��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �ޗ���`��
	for (int i = 0; i < TEXTURE_MAX - 1; i++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Gauge[i].texNo]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, g_Gauge[i].pos.x, g_Gauge[i].pos.y, g_Gauge[i].w, g_Gauge[i].h, 0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 0.4f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}





