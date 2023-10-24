#include "pch.h"
#include "CTransform.h"

#include "CDevice.h" 
#include "CConstBuffer.h"

CTransform::CTransform()
	: CComponent(COMPONENT_TYPE::TRANSFORM)
	, m_vRelativeScale(Vec3(1.f, 1.f, 1.f))			// 기본 초기값은 0,0,0으로 0배율은 말이 안되므로 초기 배율값을 1로 셋팅
	, m_bAbsolute(false)
	, m_vRelativeDir{
		  Vec3(1.f, 0.f, 0.f)		//오른쪽
		, Vec3(0.f, 1.f, 0.f)		//위
		, Vec3(0.f, 0.f, 1.f)}		//전방
{
}

CTransform::~CTransform()
{
}

void CTransform::finaltick()
{
	// 크기 행렬
	m_matWorldScale = XMMatrixIdentity();		// 단위행렬(대각선이 1, 나머지는 0인 4*4 행렬. 곱했을 때 자기 자신이 나온다)
	m_matWorldScale = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);	// 각 x,y,z 크기값을 11, 22, 33에 넣어줌

	// 회전 행렬
	Matrix matRot = XMMatrixIdentity();			
	matRot *= XMMatrixRotationX(m_vRelativeRot.x);		// 각 축별로 회전행렬을 만들어주는 함수
	matRot *= XMMatrixRotationY(m_vRelativeRot.y);
	matRot *= XMMatrixRotationZ(m_vRelativeRot.z);

	// 이동 행렬. 크기와 회전 계산을 한 후 꼭 마지막에 곱해주어야한다.
	Matrix matTranslation = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);

	// 최종 월드 행렬 = 크기 x 회전 X 이동
	m_matWorld = m_matWorldScale * matRot * matTranslation;

	// ===== 구하고자 하는 값이 좌표행렬이라면 동차좌표를 1로, 방향벡터일 경우에는 동차좌표를 0으로 설정한다 ====

	// 방향벡터를 구할 때에는 float4(Vec4)로 확장할 때 4번째 좌표(동차좌표)를 1이 아닌 0으로 확장한다
	// World 행렬에서 정확히 회전 행렬만을 가져올 수 없는 경우 World행렬 자체를 곱해 방향을 구할 때도 있는데, 
	// 동차 좌표를 1로 확장할 경우 World행렬의 4행 1,2,3열의 이동값까지 적용되어 방향벡터가 틀어지기 때문이다
	// 따라서 이동 행렬의 영향을 받지 않게 하기 위해 동차좌표는 0으로 설정
	
	// 그러나 동차좌표를 0으로 두어도 World행렬 속 크기 행렬에는 영향을 받을 수밖에 없기 때문에,
	// 정확한 회전행렬을 곱하는 것이 아닌, World행렬을 곱하여 방향벡터를 구할 때에는 
	// 반드시 행렬 계산 완료 후 Normalize(정규화)로 길이를 1로 만드는 작업을 꼭 해주어야한다.

	// 기저 행렬(기본 축 행렬) 에 회전 행렬을 곱하여 오브젝트의 각 축 방향을 구한다
	Vec3 vDefaultDir[3] = {
		  Vec3(1.f, 0.f, 0.f)		//오른쪽(R)
		, Vec3(0.f, 1.f, 0.f)		//위(UP)
		, Vec3(0.f, 0.f, 1.f) };	//전방(Front)

	for (int i = 0; i < 3; i++)
	{
		// Coord/Normal 두 함수의 차이 : Coord는 좌표를 구하는 함수, 따라서 동차좌표를 1로 만들어서 곱하는 함수
		// 우리는 방향을 구해야하므로 이때에는 Normal, 즉 동차좌표를 0으로 확장하는 함수를 사용해야한다.

		// 만일 matRot처럼 정확한 회전행렬을 구할 수 없어 World행렬로 방향을 구할 경우, 반드시 Normalize한다.
		//m_vRelativeDir[i] = XMVector3TransformNormal(vDefalutDir[i], m_matWorld);
		//m_vRelativeDir[i].Normalize();
		
		// 아래처럼 회전행렬만 따올 수 있다면, 어차피 회전행렬은 3x3 안에 유효값이 있어 네번째 자리의 영향을 받지 않으므로 
		// Coord, Normal 어느 것으로 곱해도 상관이 없다.
		//XMVector3TransformCoord(vDefalutDir[i], matRot);

		// 기본적으로 월드 방향은 본인의 상대방향(로컬스페이스상의 방향)으로 설정해둔다. 부모가 있다면 밑에서 바뀔 것
		m_vWorldDir[i] = m_vRelativeDir[i] = XMVector3TransformNormal(vDefaultDir[i], matRot);
	}
	
	// Object Hierachy (오브젝트 계층구조) : 부모가 있는지 확인한다.
	CGameObject* pParent = GetOwner()->GetParent();
	if (pParent)
	{
		// 부모가 있는 경우, 즉 자식 객체인 경우'

		if (m_bAbsolute)
		{
			// 앱솔루트 옵션, 즉 절대값 옵션이 켜져있는 경우

			// 부모의 월드행렬과 월드행렬 중 크기행렬만 따로 떼어서 가져온다.
			// tick-finaltick이 부모-자식 순으로 진행되므로, 언제나 월드행렬을 구할 오브젝트의 부모 월드행렬은 이미 구해진 상태이다
			Matrix matParentWorld = pParent->Transform()->m_matWorld;
			Matrix matParentScale = pParent->Transform()->m_matWorldScale;

			// 앱솔루트(절대값) 옵션이 켜졌다는 것은, 부모의 크기/회전/이동 중 크기에는 관여받고 싶지 않다는 뜻이므로
			// 부모의 크기 행렬의 역행렬을 구해 곱해주어 크기 부분을 단위행렬로 만들어
			// 자식 오브젝트의 월드행렬에 부모의 크기 행렬이 미치는 영향이 없게 만들어준다.

			// 부모의 크기 행렬의 역행렬을 만드는 함수 MatrixInverse
			Matrix matParentScaleInv = XMMatrixInverse(nullptr, matParentScale);

			// 앱솔루트 오브젝트의 최종 월드 행렬 = 로컬 월드행렬 * 부모크기행렬 역 * 부모 월드(크기/회전/이동)
			m_matWorld = m_matWorld * matParentScaleInv * matParentWorld;
		}
		else
		{
			// 자식의 최종 월드 행렬 : 본인의 Relative 행렬(로컬행렬) X 부모의 월드행렬
			m_matWorld *= pParent->Transform()->GetWorldMat();
			m_matWorldScale *= pParent->Transform()->GetWorldScaleMat();
		}

		// 부모가 있는 경우 자식의 WorldDir(월드 방향)은 오브젝트의 상대방향 각각에 부모의 월드행렬을 곱한 후 노멀라이즈 한 값이다
		// 이미 위에서 나의 회전 * 부모의 회전이 모두 적용된 최종 월드 행렬을 구했으므로,
		// 나의 회전값이 적용되지 않은 기저행렬에 최종 월드행렬을 곱한 뒤 크기 영향을 없애기 위해 Normalize 하는 식으로 구한다.
		for (int i = 0; i < 3; ++i)
		{
			m_vWorldDir[i] = XMVector3TransformNormal(vDefaultDir[i], m_matWorld);
			m_vWorldDir[i].Normalize();
		}
	}
}

void CTransform::UpdateData()
{
	// 위치값을 상수버퍼에 전달 및 바인딩

	// Device에서 Transform 용도의 상수버퍼 얻어오기
	CConstBuffer* pTransformBuffer = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);

	// View와 Proj 행렬은 Camera의 finaltick에서 마저 채워질 것
	g_transform.matWorld = m_matWorld;
	g_transform.matWV = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP = g_transform.matWV * g_transform.matProj;

	// 위치값 Map/Unmap
	pTransformBuffer->SetData(&g_transform);
	
 	// 상수 버퍼 레지스터에 바인딩
	pTransformBuffer->UpdateData();
}

void CTransform::SaveToLevelFile(FILE* _File)
{
	// Dir, Matrix값 같은 경우 매 finaltick에서 재계산하는 값이므로 저장할 필요 없음
	fwrite(&m_vRelativePos, sizeof(Vec3), 1, _File);
	fwrite(&m_vRelativeScale, sizeof(Vec3), 1, _File);
	fwrite(&m_vRelativeRot, sizeof(Vec3), 1, _File);
	fwrite(&m_bAbsolute, sizeof(bool), 1, _File);
}

void CTransform::LoadFromLevelFile(FILE* _File)
{
	fread(&m_vRelativePos, sizeof(Vec3), 1, _File);
	fread(&m_vRelativeScale, sizeof(Vec3), 1, _File);
	fread(&m_vRelativeRot, sizeof(Vec3), 1, _File);
	fread(&m_bAbsolute, sizeof(bool), 1, _File);
}

