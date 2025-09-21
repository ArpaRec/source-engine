#ifndef __SKYBOX_H
#define __SKYBOX_H

#pragma once

//i just shoved the code from "engine\gl_warp.cpp" into this

void R_UnloadSkys(void);
void R_LoadSkys(void);
void R_DrawSkyBox(float zFar, int nDrawFlags = 0x3F);

void SvSkyboxChangeCallback(IConVar*, const char*, float);

#endif