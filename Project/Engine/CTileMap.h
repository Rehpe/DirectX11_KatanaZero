#pragma once
#include "CRenderComponent.h"
#include "ptr.h"
#include "CTexture.h"

class CStructuredBuffer;

class CTileMap :
    public CRenderComponent
{
public:
    CTileMap();
    ~CTileMap();

public:
    CLONE(CTileMap);

private:
    Ptr<CTexture>       m_TileAtlasTex;     // Ÿ�� ��Ʋ�� �̹���
    UINT                m_iTileCountX;      // Ÿ�ϸ� ����ĭ
    UINT                m_iTileCountY;      // Ÿ�ϸ� ����ĭ

    UINT                m_SliceX;
    UINT                m_SliceY;

    Vec2                m_vSliceSize;   // Ÿ�� �ϳ��� ũ��(UV ����)

    vector<tTile>       m_vecTile;      // Ÿ�� �ϳ������� �»��, ũ�� ������ ����ִ�
    CStructuredBuffer*  m_Buffer;       // Ÿ�� ������ ���� ����ȭ����

    vector<tTileMapIdx> m_TileMap;
    CStructuredBuffer*  m_TileMapBuffer;


public:
    virtual void finaltick() override;
    virtual void render() override;

    void UpdateData();

public:
    //void init(UINT _iXCount, UINT _iYCount);
    void SetTileCount(UINT _iXCount, UINT _iYCount);
    UINT GetTileCountX() { return m_iTileCountX; }
    UINT GetTileCountY() { return m_iTileCountY; }
    
    void SetSliceSize(int _SliceX, int _SliceY);

    int GetSliceXCount() { return m_SliceX; }
    int GetSliceYCount() { return m_SliceY; }
    
    
    void SetTileAtlas(Ptr<CTexture> _Atlas) { m_TileAtlasTex = _Atlas; }
    Ptr<CTexture> GetTileAtlas() { return m_TileAtlasTex; }
    const vector<tTile>& GetTileVector() { return m_vecTile; }
    const vector<tTileMapIdx>& GetTileMapIdx() { return m_TileMap; }

    void SetTileVector(vector<tTile> _TileVector) { m_vecTile = _TileVector; }
    void SetTileMap(vector<tTileMapIdx> _TileMapVector) { m_TileMap = _TileMapVector; }

public:
    virtual void SaveToLevelFile(FILE* _File) override;
    virtual void LoadFromLevelFile(FILE* _File) override;

};

