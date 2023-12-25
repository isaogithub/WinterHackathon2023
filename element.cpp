//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"
#include "element.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(80)	// 
#define TEXTURE_HEIGHT				(80)	// 
#define TEXTURE_MAX					(ELEMENTYPE_MAX)	// �e�N�X�`���̐�


#define ELEMENT_BASESPEED			(1.0f)
#define ELEMENT_SPEED_MAX			(4.0f)
#define ELEMENT_SPACE				(100.0f)
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

};


static ELEMENT g_Element[ELEMENT_MAX];

static BOOL						g_Load = FALSE;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitElement(void)
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


	// ������
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateElement(void)
{

	for (int i = 0; i < ELEMENT_MAX; i++)
	{
		if (g_Element[i].use == FALSE)continue;

		XMVECTOR v_spd = { 0.0f,g_Element[i].spd,0.0f };
		XMStoreFloat3(&g_Element[i].pos, XMLoadFloat3(&g_Element[i].pos) + v_spd);

		//��ԉ��ɗ����������
		if (g_Element[i].pos.y > SCREEN_HEIGHT)g_Element[i].use = FALSE;
	}

	//�G�������g�̐�������

	if (rand() % 50 == 1)
	{
		int tempType = rand() % ELEMENTYPE_MAX;

		//��
		//int curLevel = GetLevel();
		//float spd = ELEMENT_BASESPEED * curLevel;

		float spd = ELEMENT_BASESPEED;
		spd = ELEMENT_BASESPEED * 3;
		SetElement(tempType, spd);
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawElement(void)
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

	// �e�N�X�`���ݒ�

	// ��������������
	for (int i = 0; i < ELEMENT_MAX; i++)
	{
		if (!g_Element[i].use)continue;
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Element[i].type]);

		// �ʒu��e�N�X�`���[���W�𔽉f
		float px = g_Element[i].pos.x;	// �X�R�A�̕\���ʒuX
		float py = g_Element[i].pos.y;			// �X�R�A�̕\���ʒuY
		float pw = g_Element[i].w;				// �X�R�A�̕\����
		float ph = g_Element[i].h;				// �X�R�A�̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;			// �e�N�X�`���̍���X���W
		float ty = 0.0f;			// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}


//=============================================================================
// �X�R�A�����Z����
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