#pragma once
#include "CSingleton.h"

enum class PLAYMODE
{
    NONE,
    FORWARD,
    BACKWARD,
};

class CRecordManager :
    public CSingleton<CRecordManager>
{
    SINGLETON(CRecordManager);

private:
    bool                                m_bRecord;
    int                                 m_iCurFrm;
    std::multimap<int, tGameObjData>    m_mapRecordData;
    map<int, int>                       m_OriginalID;

    int                       m_iFrameCounter;
    UINT                      m_PlayMode;

    bool                      m_bFinishPlay;
    int                       m_iMaxFrame;

public:
    void init();
    void tick();

public:
    void RecordLevelData();
    void Playback();
    void ReversePlayback();

public:
    void ResetRecordData();
    void SetPlayMode(UINT _Mode) { m_PlayMode = _Mode; }
    UINT GetPlayMode() { return m_PlayMode; }
    void StartRecording() { m_bRecord = true; }
    void EndRecording() { m_bRecord = false; }

    int GetCurFrm() { return m_iCurFrm; }
    void SetCurFrm(int _i) { m_iCurFrm = _i; }
    void ResetFrame() { m_iCurFrm = 0; }

    bool IsFinishPlaying() { return m_bFinishPlay; }
};


