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
	

	// �������� Duration���� �����ð��� Ŀ����
	if (m_vecFrm[m_iCurFrm].fDuration < m_fTime)
	{
		// �����ð��� Duration��ŭ ���־� �ʱ�ȭ�ϰ� �������� �ѱ��.
		m_fTime = m_fTime - m_vecFrm[m_iCurFrm].fDuration;
		++m_iCurFrm;

		// ������ �������� �Ѿ�� ������ ���������� �����ϰ� Finish�ɼ��� �Ҵ�
		if (m_vecFrm.size() <= m_iCurFrm)
		{
			m_iCurFrm = (int)m_vecFrm.size() - 1;
			m_bFinish = true;
		}
	}
}

void CAnim2D::Create(const wstring& _strAnimName, Ptr<CTexture> _AtlasTex, Vec2 _vLeftTop, Vec2 _vSlice, Vec2 _BackSize, int _FrameCount, int _FPS)
{
	// ��Ʋ�� �ؽ�ó ������ Assert
	assert(_AtlasTex.Get());

	// �ִϸ��̼� �̸� ����
	SetName(_strAnimName);

	// ��Ʋ�� �̹��� ����, �ػ� ����
	m_AtlasTex = _AtlasTex;
	m_vAtlasResolution = Vec2(m_AtlasTex->Width(), m_AtlasTex->Height());

	// FPS ����
	m_iFPS = _FPS;

	// BackSize ����
	m_vBackSize = _BackSize;
	m_vBackSizeUV = m_vBackSize / m_vAtlasResolution;

	for (size_t i = 0; i < _FrameCount; ++i)
	{
		tAnim2DFrm frm = {};

		frm.fDuration = 1.f / (float)_FPS;
		// ��Ʋ�� �̹��� ���� �� �ִϸ��̼��� �Ϸķ� ���ĵǾ����� �� ����
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

	//PathManager���� �⺻ ��θ� �����´�
	wstring strFileFolderPath = CPathManager::GetInst()->GetContentPath();
	strFileFolderPath += L"anim\\";

	//���ڷ� ���� ����θ� �ٿ� �ִϸ��̼��� ������ ���� ��θ� �ϼ��Ѵ�.
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

	//���������
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, szFilePath, L"wb");

	//���� �����Ͱ� nullptr��� ���� ������ ����� ���� ���� ������ ����
	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Animation Save ����, Error Number : %d", iErrNum);
		MessageBox(nullptr, szStr, L"���� ���� ����", MB_OK);
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

	// Ȯ���ڸ� ������ �̸��� �ִϸ��̼� �̸����� ����
	size_t dotPos = FileName.find_last_of('.');
	wstring AnimName = FileName.substr(0, dotPos);

	//===== 01. �ִϸ��̼� �̸� ����=====
	SaveWString(AnimName, pFile);

	//===== 02. �ִϸ��̼� ��� ����=====
	wstring RelativePath = L"anim\\" + FileName;
	SaveWString(RelativePath, pFile);
	SetRelativePath(RelativePath);

	//===== 03. ����ϴ� ��Ʋ���� Ű��, ��ΰ� ����=====
	SaveResourceRef(m_AtlasTex.Get(), pFile);

	//===== 04. �ִϸ��̼� ������ ������ ������ ����ü ����=====
	size_t FrameCount = m_vecFrm.size();
	fwrite(&FrameCount, sizeof(size_t), 1, pFile);
	fwrite(m_vecFrm.data(), sizeof(tAnim2DFrm), FrameCount, pFile);

	//===== 05. BackSize ���� =====
	fwrite(&m_vBackSize, sizeof(Vec2), 1, pFile);

	//===== 06. ������ ���ϸ����� �ִϸ��̼� �̸� ���� =====
	SetName(AnimName);

	//���� ������ ��ġ�� ��Ʈ���� �ݾ��ش�.
	fclose(pFile);
}

/*void CAnim2D::Save()
{
	// open a file name
	OPENFILENAME ofn = {};

	//PathManager���� �⺻ ��θ� �����´�
	wstring strFileFolderPath = CPathManager::GetInst()->GetContentPath();
	strFileFolderPath += L"anim\\";

	//���ڷ� ���� ����θ� �ٿ� �ִϸ��̼��� ������ ���� ��θ� �ϼ��Ѵ�.
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

	//���������
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, szFilePath, L"wb");

	//���� �����Ͱ� nullptr��� ���� ������ ����� ���� ���� ������ ����
	if (nullptr == pFile)
	{
		wchar_t szStr[256] = {};
		wsprintf(szStr, L"Animation Save ����, Error Number : %d", iErrNum);
		MessageBox(nullptr, szStr, L"���� ���� ����", MB_OK);
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


	//===== 01. �ִϸ��̼� �̸� ����=====
	//�޸��忡�� ������ ������ ���� �ʵ��� ó���� ���๮�ڸ� ����
	fwprintf_s(pFile, L"\n");

	//�������� ���� �� ���� �ʵ�� ����
	fwprintf_s(pFile, L"[ANIMATION_NAME]\n");
	//2����Ʈ ���ڿ��� ���Ͽ� ������ �� ���� fwprintf_s �Լ�
	fwprintf_s(pFile, FileName.c_str());
	//�ܺο��� �޸������� ������ Ȯ���� �� �������� ���� ���๮�ڵ� ����
	fwprintf_s(pFile, L"\n\n");

	//===== 02. ��Ʋ�� Ű(�̸�)�� & ��� ����=====
	fwprintf_s(pFile, L"[ATLAS_KEY]\n");
	fwprintf_s(pFile, m_AtlasTex->GetKey().c_str());
	fwprintf_s(pFile, L"\n\n");

	fwprintf_s(pFile, L"[ATLAS_PATH]\n");
	fwprintf_s(pFile, m_AtlasTex->GetRelativePath().c_str());
	fwprintf_s(pFile, L"\n\n");

	//===== 03. ������ ���� ����=====
	fwprintf_s(pFile, L"[FRAME_COUNT]\n");

	wchar_t szNum[50] = {};
	//������ ī��Ʈ�� ����������, �����Ϸ��� ���ڸ� ���ڿ��� �ٲ���Ѵ�.
	//_itow_s(���ڸ� 2����Ʈ ���ڿ��� �ٲپ��ִ� �Լ�) ���
	size_t iFrmCount = m_vecFrm.size();
	_itow_s((int)iFrmCount, szNum, 50, 10);

	fwprintf_s(pFile, szNum);
	fwprintf_s(pFile, L"\n\n");

	//ifrmcount��ŭ �ݺ����� ���� ������ ������ ����Ѵ�
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

	//���� ������ ��ġ�� ��Ʈ���� �ݾ��ش�.
	fclose(pFile);
}*/


void CAnim2D::Load()
{
	// open a file name
	OPENFILENAME ofn = {};

	//PathManager���� �⺻ ��θ� �����´�
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

	// ���������
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, szFilePath, L"rb");

	//===== 01. �ִϸ��̼� �̸� �ҷ�����=====
	wstring name;
	LoadWString(name, pFile);
	SetName(name);

	//===== 02. �ִϸ��̼� ��� �ҷ�����=====
	wstring path;
	LoadWString(path, pFile);
	SetRelativePath(path);

	//===== 02. ����ϴ� ��Ʋ�� Ű ��� �ҷ�����=====
	LoadResourceRef(m_AtlasTex, pFile);

	//===== 03. �ִϸ��̼� ������ ���� �ҷ�����=====
	size_t FrameCount = 0;
	fread(&FrameCount, sizeof(size_t), 1, pFile);

	//===== 04. ������ ������ŭ ������ ����ü �ҷ�����=====
	for (size_t i = 0; i < FrameCount; ++i)
	{
		tAnim2DFrm frm = {};
		fread(&frm, sizeof(tAnim2DFrm), 1, pFile);
		m_vecFrm.push_back(frm);
	}

	//===== 05. BackSize �ҷ����� =====
	fread(&m_vBackSize, sizeof(Vec2), 1, pFile);

	Reset();

	//���� �ε��� ��ġ�� ��Ʈ���� �ݾ��ش�. 
	fclose(pFile);

	// BackSize UV ������
	m_vBackSizeUV = m_vBackSize / m_vAtlasResolution;

}

void CAnim2D::Load(const wstring& _strFilePath)
{

	wstring strFilePath = CPathManager::GetInst()->GetContentPath();
	strFilePath += _strFilePath;

	// ���������
	FILE* pFile = nullptr;
	errno_t iErrNum = _wfopen_s(&pFile, strFilePath.c_str(), L"rb");

	//===== 01. �ִϸ��̼� �̸� �ҷ�����=====
	wstring name;
	LoadWString(name, pFile);
	SetName(name);

	//===== 02. �ִϸ��̼� ��� �ҷ�����=====
	wstring path;
	LoadWString(path, pFile);
	SetRelativePath(path);

	//===== 02. ����ϴ� ��Ʋ�� Ű ��� �ҷ�����=====
	LoadResourceRef(m_AtlasTex, pFile);

	//===== 03. �ִϸ��̼� ������ ���� �ҷ�����=====
	size_t FrameCount = 0;
	fread(&FrameCount, sizeof(size_t), 1, pFile);

	//===== 04. ������ ������ŭ ������ ����ü �ҷ�����=====
	for (size_t i = 0; i < FrameCount; ++i)
	{
		tAnim2DFrm frm = {};
		fread(&frm, sizeof(tAnim2DFrm), 1, pFile);
		m_vecFrm.push_back(frm);
	}

	//===== 05. BackSize �ҷ����� =====
	fread(&m_vBackSize, sizeof(Vec2), 1, pFile);

	Reset();

	//���� �ε��� ��ġ�� ��Ʈ���� �ݾ��ش�. 
	fclose(pFile);

	// BackSize UV ������
	m_vBackSizeUV = m_vBackSize / m_vAtlasResolution;
}

