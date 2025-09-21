#include "cbase.h"
#include "AloneMod/AmodCvars.h"
#include "cdll_int.h"
#include "engine/IEngineSound.h"
#include "fmtstr.h"
#include <filesystem.h>

KeyValues* KvRainFile;

static bool bDidInit = false;
extern IVEngineClient* clientengine;

void AmodRainChangeCallback(IConVar* cvar, const char*, float);
	
ConVar amod_rain_type("amod_rain_type", "0", 0, "", AmodRainChangeCallback);
ConVar amod_rain_enable("amod_rain_enable", "0", 0, "", AmodRainChangeCallback);
ConVar amod_rain_thunder("amod_rain_thunder", "0", 0);
ConVar amod_rain_wait_min("amod_rain_wait_min", "1200", 0);
ConVar amod_rain_wait_max("amod_rain_wait_max", "2000", 0);

void AmodRainChangeCallback(IConVar* cvar, const char*, float)
{
	if (gpGlobals->eLoadType == MapLoad_Background)
		return;

	bool bRainEnabled = true;

	int type = amod_rain_type.GetInt();
	if (!type || !amod_rain_enable.GetBool())
	{
		bRainEnabled = false;
	}
	else
	{
		if (type == 2)
			bRainEnabled = false;
	}

	if (bRainEnabled)
	{
		CBaseEntity* FirstEnt = gEntList.FirstEnt();
		while (FirstEnt)
		{
			//fire stuff
			if (!Q_strcmp(FirstEnt->GetEntityName().ToCStr(), "_rainrel"))
				FirstEnt->AcceptInput("Trigger", nullptr, nullptr, variant_t{}, 0);

			FirstEnt = gEntList.NextEnt(FirstEnt);
			continue;
		}

		clientengine->ClientCmd("amod_soundscape_startrain");
	}
	else
	{
		clientengine->ClientCmd_Unrestricted("amod_soundscape_stoprain");
	}

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (pPlayer)
	{
		pPlayer->m_fNextRainTime = gpGlobals->curtime + random->RandomFloat(amod_rain_wait_min.GetFloat() , amod_rain_wait_max.GetFloat());
		pPlayer->m_bInRain = false;
	}
}

void PlayRandomThunder()
{
	if (!KvRainFile)
		return;

	static ConCommand* playgamesound = cvar->FindCommand("playgamesound");
	if (!playgamesound)
		return;

	KeyValues* mapkv = KvRainFile->FindKey(gpGlobals->mapname.ToCStr());
	if (!mapkv)
		return;

	CUtlVector<KeyValues*> tmpvec;
	FOR_EACH_VALUE(mapkv, kv)
	{
		tmpvec.AddToTail(kv);
	}

	const char* origin = tmpvec[random->RandomInt(0,tmpvec.Count() >= 1 ? tmpvec.Count() - 1 : 0)]->GetString();

	CCommand args;
	args.Tokenize(CFmtStr("playgamesound weather.thunder%d \"%s\"", random->RandomInt(1, 3), origin));
	playgamesound->Dispatch(args);
}

CON_COMMAND(amod_play_random_thunder, "")
{
	PlayRandomThunder();
}

CON_COMMAND(amod_raininfo,"")
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (!pPlayer)
		return;

	ConMsg("Currently In rain %d\n", pPlayer->m_bInRain);
	ConMsg("Next Rain Time %f\n", pPlayer->m_fNextRainTime - gpGlobals->curtime);
	ConMsg("Next Thunder Time %f\n", pPlayer->m_fNextThunder - gpGlobals->curtime);
}

class CAutoAmodRainSystem : public CAutoGameSystem
{
public:
	void LevelInitPostEntity()
	{
		if (!bDidInit)
		{
			KvRainFile = new KeyValues("RainSystem");
			if (!KvRainFile->LoadFromFile(filesystem, "resource/thunder_locations.txt", "MOD"))
			{
				ConWarning("Failed To Load resource/thunder_locations.txt Thunder Sounds Will Not Play\n");
				KvRainFile->deleteThis();
				KvRainFile = nullptr;
			}
			bDidInit = true;
		}

		//if the r_rainradius value is to high it will crash. i made .cfg files for each map so i can make the r_rainradius
		//value the highest i can on maps where there isnt that much rain like d1_trainstation_01_d
		clientengine->ClientCmd_Unrestricted(CFmtStr("exec rain/%s", gpGlobals->mapname.ToCStr()));

		if (gpGlobals->eLoadType == MapLoad_Background)
			return;

		bool bRainEnabled = true;

		if (!amod_rain_type.GetInt() || !amod_rain_enable.GetBool())
			bRainEnabled = false;

		if (amod_rain_type.GetInt() == 2)
		{
			CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
			if (!pPlayer)
				bRainEnabled = false;
			else
				bRainEnabled = pPlayer->m_bInRain;
		}

		if (bRainEnabled)
		{
			CBaseEntity* FirstEnt = gEntList.FirstEnt();
			while (FirstEnt)
			{
				//fire stuff
				if (!Q_strcmp(FirstEnt->GetEntityName().ToCStr(), "_rainrel"))
					FirstEnt->AcceptInput("Trigger", nullptr, nullptr, variant_t{}, 0);

				FirstEnt = gEntList.NextEnt(FirstEnt);
				continue;
			}
		}
	}
};
static CAutoAmodRainSystem g_RainSystem;