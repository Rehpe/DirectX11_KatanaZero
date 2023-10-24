#pragma once

// 에디터 용으로 만들어지는 모든 오브젝트들을 관리한다

class CGameObjectEX;

class CEditorObjManager
	: public CSingleton<CEditorObjManager>
{
	SINGLETON(CEditorObjManager);

private:
	CGameObjectEX*				m_DebugShape[(UINT)SHAPE_TYPE::END];
	vector<tDebugShapeInfo>		m_DebugShapeInfo;

	vector<CGameObjectEX*>		m_vecEditorObj;		// 에디터용 오브젝트 벡터


public:
	void init();
	void progress();
	
private:
	void tick();
	void render();

};

