#pragma once
#include "ComponentUI.h"

class CTileMap;
class CTexture;

class TileMapUI :
    public ComponentUI
{
public:
    TileMapUI();
    ~TileMapUI();

private:
    int            m_iTileCountX;
    int            m_iTileCountY;

    Ptr<CTexture>   m_TileAtlas;
    Vec2            m_TileResolution;
    
    int            m_iSliceX;
    int            m_iSliceY;

    vector<tTile>   m_vecTile;
    int             m_SelectedTileIdx;

    vector<tTileMapIdx>     m_vecTileMap;

    float           m_fRatio;

    bool            m_bNewTileSetPopup;

public:
    virtual void init() override;
    virtual int render_update() override;

private:
    void CreateNewTileSetPopup();
};

