#ifndef __AMOD_SHARED_DEFS_H
#define __AMOD_SHARED_DEFS_H

extern const char* g_szMapNames[98];

bool IsCityMap(const char* szMapName);

#ifdef CLIENT_DLL
extern const char* szMapName;
#endif

extern const char* g_pBonusMaps[2];

//todo: finish me
#define AMOD_DAYTIME_EDITION 0

#endif