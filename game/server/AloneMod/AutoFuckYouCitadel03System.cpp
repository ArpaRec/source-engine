//i have to hardcode everything for ep1_citadel_03_d cause it aint compiling
#include "cbase.h"
#include "cdll_int.h"

extern IVEngineClient* clientengine;

class CAutoFuckYouCitadel03ForEpisode1System : public CAutoGameSystem
{
public:
	void LevelInitPostEntity()
	{
		if (Q_stricmp(gpGlobals->mapname.ToCStr(), "ep1_citadel_03_d"))
			return;

		bool bFoundSong = false;
		bool bFoundCore = false;

		CBaseEntity* pEntFirst = gEntList.FirstEnt();
		while (pEntFirst)
		{
			const char* entityname = pEntFirst->GetEntityName().ToCStr();
			const char* entityclassname = pEntFirst->GetClassname();
			if (!entityname || !entityclassname)
			{
				pEntFirst = gEntList.NextEnt(pEntFirst);
				continue;
			}

			if (!Q_stricmp(entityname, "song_"))
			{
				bFoundSong = true;
			}
			if (!Q_stricmp(entityname, "timer_core"))
			{
				bFoundCore = true;
			}
			else if (Q_strstr(entityclassname, "npc_"))
			{
				if (Q_stricmp(entityclassname, "npc_bullseye"))
					UTIL_Remove(pEntFirst);
			}
			else if (!Q_stricmp(entityclassname, "func_door"))
			{
				if (Q_strstr(entityname, "shutter_door_") || Q_strstr(entityname, "door_comb_1_"))
				{
					UTIL_Remove(pEntFirst);
				}
			}
			else if (!Q_stricmp(entityclassname, "logic_relay"))
			{
				if (!Q_stricmp(entityname, "relay_alarm1") || !Q_stricmp(entityname, "relay_alarm1") || !Q_stricmp(entityname, "logic_door_comb_1_close"))
				{
					UTIL_Remove(pEntFirst);
				}
			}
			else if (!Q_stricmp(entityclassname, "env_soundscape") || Q_strstr(entityname, "song") || Q_strstr(entityname, "sound_advisor") || !Q_stricmp(entityname, "pclip_door1") || !Q_stricmp(entityname, "Monitor_advisor_1")
				|| !Q_stricmp(entityname, "template_manhacks") || !Q_stricmp(entityname, "template_combine_upperfirstroom") || !Q_stricmp(entityname, "template_combine_exit") || !Q_stricmp(entityname, "template_combine_1c")
				|| !Q_stricmp(entityname, "text_linux") || !Q_stricmp(entityname, "command_linux"))
			{
				UTIL_Remove(pEntFirst);
			}
			else if (!Q_stricmp(entityname, "trigger_entry"))
			{
				pEntFirst->KeyValue("onstarttouch", "func_areaportal,open,,0,-1");
				pEntFirst->KeyValue("onstarttouch", "lift_airlock,close,,0,-1");
				pEntFirst->KeyValue("onstarttouch", "maker_balltrap,forcespawn,,0,-1");
				pEntFirst->KeyValue("onstarttouch", "Core_lift_doors,open,,8,-1");
				pEntFirst->KeyValue("onstarttouch", "Trigger_lift,enable,,0,-1");
				pEntFirst->KeyValue("onstarttouch", "relay_core_enable,trigger,,0,-1");
				pEntFirst->KeyValue("onstarttouch", "song_,playsound,,0,-1");
				pEntFirst->KeyValue("onstarttouch", "door_core_exit,setanimation,idle_open,0,-1");
				pEntFirst->KeyValue("onstarttouch", "relay_laserpower_fail,kill,,0,-1");
				pEntFirst->KeyValue("onstarttouch", "template_battery_counters,forcespawn,,0,-1");
				pEntFirst->KeyValue("onstarttouch", "template_battery_counters,kill,,0.5,-1");
				pEntFirst->KeyValue("onstarttouch", "relay_controlroom3_finished,enable,,2,-1");
			}
			else if (!Q_stricmp(entityclassname, "func_areaportal"))
			{
				pEntFirst->KeyValue("targetname", "");
				pEntFirst->AcceptInput("open", nullptr, nullptr, variant_t{}, 0);
			}
			else if (!Q_stricmp(entityname, "Trigger_lift"))
			{
				pEntFirst->KeyValue("OnStartTouch", "lift_airlock,open,,2.5,-1");
				pEntFirst->KeyValue("OnStartTouch", "pclip_core_elevator_1,enable,,0,-1");
				pEntFirst->KeyValue("OnStartTouch", "!self,kill,,3,-1");
			}
			else if (!Q_stricmp(entityname, "trigger_socket_6"))
			{
				pEntFirst->KeyValue("OnStartTouch", "relay_controlroom3_finished,enable,,2,-1");
				pEntFirst->KeyValue("OnStartTouch", "relay_controlroom3_finished,trigger,,9,-1");
				pEntFirst->KeyValue("OnStartTouch", "timer_core,StopCoreTimer,,0,-1");
				pEntFirst->KeyValue("OnStartTouch", "timer_core,Disable,,0,-1");
			}
			else if (pEntFirst->GetAbsOrigin() == Vector(1152, 13654, 5312))
			{
				pEntFirst->KeyValue("onstarttouch", "Teleport_lift_doors,close,,0,-1");
				pEntFirst->KeyValue("onstarttouch", "Train_lift_coreexit,startforward,,3,-1");
				pEntFirst->KeyValue("onstarttouch", "!self,kill,,1,-2");
			}
			pEntFirst = gEntList.NextEnt(pEntFirst);
		}

		clientengine->ClientCmd_Unrestricted("wait 200; playsoundscape inside.citadel_ep1");

		if (!bFoundSong)
		{
			CBaseEntity* pSong = CreateEntityByName("ambient_generic");
			pSong->KeyValue("targetname", "song_");
			pSong->KeyValue("message", "music/away.mp3");
			pSong->KeyValue("health", "8");
			pSong->KeyValue("spawnflags", "49");
			pSong->KeyValue("radius", "1250");
			pSong->Precache();
			DispatchSpawn(pSong);
			pSong->Activate();
		}

		if (!bFoundCore)
		{
			CBaseEntity* core = CreateEntityByName("amod_core_timer");
			core->KeyValue("targetname", "timer_core");
			
			CBaseEntity* la_mapspawn = CreateEntityByName("logic_auto");
			la_mapspawn->KeyValue("spawnflags", "1");
			la_mapspawn->KeyValue("onmapspawn", "timer_core,StartCoreTimer,450");

			CBaseEntity* la_loadgame = CreateEntityByName("logic_auto");
			la_loadgame->KeyValue("spawnflags", "0");
			la_loadgame->KeyValue("OnLoadGame", "timer_core,ShowCoreTimer,,5.5,-1");
			la_loadgame->KeyValue("OnMapTransition", "timer_core,ShowCoreTimer,,5,-1");

			core->Precache();
			DispatchSpawn(core);
			core->Activate();

			la_loadgame->Precache();
			DispatchSpawn(la_loadgame);
			la_loadgame->Activate();
			
			la_mapspawn->Precache();
			DispatchSpawn(la_mapspawn);
			la_mapspawn->Activate();
		}
	}
};
static CAutoFuckYouCitadel03ForEpisode1System g_workplease;