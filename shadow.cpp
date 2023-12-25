//=============================================================================
//
// �e���� [shadow.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "shadow.h"
#include "light.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)				// �e�N�X�`���̐�

#define	SHADOW_SIZE_X		(50.0f)			// ���_�T�C�Y
#define	SHADOW_SIZE_Z		(50.0f)			// ���_�T�C�Y

#define	MAX_SHADOW			(1024)			// �e�ő吔

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3	pos;		// �ʒu
	XMFLOAT3	rot;		// ��]
	XMFLOAT3	scl;		// �X�P�[��
	MATERIAL	material;	// �}�e���A��
	BOOL		use;		// �g�p���Ă��邩�ǂ���

} SHADOW;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexShadow(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_���
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static SHADOW					g_aShadow[MAX_SHADOW];		// �e���[�N
static int						g_TexNo;					// �e�N�X�`���ԍ�

static char* g_TextureName[] = {
	"data/TEXTURE/shadow000.jpg",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitShadow(void)
{
	// ���_�o�b�t�@�̍쐬
	MakeVertexShadow();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// �e���[�N������
	for(int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		ZeroMemory(&g_aShadow[nCntShadow].material, sizeof(g_aShadow[nCntShadow].material));
		g_aShadow[nCntShadow].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_aShadow[nCntShadow].pos = XMFLOAT3(0.0f, 0.1f, 0.0f);
		g_aShadow[nCntShadow].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_aShadow[nCntShadow].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_aShadow[nCntShadow].use = FALSE;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitShadow(void)
{
	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// �e�N�X�`���̉��
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateShadow(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawShadow(void)
{
	// ���Z����
	SetBlendState(BLEND_MODE_SUBTRACT);

	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// �t�H�O����
	SetFogEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for(int i = 0; i < MAX_SHADOW; i++)
	{
		if(g_aShadow[i].use)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_aShadow[i].scl.x, g_aShadow[i].scl.y, g_aShadow[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// ��]�𔽉f
			mtxRot = XMMatrixRotationRollPitchYaw(g_aShadow[i].rot.x, g_aShadow[i].rot.y, g_aShadow[i].rot.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_aShadow[i].pos.x, g_aShadow[i].pos.y, g_aShadow[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���̐ݒ�
			SetMaterial(g_aShadow[i].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// �ʏ�u�����h
	SetBlendState(BLEND_MODE_ALPHABLEND);
	
	// Z��r����
	SetDepthEnable(TRUE);

	// �t�H�O�����ɖ߂�
	SetFogEnable(GetFogEnable());
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexShadow()
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-SHADOW_SIZE_X / 2, 0.0f, SHADOW_SIZE_Z / 2);
		vertex[1].Position = XMFLOAT3(SHADOW_SIZE_X / 2, 0.0f, SHADOW_SIZE_Z / 2);
		vertex[2].Position = XMFLOAT3(-SHADOW_SIZE_X / 2, 0.0f, -SHADOW_SIZE_Z / 2);
		vertex[3].Position = XMFLOAT3(SHADOW_SIZE_X / 2, 0.0f, -SHADOW_SIZE_Z / 2);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

		// �g�U���̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[1].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[2].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
		vertex[3].Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
		vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
		vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �e�̍쐬
//=============================================================================
int CreateShadow(XMFLOAT3 pos, float fSizeX, float fSizeZ)
{
	int nIdxShadow = -1;

	for(int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		// ���g�p�iFALSE�j�������炻����g��
		if(!g_aShadow[nCntShadow].use)
		{
			g_aShadow[nCntShadow].pos = pos;
			g_aShadow[nCntShadow].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
			g_aShadow[nCntShadow].scl = XMFLOAT3(fSizeX, 1.0f, fSizeZ);
			g_aShadow[nCntShadow].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			g_aShadow[nCntShadow].use = TRUE;

			nIdxShadow = nCntShadow;	// ���ꂪIndex�ԍ�
			break;
		}
	}

	return nIdxShadow;					// Index�ԍ���Ԃ��Ă���
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorShadow(int nIdxShadow, XMFLOAT4 col)
{
	if (nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		g_aShadow[nIdxShadow].material.Diffuse = col;
	}
}

//=============================================================================
// �e�̔j��
//=============================================================================
void ReleaseShadow(int nIdxShadow)
{
	if(nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		g_aShadow[nIdxShadow].use = FALSE;
	}
}

//=============================================================================
// �ʒu�̐ݒ�
//=============================================================================
void SetPositionShadow(int nIdxShadow, XMFLOAT3 pos)
{
	if (nIdxShadow >= 0 && nIdxShadow < MAX_SHADOW)
	{
		g_aShadow[nIdxShadow].pos = pos;
	}
}

