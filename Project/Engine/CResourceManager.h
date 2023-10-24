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
    // ResourceManager은 Ptr(스마트 포인터)로 리소스들을 관리할 것이다.
    // 리소스들은 생성되자마자 리소스매니저에 의해 관리되므로, 기본적으로 RefCount가 1 증가된 상태이다
    // 리소스 타입의 갯수만큼 맵을 만들어 배열로 관리할 것이다
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
    // 특정 리소스 타입 리소스를 복사하여 맵으로 받아오는 함수
    const map<wstring, Ptr<CResource>>& GetResources(RES_TYPE _Type) { return m_arrRes[(UINT)_Type]; }

    // CreateTexture 함수는 CTexture 전용 함수이므로 템플릿을 사용하지 않음
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
   // global.h 에 #include typeinfo 추가
   // 임의의 타입 T에 대한 타입 정보를 알 수 있다

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
    // 리소스를 추가하기 전 같은 키값으로 들어온 리소스가 있는지 검사한다(중복체크)
    assert(!(FindRes<T>(_strKey).Get()));

    RES_TYPE type = GetResType<T>();

    m_arrRes[(UINT)type].insert(make_pair(_strKey, _Res.Get()));
    _Res->SetKey(_strKey);

    m_Changed = true;
}

template<typename T>
inline Ptr<T> CResourceManager::Load(const wstring& _strKey, const wstring& _strRelativePath)
{
    // 리소스를 추가하기 전 같은 키값으로 들어온 리소스가 있는지 검사한다(중복체크)
    Ptr<CResource> pRes = FindRes<T>(_strKey).Get();

    // 해당 키값으로 이미 저장되어있는 리소스가 있다면, 그걸 반환한다.
    if (nullptr != pRes)
        return (T*)pRes.Get();

    // 해당 키값으로 저장되어있는 리소스가 없다면, 키값, 경로를 셋팅하고 로드한 후 추가한다.
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
