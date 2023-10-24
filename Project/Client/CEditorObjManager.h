#pragma once

// ������ ������ ��������� ��� ������Ʈ���� �����Ѵ�

class CGameObjectEX;

class CEditorObjManager
	: public CSingleton<CEditorObjManager>
{
	SINGLETON(CEditorObjManager);

private:
	CGameObjectEX*				m_DebugShape[(UINT)SHAPE_TYPE::END];
	vector<tDebugShapeInfo>		m_DebugShapeInfo;

	vector<CGameObjectEX*>		m_vecEditorObj;		// �����Ϳ� ������Ʈ ����


public:
	void init();
	void progress();
	
private:
	void tick();
	void render();

};

