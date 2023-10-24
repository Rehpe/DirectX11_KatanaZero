#pragma once
#include "UI.h"

class TreeUI;

class ContentUI :
    public UI
{
public:
    ContentUI();
    ~ContentUI();

private:
    TreeUI*             m_Tree;
    vector<wstring>     m_vecResPath;   // Contents 폴더 내의 모든 리소스의 경로(이자 키값) 벡터

public:
    virtual void init() override;
    virtual void tick() override;
    virtual int render_update();

public:
    void Reload(); // Content 폴더에 있는 리소스를 로딩
    void ResetContent();
    void SetTargetToInspector(DWORD_PTR _SelectedNode);

private:
    void FindFileName(const wstring& _FolderPath);
    RES_TYPE GetResTypeByExt(const wstring& _relativepath);
};

