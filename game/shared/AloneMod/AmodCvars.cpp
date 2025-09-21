#include "cbase.h"

ConVar amod_viewbob_enabled("amod_viewbob_enabled", "0", 0, "Viewbob Toggle");
ConVar amod_viewbob_speed_x("amod_viewbob_speed_x", "5", 0, "X Speed of Viewbob");
ConVar amod_viewbob_speed_y("amod_viewbob_speed_y", "3", 0, "Y Speed of Viewbob");
ConVar amod_viewbob_speed_z("amod_viewbob_speed_z", "6", 0, "Z Speed of Viewbob");
ConVar amod_viewbob_scale_x("amod_viewbob_scale_x", "0.042", 0, "X Scale of Viewbob");
ConVar amod_viewbob_scale_y("amod_viewbob_scale_y", "0.03", 0, "Y Scale of Viewbob");
ConVar amod_viewbob_scale_z("amod_viewbob_scale_z", "0.05", 0, "Z Scale of Viewbob");
ConVar amod_land_zvel_min("amod_land_zvel_min", "-415", 0, "The Minimum Velocity Needed When Falling To Do The Land Viewpunch");
ConVar amod_jump_vel_min("amod_jump_vel_min", "300", 0, "The Minimum Velocity Needed When Jumping To Do The Viewpunch In The Air");
ConVar amod_jump_punch_enable("amod_jump_punch_enable", "0", 0, "Enable The Jump Viewpunch");
ConVar amod_land_punch_enable("amod_land_punch_enable", "0", 0, "Enable The Land Viewpunch");
ConVar amod_enable_god("amod_enable_god", "0", 0, "Makes It So You Cant Take Damage At All");

//filter convars
ConVar amod_filter_on_brightness("amod_filter_on_brightness", "2.15");
ConVar amod_filter_on_exp("amod_filter_on_exp", "1.45");
ConVar amod_filter_off_brightness("amod_filter_off_brightness", "2");

#ifndef CLIENT_DLL

//a little test thing i did
ConVar amod_nomap_restart("amod_nomap_restart", "0");

#else

//lense dirt
ConVar amod_view_lense_dirt("amod_view_lense_dirt", "0", FCVAR_CLIENTDLL);
ConVar amod_view_binoculars("amod_view_binoculars", "0", FCVAR_CLIENTDLL);
ConVar amod_view_bodycam("amod_view_bodycam", "0", FCVAR_CLIENTDLL);
ConVar amod_view_blur("amod_view_blur", "0", FCVAR_CLIENTDLL);

ConVar amod_view_override_xyz_amt("amod_view_override_xyz_amt", "0 0 0", FCVAR_CLIENTDLL);
ConVar amod_view_override_pyr_amt("amod_view_override_pyr_amt", "0 0 0", FCVAR_CLIENTDLL);

ConVar amod_view_square("amod_view_square", "0", FCVAR_CLIENTDLL);
ConVar amod_view_square_width("amod_view_square_width", "0.375", FCVAR_CLIENTDLL);
ConVar amod_view_square_height("amod_view_square_height", "0.2", FCVAR_CLIENTDLL);
ConVar amod_view_claustrophobia("amod_view_claustrophobia", "0", FCVAR_CLIENTDLL);
ConVar amod_view_claustrophobia_amt("amod_view_claustrophobia_amt", "5", FCVAR_CLIENTDLL);
ConVar amod_view_filter_video1("amod_view_filter_video1", "0", FCVAR_CLIENTDLL);
ConVar amod_view_filter_video2("amod_view_filter_video2", "0", FCVAR_CLIENTDLL);
ConVar amod_view_filter_video3("amod_view_filter_video3", "0", FCVAR_CLIENTDLL);
ConVar amod_view_filter_video4("amod_view_filter_video4", "0", FCVAR_CLIENTDLL);
ConVar amod_view_filter_video5("amod_view_filter_video5", "0", FCVAR_CLIENTDLL);
ConVar amod_view_filter_video6("amod_view_filter_video6", "0", FCVAR_CLIENTDLL);
ConVar amod_view_filter_video7("amod_view_filter_video7", "0", FCVAR_CLIENTDLL);
ConVar amod_view_filter_video8("amod_view_filter_video8", "0", FCVAR_CLIENTDLL);

//flashlight
ConVar amod_flashlightlag("amod_flashlightlag", "0", FCVAR_CLIENTDLL);
ConVar amod_flashlightlag_amt("amod_flashlightlag_amt", "0.125", FCVAR_CLIENTDLL, "lower is slower");

ConVar amod_flashlightflicker("amod_flashlightflicker", "1", FCVAR_CLIENTDLL);
ConVar amod_flashlightflicker_wait_time_min("amod_flashlightflicker_wait_time_min", "10", FCVAR_CLIENTDLL);
ConVar amod_flashlightflicker_wait_time_max("amod_flashlightflicker_wait_time_max", "30", FCVAR_CLIENTDLL);
ConVar amod_flashlightflicker_duration_min("amod_flashlightflicker_duration_min", "0.9", FCVAR_CLIENTDLL);
ConVar amod_flashlightflicker_duration_max("amod_flashlightflicker_duration_max", "1.8", FCVAR_CLIENTDLL);
ConVar amod_flashlightflicker_brightness_min("amod_flashlightflicker_brightness_min", "10", FCVAR_CLIENTDLL);
ConVar amod_flashlightflicker_brightness_max("amod_flashlightflicker_brightness_max", "75", FCVAR_CLIENTDLL);
ConVar amod_flashlightflicker_time_interval_min("amod_flashlightflicker_time_interval_min", "0.03", FCVAR_CLIENTDLL);
ConVar amod_flashlightflicker_time_interval_max("amod_flashlightflicker_time_interval_max", "0.085", FCVAR_CLIENTDLL);

//camera
ConVar amod_mirrored("amod_mirrored", "0", FCVAR_CLIENTDLL);

//idk why i made this i kinda just felt like it
ConVar amod_camera_cinematic("amod_camera_cinematic", "0", FCVAR_CLIENTDLL);
ConVar amod_camera_cinematic_lag_origin("amod_camera_cinematic_lag_origin", "0", FCVAR_CLIENTDLL);
ConVar amod_camera_cinematic_lag_angles("amod_camera_cinematic_lag_angles", "1", FCVAR_CLIENTDLL);
ConVar amod_camera_cinematic_lag_origin_amt("amod_camera_cinematic_lag_origin_amt", "0.1", FCVAR_CLIENTDLL);
ConVar amod_camera_cinematic_lag_angles_amt("amod_camera_cinematic_lag_angles_amt", "0.1", FCVAR_CLIENTDLL);
ConVar amod_camera_cinematic_fix("amod_camera_cinematic_fix", "0", FCVAR_CLIENTDLL);

//fog
ConVar amod_fog_disabled("amod_fog_disabled", "0", FCVAR_CLIENTDLL);

ConVar amod_vignette("amod_vignette", "0", FCVAR_CLIENTDLL);

#endif