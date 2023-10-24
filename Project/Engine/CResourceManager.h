#pragma once
#include "CSingleton.h"

#include "ptr.h"
#include "CMesh.h"
#include "CGraphicsShader.h"
#include "CComputeShader.h"
#include "CTexture.h"
#include "CMaterial.h"
#include "CPrefab.h"
#include "CSound.h"

#include "CPathManager.h"


class CMesh;

class CResourceManager :
    public CSingleton<CResourceManager>
{
    SINGLETON(CResourceManager);

    friend class CEventManager;

private:
    // ResourceManager�� Ptr(����Ʈ ������)�� ���ҽ����� ������ ���̴�.
    // ���ҽ����� �������ڸ��� ���ҽ��Ŵ����� ���� �����ǹǷ�, �⺻������ RefCount�� 1 ������ �����̴�
    // ���ҽ� Ÿ���� ������ŭ ���� ����� �迭�� ������ ���̴�
    map<wstring, Ptr<CResource>> m_arrRes[(UINT)RES_TYPE::END];
    bool                         m_Changed;


public:
    void init();
    void tick();

private:
    void InitSound();
    void CreateDefaultMesh();
    void CreateDefaultGraphicsShader();
    void CreateDefaultComputeShader();
    void CreateDefaultMaterial();
    void LoadDefaultTexture();

public:
    // Ư�� ���ҽ� Ÿ�� ���ҽ��� �����Ͽ� ������ �޾ƿ��� �Լ�
    const map<wstring, Ptr<CResource>>& GetResources(RES_TYPE _Type) { return m_arrRes[(UINT)_Type]; }

    // CreateTexture �Լ��� CTexture ���� �Լ��̹Ƿ� ���ø��� ������� ����
    // _BindFlag = D3D11_BIND_FLAG
    Ptr<CTexture> CreateTexture(const wstring& _strKey, UINT _Width, UINT _Height
        , DXGI_FORMAT _pixelformat, UINT _BindFlag, D3D11_USAGE _Usage);            

    Ptr<CTexture> CreateTexture(const wstring& _strKey, ComPtr<ID3D11Texture2D> _Tex2D);

    void DeleteTexture(const wstring& _strKey);
    void DeleteTexture(Ptr<CTexture> _Tex);

    bool IsResourceChanged() { return m_Changed; }

    template<typename T>
    Ptr<T> FindRes(const wstring& _strKey);

    template<typename T>
    void AddRes(const wstring& _strKey, Ptr<T>& _Res);

    template<typename T>
    Ptr<T> Load(const wstring& _strKey, const wstring& _strRelativePath);

private:
    void DeleteRes(RES_TYPE _type, const wstring& _strKey);

};

template<typename T>
RES_TYPE GetResType()
{
   // global.h �� #include typeinfo �߰�
   // ������ Ÿ�� T�� ���� Ÿ�� ������ �� �� �ִ�

    const type_info& mesh = typeid(CMesh);
    //const type_info& meshdata = typeid(CMeshData);
    const type_info& material = typeid(CMaterial);
    const type_info& texture = typeid(CTexture);
    const type_info& sound = typeid(CSound);
    const type_info& prefab = typeid(CPrefab);
    const type_info& gs = typeid(CGraphicsShader);
    const type_info& cs = typeid(CComputeShader);

    if (typeid(T).hash_code() == mesh.hash_code())
        return RES_TYPE::MESH;
    if (typeid(T).hash_code() == sound.hash_code())
        return RES_TYPE::SOUND;
    if (typeid(T).hash_code() == gs.hash_code())
        return RES_TYPE::GRAPHICS_SHADER;
    if (typeid(T).hash_code() == cs.hash_code())
        return RES_TYPE::COMPUTE_SHADER;
    if (typeid(T).hash_code() == texture.hash_code())
        return RES_TYPE::TEXTURE;
    if (typeid(T).hash_code() == material.hash_code())
        return RES_TYPE::MATERIAL;
    if (typeid(T).hash_code() == prefab.hash_code())
        return RES_TYPE::PREFAB;


    return RES_TYPE::END;
}



template<typename T>
inline Ptr<T> CResourceManager::FindRes(const wstring& _strKey)
{
    RES_TYPE type = GetResType<T>();

    map<wstring, Ptr<CResource>>::iterator iter = m_arrRes[(UINT)type].find(_strKey);
    if (iter == m_arrRes[(UINT)type].end())
        return nullptr;

    return (T*)iter->second.Get(); 
}

template<typename T>
inline void CResourceManager::AddRes(const wstring& _strKey, Ptr<T>& _Res)
{
    // ���ҽ��� �߰��ϱ� �� ���� Ű������ ���� ���ҽ��� �ִ��� �˻��Ѵ�(�ߺ�üũ)
    assert(!(FindRes<T>(_strKey).Get()));

    RES_TYPE type = GetResType<T>();

    m_arrRes[(UINT)type].insert(make_pair(_strKey, _Res.Get()));
    _Res->SetKey(_strKey);

    m_Changed = true;
}

template<typename T>
inline Ptr<T> CResourceManager::Load(const wstring& _strKey, const wstring& _strRelativePath)
{
    // ���ҽ��� �߰��ϱ� �� ���� Ű������ ���� ���ҽ��� �ִ��� �˻��Ѵ�(�ߺ�üũ)
    Ptr<CResource> pRes = FindRes<T>(_strKey).Get();

    // �ش� Ű������ �̹� ����Ǿ��ִ� ���ҽ��� �ִٸ�, �װ� ��ȯ�Ѵ�.
    if (nullptr != pRes)
        return (T*)pRes.Get();

    // �ش� Ű������ ����Ǿ��ִ� ���ҽ��� ���ٸ�, Ű��, ��θ� �����ϰ� �ε��� �� �߰��Ѵ�.
    pRes = new T;
    pRes->SetKey(_strKey);
    pRes->SetRelativePath(_strRelativePath);

    wstring strFilePath = CPathManager::GetInst()->GetContentPath();
    strFilePath += _strRelativePath;

    if (FAILED(pRes->Load(strFilePath)))
    {
        assert(nullptr);
    }

    RES_TYPE type = GetResType<T>();
    m_arrRes[(UINT)type].insert(make_pair(_strKey, pRes));

    m_Changed = true;

    return (T*)pRes.Get();
}
