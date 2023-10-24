#include "pch.h"
#include "CAnim2D.h"

#include "CTimeManager.h"
#include "CPathManager.h"
#include "CResourceManager.h"
#include "CAnimator2D.h"

CAnim2D::CAnim2D()
	: m_pOwner(nullptr)
	, m_iCurFrm(0)
	, m_bFinish(false)
	, m_fTime(0)
{
}

CAnim2D::~CAnim2D()
{
}

void CAnim2D::finaltick()
{
	if (m_iCurFrm > m_vecFrm.size() - 1)
		return;

	if (m_bFinish)
		return;

	m_vBackSizeUV = m_vBackSize / m_vAtlasResolution;
	m_vAtlasResolution = Vec2(m_AtlasTex->Width(), m_AtlasTex->Height());

	m_fTime += DT;
	

	// 프레임의 Duration보다 누적시간이 커지면
	if (m_vecFrm[m_iCurFrm].fDuration < m_fTime)
	{
		// 누적시간을 Duration만큼 빼주어 초기화하고 프레임을 넘긴다.
		m_fTime = m_fTime - m_vecFrm[m_iCurFrm].fDuration;
		++m_iCurFrm;

		// 마지막 프레임을 넘어가면 마지막 프레임으로 고정하고 Finish옵션을 켠다
		if (m_vecFrm.size() <= m_iCurFrm)
		{
			m_iCurFrm = (int)m_vecFrm.size() - 1;
			m_bFinish = true;
		}
	}
}

void CAnim2D::Create(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, Vec2 _BackSize, int _FrameCount, int _FPS)
{
	// 아틀라스 텍스처 없으면 Assert
	assert(_AtlasTex.Get());

	// 애니메이션 이름 설정
	SetName(_strAnimName);

	// 아틀라스 이미지 대입, 해상도 저장
	m_AtlasTex = _AtlasTex;
	m_vAtlasResolution = Vec2(m_AtlasTex->Width(), m_AtlasTex->Height());

	// FPS 대입
	m_iFPS = _FPS;

	// BackSize 대입
	m_vBackSize = _BackSize;
	m_vBackSizeUV = m_vBackSize / m_vAtlasResolution;

	for (size_t i = 0; i < _FrameCount; ++i)
	{
		tAnim2DFrm frm = {};

		frm.fDuration = 1.f / (float)_FPS;
		// 아틀라스 이미지 내의 한 애니메이션이 일렬로 정렬되어있을 때 기준
		frm.LeftTopUV = Vec2(_vLeftTop.x + _vSlice.x * i, _vLeftTop.y) / m_vAtlasResolution;
		frm.SliceUV = _vSlice / m_vAtlasResolution;

		m_vecFrm.push_back(frm);
	}
}

void CAnim2D::AddFrame(tAnim2DFrm _frm)
{
	m_vecFrm.push_back(_frm);
}

void CAnim2D::Save()
{
	// open a file name
	OPENFILENAME ofn = {};

	//PathManager에서 기본 경로를 가져온다
	wstring strFileFolderPath = CPathManager::GetInst()->GetContentPath();
	strFileFolderPath += L"anim\\";

	//인자로 들어온 상대경로를 붙여 애니메이션을 저장할 최종 경로를 완성한다.
	//strFileFolderPath += _strRelativePath;

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"Anim\0*.anim\0ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strFileFolderPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (false == GetSaveFileName(&ofn))
		return;

	//파일입출력
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, szFilePath, L"wb");

	//파일 포인터가 nullptr라면 파일 생성이 제대로 되지 않은 것으로 리턴
	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Animation Save 실패, Error Number : %d", iErrNum);
		MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
		return;
	}

	size_t i = 0;
	wstring ContentFolderPath = CPathManager::GetInst()->GetContentPath();
	while (i < wstring(szFilePath).length() && i < strFileFolderPath.length()
		&& wstring(szFilePath)[i] == strFileFolderPath[i])
	{
		++i;
	}
	wstring FileName = wstring(szFilePath).substr(i);

	// 확장자명 제거한 이름을 애니메이션 이름으로 지정
	size_t dotPos = FileName.find_last_of('.');
	wstring AnimName = FileName.substr(0, dotPos);

	//===== 01. 애니메이션 이름 저장=====
	SaveWString(AnimName, pFile);

	//===== 02. 애니메이션 경로 저장=====
	wstring RelativePath = L"anim\\" + FileName;
	SaveWString(RelativePath, pFile);
	SetRelativePath(RelativePath);

	//===== 03. 사용하는 아틀라스의 키값, 경로값 저장=====
	SaveResourceRef(m_AtlasTex.Get(), pFile);

	//===== 04. 애니메이션 프레임 개수와 프레임 구조체 저장=====
	size_t FrameCount = m_vecFrm.size();
	fwrite(&FrameCount, sizeof(size_t), 1, pFile);
	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrm), FrameCount, pFile);

	//===== 05. BackSize 저장 =====
	fwrite(&m_vBackSize, sizeof(Vec2), 1, pFile);

	//===== 06. 저장한 파일명으로 애니메이션 이름 설정 =====
	SetName(AnimName);

	//파일 저장을 마치면 스트림을 닫아준다.
	fclose(pFile);
}

/*void CAnim2D::Save()
{
	// open a file name
	OPENFILENAME ofn = {};

	//PathManager에서 기본 경로를 가져온다
	wstring strFileFolderPath = CPathManager::GetInst()->GetContentPath();
	strFileFolderPath += L"anim\\";

	//인자로 들어온 상대경로를 붙여 애니메이션을 저장할 최종 경로를 완성한다.
	//strFileFolderPath += _strRelativePath;

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"Anim\0*.anim\0ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strFileFolderPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (false == GetSaveFileName(&ofn))
		return;

	//파일입출력
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, szFilePath, L"wb");

	//파일 포인터가 nullptr라면 파일 생성이 제대로 되지 않은 것으로 리턴
	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Animation Save 실패, Error Number : %d", iErrNum);
		MessageBox(nullptr, szStr, L"파일 저장 실패", MB_OK);
		return;
	}

	size_t i = 0;
	while (i < wstring(szFilePath).length() && i < strFileFolderPath.length() 
		&& wstring(szFilePath)[i] == strFileFolderPath[i])
	{
		++i;
	}
	wstring FileName = wstring(szFilePath).substr(i);
	
	//wstring FileName(result.first, wstring(szFilePath).end());


	//===== 01. 애니메이션 이름 저장=====
	//메모장에서 수정시 오류가 나지 않도록 처음에 개행문자를 저장
	fwprintf_s(pFile, L"\n");

	//가독성을 위해 맨 위에 필드명 저장
	fwprintf_s(pFile, L"[ANIMATION_NAME]\n");
	//2바이트 문자열을 파일에 저장할 때 쓰는 fwprintf_s 함수
	fwprintf_s(pFile, FileName.c_str());
	//외부에서 메모장으로 파일을 확인할 때 가독성을 위해 개행문자도 저장
	fwprintf_s(pFile, L"\n\n");

	//===== 02. 아틀라스 키(이름)값 & 경로 저장=====
	fwprintf_s(pFile, L"[ATLAS_KEY]\n");
	fwprintf_s(pFile, m_AtlasTex->GetKey().c_str());
	fwprintf_s(pFile, L"\n\n");

	fwprintf_s(pFile, L"[ATLAS_PATH]\n");
	fwprintf_s(pFile, m_AtlasTex->GetRelativePath().c_str());
	fwprintf_s(pFile, L"\n\n");

	//===== 03. 프레임 정보 저장=====
	fwprintf_s(pFile, L"[FRAME_COUNT]\n");

	wchar_t szNum[50] = {};
	//프레임 카운트는 정수형으로, 저장하려면 숫자를 문자열로 바꿔야한다.
	//_itow_s(숫자를 2바이트 문자열로 바꾸어주는 함수) 사용
	size_t iFrmCount = m_vecFrm.size();
	_itow_s((int)iFrmCount, szNum, 50, 10);

	fwprintf_s(pFile, szNum);
	fwprintf_s(pFile, L"\n\n");

	//ifrmcount만큼 반복문을 돌며 프레임 정보를 기록한다
	for (size_t i = 0; i < iFrmCount; i++)
	{
		fwprintf_s(pFile, L"[%zd_FRAME]\n", i);

		//Left Top
		fwprintf_s(pFile, L"[LEFT_TOP]\n");
		fwprintf_s(pFile, L"%f %f\n", m_vecFrm[i].LeftTopUV.x, m_vecFrm[i].LeftTopUV.y);

		//Size
		fwprintf_s(pFile, L"[SIZE]\n");
		fwprintf_s(pFile, L"%f %f\n", m_vecFrm[i].SliceUV.x, m_vecFrm[i].SliceUV.y);

		//Offset
		fwprintf_s(pFile, L"[OFFSET]\n");
		fwprintf_s(pFile, L"%f %f\n", m_vecFrm[i].OffsetUV.x, m_vecFrm[i].OffsetUV.y);

		//Duration
		fwprintf_s(pFile, L"[DURATION]\n");
		fwprintf_s(pFile, L"%f\n", m_vecFrm[i].fDuration);
		fwprintf_s(pFile, L"\n");
	}

	//파일 저장을 마치면 스트림을 닫아준다.
	fclose(pFile);
}*/


void CAnim2D::Load()
{
	// open a file name
	OPENFILENAME ofn = {};

	//PathManager에서 기본 경로를 가져온다
	wstring strFileFolderPath = CPathManager::GetInst()->GetContentPath();
	strFileFolderPath += L"anim\\";

	wchar_t szFilePath[256] = {};

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFilePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 256;
	ofn.lpstrFilter = L"Anim\0*.anim\0ALL\0*.*";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = strFileFolderPath.c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (false == GetOpenFileName(&ofn))
		return;

	if(!m_vecFrm.empty())
		m_vecFrm.clear();

	// 파일입출력
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, szFilePath, L"rb");

	//===== 01. 애니메이션 이름 불러오기=====
	wstring name;
	LoadWString(name, pFile);
	SetName(name);

	//===== 02. 애니메이션 경로 불러오기=====
	wstring path;
	LoadWString(path, pFile);
	SetRelativePath(path);

	//===== 02. 사용하는 아틀라스 키 경로 불러오기=====
	LoadResourceRef(m_AtlasTex, pFile);

	//===== 03. 애니메이션 프레임 개수 불러오기=====
	size_t FrameCount = 0;
	fread(&FrameCount, sizeof(size_t), 1, pFile);

	//===== 04. 프레임 개수만큼 프레임 구조체 불러오기=====
	for (size_t i = 0; i < FrameCount; ++i)
	{
		tAnim2DFrm frm = {};
		fread(&frm, sizeof(tAnim2DFrm), 1, pFile);
		m_vecFrm.push_back(frm);
	}

	//===== 05. BackSize 불러오기 =====
	fread(&m_vBackSize, sizeof(Vec2), 1, pFile);

	Reset();

	//파일 로딩을 마치면 스트림을 닫아준다. 
	fclose(pFile);

	// BackSize UV 재지정
	m_vBackSizeUV = m_vBackSize / m_vAtlasResolution;

}

void CAnim2D::Load(const wstring& _strFilePath)
{

	wstring strFilePath = CPathManager::GetInst()->GetContentPath();
	strFilePath += _strFilePath;

	// 파일입출력
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	//===== 01. 애니메이션 이름 불러오기=====
	wstring name;
	LoadWString(name, pFile);
	SetName(name);

	//===== 02. 애니메이션 경로 불러오기=====
	wstring path;
	LoadWString(path, pFile);
	SetRelativePath(path);

	//===== 02. 사용하는 아틀라스 키 경로 불러오기=====
	LoadResourceRef(m_AtlasTex, pFile);

	//===== 03. 애니메이션 프레임 개수 불러오기=====
	size_t FrameCount = 0;
	fread(&FrameCount, sizeof(size_t), 1, pFile);

	//===== 04. 프레임 개수만큼 프레임 구조체 불러오기=====
	for (size_t i = 0; i < FrameCount; ++i)
	{
		tAnim2DFrm frm = {};
		fread(&frm, sizeof(tAnim2DFrm), 1, pFile);
		m_vecFrm.push_back(frm);
	}

	//===== 05. BackSize 불러오기 =====
	fread(&m_vBackSize, sizeof(Vec2), 1, pFile);

	Reset();

	//파일 로딩을 마치면 스트림을 닫아준다. 
	fclose(pFile);

	// BackSize UV 재지정
	m_vBackSizeUV = m_vBackSize / m_vAtlasResolution;
}

