#include "plugin.h"
#include "Utility.h"
#include "BulletTrails.h"
#include "CCamera.h"
#include "CTxdStore.h"
#include "RenderBuffer.h"
#include "CAudioEngine.h"
#include "CVector.h"
#include "CGeneral.h"
#include "ini.h"
#include <format>
#define ARRAY_SIZE(array)                (sizeof(array) / sizeof(array[0]))
#define FIX_BUGS // Undefine to play with bugs
RwTexture* gpSmokeTrailTexture;
RwTexture* gpTraceTexture;

int CBulletTraces::type[512];
int CBulletTraces::lifetime[512];
int CBulletTraces::randomchance[512];
int CBulletTraces::applysound[512];
int CBulletTraces::randomlength[512];
float CBulletTraces::thickness[512];
int CBulletTraces::red[512];
int CBulletTraces::green[512];
int CBulletTraces::blue[512];
int CBulletTraces::visibility[512];

static RwIm3DVertex TraceVerticesSA[6];
static uint16_t TraceIndexListSA[12] = { 4, 1, 3, 1, 0, 3, 0, 2, 3, 3, 2, 5 };

RwIm3DVertex VCTraceVertices[10];
static RwImVertexIndex VCTraceIndexList[48] = { 0, 5, 7, 0, 7, 2, 0, 7, 5, 0, 2, 7, 0, 4, 9, 0,
										9, 5, 0, 9, 4, 0, 5, 9, 0, 1, 6, 0, 6, 5, 0, 6,
										1, 0, 5, 6, 0, 3, 8, 0, 8, 5, 0, 8, 3, 0, 5, 8 };

RwIm3DVertex TraceVerticesIII[6];
RwImVertexIndex TraceIndexListIII[12];


void CBulletTraces::Init(void)
{
	for (int i = 0; i < 255; i++)
		aTraces[i].m_bInUse = false;
	CTxdStore::PushCurrentTxd();
	int32_t slot2 = CTxdStore::AddTxdSlot("BulletTrails");
	CTxdStore::LoadTxd(slot2, PLUGIN_PATH((char*)"MODELS\\BULLETTRAILS.TXD"));
	int32_t slot = CTxdStore::FindTxdSlot("BulletTrails");
	CTxdStore::SetCurrentTxd(slot);
	gpSmokeTrailTexture = RwTextureRead("smoketrail", NULL);
	gpTraceTexture = RwTextureRead("3trace", NULL);
	CTxdStore::PopCurrentTxd();

	// III

	RwIm3DVertexSetRGBA(&TraceVerticesIII[0], 20, 20, 20, 255);
	RwIm3DVertexSetRGBA(&TraceVerticesIII[1], 20, 20, 20, 255);
	RwIm3DVertexSetRGBA(&TraceVerticesIII[2], 70, 70, 70, 255);
	RwIm3DVertexSetRGBA(&TraceVerticesIII[3], 70, 70, 70, 255);
	RwIm3DVertexSetRGBA(&TraceVerticesIII[4], 10, 10, 10, 255);
	RwIm3DVertexSetRGBA(&TraceVerticesIII[5], 10, 10, 10, 255);
	RwIm3DVertexSetU(&TraceVerticesIII[0], 0.0);
	RwIm3DVertexSetV(&TraceVerticesIII[0], 0.0);
	RwIm3DVertexSetU(&TraceVerticesIII[1], 1.0);
	RwIm3DVertexSetV(&TraceVerticesIII[1], 0.0);
	RwIm3DVertexSetU(&TraceVerticesIII[2], 0.0);
	RwIm3DVertexSetV(&TraceVerticesIII[2], 0.5);
	RwIm3DVertexSetU(&TraceVerticesIII[3], 1.0);
	RwIm3DVertexSetV(&TraceVerticesIII[3], 0.5);
	RwIm3DVertexSetU(&TraceVerticesIII[4], 0.0);
	RwIm3DVertexSetV(&TraceVerticesIII[4], 1.0);
	RwIm3DVertexSetU(&TraceVerticesIII[5], 1.0);
	RwIm3DVertexSetV(&TraceVerticesIII[5], 1.0);

	TraceIndexListIII[0] = 0;
	TraceIndexListIII[1] = 2;
	TraceIndexListIII[2] = 1;
	TraceIndexListIII[3] = 1;
	TraceIndexListIII[4] = 2;
	TraceIndexListIII[5] = 3;
	TraceIndexListIII[6] = 2;
	TraceIndexListIII[7] = 4;
	TraceIndexListIII[8] = 3;
	TraceIndexListIII[9] = 3;
	TraceIndexListIII[10] = 4;
	TraceIndexListIII[11] = 5;

	// VC & Stories
	RwIm3DVertexSetU(&VCTraceVertices[0], 0.0);
	RwIm3DVertexSetV(&VCTraceVertices[0], 0.0);
	RwIm3DVertexSetU(&VCTraceVertices[1], 1.0);
	RwIm3DVertexSetV(&VCTraceVertices[1], 0.0);
	RwIm3DVertexSetU(&VCTraceVertices[2], 1.0);
	RwIm3DVertexSetV(&VCTraceVertices[2], 0.0);
	RwIm3DVertexSetU(&VCTraceVertices[3], 1.0);
	RwIm3DVertexSetV(&VCTraceVertices[3], 0.0);
	RwIm3DVertexSetU(&VCTraceVertices[4], 1.0);
	RwIm3DVertexSetV(&VCTraceVertices[4], 0.0);
	RwIm3DVertexSetU(&VCTraceVertices[5], 0.0);
	RwIm3DVertexSetU(&VCTraceVertices[6], 1.0);
	RwIm3DVertexSetU(&VCTraceVertices[7], 1.0);
	RwIm3DVertexSetU(&VCTraceVertices[8], 1.0);
	RwIm3DVertexSetU(&VCTraceVertices[9], 1.0);

	VCTraceIndexList[0] = 0;
	VCTraceIndexList[1] = 2;
	VCTraceIndexList[2] = 1;
	VCTraceIndexList[3] = 1;
	VCTraceIndexList[4] = 2;
	VCTraceIndexList[5] = 3;
	VCTraceIndexList[6] = 2;
	VCTraceIndexList[7] = 4;
	VCTraceIndexList[8] = 3;
	VCTraceIndexList[9] = 3;
	VCTraceIndexList[10] = 4;
	VCTraceIndexList[11] = 5;

	// SA
	RwIm3DVertexSetRGBA(&TraceVerticesSA[0], 255, 255, 128, 0);
	RwIm3DVertexSetRGBA(&TraceVerticesSA[1], 255, 255, 128, 0);
	RwIm3DVertexSetRGBA(&TraceVerticesSA[2], 255, 255, 128, 0);
	RwIm3DVertexSetRGBA(&TraceVerticesSA[3], 255, 255, 128, 0);
	RwIm3DVertexSetRGBA(&TraceVerticesSA[4], 255, 255, 128, 0);
	RwIm3DVertexSetRGBA(&TraceVerticesSA[5], 255, 255, 128, 0);

	//Traces ini load
	mINI::INIFile file(PLUGIN_PATH((char*)"BulletTrails.ini"));
	mINI::INIStructure ini;
	file.read(ini);

	//Reading weapons params
	for (int32_t i = 0; i < 512; i++)
	{

		std::string name = "WEP";
		std::string formatted_str = std::format(
			"{}{}", name,
			i);

		const char* formatted_str2 = formatted_str.c_str();
		
		std::string strb = ini.get(formatted_str2).get("type");
		const char* strb2 = strb.c_str();
		std::string strc = ini.get(formatted_str2).get("lifetime");
		const char* strc2 = strc.c_str();
		std::string strd = ini.get(formatted_str2).get("randomchance");
		const char* strd2 = strd.c_str();
		std::string stre = ini.get(formatted_str2).get("applysound");
		const char* stre2 = stre.c_str();
		std::string strf = ini.get(formatted_str2).get("randomlength");
		const char* strf2 = strf.c_str();
		std::string strg = ini.get(formatted_str2).get("thickness");
		const char* strg2 = strg.c_str();
		std::string strh = ini.get(formatted_str2).get("red");
		const char* strh2 = strh.c_str();
		std::string stri = ini.get(formatted_str2).get("green");
		const char* stri2 = stri.c_str();
		std::string strj = ini.get(formatted_str2).get("blue");
		const char* strj2 = strj.c_str();
		std::string strk = ini.get(formatted_str2).get("visibility");
		const char* strk2 = strk.c_str();

		type[i] = std::atoi(strb2);
		lifetime[i] = std::atoi(strc2);
		randomchance[i] = std::atoi(strd2);
		applysound[i] = std::atoi(stre2);
		randomlength[i] = std::atof(strf2);
		thickness[i] = std::atof(strg2);
		red[i] = std::atoi(strh2);
		green[i] = std::atoi(stri2);
		blue[i] = std::atoi(strj2);
		visibility[i] = std::atoi(strk2);

	}

}

CBulletTrace CBulletTraces::aTraces[255];

void CBulletTraces::AddTrace(CVector* start, CVector* end, uint8_t applysound, uint8_t randomlength, float thickness,
	uint32_t lifeTime, uint8_t visibility, uint8_t type, uint8_t red, uint8_t green, uint8_t blue)
{
	int32_t enabledCount;
	uint32_t modifiedLifeTime;
	int32_t nextSlot;

	enabledCount = 0;
	for (int i = 0; i < 255; i++)
		if (aTraces[i].m_bInUse)
			enabledCount++;

	switch (type) {
		case TYPE_III:
			modifiedLifeTime = lifeTime; // restrictions weren't applied to III traces in the game
			break;
		case TYPE_VC:
		case TYPE_SA:
		default:
			if (enabledCount >= 10)
				modifiedLifeTime = lifeTime / 4;
			else if (enabledCount >= 5)
				modifiedLifeTime = lifeTime / 2;
			else
				modifiedLifeTime = lifeTime;
			break;
	}

	nextSlot = 0;
	for (int i = 0; nextSlot < 255 && aTraces[i].m_bInUse; i++)
		nextSlot++;
	if (nextSlot < 255) {
		aTraces[nextSlot].m_vecStartPos = *start;
		aTraces[nextSlot].m_vecEndPos = *end;
		aTraces[nextSlot].m_bInUse = true;
		aTraces[nextSlot].m_nCreationTime = CTimer::m_snTimeInMilliseconds;
		aTraces[nextSlot].type = type;
		aTraces[nextSlot].m_nLifeTime = modifiedLifeTime;
		aTraces[nextSlot].m_fThickness = thickness;
		aTraces[nextSlot].m_nRed = red;
		aTraces[nextSlot].m_nGreen = green;
		aTraces[nextSlot].m_nBlue = blue;
		aTraces[nextSlot].m_fVisibility = visibility;
		//aTraces[nextSlot].m_framesInUse = 0; // cut III bs
	}

	if (applysound)
		CBulletTraces::ProcessEffects(&aTraces[nextSlot]);

	// why we need this code?
	/*
	float startProjFwd = DotProduct(TheCamera.GetForward(), *start - TheCamera.GetPosition());
	float endProjFwd = DotProduct(TheCamera.GetForward(), *end - TheCamera.GetPosition());
	if (startProjFwd * endProjFwd < 0.0f) { //if one of point behind us and second before us
		float fStartDistFwd = abs(startProjFwd) / (abs(startProjFwd) + abs(endProjFwd));

		float startProjUp = DotProduct(TheCamera.GetUp(), *start - TheCamera.GetPosition());
		float endProjUp = DotProduct(TheCamera.GetUp(), *end - TheCamera.GetPosition());
		float distUp = (endProjUp - startProjUp) * fStartDistFwd + startProjUp;

		float startProjRight = DotProduct(TheCamera.GetRight(), *start - TheCamera.GetPosition());
		float endProjRight = DotProduct(TheCamera.GetRight(), *end - TheCamera.GetPosition());
		float distRight = (endProjRight - startProjRight) * fStartDistFwd + startProjRight;

		float dist = sqrt(SQR(distUp) + SQR(distRight));
	}*/
}

void CBulletTraces::AddTrace2(CVector* start, CVector* end, int32_t weaponType, class CEntity* shooter)
{
	CPhysical* player;
	float speed;
	int16_t camMode;

	if ((CGeneral::GetRandomNumber() % 100) > randomchance[weaponType])
		return;

	if (shooter == (CEntity*)FindPlayerPed() || (FindPlayerVehicle(NULL, NULL) != NULL && FindPlayerVehicle(NULL, NULL) == (CVehicle*)shooter)) {
		camMode = TheCamera.m_aCams[TheCamera.m_nActiveCam].m_nMode;
		if (camMode == MODE_M16_1STPERSON
			|| camMode == MODE_CAMERA
			|| camMode == MODE_SNIPER
			|| camMode == MODE_M16_1STPERSON_RUNABOUT
			|| camMode == MODE_ROCKETLAUNCHER
			|| camMode == MODE_ROCKETLAUNCHER_RUNABOUT
			|| camMode == MODE_SNIPER_RUNABOUT
			|| camMode == MODE_HELICANNON_1STPERSON) {

			player = FindPlayerVehicle(NULL, NULL) ? (CPhysical*)FindPlayerVehicle(NULL, NULL) : (CPhysical*)FindPlayerPed();
			speed = player->m_vecMoveSpeed.Magnitude();
			if (speed < 0.05f)
				return;
		}
	}

	CVector from = *start;
	CVector to = *end;

	if (randomlength[weaponType])
	{
		CVector dir = *end - *start;
		const float traceLengthOriginal = dir.Magnitude();
		dir.Normalise();

		const float traceLengthNew = CGeneral::GetRandomNumberInRange(0.0f, traceLengthOriginal);
		const float fRadius = std::min(CGeneral::GetRandomNumberInRange(2.0f, 5.0f), traceLengthOriginal - traceLengthNew);

		from = *start + dir * traceLengthNew;
		to = from + dir * fRadius;
	}

	/*AddTrace(
		start,
		end,
		1.0f,
		2000,
		220
	);
	*/

	AddTrace(
		&from,
		&to,
		applysound[weaponType],
		randomlength[weaponType],
		thickness[weaponType],
		lifetime[weaponType],
		visibility[weaponType],
		type[weaponType],
		red[weaponType],
		green[weaponType],
		blue[weaponType]
	);
}

void CBulletTraces::CreateTraceViaHitRound(CVector* start, CVector* end, float thickness, uint8_t time, uint8_t alpha) {
	AddTrace2(
		start,
		end,
		0,
		nullptr
	);
}

CVector operator/(const CVector& vec, float dividend) {
	return { vec.x / dividend, vec.y / dividend, vec.z / dividend };
}

void CBulletTraces::Render(void)
{
	for (int i = 0; i < 255; i++) {
		if (!aTraces[i].m_bInUse)
			continue;

		switch (aTraces[i].type) {
			case TYPE_III:
				Render_III(i);
				break;
			case TYPE_VC:
			case TYPE_STORIES:
				Render_VC(i);
				break;
			case TYPE_SA:
			default:
				Render_SA(i);
				break;
		}
	}
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
}

void CBulletTraces::Update(void)
{
	for (int i = 0; i < 255; i++) {
		if (aTraces[i].m_bInUse)
			aTraces[i].Update();
	}
}

void CBulletTrace::Update(void)
{
	switch (type) {
	case TYPE_III:
	case TYPE_VC:
	case TYPE_SA:
	default:
		if (CTimer::m_snTimeInMilliseconds - m_nCreationTime >= m_nLifeTime)
			m_bInUse = false;
		break;
	}
}

void CBulletTraces::Shutdown(void) {
	if (gpSmokeTrailTexture) {
		RwTextureDestroy(gpSmokeTrailTexture);
		gpSmokeTrailTexture = nullptr;
	}

	if (gpTraceTexture) {
		RwTextureDestroy(gpTraceTexture);
		gpTraceTexture = nullptr;
	}
}

void CBulletTraces::Render_III(int current_slot) {

	const float t = 1.0f - (float)(CTimer::m_snTimeInMilliseconds - aTraces[current_slot].m_nCreationTime) / (float)aTraces[current_slot].m_nLifeTime;
	CVector currPosOnTrace = aTraces[current_slot].m_vecEndPos - (aTraces[current_slot].m_vecEndPos - aTraces[current_slot].m_vecStartPos) * t;

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
#ifdef FIX_BUGS
	// Raster has no transparent pixels so it relies on the raster format having alpha
	// to turn on blending. librw image conversion might get rid of it right now so let's
	// just force it on.
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
#endif
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDONE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RwTextureGetRaster(gpTraceTexture));

	CVector inf = currPosOnTrace;
	CVector sup = aTraces[current_slot].m_vecEndPos;

	CVector center = (inf + sup) / 2;

	CVector width = aTraces[current_slot].CrossProduct(TheCamera.GetForward(), (sup - inf));
	width.Normalise();
	width *= aTraces[current_slot].m_fThickness; //width /= 20;

	//uint8_t intensity = (uint8_t)(t * 60.0); //aTraces[current_slot].m_nLifeTime;

	for (int i = 0; i < ARRAY_SIZE(TraceVerticesIII); i++)
		RwIm3DVertexSetRGBA(&TraceVerticesIII[i], (uint8_t)((float)(aTraces[current_slot].m_nRed) * t),
			(uint8_t)((float)(aTraces[current_slot].m_nGreen) * t), (uint8_t)((float)(aTraces[current_slot].m_nBlue) * t), aTraces[current_slot].m_fVisibility);
		//RwIm3DVertexSetRGBA(&TraceVerticesIII[i], intensity, intensity, intensity, 0xFF);
	RwIm3DVertexSetPos(&TraceVerticesIII[0], inf.x + width.x, inf.y + width.y, inf.z + width.z);
	RwIm3DVertexSetPos(&TraceVerticesIII[1], inf.x - width.x, inf.y - width.y, inf.z - width.z);
	RwIm3DVertexSetPos(&TraceVerticesIII[2], center.x + width.x, center.y + width.y, center.z + width.z);
	RwIm3DVertexSetPos(&TraceVerticesIII[3], center.x - width.x, center.y - width.y, center.z - width.z);
	RwIm3DVertexSetPos(&TraceVerticesIII[4], sup.x + width.x, sup.y + width.y, sup.z + width.z);
	RwIm3DVertexSetPos(&TraceVerticesIII[5], sup.x - width.x, sup.y - width.y, sup.z - width.z);
	if (RwIm3DTransform(TraceVerticesIII, ARRAY_SIZE(TraceVerticesIII), NULL, rwIM3D_VERTEXUV)) {
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, TraceIndexListIII, ARRAY_SIZE(TraceIndexListIII));
		RwIm3DEnd();
	}
}

void CBulletTraces::Render_VC(int current_slot) {
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);

	if (aTraces[current_slot].type == TYPE_VC)
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)(RwTextureGetRaster(gpSmokeTrailTexture)));
	else
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);

	float timeAlive = CTimer::m_snTimeInMilliseconds - aTraces[current_slot].m_nCreationTime;

	float traceThickness = aTraces[current_slot].m_fThickness * timeAlive / aTraces[current_slot].m_nLifeTime;

	CVector horizontalOffset = aTraces[current_slot].m_vecEndPos - aTraces[current_slot].m_vecStartPos;
	horizontalOffset.Normalise();
	horizontalOffset *= traceThickness;

	//then closer trace to die then it more transparent
	uint8_t nAlphaValue = aTraces[current_slot].m_fVisibility * (aTraces[current_slot].m_nLifeTime - timeAlive) / aTraces[current_slot].m_nLifeTime;

	CVector start = aTraces[current_slot].m_vecStartPos;
	CVector end = aTraces[current_slot].m_vecEndPos;
	float startProj = DotProduct(start - TheCamera.GetPosition(), TheCamera.GetForward()) - 0.7f;
	float endProj = DotProduct(end - TheCamera.GetPosition(), TheCamera.GetForward()) - 0.7f;
	if (startProj < 0.0f && endProj < 0.0f) //we dont need render trace behind us
		return;

	if (startProj < 0.0f) { //if strat behind us move it closer
		float absStartProj = std::abs(startProj);
		float absEndProj = std::abs(endProj);
		start = (absEndProj * start + absStartProj * end) / (absStartProj + absEndProj);
	}
	else if (endProj < 0.0f) {
		float absStartProj = std::abs(startProj);
		float absEndProj = std::abs(endProj);
		end = (absEndProj * start + absStartProj * end) / (absStartProj + absEndProj);
	}

	//we divide trace at three parts
	CVector start2 = (7.0f * start + end) / 8;
	CVector end2 = (7.0f * end + start) / 8;

	RwIm3DVertexSetV(&VCTraceVertices[5], 10.0f);
	RwIm3DVertexSetV(&VCTraceVertices[6], 10.0f);
	RwIm3DVertexSetV(&VCTraceVertices[7], 10.0f);
	RwIm3DVertexSetV(&VCTraceVertices[8], 10.0f);
	RwIm3DVertexSetV(&VCTraceVertices[9], 10.0f);

	for (int i = 0; i < 10; i++) {

		RwIm3DVertexSetRGBA(&VCTraceVertices[i], aTraces[current_slot].m_nRed, aTraces[current_slot].m_nGreen, aTraces[current_slot].m_nBlue, nAlphaValue);

		// old
		
		//if (aTraces[current_slot].type == TYPE_STORIES)
		//	RwIm3DVertexSetRGBA(&VCTraceVertices[i], 255, 140, 0, nAlphaValue);
		//else 
		//	RwIm3DVertexSetRGBA(&VCTraceVertices[i], 255, 255, 255, nAlphaValue);
	}

	//two points in center
	RwIm3DVertexSetPos(&VCTraceVertices[0], start2.x, start2.y, start2.z);
	RwIm3DVertexSetPos(&VCTraceVertices[5], end2.x, end2.y, end2.z);

	//vertical planes
	RwIm3DVertexSetPos(&VCTraceVertices[1], start2.x, start2.y, start2.z + traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[3], start2.x, start2.y, start2.z - traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[6], end2.x, end2.y, end2.z + traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[8], end2.x, end2.y, end2.z - traceThickness);

	//horizontal planes
	RwIm3DVertexSetPos(&VCTraceVertices[2], start2.x + horizontalOffset.y, start2.y - horizontalOffset.x, start2.z);
	RwIm3DVertexSetPos(&VCTraceVertices[7], end2.x + horizontalOffset.y, end2.y - horizontalOffset.x, end2.z);
#ifdef FIX_BUGS //this point calculated wrong for some reason
	RwIm3DVertexSetPos(&VCTraceVertices[4], start2.x - horizontalOffset.y, start2.y + horizontalOffset.x, start2.z);
	RwIm3DVertexSetPos(&VCTraceVertices[9], end2.x - horizontalOffset.y, end2.y + horizontalOffset.x, end2.z);
#else
	RwIm3DVertexSetPos(&VCTraceVertices[4], start2.x - horizontalOffset.y, start2.y - horizontalOffset.y, start2.z);
	RwIm3DVertexSetPos(&VCTraceVertices[9], end2.x - horizontalOffset.y, end2.y - horizontalOffset.y, end2.z);
#endif

	if (RwIm3DTransform(VCTraceVertices, ARRAY_SIZE(VCTraceVertices), nullptr, 1)) {
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, VCTraceIndexList, ARRAY_SIZE(VCTraceIndexList));
		RwIm3DEnd();
	}

	RwIm3DVertexSetV(&VCTraceVertices[5], 2.0f);
	RwIm3DVertexSetV(&VCTraceVertices[6], 2.0f);
	RwIm3DVertexSetV(&VCTraceVertices[7], 2.0f);
	RwIm3DVertexSetV(&VCTraceVertices[8], 2.0f);
	RwIm3DVertexSetV(&VCTraceVertices[9], 2.0f);

	for (int i = 0; i < 5; i++) {
		if (aTraces[current_slot].type == TYPE_STORIES)
			RwIm3DVertexSetRGBA(&VCTraceVertices[i], 255, 140, 0, 0);
		else 
			RwIm3DVertexSetRGBA(&VCTraceVertices[i], 255, 255, 255, 0);
	}

	RwIm3DVertexSetPos(&VCTraceVertices[0], start.x, start.y, start.z);
	RwIm3DVertexSetPos(&VCTraceVertices[1], start.x, start.y, start.z + traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[3], start.x, start.y, start.z - traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[2], start.x + horizontalOffset.y, start.y - horizontalOffset.x, start.z);

	RwIm3DVertexSetPos(&VCTraceVertices[5], start2.x, start2.y, start2.z);
	RwIm3DVertexSetPos(&VCTraceVertices[6], start2.x, start2.y, start2.z + traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[8], start2.x, start2.y, start2.z - traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[7], start2.x + horizontalOffset.y, start2.y - horizontalOffset.x, start2.z);
#ifdef FIX_BUGS
	RwIm3DVertexSetPos(&VCTraceVertices[4], start.x - horizontalOffset.y, start.y + horizontalOffset.x, start.z);
	RwIm3DVertexSetPos(&VCTraceVertices[9], start2.x - horizontalOffset.y, start2.y + horizontalOffset.x, start2.z);
#else
	RwIm3DVertexSetPos(&VCTraceVertices[4], start.x - horizontalOffset.y, start.y - horizontalOffset.y, start.z);
	RwIm3DVertexSetPos(&VCTraceVertices[9], start2.x - horizontalOffset.y, start2.y - horizontalOffset.y, start2.z);
#endif

	if (RwIm3DTransform(VCTraceVertices, ARRAY_SIZE(VCTraceVertices), nullptr, rwIM3D_VERTEXUV)) {
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, VCTraceIndexList, ARRAY_SIZE(VCTraceIndexList));
		RwIm3DEnd();
	}

	RwIm3DVertexSetPos(&VCTraceVertices[1], end.x, end.y, end.z);
	RwIm3DVertexSetPos(&VCTraceVertices[2], end.x, end.y, end.z + traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[4], end.x, end.y, end.z - traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[3], end.x + horizontalOffset.y, end.y - horizontalOffset.x, end.z);

	RwIm3DVertexSetPos(&VCTraceVertices[5], end2.x, end2.y, end2.z);
	RwIm3DVertexSetPos(&VCTraceVertices[6], end2.x, end2.y, end2.z + traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[8], end2.x, end2.y, end2.z - traceThickness);
	RwIm3DVertexSetPos(&VCTraceVertices[7], end2.x + horizontalOffset.y, end2.y - horizontalOffset.x, end2.z);
#ifdef FIX_BUGS
	RwIm3DVertexSetPos(&VCTraceVertices[5], end.x - horizontalOffset.y, end.y + horizontalOffset.x, end.z);
	RwIm3DVertexSetPos(&VCTraceVertices[9], end2.x - horizontalOffset.y, end2.y + horizontalOffset.x, end2.z);
#else
	RwIm3DVertexSetPos(&VCTraceVertices[5], end.x - horizontalOffset.y, end.y - horizontalOffset.y, end.z);
	RwIm3DVertexSetPos(&VCTraceVertices[9], end2.x - horizontalOffset.y, end2.y - horizontalOffset.y, end2.z);
#endif

	if (RwIm3DTransform(VCTraceVertices, ARRAY_SIZE(VCTraceVertices), nullptr, rwIM3D_VERTEXUV)) {
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, VCTraceIndexList, ARRAY_SIZE(VCTraceIndexList));
		RwIm3DEnd();
	}
}

void CBulletTraces::Render_SA(int current_slot) {
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)TRUE);
	RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);

		CBulletTrace& trace = aTraces[current_slot];
		if (!trace.m_bInUse) return;

		const float t = 1.0f - (float)(CTimer::m_snTimeInMilliseconds - trace.m_nCreationTime) / (float)trace.m_nLifeTime;

		CVector camToOriginDir = (trace.m_vecStartPos - TheCamera.GetPosition());
		camToOriginDir.Normalise();

		CVector direction = trace.m_vecEndPos - trace.m_vecStartPos;
		direction.Normalise();

		CVector up = aTraces[current_slot].CrossProduct(camToOriginDir, direction);
		up.Normalise();

		CVector sizeVec = up * (trace.m_fThickness * t);
		CVector currPosOnTrace = trace.m_vecEndPos - (trace.m_vecEndPos - trace.m_vecStartPos) * t;

		RwIm3DVertexSetPos(&TraceVerticesSA[0], currPosOnTrace.x, currPosOnTrace.y, currPosOnTrace.z);
		RwIm3DVertexSetPos(&TraceVerticesSA[1], currPosOnTrace.x + sizeVec.x, currPosOnTrace.y + sizeVec.y, currPosOnTrace.z + sizeVec.z);
		RwIm3DVertexSetPos(&TraceVerticesSA[2], currPosOnTrace.x - sizeVec.x, currPosOnTrace.y - sizeVec.y, currPosOnTrace.z - sizeVec.z);
		RwIm3DVertexSetPos(&TraceVerticesSA[3], trace.m_vecEndPos.x, trace.m_vecEndPos.y, trace.m_vecEndPos.z);
		RwIm3DVertexSetPos(&TraceVerticesSA[4], trace.m_vecEndPos.x + sizeVec.x, trace.m_vecEndPos.y + sizeVec.y, trace.m_vecEndPos.z + sizeVec.z);
		RwIm3DVertexSetPos(&TraceVerticesSA[5], trace.m_vecEndPos.x - sizeVec.x, trace.m_vecEndPos.y - sizeVec.y, trace.m_vecEndPos.z - sizeVec.z);

		RwIm3DVertexSetRGBA(&TraceVerticesSA[3], aTraces[current_slot].m_nRed, aTraces[current_slot].m_nGreen, aTraces[current_slot].m_nBlue, (RwUInt8)(t * trace.m_fVisibility));
		
		if (RwIm3DTransform(TraceVerticesSA, ARRAY_SIZE(TraceVerticesSA), NULL, rwIM3D_VERTEXRGBA))
		{
			RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, TraceIndexListSA, ARRAY_SIZE(TraceIndexListSA));
			RwIm3DEnd();
		}

	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)FALSE);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLBACK);
}

void CBulletTraces::ProcessEffects(CBulletTrace* trace)
{
	CMatrix& camMat = TheCamera.m_mCameraMatrix;
	const CVector& camPos = camMat.GetPosition();


	// Make their position relative to the camera's
	const auto fromRelToCam = trace->m_vecStartPos - camPos;
	const auto toRelToCam = trace->m_vecEndPos - camPos;

	// Transform both points into the camera's space ((C)cam (S)pace - CS)
	const float fromCSY = DotProduct(fromRelToCam, camMat.GetForward());

	const float toCSY = DotProduct(toRelToCam, camMat.GetForward());

	if (std::signbit(toCSY) == std::signbit(fromCSY)) { // Originally: toCSY * fromCSY < 0.0f - Check if signs differ
		return; // Both points are either in front or behind us
	}

	// They do, in this case points are on opposite sides (one behind, one in front of the camera)

	// Now calculate the remaining coordinates
	const float fromCSX = DotProduct(fromRelToCam, camMat.GetRight());
	const float fromCSZ = DotProduct(fromRelToCam, camMat.GetUp());

	const float toCSX = DotProduct(toRelToCam, camMat.GetRight());
	const float toCSZ = DotProduct(toRelToCam, camMat.GetUp());

	// Calculate distance to point on line that is on the same Y axis as the camera
	// (This point on line is basically the bullet when passing by the camera)

	// Interpolation on line
	const float t = fabs(fromCSY) / (fabs(fromCSY) + fabs(toCSY));

	const float pointOnLineZ = fromCSZ + (toCSZ - fromCSZ) * t;
	const float pointOnLineX = fromCSX + (toCSX - fromCSX) * t;

	// Calculate distance from camera to point on line
	const float camToLineDist = std::hypotf(pointOnLineZ, pointOnLineX);

	if (camToLineDist >= 2.0f) {
		return; // Point too far from camera
	}

	const auto ReportBulletAudio = [&](int event) {
		const float volDistFactor = 1.0f - camToLineDist * 0.5f;
		const float volumeChange = volDistFactor == 0.0f ? -100.0f : std::log10(volDistFactor);
		AudioEngine.ReportFrontendAudioEvent(event, volumeChange, 1.0f);
		};

	const bool isComingFromBehind = fromCSY <= 0.0f; // Is the bullet coming from behind us?
	if (0.f <= pointOnLineX) { // Is bullet passing on the right of the camera?
		ReportBulletAudio(isComingFromBehind ? AE_FRONTEND_BULLET_PASS_RIGHT_REAR : AE_FRONTEND_BULLET_PASS_RIGHT_FRONT);
	}
	else { // Bullet passing on left of the camera.
		ReportBulletAudio(isComingFromBehind ? AE_FRONTEND_BULLET_PASS_LEFT_REAR : AE_FRONTEND_BULLET_PASS_LEFT_FRONT);
	}
}