#include "pch.h"
#include "MeshDataUI.h"

MeshDataUI::MeshDataUI()
    : ResourceUI(RES_TYPE::MESHDATA)
{
    SetName("MeshData");
}

MeshDataUI::~MeshDataUI()
{
}

int MeshDataUI::render_update()
{
    ResourceUI::render_update();
    return 0;
}
