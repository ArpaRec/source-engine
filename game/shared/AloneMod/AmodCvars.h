#include "convar.h"

extern ConVar amod_viewbob_enabled;
extern ConVar amod_viewbob_speed_x;
extern ConVar amod_viewbob_speed_y;
extern ConVar amod_viewbob_speed_z;
extern ConVar amod_viewbob_scale_x;
extern ConVar amod_viewbob_scale_y;
extern ConVar amod_viewbob_scale_z;

extern ConVar amod_jump_punch_enable;
extern ConVar amod_land_punch_enable;

extern ConVar amod_enable_god;

extern ConVar amod_land_zvel_min;
extern ConVar amod_jump_vel_min;

#ifndef CLIENT_DLL

extern ConVar amod_nomap_restart;

#else

extern ConVar amod_view_lense_dirt;
extern ConVar amod_view_binoculars;
extern ConVar amod_view_bodycam;
extern ConVar amod_view_blur;

extern ConVar amod_view_override_xyz_amt;
extern ConVar amod_view_override_pyr_amt;

extern ConVar amod_view_square;
extern ConVar amod_view_square_width;
extern ConVar amod_view_square_height;
extern ConVar amod_view_claustrophobia;
extern ConVar amod_view_claustrophobia_amt;

extern ConVar amod_view_filter_video1;
extern ConVar amod_view_filter_video2;
extern ConVar amod_view_filter_video3;
extern ConVar amod_view_filter_video4;
extern ConVar amod_view_filter_video5;
extern ConVar amod_view_filter_video6;
extern ConVar amod_view_filter_video7;
extern ConVar amod_view_filter_video8;

extern ConVar amod_filter_on_brightness;
extern ConVar amod_filter_on_exp;
extern ConVar amod_filter_off_brightness;

extern ConVar amod_mirrored;
extern ConVar amod_camera_cinematic;
extern ConVar amod_camera_cinematic_lag_origin;
extern ConVar amod_camera_cinematic_lag_angles;
extern ConVar amod_camera_cinematic_lag_origin_amt;
extern ConVar amod_camera_cinematic_lag_angles_amt;
extern ConVar amod_camera_cinematic_fix;

extern ConVar amod_flashlightlag;
extern ConVar amod_flashlightlag_amt;

extern ConVar amod_flashlightflicker;
extern ConVar amod_flashlightflicker_wait_time_min;
extern ConVar amod_flashlightflicker_wait_time_max;
extern ConVar amod_flashlightflicker_duration_min;
extern ConVar amod_flashlightflicker_duration_max;
extern ConVar amod_flashlightflicker_brightness_min;
extern ConVar amod_flashlightflicker_brightness_max;
extern ConVar amod_flashlightflicker_time_interval_min;
extern ConVar amod_flashlightflicker_time_interval_max;

extern ConVar amod_fog_disabled;

extern ConVar amod_vignette;

#endif