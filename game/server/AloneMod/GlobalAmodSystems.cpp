//a shitty little system for the skybox changer
#include "cbase.h"
#include "cdll_int.h"
#include "fmtstr.h"
#include <filesystem.h>
#include "AloneMod/Amod_SharedDefs.h"

#if !AMOD_DAYTIME_EDITION

extern IVEngineClient* clientengine;

void AmodSkyCallback(IConVar* var, const char*, float);
void AmodEpicFilterCallback(IConVar* var, const char*, float);

ConVar amod_sky_override("amod_sky_override", "1");
ConVar amod_sky("amod_sky", "sky_borealis01", 0, "", AmodSkyCallback);
ConVar amod_epic_filter("amod_epic_filter", "0", 0, "", AmodEpicFilterCallback);

//
CBaseEntity* CreateEpicFilter()
{
	CBaseEntity* cc = gEntList.FindEntityByName(nullptr, "_cc_epic_filter_");
	if (cc)
		return cc;
		
	cc = CreateEntityByName("color_correction");
	cc->KeyValue("targetname", "_cc_epic_filter_");
	cc->KeyValue("minfalloff", "50000");
	cc->KeyValue("maxfalloff", "50000");
	cc->KeyValue("maxweight", "0.8");
	cc->KeyValue("filename", "scripts/colorcorrection/cc_epic_filter.raw");;

	cc->Precache();
	DispatchSpawn(cc);
	cc->Activate();

	return cc;
}

//im not even kidding i tinkered around with this for like an hour to get it to work
void AmodSkyCallback(IConVar* var, const char*, float)
{
	if (!amod_sky_override.GetBool())
		return;

	if (Q_strstr(gpGlobals->mapname.ToCStr(), "ep1") || Q_strstr(gpGlobals->mapname.ToCStr(), "amod_outro") || !Q_strcmp(gpGlobals->mapname.ToCStr(), "background08_d") || !Q_strcmp(gpGlobals->mapname.ToCStr(), "background10_d"))
		return;

	static ConVar* day = cvar->FindVar("amod_day");
	if (!day)
		return;

	if (day->GetBool() && Q_strcmp(amod_sky.GetString(), "sky_day02_09") && IsCityMap(gpGlobals->mapname.ToCStr()))
		return;

	if (gpGlobals->eLoadType == MapLoad_Background)
		clientengine->ExecuteClientCmd(CFmtStr("map_background %s", gpGlobals->mapname.ToCStr()));
	else
		clientengine->ExecuteClientCmd("save quick001_sky; load quick001_sky");
}

void AmodEpicFilterCallback(IConVar* var, const char*, float)
{
	CBaseEntity* epicfilter = gEntList.FindEntityByName(nullptr, "_cc_epic_filter_");
	if (!epicfilter)
		epicfilter = CreateEpicFilter();

	if (amod_epic_filter.GetBool())
		epicfilter->AcceptInput("Enable", nullptr, nullptr, variant_t{}, 0);
	else
		epicfilter->AcceptInput("Disable", nullptr, nullptr, variant_t{}, 0);
}

KeyValues* KvSkyes = nullptr;

void DefaultFogOverride()
{
	clientengine->ClientCmd_Unrestricted("fog_override 1");

	clientengine->ClientCmd_Unrestricted("fog_enable 1");
	clientengine->ClientCmd_Unrestricted("fog_enableskybox 1");

	clientengine->ClientCmd_Unrestricted("fog_start 1500");
	clientengine->ClientCmd_Unrestricted("fog_end 2750");

	clientengine->ClientCmd_Unrestricted("fog_startskybox 1500");
	clientengine->ClientCmd_Unrestricted("fog_endskybox 3000");

	clientengine->ClientCmd_Unrestricted("fog_maxdensity 0.95");
	clientengine->ClientCmd_Unrestricted("fog_maxdensityskybox 0.8");

	clientengine->ClientCmd_Unrestricted("fog_color 55 55 70");
	clientengine->ClientCmd_Unrestricted("fog_colorskybox 55 55 70");

	clientengine->ClientCmd_Unrestricted("r_farz -1");
	clientengine->ClientCmd_Unrestricted("r_pixelfog 1");
}

void DoFogOverride(KeyValues* kv, const char* str = nullptr)
{
	KeyValues* mapkv = kv->FindKey(str ? str : gpGlobals->mapname.ToCStr());
	if (!mapkv)
	{
		DefaultFogOverride();
		return;
	}

	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_override %d", mapkv->GetBool("fog_override", true)));

	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_enable %d", mapkv->GetBool("fog_enable", true)));
	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_enableskybox %d", mapkv->GetBool("fog_enableskybox", true)));

	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_start %d", mapkv->GetInt("fog_start", 1500)));
	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_end %d", mapkv->GetInt("fog_end", 2750)));

	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_startskybox %d", mapkv->GetInt("fog_startskybox", true)));
	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_endskybox %d", mapkv->GetInt("fog_endskybox", true)));

	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_maxdensity %f", mapkv->GetFloat("fog_maxdensity", 0.95)));
	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_maxdensityskybox %f", mapkv->GetFloat("fog_maxdensityskybox", 0.7)));

	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_color %s", mapkv->GetString("fog_color", "55 55 70")));
	clientengine->ClientCmd_Unrestricted(CFmtStr("fog_colorskybox %s", mapkv->GetString("fog_colorskybox", "55 55 70")));

	clientengine->ClientCmd_Unrestricted(CFmtStr("r_farz %s", mapkv->GetString("r_farz", "-1")));
	clientengine->ClientCmd_Unrestricted(CFmtStr("r_pixelfog %d", mapkv->GetBool("r_pixelfog", true)));
}

CON_COMMAND(amod_fog_reset, "")
{
	ConVar* day = cvar->FindVar("amod_day");
	if (!day)
		return;

	if (!day->GetBool())
		return;

	if (KvSkyes)
		KvSkyes->deleteThis();

	KvSkyes = nullptr;

	KvSkyes = new KeyValues("");
	if (!KvSkyes->LoadFromFile(filesystem, "resource\\amod_city_fogs.txt", "MOD"))
	{
		ConWarning("Failed To Load amod_city_fog.txt Day Time Section Use Default Fog\n");
		KvSkyes->deleteThis();
		KvSkyes = nullptr;

		DefaultFogOverride();

		return;
	}
	else
	{

		if (day->GetBool())
		{
			if (IsCityMap(gpGlobals->mapname.ToCStr()))
			{
				DoFogOverride(KvSkyes);
			}
		}
	}
}

static bool bDidFilterOn = false;

bool DoBonusMapCheck(const char* mapname)
{
	for (int i = 0; i < sizeof(g_pBonusMaps) / sizeof(g_pBonusMaps[0]); i++)
	{
		if (!Q_strcmp(g_pBonusMaps[i], mapname))
		{
			ConVar* sname = cvar->FindVar("sv_skyname");
			if (sname && amod_sky_override.GetBool())
				sname->SetValue(amod_sky.GetString());

			return false;
		}
	}

	return true;
}

//TODO: MAKE ALGORITHM SO I CAN USE MORE SKYBOXES
class CAmodAutoGameSystem : public CAutoGameSystem
{
public:
	void LevelInitPreEntity()
	{
		static ConVar* fog_override = cvar->FindVar("fog_override");
		if (!fog_override)
			return;
		
		fog_override->SetValue(0);

		//no 
		if (Q_strstr(gpGlobals->mapname.ToCStr(), "ep1") || Q_strstr(gpGlobals->mapname.ToCStr(), "amod_outro") || !Q_strcmp(gpGlobals->mapname.ToCStr(), "background08_d") || !Q_strcmp(gpGlobals->mapname.ToCStr(), "background10_d"))
			return;

		if (!DoBonusMapCheck(gpGlobals->mapname.ToCStr()))
			return;

		if (!KvSkyes)
		{
			KvSkyes = new KeyValues("");
			if (!KvSkyes->LoadFromFile(filesystem, "resource\\amod_city_fogs.txt", "MOD"))
			{
				ConWarning("Failed To Load amod_city_fog.txt Day Time Section Use Default Fog\n");
				KvSkyes->deleteThis();
				KvSkyes = nullptr;
			}
		}

		clientengine->ClientCmd_Unrestricted("r_pixelfog 1");

		if (!amod_sky_override.GetBool())
			return;

		static ConVar* sname = cvar->FindVar("sv_skyname");
		if (!sname)
			return;

		static ConVar* day = cvar->FindVar("amod_day");
		if (!day)
			return;
		
		static ConVar* day_rav = cvar->FindVar("amod_day_ravenholm");
		if (!day_rav)
			return;

		if (day->GetBool() || day_rav->GetBool())
		{
			if (IsCityMap(gpGlobals->mapname.ToCStr()) && day->GetBool())
			{
				if (!bDidFilterOn)
				{
					clientengine->ClientCmd_Unrestricted("tf2");
					clientengine->ClientCmd_Unrestricted("alias Amod_ToggleFilter");
				}
				bDidFilterOn = true;

				sname->SetValue("sky_day02_09");

				if (KvSkyes)
					DoFogOverride(KvSkyes);
				else
					DefaultFogOverride();

				return;
			}
			else if (day_rav->GetBool() && Q_strcmp(gpGlobals->mapname.ToCStr(), "d1_town_05_d") && Q_strstr(gpGlobals->mapname.ToCStr(), "d1_town_"))
			{
				if (!bDidFilterOn)
				{
					clientengine->ClientCmd_Unrestricted("tf2");
					clientengine->ClientCmd_Unrestricted("alias Amod_ToggleFilter");
				}
				bDidFilterOn = true;

				sname->SetValue("sky_day01_05");

				if (KvSkyes)
					DoFogOverride(KvSkyes);
				else
					DefaultFogOverride();

				return;
			}
			else
			{
				if (bDidFilterOn)
				{
					clientengine->ClientCmd_Unrestricted("alias Amod_ToggleFilter tf2; tf1");
					bDidFilterOn = false;
				}

				fog_override->SetValue(false);

				if (((!Q_strcmp(sname->GetString(), "sky_day02_09") && Q_strcmp(amod_sky.GetString(), "sky_day02_09")) || (Q_strcmp(sname->GetString(), "sky_day02_09") && Q_strcmp(amod_sky.GetString(), "sky_day02_09")))
					|| ((!Q_strcmp(sname->GetString(), "sky_day01_05") && Q_strcmp(amod_sky.GetString(), "sky_day01_05")) || (Q_strcmp(sname->GetString(), "sky_day01_05") && Q_strcmp(amod_sky.GetString(), "sky_day01_05"))))
					sname->SetValue(amod_sky.GetString());
				else
					sname->SetValue("sky_borealis01");
			}
		}
		else
		{
			if (bDidFilterOn)
			{
				clientengine->ClientCmd_Unrestricted("alias Amod_ToggleFilter tf2; tf1");
				bDidFilterOn = false;
			}

			fog_override->SetValue(false);

			if (((!Q_strcmp(sname->GetString(), "sky_day02_09") && Q_strcmp(amod_sky.GetString(), "sky_day02_09")) || (Q_strcmp(sname->GetString(), "sky_day02_09") && Q_strcmp(amod_sky.GetString(), "sky_day02_09")))
				|| ((!Q_strcmp(sname->GetString(), "sky_day01_05") && Q_strcmp(amod_sky.GetString(), "sky_day01_05")) || (Q_strcmp(sname->GetString(), "sky_day01_05") && Q_strcmp(amod_sky.GetString(), "sky_day01_05"))))
				sname->SetValue(amod_sky.GetString());
			else
				sname->SetValue("sky_borealis01");
		}
	}

	void LevelInitPostEntity()
	{
		CBaseEntity* cc = CreateEpicFilter();

		if (amod_epic_filter.GetBool())
			cc->AcceptInput("Enable", nullptr, nullptr, variant_t{}, 0);
		else
			cc->AcceptInput("Disable", nullptr, nullptr, variant_t{}, 0);

		if (gpGlobals->eLoadType == MapLoad_Transition)
			clientengine->ClientCmd_Unrestricted("amod_songpanel_changelevel");
		else
			clientengine->ClientCmd_Unrestricted("amod_songpanel_newlevel");

		//i made these so the self esteem fund song from the start will go througth the 1st and 2nd map (continue playing)
		static ConCommand* cmd_stop = cvar->FindCommand("Amod_SefStop");
		static ConCommand* cmd_start = cvar->FindCommand("Amod_SefStart");
		static ConCommand* cmd_start01 = cvar->FindCommand("Amod_SefStart_01");
		if (gpGlobals->eLoadType != MapLoad_NewGame && !Q_strcmp(gpGlobals->mapname.ToCStr(), "d1_trainstation_01_d"))
		{
			if (cmd_stop)
				cmd_stop->Dispatch(CCommand{});
			else
				clientengine->ClientCmd_Unrestricted("Amod_SefStop");
		}
		else if (gpGlobals->eLoadType == MapLoad_NewGame && !Q_strcmp(gpGlobals->mapname.ToCStr(), "d1_trainstation_01_d"))
		{
			if (cmd_start01)
				cmd_start01->Dispatch(CCommand{});
			else
				clientengine->ClientCmd_Unrestricted("Amod_SefStart_01");
		}
		else if (gpGlobals->eLoadType == MapLoad_Transition && !Q_strcmp(gpGlobals->mapname.ToCStr(), "d1_trainstation_02_d"))
		{
			if (cmd_start)
				cmd_start->Dispatch(CCommand{});
			else
				clientengine->ClientCmd_Unrestricted("Amod_SefStart");
		}
	}
};
CAmodAutoGameSystem g_AmodAGS;

//simple entity for background06_d
class CAmodFogSetter : public CBaseEntity
{
public:
	DECLARE_CLASS(CAmodFogSetter, CBaseEntity);
	DECLARE_DATADESC();

	void InputSetFog(inputdata_t& input);
	void OnRestore();
private:
	string_t m_szFogName;
};

LINK_ENTITY_TO_CLASS(amod_fog_setter, CAmodFogSetter);

BEGIN_DATADESC(CAmodFogSetter)
DEFINE_KEYFIELD(m_szFogName, FIELD_STRING, "fogname"),
DEFINE_INPUTFUNC(FIELD_STRING, "SetFog", InputSetFog)
END_DATADESC()

void CAmodFogSetter::InputSetFog(inputdata_t& input)
{
	static ConVarRef amod_day("amod_day");
	if (!amod_day.GetBool())
		return;

	m_szFogName = AllocPooledString(input.value.String());
	DoFogOverride(KvSkyes, m_szFogName.ToCStr());
}

void CAmodFogSetter::OnRestore()
{
	static ConVarRef amod_day("amod_day");
	if (!amod_day.GetBool())
		return;

	DoFogOverride(KvSkyes, m_szFogName.ToCStr());
	BaseClass::OnRestore();
}

#endif

//core timer thinggy
class CAmodCoreTimer : public CBaseEntity
{
public:
	DECLARE_CLASS(CAmodCoreTimer, CBaseEntity);
	DECLARE_DATADESC();

	CAmodCoreTimer()
	{
		m_bEnabled = true;
	}

	void InputStartCoreTimer(inputdata_t& input);
	void InputStartCitadelTimer(inputdata_t& input);

	void InputStopCoreTimer(inputdata_t& input);
	void InputStopCitadelTimer(inputdata_t& input);

	void InputShowCoreTime(inputdata_t& input);
	void InputShowCitadelTime(inputdata_t& input);

	void InputDisable(inputdata_t& input)
	{
		m_bEnabled = false;
	}
private:
	bool m_bEnabled = true;
};

LINK_ENTITY_TO_CLASS(amod_core_timer, CAmodCoreTimer);

BEGIN_DATADESC(CAmodCoreTimer)
DEFINE_INPUTFUNC(FIELD_INTEGER, "StartCoreTimer", InputStartCoreTimer),
DEFINE_INPUTFUNC(FIELD_INTEGER, "StartCitadelTimer", InputStartCitadelTimer),
DEFINE_INPUTFUNC(FIELD_VOID, "StopCoreTimer", InputStopCoreTimer),
DEFINE_INPUTFUNC(FIELD_VOID, "StopCitadelTimer", InputStopCitadelTimer),
DEFINE_INPUTFUNC(FIELD_VOID, "ShowCoreTime", InputShowCoreTime),
DEFINE_INPUTFUNC(FIELD_VOID, "ShowCitadel", InputShowCitadelTime),
DEFINE_INPUTFUNC(FIELD_VOID, "Disable", InputDisable),
END_DATADESC()

ConVar amod_do_core_timer("amod_do_core_timer", "1");
ConVar amod_do_citadel_timer("amod_do_citadel_timer", "1");

void CAmodCoreTimer::InputStartCoreTimer(inputdata_t& input)
{
	if (!amod_do_core_timer.GetBool())
		return;

	int seconds = input.value.Int();

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	pPlayer->m_fCoreTimerTime = gpGlobals->curtime + (float)seconds;
	pPlayer->m_bInCoreTimer = true;

	char buf[128];
	Q_snprintf(buf, sizeof(buf), "%d:%02d Minutes Till Core Collapse", seconds / 60, seconds % 60);
	UTIL_HudHintText(pPlayer, buf);
}

void CAmodCoreTimer::InputStartCitadelTimer(inputdata_t& input)
{
	if (!amod_do_citadel_timer.GetBool())
		return;

	int seconds = input.value.Int();

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	pPlayer->m_fCitadelTimerTime = gpGlobals->curtime + (float)seconds;
	pPlayer->m_bInCitadelTimer = true;


	char buf[128];
	Q_snprintf(buf, sizeof(buf), "%d:%02d Minutes Till Citadel Explosion", seconds / 60, seconds % 60);
	UTIL_HudHintText(pPlayer, buf);
}

void CAmodCoreTimer::InputStopCoreTimer(inputdata_t& input)
{
	if (!amod_do_core_timer.GetBool())
		return;

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	UTIL_HudHintText(pPlayer, "Citadel Core Neutralized");
	pPlayer->m_bInCoreTimer = false;
}

void CAmodCoreTimer::InputStopCitadelTimer(inputdata_t& input)
{
	if (!amod_do_citadel_timer.GetBool())
		return;

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	pPlayer->m_bInCitadelTimer = false;
	UTIL_HudHintText(pPlayer, "Citadel Explosion Starting Now..");
}

void CAmodCoreTimer::InputShowCitadelTime(inputdata_t& input)
{
	if (!m_bEnabled || !amod_do_citadel_timer.GetBool())
		return;

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer->m_bInCitadelTimer)
		return;

	char buf[128];
	int ctime = (int)(pPlayer->m_fCitadelTimerTime - gpGlobals->curtime);
	Q_snprintf(buf, sizeof(buf), "%d:%02d Minutes Till Citadel Explosion", (int)(ctime / 60), (int)((int)ctime % 60));
	UTIL_HudHintText(pPlayer, buf);
}

void CAmodCoreTimer::InputShowCoreTime(inputdata_t& input)
{
	if (!m_bEnabled || !amod_do_core_timer.GetBool())
		return;

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer->m_bInCoreTimer)
		return;

	char buf[128];
	int ctime = (int)(pPlayer->m_fCoreTimerTime - gpGlobals->curtime);
	Q_snprintf(buf, sizeof(buf), "%d:%02d Minutes Till Core Collapse", (int)(ctime / 60), (int)((int)ctime % 60));
	UTIL_HudHintText(pPlayer, buf);
}