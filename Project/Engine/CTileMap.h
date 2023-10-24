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
    Ptr<CTexture>       m_TileAtlasTex;     // 타일 아틀라스 이미지
    UINT                m_iTileCountX;      // 타일맵 가로칸
    UINT                m_iTileCountY;      // 타일맵 세로칸

    UINT                m_SliceX;
    UINT                m_SliceY;

    Vec2                m_vSliceSize;   // 타일 하나의 크기(UV 단위)

    vector<tTile>       m_vecTile;      // 타일 하나마다의 좌상단, 크기 정보가 들어있다
    CStructuredBuffer*  m_Buffer;       // 타일 정보를 보낼 구조화버퍼

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

