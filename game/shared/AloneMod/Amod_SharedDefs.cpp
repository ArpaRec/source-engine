#include "cbase.h"

const char* g_szMapNames[] = {
	//Station
	"d1_trainstation_01_d",
	"d1_trainstation_02_d",
	"d1_trainstation_03_d",
	"d1_trainstation_04_d",
	"d1_trainstation_05_d",
	"d1_trainstation_06_d",

	//Canals
	"d1_canals_01_d",
	"d1_canals_01a_d",
	"d1_canals_02_d",
	"d1_canals_03_d",
	"d1_canals_05_d",
	"d1_canals_06_d",
	"d1_canals_07_d",
	"d1_canals_08_d",
	"d1_canals_09_d",
	"d1_canals_10_d",
	"d1_canals_11_d",
	"d1_canals_12_d",
	"d1_canals_13_d",

	//Elis Lab
	"d1_eli_01_d",
	"d1_eli_02_d",

	//Town
	"d1_town_01_d",
	"d1_town_01a_d",
	"d1_town_02_d",
	"d1_town_02a_d",
	"d1_town_03_d",
	"d1_town_04_d",
	"d1_town_05_d",

	//Coast
	"d2_coast_01_d",
	"d2_coast_03_d",
	"d2_coast_04_d",
	"d2_coast_05_d",
	"d2_coast_07_d",
	"d2_coast_08_d",
	"d2_coast_09_d",
	"d2_coast_10_d",
	"d2_coast_11_d",
	"d2_coast_12_d",

	//Prison
	"d2_prison_01_d",
	"d2_prison_02_d",
	"d2_prison_03_d",
	"d2_prison_04_d",
	"d2_prison_05_d",
	"d2_prison_06_d",
	"d2_prison_07_d",
	"d2_prison_08_d",

	//City
	"d3_c17_01_d",
	"d3_c17_02_d",
	"d3_c17_03_d",
	"d3_c17_04_d",
	"d3_c17_05_d",
	"d3_c17_06a_d",
	"d3_c17_06b_d",
	"d3_c17_07_d",
	"d3_c17_08_d",
	"d3_c17_09_d",
	"d3_c17_10a_d",
	"d3_c17_10b_d",
	"d3_c17_11_d",
	"d3_c17_12b_d",
	"d3_c17_13_d",

	//citadel
	"d3_citadel_01_d",
	"d3_citadel_02_d",
	"d3_citadel_03_d",
	"d3_citadel_04_d",
	"d3_citadel_05_d",

	//final map for hl2
	"d3_breen_01_d",

	//episode 1
	"ep1_citadel_00_d",
	"ep1_citadel_01_d",
	"ep1_citadel_02_d",
	"ep1_citadel_02b_d",
	"ep1_citadel_03_d",
	"ep1_citadel_04_d",

	"ep1_c17_00_d",
	"ep1_c17_00a_d",
	"ep1_c17_01_d",
	"ep1_c17_02_d",
	"ep1_c17_02b_d",
	"ep1_c17_02a_d",
	"ep1_c17_05_d",
	"ep1_c17_06_d",

	"ep2_outland_01_d",
	"ep2_outland_01a_d",
	"ep2_outland_02_d",
	"ep2_outland_03_d",
	"ep2_outland_04_d",
	"ep2_outland_05_d",
	"ep2_outland_06_d",
	"ep2_outland_06a_d",
	"ep2_outland_07_d",
	"ep2_outland_08_d",
	"ep2_outland_09_d",
	"ep2_outland_10_d",
	"ep2_outland_10a_d",
	"ep2_outland_11_d",
	"ep2_outland_11a_d",
	"ep2_outland_12_d",
	"ep2_outland_12a_d",
};

bool IsCityMap(const char* szName)
{
	if (!Q_strcmp(szName, "background06_d") || !Q_strcmp(szName, "background07_d"))
		return true;

	for (unsigned int i = 0; i < sizeof(g_szMapNames) / sizeof(g_szMapNames[0]); i++)
	{
		if (!Q_strcmp(szName, g_szMapNames[i]))
		{
			if (i + 1 > 47 && i + 1 < 68)
				return true;

			return false;
		}
	}
	return false;
}

#ifdef CLIENT_DLL
const char* szMapName = "";
#endif 

const char* g_pBonusMaps[] = {
	"d1_trainstation_01_snowey",
	"d1_trainstation_02_snowey"
};