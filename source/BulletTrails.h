#pragma once

struct CBulletTrace
{
	CVector m_vecStartPos;
	CVector m_vecEndPos;
	bool m_bInUse;
	uint32_t m_nCreationTime;
	//uint8_t m_framesInUse;
	uint8_t type;
	uint32_t m_nLifeTime;
	float m_fMaxLength;
	float m_fThickness;
	uint8_t m_nRed;
	uint8_t m_nGreen;
	uint8_t m_nBlue;
	uint8_t m_fVisibility;

	void Update(void);

	// III

	CVector CrossProduct(const CVector& v1, const CVector& v2) { return CVector(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }
};

enum TraceType {
	TYPE_NONE = -1,
	TYPE_III,
	TYPE_VC,
	TYPE_SA,
	TYPE_STORIES
};


class CBulletTraces
{
public:
	static CBulletTrace aTraces[255];

	static void Init(void);
	static void Render(void);
	static void Update(void);
	static void Shutdown(void);
	static void AddTrace(CVector* start, CVector* end, uint8_t applysound, uint8_t randomlength, float thickness,
		uint32_t lifeTime, uint8_t visibility, uint8_t type, uint8_t red, uint8_t green, uint8_t blue);
	static void AddTrace2(CVector* start, CVector* end, int32_t weaponType, class CEntity* shooter);

	static void Render_III(int current_slot);
	static void Render_VC(int current_slot);
	static void Render_SA(int current_slot);
	static void ProcessEffects(CBulletTrace* trace);

public:
	static int type[512];
	static int lifetime[512];
	static int applysound[512];
	static int randomlength[512];
	static float thickness[512];
	static int red[512];
	static int green[512];
	static int blue[512];
	static int visibility[512];

};
