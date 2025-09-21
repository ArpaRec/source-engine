//HOLY FUCK IM FINALLY DONE (i fucking hope so) I HAVBE BEEN DOING THIS FOR THE LAST 8 HOURS AND NOW IM FINALLY DONE
//IT HAS TAKEN OVER 3 DAYS YIPPE
#include "cbase.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <filesystem.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/label.h>
#include <vgui_controls/Slider.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Divider.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/ISurface.h>
#include "fmtstr.h"
#include "IoptionsPanel.h"
#include "AloneMod/AmodCvars.h"
#include "movevars_shared.h"
#include "AloneMod/Amod_SharedDefs.h"

using namespace vgui;

// this is probably the shittest code i have ever written but it works so im not gonna change it

//also idk why i did m_ cause it aint a member in any class
static bool m_bIsCPanelOpen = false;
static bool m_bIsSPanelOpen = false;
static bool m_bIsRPanelOpen = false;
static bool bDisableSoundscapeDisable = false;

void AmodDayChangeCallback(IConVar* var, const char*, float);
void AmodDayRavChangeCallback(IConVar* var, const char*, float);
void AmodNewEndingCallback(IConVar* var, const char*, float);

#if !AMOD_DAYTIME_EDITION
ConVar amod_day("amod_day", "0", 0, "", AmodDayChangeCallback);
ConVar amod_day_ravenholm("amod_day_ravenholm", "0", 0, "", AmodDayRavChangeCallback);
#endif

ConVar amod_new_ending("amod_new_ending", "0", 0, "", AmodNewEndingCallback);

bool bUseNewEnding = false;

void AmodNewEndingCallback(IConVar* var, const char*, float)
{
	bUseNewEnding = amod_new_ending.GetBool();
}

class CAutoAmodDaySystem : public CAutoGameSystemPerFrame
{
public:
	void LevelInitPreEntity()
	{
		szMapName = MapName();

		//there was code here but i moved it from client to server because...
		//well, i dont know why but it works on the server and i tried on client but no work :(
	}
};
CAutoAmodDaySystem g_AutoAmodDaySys;

class CAModCreditsPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CAModCreditsPanel, vgui::Frame);

	CAModCreditsPanel(Panel* parent) : vgui::Frame(parent, "CAmodCreditsPanel")
	{
		SetParent(parent);

		SetKeyBoardInputEnabled(true);
		SetMouseInputEnabled(true);

		SetProportional(false);
		SetTitleBarVisible(true);
		SetMinimizeButtonVisible(false);
		SetMaximizeButtonVisible(false);
		SetCloseButtonVisible(true);
		SetSizeable(false);
		SetMoveable(true);
		SetVisible(true);

		SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

		SetBounds(100, 100, 245, 360);
		SetTitle("Credits", false);

		RichText* rt = new RichText(this, "rt");
		rt->SetBounds(10, 25, 225, 330);
		rt->SetVerticalScrollbar(false);
		rt->SetText("Creator: Waddelz\n\nGame: Valve\n\nSoundtrack:\n portal ->\n Self Esteem Fund, Android Hell\n Stop What You Are Doing,\n HL2 Episode 2 ->\n Dark Inverval\n\nHuge Thanks To dominicb\nFor The Song: Gone\nAnd TheAR3Guy For The Song\nEnd\n\nMirrored Source Code: NvC-DmN-CH\n\nModdb: For Containing The Mod\n\n");
	}

	void OnClose()
	{
		if (this)
		{
			m_bIsCPanelOpen = false;
			*m_this = nullptr;
			delete this;
		}
	}

	CAModCreditsPanel** m_this;
};

class CAmodCPanelInterface
{
public:
	void Open(Panel* pan)
	{
		if (!cpanel)
			cpanel = new CAModCreditsPanel(pan);

		cpanel->Activate();
		cpanel->m_this = &cpanel;
		m_bIsCPanelOpen = true;
	}

	void Close()
	{
		if (cpanel)
			delete cpanel;

		cpanel = nullptr;
		m_bIsCPanelOpen = false;
	}
private:
	CAModCreditsPanel* cpanel;
};
static CAmodCPanelInterface g_amodCPanInterface;

class CAModWeatherPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CAModWeatherPanel, vgui::Frame);

	CAModWeatherPanel(Panel* parent) : vgui::Frame(parent, "CAModWeatherPanel")
	{
		SetParent(parent);

		SetKeyBoardInputEnabled(true);
		SetMouseInputEnabled(true);

		SetProportional(false);
		SetTitleBarVisible(true);
		SetMinimizeButtonVisible(false);
		SetMaximizeButtonVisible(false);
		SetCloseButtonVisible(false);
		SetSizeable(false);
		SetMoveable(true);
		SetVisible(true);

		SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

		SetBounds(100, 100, 180, 190);
		SetTitle("Weather Panel", false);

		ConVarRef amod_rain_type("amod_rain_type");
		ConVarRef amod_rain_enable("amod_rain_enable");
		ConVarRef amod_rain_thunder("amod_rain_thunder");

		cbEnable = new CheckButton(this, "cbEnable", "Enable Rain");
		cbEnable->SetSelected(amod_rain_enable.GetBool());
		cbEnable->SetBounds(2, 21, 150, 25);

		cbThunder = new CheckButton(this, "cbEnable", "Enable Thunder Sounds");
		cbThunder->SetSelected(amod_rain_thunder.GetBool());
		cbThunder->SetBounds(2, 42, 175, 25);
		cbThunder->SetEnabled(amod_rain_enable.GetBool());

		Label* lb = new Label(this, "", "Rain Type");
		lb->SetBounds(60, 64, 160, 20);

		cbType = new ComboBox(this, "", 2, false);
		cbType->SetBounds(7, 85, 167, 20);
		cbType->AddItem("Always Rain", nullptr);
		cbType->AddItem("Rain In Intervals", nullptr);
		cbType->ActivateItem(amod_rain_type.GetInt() ? amod_rain_type.GetInt() - 1 : 0);
		cbType->SetEnabled(amod_rain_enable.GetBool());

		bApply = new Button(this, "", "Apply Settings");
		bApply->SetBounds(5, 164, 167, 20);
		bApply->SetCommand("apply");

		vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
	}

	void OnTick()
	{
		BaseClass::OnTick();

		cbThunder->SetEnabled(cbEnable->IsSelected());
		cbType->SetEnabled(cbEnable->IsSelected());
	}

	void OnCommand(const char* command)
	{
		static ConVarRef amod_rain_enable("amod_rain_enable");
		static ConVarRef amod_rain_type("amod_rain_type");

		if (cbEnable->IsSelected() ^ amod_rain_enable.GetBool())
			engine->ClientCmd_Unrestricted(CFmtStr("amod_rain_enable %d", cbEnable->IsSelected()));

		engine->ClientCmd_Unrestricted(CFmtStr("amod_rain_thunder %d", cbThunder->IsSelected()));

		if (cbType->GetActiveItem() + 1 ^ amod_rain_type.GetInt())
			engine->ClientCmd_Unrestricted(CFmtStr("amod_rain_type %d", cbType->GetActiveItem() + 1));

		bDisableSoundscapeDisable = cbEnable->IsSelected();
	}

	CheckButton* cbEnable;
	CheckButton* cbThunder;
	ComboBox* cbType;
	Button* bApply;
	CAModWeatherPanel** m_this;
};

class CAmodWPanelInterface
{
public:
	void Open(Panel* pan)
	{
		if (!cpanel)
			cpanel = new CAModWeatherPanel(pan);

		cpanel->Activate();
		cpanel->m_this = &cpanel;
		m_bIsRPanelOpen = true;
	}

	void Close()
	{
		if (cpanel)
			delete cpanel;

		cpanel = nullptr;
		m_bIsRPanelOpen = false;
	}
private:
	CAModWeatherPanel* cpanel;
};
static CAmodWPanelInterface g_amodWPanInterface;

class CAModSkyboxPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CAModSkyboxPanel, vgui::Frame);

	CAModSkyboxPanel(Panel* parent) : vgui::Frame(parent, "CAModSkyboxPanel")
	{
		SetParent(parent);

		SetKeyBoardInputEnabled(true);
		SetMouseInputEnabled(true);

		SetProportional(false);
		SetTitleBarVisible(true);
		SetMinimizeButtonVisible(false);
		SetMaximizeButtonVisible(false);
		SetCloseButtonVisible(false);
		SetSizeable(false);
		SetMoveable(true);
		SetVisible(true);

		SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

		vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

		SetBounds(150, 150, 180, 230);
		SetTitle("Skybox Panel", false);

		kv = new KeyValues("kv");
		if (!kv->LoadFromFile(filesystem, "resource/panels/SkyPanel.txt"))
		{
			kv->deleteThis();
			ConWarning("Failed To Open resource/panels/SkyPanel.txt\n");
			return;
		}

		ConVarRef amod_sky("amod_sky");

		int index = 0;
		int j = 0;

		CUtlVector<const char*> ptr;
		FOR_EACH_VALUE(kv, sub)
		{
			ptr.AddToTail(sub->GetName());
			m_Skys.AddToTail(sub->GetString());
			if (!Q_strcmp(amod_sky.GetString(), sub->GetString()))
			{
				index = j;
			}
			j++;
		}

		m_skyComboBox = new ComboBox(this, "m_skyComboBox", ptr.Size(), false);
		for (int i = 0; i < ptr.Size(); i++)
		{
			m_skyComboBox->AddItem(ptr[i], nullptr);
		}

		m_skyComboBox->SetBounds(10, 180, 160, 20);
		m_skyComboBox->ActivateItem(index);

		m_FogDisable = new CheckButton(this, "", "Disable Fog");
		m_FogDisable->SetBounds(29, 159, 161, 20);
		m_FogDisable->SetSelected(amod_fog_disabled.GetBool());

		char buf[218];
		Q_snprintf(buf, sizeof(buf), "skybox/%s", amod_sky.GetString());

		m_skyImage = new ImagePanel(this, "m_SkyImage");
		m_skyImage->SetImage((m_Skys.Size() != 0) ? m_Skys[0] : buf);
		m_skyImage->SetBounds(10, 30, 160, 130);

		m_skyBut = new Button(this, "but", "Set Skybox");
		m_skyBut->SetBounds(10, 205, 160, 20);
		m_skyBut->SetCommand("cmd");

		m_bInit = true;
	}

	void OnTick()
	{
		if (!m_bInit)
			return;

		if (m_bPrevSelected != m_FogDisable->IsSelected())
		{
			m_bPrevSelected = m_FogDisable->IsSelected();
			amod_fog_disabled.SetValue(m_bPrevSelected);
			engine->ClientCmd_Unrestricted(CFmtStr("amod_fog_disabled %d", m_bPrevSelected));
		}
		m_skyImage->SetImage((m_Skys.Size() != 0) ? m_Skys[m_skyComboBox->GetActiveItem()] : "skybox/borealis01");
	}

	void OnCommand(const char* pcCommand)
	{
		if (!Q_strcmp(pcCommand, "cmd"))
		{
			ConVar* sname = cvar->FindVar("sv_skyname");
			if (sname)
			{
				if (!Q_strcmp(sname->GetString(), (m_Skys.Size() != 0) ? m_Skys[m_skyComboBox->GetActiveItem()] : "skybox/borealis01"))
					return;
			}
			engine->ExecuteClientCmd(CFmtStr("amod_sky %s", (m_Skys.Size() != 0) ? m_Skys[m_skyComboBox->GetActiveItem()] : "skybox/borealis01"));
		}
	}

	void OnClose()
	{
		m_bIsSPanelOpen = false;
		*m_this = nullptr;
		delete this;
	}

	~CAModSkyboxPanel()
	{
		if (kv)
			kv->deleteThis();
	}

	CUtlVector<const char*> m_Skys;
	ComboBox* m_skyComboBox;
	Button* m_skyBut;
	ImagePanel* m_skyImage;
	CheckButton* m_FogDisable;
	CAModSkyboxPanel** m_this;
	KeyValues* kv;

	bool m_bPrevSelected = false;
	bool m_bInit = false;
};

class CAmodSPanelInterface
{
public:
	void Open(Panel* pan)
	{
		if (!spanel)
			spanel = new CAModSkyboxPanel(pan);

		spanel->Activate();
		spanel->m_this = &spanel;
		m_bIsSPanelOpen = true;
	}

	void Close()
	{
		if (spanel)
			delete spanel;

		spanel = nullptr;
		m_bIsSPanelOpen = false;
	}
private:
	CAModSkyboxPanel* spanel;

};
static CAmodSPanelInterface g_amodSPanInterface;

extern ConVar r_flashlightfar;
extern ConVar r_flashlightfov;

char* ConvertEscapeChars(const char* input) {
	size_t input_length = strlen(input);
	char* result = new char[input_length + 1];
	size_t result_index = 0;

	for (size_t i = 0; i < input_length; ++i) {
		if (input[i] == '\\' && i + 1 < input_length) {
			switch (input[i + 1]) {
			case 'n':
				result[result_index++] = '\n';
				break;
			case 't':
				result[result_index++] = '\t';
				break;
			default:
				result[result_index++] = input[i];
				break;
			}
			++i;
		}
		else {
			result[result_index++] = input[i];
		}
	}
	result[result_index] = '\0';

	return result;
}

float GetFilterOnVal(int fv)
{
	switch (fv)
	{
	case 12:
		return 1.9f;
	case 11:
		return 1.95f;
	case 10:
		return 2.0f;
	case 9:
		return 2.05f;
	case 8:
		return 2.1f;
	case 7:
		return 2.15f;
	case 6:
		return 2.2f;
	case 5:
		return 2.25f;
	case 4:
		return 2.3f;
	case 3:
		return 2.35f;
	case 2:
		return 2.4f;
	case 1:
		return 2.45f;
	case 0:
		return 2.5f;
	}
	return 0.0f;
}

float GetFilterOnExpVal(int fv)
{
	switch (fv)
	{
	case 0:
		return 1.15f;
	case 1:
		return 1.2f;
	case 2:
		return 1.25f;
	case 3:
		return 1.3f;
	case 4:
		return 1.35f;
	case 5:
		return 1.4f;
	case 6:
		return 1.45f;
	case 7:
		return 1.5f;
	case 8:
		return 1.55f;
	case 9:
		return 1.6f;
	case 10:
		return 1.65f;
	case 11:
		return 1.7f;
	case 12:
		return 1.75f;
	}
	return 0.0f;
}

float GetFilterOffVal(int fv)
{
	switch (fv)
	{
	case 10:
		return 1.8f;
	case 9:
		return 1.85f;
	case 8:
		return 1.9f;
	case 7:
		return 1.95f;
	case 6:
		return 2.0f;
	case 5:
		return 2.05f;
	case 4:
		return 2.1f;
	case 3:
		return 2.15f;
	case 2:
		return 2.2f;
	case 1:
		return 2.25f;
	case 0:
		return 2.3f;
	}
	return 0.0f;
}

class COptionsPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(COptionsPanel, vgui::Frame);

	COptionsPanel(vgui::VPANEL parent);
	~COptionsPanel()
	{
		if (m_bIsCPanelOpen)
			g_amodCPanInterface.Close();
		if (m_bIsSPanelOpen)
			g_amodSPanInterface.Close();
		if (m_bIsRPanelOpen)
			g_amodWPanInterface.Close();

		if (kvMain)
			kvMain->deleteThis();
	}

	void Init();

	void OnClose();
protected:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

	void InitConVars();
private:
	KeyValues* kvMain;
	Button* m_ApplyButton;
	Button* m_CreditButton;
	Button* m_EffectsPanel;

#if !AMOD_DAYTIME_EDITION
	Button* m_SBoxButton;
#endif

	CheckButton* cbNoSoundscapes;
	CheckButton* cbNoMusic;
	CheckButton* m_cbDayAfterNP;
	CheckButton* m_cbDayAfterRav;
	CheckButton* m_cbStandBob;
	Label* m_LabelView;
	CheckButton* ViewBobbingCB;
	CheckButton* MirroredCB;
	CheckButton* RollAngleCB;
	Label* m_labelVRAngle;
	Slider* RollAngle;
	CheckButton* JumpViewpunch;
	CheckButton* LandViewpunch;
	CheckButton* m_cbDFootstepsSnd;
	CheckButton* m_cbDHud;
	CheckButton* m_CBVignette;
	CheckButton* EpicFilterCB;
	Label* m_labelGameplay;
	Label* m_labelFFar;
	ComboBox* FlashlightFar;
	Label* m_labelFFov;
	CheckButton* m_cbFLag;
	CheckButton* m_cbFlicker;
	ComboBox* FlashlightFov;
	Divider* SettingsDivider1;
	Divider* SettingsDivider2;
	Divider* SettingsDivider4;
	Divider* SettingsDivider5;
	Divider* SettingsDivider6;
	Label* m_labelCheat;
	CheckButton* EnableGod;
	Label* FilterSettingsLabel;
	Label* FilterOnBLabel;
	Slider* FilterOnBrightness;
	Label* FilterOnExpLabel;
	Slider* FilterOnExps;
	Label* FilterOffBLabel;
	Slider* FilterOffBrightness;
	Button* m_WBoxButton;
	CheckButton* CoreTimerCB;
	CheckButton* CitadelTimerCB;
	ComboBox* EndingComboBox;

	CUtlVector<const char*> ffarcbnames, ffarcbndata, ffovcbnames, ffovcbndata;

	int FilterOn, FilterOnExp, FilterOff;
	bool m_bInit;
};

COptionsPanel::COptionsPanel(vgui::VPANEL parent)
	: BaseClass(NULL, "OptionsPanel")
{
	m_bInit = false;

	SetParent(parent);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetProportional(false);
	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(true);
	SetSizeable(false);
	SetMoveable(true);
	SetVisible(true);

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	kvMain = new KeyValues("OptionsPanel");
	if (!kvMain->LoadFromFile(filesystem, "resource/panels/OptionsPanel.txt", "MOD"))
	{
		ConWarning("Error Opening OptionsPanel.txt Aborting Panel Load\n");
		return;
	}

	InitConVars();

#if !AMOD_DAYTIME_EDITION
	float fonval = GetFilterOnVal(FilterOn);
	float foffval = GetFilterOffVal(FilterOff);
	float fexponval = GetFilterOnExpVal(FilterOnExp);

	engine->ClientCmd_Unrestricted(CFmtStr("mat_monitorgamma_tv_exp %f", fexponval));
	engine->ClientCmd_Unrestricted(CFmtStr("mat_monitorgamma %f", fonval));

	engine->ClientCmd_Unrestricted(CFmtStr("alias tf1 \"mat_monitorgamma %f; mat_monitorgamma_tv_enabled 1; Amod_FilterSet 1; alias Amod_ToggleFilter tf2\"", fonval));
	engine->ClientCmd_Unrestricted(CFmtStr("alias tf2 \"mat_monitorgamma %f; mat_monitorgamma_tv_enabled 0; Amod_FilterSet 0; alias Amod_ToggleFilter tf1\"", foffval));
#endif

	Init();
}

void COptionsPanel::Init()
{
	KeyValues* AButtonKv = kvMain->FindKey("ApplyButton");
	if (!AButtonKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"ApplyButton\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* CButtonKv = kvMain->FindKey("CreditsButton");
	if (!CButtonKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"CreditsButton\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}
	
	KeyValues* EButtonKv = kvMain->FindKey("EffectsButton");
	if (!EButtonKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"EffectsButton\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

#if !AMOD_DAYTIME_EDITION
	KeyValues* SBxButtonKv = kvMain->FindKey("SkyboxButton");
	if (!SBxButtonKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"SkyboxButton\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}
#endif

	KeyValues* WButtonKv = kvMain->FindKey("WeatherButton");
	if (!WButtonKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"WeatherButton\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* NoSoundscapesCB = kvMain->FindKey("NoSoundscapesCB");
	if (!NoSoundscapesCB)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"NoSoundscapesCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* CoreTimerKV = kvMain->FindKey("CoreTimerCB");
	if (!CoreTimerKV)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"CoreTimerCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* CitadelTimerKV = kvMain->FindKey("CitadelTimerCB");
	if (!CitadelTimerKV)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"CitadelTimerCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* KvVignette = kvMain->FindKey("VignetteCB");
	if (!KvVignette)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"VignetteCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}
	
	KeyValues* EpicFilterCBKV = kvMain->FindKey("EpicFilterCB");
	if (!EpicFilterCBKV)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"EpicFilterCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* NoMusicCB = kvMain->FindKey("NoMusicCB");
	if (!NoMusicCB)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"NoMusicCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* DayAfterNPCBKv = kvMain->FindKey("DayAfterNPCBox");
	if (!DayAfterNPCBKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"DayAfterNPCBox\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* DayAfterRavCBKv = kvMain->FindKey("DayAfterRavCB");
	if (!DayAfterRavCBKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"DayAfterRavCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* StandBobCB = kvMain->FindKey("StandBobCB");
	if (!StandBobCB)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"StandBobCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* VSettingsKv = kvMain->FindKey("ViewSettingsLabel");
	if (!VSettingsKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"ViewSettingsLabel\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* KVDFootstebSnds = kvMain->FindKey("CBDFootstebSnds");
	if (!KVDFootstebSnds)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"CBDFootstebSnds\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* KVDisableHud = kvMain->FindKey("CBDisableHud");
	if (!KVDisableHud)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"CBDisableHud\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* VBKv = kvMain->FindKey("ViewBobbingCB");
	if (!VBKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"ViewBobbingCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* RAKV = kvMain->FindKey("RollAngleCB");
	if (!RAKV)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"RollAngleCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* JumpVPKv = kvMain->FindKey("JumpViewpunchCB");
	if (!JumpVPKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"JumpViewpunchCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* LandVPKv = kvMain->FindKey("LandViewpunchCB");
	if (!LandVPKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"LandViewpunchCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* MirroredKv = kvMain->FindKey("MirroredCB");
	if (!MirroredKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"MirroredCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* ViewRollLabel = kvMain->FindKey("RollAngleLabel");
	if (!ViewRollLabel)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"RollAngleLabel\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* RASliderKV = kvMain->FindKey("RollAngleSlider");
	if (!RASliderKV)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"RollAngleSlider\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* GSettingsKV = kvMain->FindKey("GameplaySettingsLabel");
	if (!GSettingsKV)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"GameplaySettingsLabel\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FFarLabelKv = kvMain->FindKey("FlashlightFarLabel");
	if (!FFarLabelKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FlashlightFarLabel\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FFovLabelKv = kvMain->FindKey("FlashlightFovLabel");
	if (!FFovLabelKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FlashlightFovLabel\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FFarCmbBoxKv = kvMain->FindKey("FlashlightFarCombobox");
	if (!FFarCmbBoxKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FlashlightFarCombobox\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FFarCmbBoxData = FFarCmbBoxKv->FindKey("Data");
	if (!FFarCmbBoxData)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"Data\" In Subkey \"FlashlightFarCombobox\" File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	if (!FFarCmbBoxData->GetFirstValue())
	{
		kvMain->deleteThis();
		ConWarning("Got Empty Data In Subkey: \"Data\" For \"FlashlightFarCombobox\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FFovCmbBoxKv = kvMain->FindKey("FlashlightFovCombobox");
	if (!FFovCmbBoxKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FlashlightFovCombobox\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FFovCmbBoxData = FFovCmbBoxKv->FindKey("Data");
	if (!FFovCmbBoxData)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"Data\" In Subkey \"FlashlightFovCombobox\" File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	if (!FFovCmbBoxData->GetFirstValue())
	{
		kvMain->deleteThis();
		ConWarning("Got Empty Data In Subkey: \"Data\" For \"FlashlightFovCombobox\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FLagCBoxKv = kvMain->FindKey("FLagCBox");
	if (!FLagCBoxKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FLagCBox\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FFlickerKv = kvMain->FindKey("FFlickerCbox");
	if (!FFlickerKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FFlickerCbox\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* SDvdrKv1 = kvMain->FindKey("SettingsDivider1");
	if (!SDvdrKv1)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"SettingsDivider1\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* SDvdrKv2 = kvMain->FindKey("SettingsDivider2");
	if (!SDvdrKv2)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"SettingsDivider2\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* SDvdrKv4 = kvMain->FindKey("SettingsDivider4");
	if (!SDvdrKv4)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"SettingsDivider4\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* SDvdrKv5 = kvMain->FindKey("SettingsDivider5");
	if (!SDvdrKv5)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"SettingsDivider5\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}
	
	KeyValues* SDvdrKv6 = kvMain->FindKey("SettingsDivider6");
	if (!SDvdrKv6)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"SettingsDivider6\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* GModeCB = kvMain->FindKey("GodModeCB");
	if (!GModeCB)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"GodModeCB\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* CheatSettingsKV = kvMain->FindKey("CheatSettings");
	if (!CheatSettingsKV)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"CheatSettings\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FSettingsLabelKv = kvMain->FindKey("FilterSettingsLabel");
	if (!FSettingsLabelKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FilterSettingsLabel\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FilterBOnLabelKv = kvMain->FindKey("FilterBOnLabel");
	if (!FilterBOnLabelKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FilterBOnLabel\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FilterBOnkv = kvMain->FindKey("FilterOnBSlider");
	if (!FilterBOnkv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FilterOnBSlider\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FilterBOnExpLabelKv = kvMain->FindKey("FilterBOnExpLabel");
	if (!FilterBOnExpLabelKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FilterBOnExpLabel\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FilterBOnExpkv = kvMain->FindKey("FilterOnExpBSlider");
	if (!FilterBOnExpkv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FilterOnExpBSlider\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FilterOffLabelKv = kvMain->FindKey("FilterBOffLabel");
	if (!FilterBOnLabelKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FilterBOffLabel\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* FilterBoffnkv = kvMain->FindKey("FilterOffBSlider");
	if (!FilterBoffnkv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"FilterOffBSlider\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* EndingCB = kvMain->FindKey("EndingComboBox");
	if (!EndingCB)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"EndingComboBox\" In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	FOR_EACH_VALUE(FFarCmbBoxData, CBoxDat)
	{
		if (ffarcbnames.Find(CBoxDat->GetName()) != -1)
		{
			kvMain->deleteThis();
			ConWarning("Got Multiple Of The Same Subkey In Subkey: \"Data\" For FlashlightFarCombobox In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
			return;
		}

		ffarcbnames.AddToTail(CBoxDat->GetName());
		ffarcbndata.AddToTail(CBoxDat->GetString());
	}

	FOR_EACH_VALUE(FFovCmbBoxData, CBoxDat)
	{
		if (ffovcbnames.Find(CBoxDat->GetName()) != -1)
		{
			kvMain->deleteThis();
			ConWarning("Got Multiple Of The Same Subkey In Subkey: \"Data\" For FlashlightFarCombobox In File: \"OptionsPanel.txt\" Aborting Panel Load!\n");
			return;
		}

		ffovcbnames.AddToTail(CBoxDat->GetName());
		ffovcbndata.AddToTail(CBoxDat->GetString());
	}

	SetTitle(kvMain->GetString("Title", "Title"), false);

	int wide = kvMain->GetInt("Wide"), tall = kvMain->GetInt("Tall");
	int screenWidth, screenHeight;

	vgui::surface()->GetScreenSize(screenWidth, screenHeight);

	const char* x = kvMain->GetString("XPos"), * y = kvMain->GetString("YPos");

	int xPos = atoi(x);
	int yPos = atoi(y);

	if (!Q_strcmp(x, "center"))
	{
		xPos = (screenWidth - wide) / 2;
	}

	if (!Q_strcmp(y, "center"))
	{
		yPos = (screenHeight - tall) / 2;
	}

	SetSize(wide, tall);
	SetPos(xPos, yPos);

	ConVarRef sv_footstep("sv_footsteps");
	ConVarRef hidehud("hidehud");
	ConVarRef amod_epic_filter("amod_epic_filter");

	m_cbDFootstepsSnd = new CheckButton(this, "m_cbDFootstepsSnd", KVDFootstebSnds->GetString("Text"));
	m_cbDFootstepsSnd->SetBounds(KVDFootstebSnds->GetInt("XPos"), KVDFootstebSnds->GetInt("YPos"), KVDFootstebSnds->GetInt("Wide"), KVDFootstebSnds->GetInt("Tall"));
	m_cbDFootstepsSnd->SetSelected(!sv_footstep.GetInt());

	m_cbDHud = new CheckButton(this, "m_cbDHud", KVDisableHud->GetString("Text"));
	m_cbDHud->SetBounds(KVDisableHud->GetInt("XPos"), KVDisableHud->GetInt("YPos"), KVDisableHud->GetInt("Wide"), KVDisableHud->GetInt("Tall"));
	m_cbDHud->SetSelected(hidehud.GetInt() == 8 ? true : false);

	m_LabelView = new Label(this, "m_LabelView", VSettingsKv->GetString("Text"));
	m_LabelView->SetBounds(VSettingsKv->GetInt("XPos"), VSettingsKv->GetInt("YPos"), VSettingsKv->GetInt("Wide"), VSettingsKv->GetInt("Tall"));

	ViewBobbingCB = new CheckButton(this, "ViewBobbingCB", VBKv->GetString("Text", ""));
	ViewBobbingCB->SetBounds(VBKv->GetInt("XPos"), VBKv->GetInt("YPos"), VBKv->GetInt("Wide"), VBKv->GetInt("Tall"));
	ViewBobbingCB->SetSelected(amod_viewbob_enabled.GetInt());

	m_CBVignette = new CheckButton(this, "m_CBVignette", KvVignette->GetString("Text", ""));
	m_CBVignette->SetBounds(KvVignette->GetInt("XPos"), KvVignette->GetInt("YPos"), KvVignette->GetInt("Wide"), KvVignette->GetInt("Tall"));
	m_CBVignette->SetSelected(amod_vignette.GetInt());
	
	EpicFilterCB = new CheckButton(this, "EpicFilterCB", EpicFilterCBKV->GetString("Text", ""));
	EpicFilterCB->SetBounds(EpicFilterCBKV->GetInt("XPos"), EpicFilterCBKV->GetInt("YPos"), EpicFilterCBKV->GetInt("Wide"), EpicFilterCBKV->GetInt("Tall"));
	EpicFilterCB->SetSelected(amod_epic_filter.GetInt());

	RollAngleCB = new CheckButton(this, "RollAngleCB", RAKV->GetString("Text", ""));
	RollAngleCB->SetBounds(RAKV->GetInt("XPos"), RAKV->GetInt("YPos"), RAKV->GetInt("Wide"), RAKV->GetInt("Tall"));
	RollAngleCB->SetSelected(sv_rollangle.GetInt());

	JumpViewpunch = new CheckButton(this, "JumpViewpunch", JumpVPKv->GetString("Text", ""));
	JumpViewpunch->SetBounds(JumpVPKv->GetInt("XPos"), JumpVPKv->GetInt("YPos"), JumpVPKv->GetInt("Wide"), JumpVPKv->GetInt("Tall"));
	JumpViewpunch->SetSelected(amod_jump_punch_enable.GetInt());

	LandViewpunch = new CheckButton(this, "LandViewpunch", LandVPKv->GetString("Text", ""));
	LandViewpunch->SetBounds(LandVPKv->GetInt("XPos"), LandVPKv->GetInt("YPos"), LandVPKv->GetInt("Wide"), LandVPKv->GetInt("Tall"));
	LandViewpunch->SetSelected(amod_land_punch_enable.GetInt());

	MirroredCB = new CheckButton(this, "MirroredCB", MirroredKv->GetString("Text", ""));
	MirroredCB->SetBounds(MirroredKv->GetInt("XPos"), MirroredKv->GetInt("YPos"), MirroredKv->GetInt("Wide"), MirroredKv->GetInt("Tall"));
	MirroredCB->SetSelected(amod_mirrored.GetBool());

	m_labelVRAngle = new Label(this, "m_labelVRAngle", ConvertEscapeChars(ViewRollLabel->GetString("Text")));
	m_labelVRAngle->SetBounds(ViewRollLabel->GetInt("XPos"), ViewRollLabel->GetInt("YPos"), ViewRollLabel->GetInt("Wide"), ViewRollLabel->GetInt("Tall"));

	RollAngle = new Slider(this, "RollAngle");
	RollAngle->SetBounds(RASliderKV->GetInt("XPos"), RASliderKV->GetInt("YPos"), RASliderKV->GetInt("Wide"), RASliderKV->GetInt("Tall"));
	RollAngle->SetRange(RASliderKV->GetInt("Min"), RASliderKV->GetInt("Max"));
	RollAngle->SetValue(sv_rollangle.GetInt());

	m_labelGameplay = new Label(this, "m_LabelView", GSettingsKV->GetString("Text"));
	m_labelGameplay->SetBounds(GSettingsKV->GetInt("XPos"), GSettingsKV->GetInt("YPos"), GSettingsKV->GetInt("Wide"), GSettingsKV->GetInt("Tall"));

	m_labelFFar = new Label(this, "m_labelFFar", ConvertEscapeChars(FFarLabelKv->GetString("text")));
	m_labelFFar->SetBounds(FFarLabelKv->GetInt("XPos"), FFarLabelKv->GetInt("YPos"), FFarLabelKv->GetInt("Wide"), FFarLabelKv->GetInt("Tall"));

	m_labelFFov = new Label(this, "m_labelFFov", ConvertEscapeChars(FFovLabelKv->GetString("text")));
	m_labelFFov->SetBounds(FFovLabelKv->GetInt("XPos"), FFovLabelKv->GetInt("YPos"), FFovLabelKv->GetInt("Wide"), FFovLabelKv->GetInt("Tall"));

	FlashlightFar = new ComboBox(this, "FlashlightFar", ffarcbnames.Size(), false);
	for (int i = 0; i < ffarcbnames.Size(); i++)
	{
		FlashlightFar->AddItem(ffarcbnames[i], nullptr);
	}

	FlashlightFar->SetBounds(FFarCmbBoxKv->GetInt("XPos"), FFarCmbBoxKv->GetInt("YPos"), FFarCmbBoxKv->GetInt("Wide"), FFarCmbBoxKv->GetInt("Tall"));
	FlashlightFar->ActivateItem(0);

	for (int j = 0; j < ffarcbndata.Size(); j++)
	{
		if (r_flashlightfar.GetInt() == atoi(ffarcbndata[j]))
		{
			FlashlightFar->ActivateItem(j);
			break;
		}
	}

	FlashlightFov = new ComboBox(this, "FlashlightFar", ffovcbnames.Size(), false);
	for (int i = 0; i < ffovcbnames.Size(); i++)
	{
		FlashlightFov->AddItem(ffovcbnames[i], nullptr);
	}

	FlashlightFov->SetBounds(FFovCmbBoxKv->GetInt("XPos"), FFovCmbBoxKv->GetInt("YPos"), FFovCmbBoxKv->GetInt("Wide"), FFovCmbBoxKv->GetInt("Tall"));
	FlashlightFov->ActivateItem(0);

	for (int j = 0; j < ffovcbndata.Size(); j++)
	{
		if (r_flashlightfov.GetInt() == atoi(ffovcbndata[j]))
		{
			FlashlightFov->ActivateItem(j);
			break;
		}
	}

	m_cbFLag = new CheckButton(this, "name", FLagCBoxKv->GetString("Text"));
	m_cbFLag->SetBounds(FLagCBoxKv->GetInt("XPos"), FLagCBoxKv->GetInt("YPos"), FLagCBoxKv->GetInt("Wide"), FLagCBoxKv->GetInt("Tall"));
	m_cbFLag->SetSelected(amod_flashlightlag.GetBool());

	m_cbFlicker = new CheckButton(this, "name", ConvertEscapeChars(FFlickerKv->GetString("Text")));
	m_cbFlicker->SetBounds(FFlickerKv->GetInt("XPos"), FFlickerKv->GetInt("YPos"), FFlickerKv->GetInt("Wide"), FFlickerKv->GetInt("Tall"));
	m_cbFlicker->SetSelected(amod_flashlightflicker.GetBool());

	SettingsDivider1 = new Divider(this, "");
	SettingsDivider1->SetBounds(SDvdrKv1->GetInt("XPos"), SDvdrKv1->GetInt("YPos"), SDvdrKv1->GetInt("Wide"), SDvdrKv1->GetInt("Tall"));

	SettingsDivider2 = new Divider(this, "");
	SettingsDivider2->SetBounds(SDvdrKv2->GetInt("XPos"), SDvdrKv2->GetInt("YPos"), SDvdrKv2->GetInt("Wide"), SDvdrKv2->GetInt("Tall"));

	SettingsDivider4 = new Divider(this, "");
	SettingsDivider4->SetBounds(SDvdrKv4->GetInt("XPos"), SDvdrKv4->GetInt("YPos"), SDvdrKv4->GetInt("Wide"), SDvdrKv4->GetInt("Tall"));

	SettingsDivider5 = new Divider(this, "");
	SettingsDivider5->SetBounds(SDvdrKv5->GetInt("XPos"), SDvdrKv5->GetInt("YPos"), SDvdrKv5->GetInt("Wide"), SDvdrKv5->GetInt("Tall"));
	
	SettingsDivider6 = new Divider(this, "");
	SettingsDivider6->SetBounds(SDvdrKv6->GetInt("XPos"), SDvdrKv6->GetInt("YPos"), SDvdrKv6->GetInt("Wide"), SDvdrKv6->GetInt("Tall"));

	m_labelCheat = new Label(this, "m_labelCheat", ConvertEscapeChars(CheatSettingsKV->GetString("text")));
	m_labelCheat->SetBounds(CheatSettingsKV->GetInt("XPos"), CheatSettingsKV->GetInt("YPos"), CheatSettingsKV->GetInt("Wide"), CheatSettingsKV->GetInt("Tall"));

	EnableGod = new CheckButton(this, "EnableGod", GModeCB->GetString("Text", ""));
	EnableGod->SetBounds(GModeCB->GetInt("XPos"), GModeCB->GetInt("YPos"), GModeCB->GetInt("Wide"), GModeCB->GetInt("Tall"));

	ConVarRef amo_god("amod_enable_god");

	EnableGod->SetSelected(amo_god.GetInt());

	FilterSettingsLabel = new Label(this, "FilterSettingsLabel", ConvertEscapeChars(FSettingsLabelKv->GetString("text")));
	FilterSettingsLabel->SetBounds(FSettingsLabelKv->GetInt("XPos"), FSettingsLabelKv->GetInt("YPos"), FSettingsLabelKv->GetInt("Wide"), FSettingsLabelKv->GetInt("Tall"));

	FilterOnBLabel = new Label(this, "FilterOnBLabel", ConvertEscapeChars(FilterBOnLabelKv->GetString("text")));
	FilterOnBLabel->SetBounds(FilterBOnLabelKv->GetInt("XPos"), FilterBOnLabelKv->GetInt("YPos"), FilterBOnLabelKv->GetInt("Wide"), FilterBOnLabelKv->GetInt("Tall"));

	FilterOnBrightness = new Slider(this, "FilterOnBrightness");
	FilterOnBrightness->SetBounds(FilterBOnkv->GetInt("XPos"), FilterBOnkv->GetInt("YPos"), FilterBOnkv->GetInt("Wide"), FilterBOnkv->GetInt("Tall"));
	FilterOnBrightness->SetRange(0, 10);
	FilterOnBrightness->SetValue(FilterOn);

	FilterOnExpLabel = new Label(this, "FilterOnBLabel", ConvertEscapeChars(FilterBOnExpLabelKv->GetString("text")));
	FilterOnExpLabel->SetBounds(FilterBOnExpLabelKv->GetInt("XPos"), FilterBOnExpLabelKv->GetInt("YPos"), FilterBOnExpLabelKv->GetInt("Wide"), FilterBOnExpLabelKv->GetInt("Tall"));

	FilterOnExps = new Slider(this, "FilterOnExps");
	FilterOnExps->SetBounds(FilterBOnExpkv->GetInt("XPos"), FilterBOnExpkv->GetInt("YPos"), FilterBOnExpkv->GetInt("Wide"), FilterBOnExpkv->GetInt("Tall"));
	FilterOnExps->SetRange(0, 10);
	FilterOnExps->SetValue(FilterOnExp);

	FilterOffBLabel = new Label(this, "FilterOffBLabel", ConvertEscapeChars(FilterOffLabelKv->GetString("text")));
	FilterOffBLabel->SetBounds(FilterOffLabelKv->GetInt("XPos"), FilterOffLabelKv->GetInt("YPos"), FilterOffLabelKv->GetInt("Wide"), FilterOffLabelKv->GetInt("Tall"));

	FilterOffBrightness = new Slider(this, "FilterOffBrightness");
	FilterOffBrightness->SetBounds(FilterBoffnkv->GetInt("XPos"), FilterBoffnkv->GetInt("YPos"), FilterBoffnkv->GetInt("Wide"), FilterBoffnkv->GetInt("Tall"));
	FilterOffBrightness->SetRange(0, 10);
	FilterOffBrightness->SetValue(FilterOff);

	ConVarRef amod_soundscapes_disable("amod_soundscapes_disable");
	ConVarRef amod_music_disable_tmp("amod_music_disable");
	ConVarRef amod_standbob_enabled("amod_standbob_enabled");

	cbNoSoundscapes = new CheckButton(this, "NoSoundscapes", NoSoundscapesCB->GetString("Text", ""));
	cbNoSoundscapes->SetBounds(NoSoundscapesCB->GetInt("XPos"), NoSoundscapesCB->GetInt("YPos"), NoSoundscapesCB->GetInt("Wide"), NoSoundscapesCB->GetInt("Tall"));
	cbNoSoundscapes->SetSelected(amod_soundscapes_disable.GetBool());

	cbNoMusic = new CheckButton(this, "NoSoundscapes", NoMusicCB->GetString("Text", ""));
	cbNoMusic->SetBounds(NoMusicCB->GetInt("XPos"), NoMusicCB->GetInt("YPos"), NoMusicCB->GetInt("Wide"), NoMusicCB->GetInt("Tall"));
	cbNoMusic->SetSelected(amod_music_disable_tmp.GetBool());

	m_cbDayAfterNP = new CheckButton(this, "DayAfterNP", DayAfterNPCBKv->GetString("Text", ""));
	m_cbDayAfterNP->SetBounds(DayAfterNPCBKv->GetInt("XPos"), DayAfterNPCBKv->GetInt("YPos"), DayAfterNPCBKv->GetInt("Wide"), DayAfterNPCBKv->GetInt("Tall"));

#if !AMOD_DAYTIME_EDITION
	m_cbDayAfterNP->SetSelected(amod_day.GetBool());
#endif

	m_cbDayAfterRav = new CheckButton(this, "m_cbDayAfterRav", DayAfterRavCBKv->GetString("Text", ""));
	m_cbDayAfterRav->SetBounds(DayAfterRavCBKv->GetInt("XPos"), DayAfterRavCBKv->GetInt("YPos"), DayAfterRavCBKv->GetInt("Wide"), DayAfterRavCBKv->GetInt("Tall"));

#if !AMOD_DAYTIME_EDITION
	m_cbDayAfterRav->SetSelected(amod_day_ravenholm.GetBool());
#endif

	m_cbStandBob = new CheckButton(this, "DayAfterNP", StandBobCB->GetString("Text", ""));
	m_cbStandBob->SetBounds(StandBobCB->GetInt("XPos"), StandBobCB->GetInt("YPos"), StandBobCB->GetInt("Wide"), StandBobCB->GetInt("Tall"));
	m_cbStandBob->SetSelected(amod_standbob_enabled.GetBool());

	m_CreditButton = new Button(this, "m_CreditButton", CButtonKv->GetString("Text", "Button"));
	m_CreditButton->SetBounds(CButtonKv->GetInt("XPos"), CButtonKv->GetInt("YPos"), CButtonKv->GetInt("Wide"), CButtonKv->GetInt("Tall"));
	m_CreditButton->SetCommand("Credits");
	
	m_EffectsPanel = new Button(this, "EButtonKv", EButtonKv->GetString("Text", "Button"));
	m_EffectsPanel->SetBounds(EButtonKv->GetInt("XPos"), EButtonKv->GetInt("YPos"), EButtonKv->GetInt("Wide"), EButtonKv->GetInt("Tall"));
	m_EffectsPanel->SetCommand("EffectsPanel");

#if !AMOD_DAYTIME_EDITION
	m_SBoxButton = new Button(this, "m_SBoxButton", SBxButtonKv->GetString("Text", "Button"));
	m_SBoxButton->SetBounds(SBxButtonKv->GetInt("XPos"), SBxButtonKv->GetInt("YPos"), SBxButtonKv->GetInt("Wide"), SBxButtonKv->GetInt("Tall"));
	m_SBoxButton->SetCommand("SBxPanel");
#endif

#if AMOD_DAYTIME_EDITION
	FilterOnBrightness->SetEnabled(false);
	FilterOnExps->SetEnabled(false);
	FilterOffBrightness->SetEnabled(false);
	m_cbDayAfterNP->SetEnabled(false);
	m_cbDayAfterRav->SetEnabled(false);
#endif

	m_WBoxButton = new Button(this, "m_SBoxButton", WButtonKv->GetString("Text", "Button"));
	m_WBoxButton->SetBounds(WButtonKv->GetInt("XPos"), WButtonKv->GetInt("YPos"), WButtonKv->GetInt("Wide"), WButtonKv->GetInt("Tall"));
	m_WBoxButton->SetCommand("WPanel");

	ConVarRef amod_do_core_timer("amod_do_core_timer");
	ConVarRef amod_do_citadel_timer("amod_do_citadel_timer");

	CoreTimerCB = new CheckButton(this, "", CoreTimerKV->GetString("Text"));
	CoreTimerCB->SetSelected(amod_do_core_timer.GetBool());
	CoreTimerCB->SetBounds(CoreTimerKV->GetInt("XPos"), CoreTimerKV->GetInt("YPos"), CoreTimerKV->GetInt("Wide"), CoreTimerKV->GetInt("Tall"));

	CitadelTimerCB = new CheckButton(this, "", CitadelTimerKV->GetString("Text"));
	CitadelTimerCB->SetSelected(amod_do_citadel_timer.GetBool());
	CitadelTimerCB->SetBounds(CitadelTimerKV->GetInt("XPos"), CitadelTimerKV->GetInt("YPos"), CitadelTimerKV->GetInt("Wide"), CitadelTimerKV->GetInt("Tall"));

	EndingComboBox = new ComboBox(this, "EndingComboBox", 2, false);
	EndingComboBox->AddItem("Mod Ending: Ending 1", nullptr);
	EndingComboBox->AddItem("Mod Ending: Ending 2", nullptr);
	EndingComboBox->ActivateItem(0);
	EndingComboBox->SetBounds(EndingCB->GetInt("XPos"), EndingCB->GetInt("YPos"), EndingCB->GetInt("Wide"), EndingCB->GetInt("Tall"));

	m_ApplyButton = new Button(this, "m_ApplyButton", AButtonKv->GetString("Text", "Button"));
	m_ApplyButton->SetBounds(AButtonKv->GetInt("XPos"), AButtonKv->GetInt("YPos"), AButtonKv->GetInt("Wide"), AButtonKv->GetInt("Tall"));
	m_ApplyButton->SetCommand("Apply");

	m_bInit = true;
}

class COptionsPanelInterface : public OptionsPanel
{
public:
	COptionsPanel* OPanel;

	COptionsPanelInterface()
	{
		OPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		OPanel = new COptionsPanel(parent);
	}
	void Destroy()
	{
		if (OPanel)
		{
			OPanel->SetParent((vgui::Panel*)NULL);
			delete OPanel;
		}
	}
	void Activate(void)
	{
		if (OPanel)
		{
			OPanel->Activate();
		}
	}
};
static COptionsPanelInterface g_OptionsPanel;
OptionsPanel* optionspanel = (OptionsPanel*)&g_OptionsPanel;

#include "ienginevgui.h"

CON_COMMAND(amod_opanel_reset, "For Making The Panel")
{
	if (g_OptionsPanel.OPanel)
	{
		VPANEL GameUiDll = enginevgui->GetPanel(PANEL_GAMEUIDLL);
		g_OptionsPanel.Destroy();
		g_OptionsPanel.Create(GameUiDll);
	}
}

ConVar cl_drawoptionspanel("cl_drawoptionspanel", "0", FCVAR_CLIENTDLL, "Sets the state of myPanel <state>");

void COptionsPanel::OnClose()
{
	cl_drawoptionspanel.SetValue(0);

	if (m_bIsCPanelOpen)
		g_amodCPanInterface.Close();

	if (m_bIsRPanelOpen)
		g_amodWPanInterface.Close();

	if (m_bIsSPanelOpen)
		g_amodSPanInterface.Close();
}

void COptionsPanel::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_drawoptionspanel.GetBool());

	if (m_bInit)
	{
		RollAngle->SetEnabled(RollAngleCB->IsSelected());

#if !AMOD_DAYTIME_EDITION
		if (szMapName)
			m_SBoxButton->SetEnabled(Q_strcmp(szMapName, "ep1"));
#endif

		cbNoSoundscapes->SetEnabled(!bDisableSoundscapeDisable);
	}
}

CON_COMMAND_F(ToggleOptionsPanel, "Toggles The Alone Mod Options Panel", FCVAR_HIDDEN)
{
	cl_drawoptionspanel.SetValue(!cl_drawoptionspanel.GetBool());
	optionspanel->Activate();
};

bool FOn = true;
float CurrFOffVal = 2.0f;
float CurrFOnVal = 2.1f;

CON_COMMAND_F(Amod_FilterSet, "", FCVAR_HIDDEN)
{
	FOn = atoi(args.Arg(1));
}

void COptionsPanel::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);
	static bool bWasSoundscapesChecled = cbNoSoundscapes->IsSelected();
	if (!Q_strcmp(pcCommand, "Apply"))
	{
		ConVar* sv_footstep = cvar->FindVar("sv_footsteps");
		if (sv_footstep)
		{
			sv_footstep->SetValue(!m_cbDFootstepsSnd->IsSelected());
		}
		engine->ClientCmd_Unrestricted(CFmtStr("sv_footsteps %d", !m_cbDFootstepsSnd->IsSelected()));

		ConVar* hidehud = cvar->FindVar("hidehud");
		if (hidehud)
		{
			hidehud->SetValue(m_cbDHud->IsSelected() ? 8 : 0);
		}
		engine->ClientCmd_Unrestricted(CFmtStr("hidehud %d", m_cbDHud->IsSelected() ? 8 : 0));

		amod_viewbob_enabled.SetValue(ViewBobbingCB->IsSelected());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_viewbob_enabled %d", ViewBobbingCB->IsSelected()));

		amod_jump_punch_enable.SetValue(JumpViewpunch->IsSelected());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_jump_punch_enable %d", JumpViewpunch->IsSelected()));

		amod_land_punch_enable.SetValue(LandViewpunch->IsSelected());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_land_punch_enable %d", LandViewpunch->IsSelected()));

		sv_rollangle.SetValue((RollAngleCB->IsSelected()) ? RollAngle->GetValue() : 0);
		engine->ClientCmd_Unrestricted(CFmtStr("sv_rollangle %d", (RollAngleCB->IsSelected()) ? RollAngle->GetValue() : 0));

		r_flashlightfar.SetValue(atoi(ffarcbndata[FlashlightFar->GetActiveItem()]));
		engine->ClientCmd_Unrestricted(CFmtStr("r_flashlightfar %s", ffarcbndata[FlashlightFar->GetActiveItem()]));

		r_flashlightfov.SetValue(atoi(ffovcbndata[FlashlightFov->GetActiveItem()]));
		engine->ClientCmd_Unrestricted(CFmtStr("r_flashlightfov %s", ffovcbndata[FlashlightFov->GetActiveItem()]));

		amod_enable_god.SetValue(EnableGod->IsSelected());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_enable_god %d", EnableGod->IsSelected()));

#if !AMOD_DAYTIME_EDITION
		amod_filter_on_brightness.SetValue(FilterOnBrightness->GetValue());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_filter_on_brightness %d", FilterOnBrightness->GetValue()));

		amod_filter_on_exp.SetValue(FilterOnExps->GetValue());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_filter_on_exp %d", FilterOnExps->GetValue()));

		amod_filter_off_brightness.SetValue(FilterOffBrightness->GetValue());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_filter_off_brightness %d", FilterOffBrightness->GetValue()));
#endif

		amod_flashlightlag.SetValue(m_cbFLag->IsSelected());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_flashlightlag %d", m_cbFLag->IsSelected()));

		amod_flashlightflicker.SetValue(m_cbFlicker->IsSelected());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_flashlightflicker %d", m_cbFlicker->IsSelected()));

		amod_mirrored.SetValue(MirroredCB->IsSelected());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_mirrored %d", MirroredCB->IsSelected()));

		amod_vignette.SetValue(m_CBVignette->IsSelected());
		engine->ClientCmd_Unrestricted(CFmtStr("amod_vignette %d", m_CBVignette->IsSelected()));		
		
		static ConVar* amod_epic_filter = cvar->FindVar("amod_epic_filter");
		if (amod_epic_filter)
			amod_vignette.SetValue(EpicFilterCB->IsSelected());

		engine->ClientCmd_Unrestricted(CFmtStr("amod_epic_filter %d", EpicFilterCB->IsSelected()));

#if !AMOD_DAYTIME_EDITION
		if (amod_day.GetBool() ^ m_cbDayAfterNP->IsSelected())
		{
			amod_day.SetValue(m_cbDayAfterNP->IsSelected());
			engine->ClientCmd_Unrestricted(CFmtStr("amod_day %d", m_cbDayAfterNP->IsSelected()));
		}
		
		if (amod_day_ravenholm.GetBool() ^ m_cbDayAfterRav->IsSelected())
		{
			amod_day_ravenholm.SetValue(m_cbDayAfterNP->IsSelected());
			engine->ClientCmd_Unrestricted(CFmtStr("amod_day_ravenholm %d", m_cbDayAfterRav->IsSelected()));
		}
#endif

		ConVar* amod_standbob_enabled = cvar->FindVar("amod_standbob_enabled");
		if (amod_standbob_enabled)
		{
			amod_standbob_enabled->SetValue(m_cbStandBob->IsSelected());
		}
		engine->ClientCmd_Unrestricted(CFmtStr("amod_standbob_enabled %d", m_cbStandBob->IsSelected()));

		ConVar* amod_soundscapes_disable = cvar->FindVar("amod_soundscapes_disable");
		if (amod_soundscapes_disable)
		{
			amod_soundscapes_disable->SetValue((!bDisableSoundscapeDisable && cbNoSoundscapes->IsSelected()));
		}		
		
		ConVar* amod_do_citadel_timer = cvar->FindVar("amod_do_citadel_timer");
		if (amod_do_citadel_timer)
		{
			amod_do_citadel_timer->SetValue(CitadelTimerCB->IsSelected());
		}
		
		ConVar* amod_do_core_timer = cvar->FindVar("amod_do_core_timer");
		if (amod_do_core_timer)
		{
			amod_do_core_timer->SetValue(CoreTimerCB->IsSelected());
		}

		if (bWasSoundscapesChecled ^ (cbNoSoundscapes->IsSelected() && !bDisableSoundscapeDisable))
			engine->ExecuteClientCmd("playsoundscape Nothing");

		bWasSoundscapesChecled = (cbNoSoundscapes->IsSelected() && !bDisableSoundscapeDisable);

		ConVar* amod_music_disable_tmp = cvar->FindVar("amod_music_disable");
		if (amod_music_disable_tmp)
			amod_music_disable_tmp->SetValue(cbNoMusic->IsSelected());

		engine->ClientCmd_Unrestricted(CFmtStr("amod_soundscapes_disable %d", cbNoSoundscapes->IsSelected()));
		engine->ClientCmd_Unrestricted(CFmtStr("amod_music_disable %d", cbNoMusic->IsSelected()));

#if !AMOD_DAYTIME_EDITION
		float fonval = CurrFOnVal = GetFilterOnVal(FilterOnBrightness->GetValue());
		float fexp = GetFilterOnExpVal(FilterOnExps->GetValue());
		float foff = CurrFOffVal = GetFilterOffVal(FilterOffBrightness->GetValue());

		engine->ClientCmd_Unrestricted(CFmtStr("mat_monitorgamma_tv_exp %f", fexp));

		engine->ClientCmd_Unrestricted(CFmtStr("alias tf1 \"mat_monitorgamma %f; mat_monitorgamma_tv_enabled 1; Amod_FilterSet 1; alias Amod_ToggleFilter tf2\"", fonval));
		engine->ClientCmd_Unrestricted(CFmtStr("alias tf2 \"mat_monitorgamma %f; mat_monitorgamma_tv_enabled 0; Amod_FilterSet 0; alias Amod_ToggleFilter tf1\"", foff));

		if (FOn && !(amod_day.GetBool() && IsCityMap(szMapName)))
		{
			engine->ClientCmd_Unrestricted("tf1");
		}
		else
		{
			engine->ClientCmd_Unrestricted("tf2");
		}
#endif

		amod_new_ending.SetValue(EndingComboBox->GetActiveItem());

		engine->ClientCmd_Unrestricted("Amod_WriteConfig");
	}
	else if (!Q_strcmp(pcCommand, "Credits"))
	{
		if (m_bIsCPanelOpen)
			g_amodCPanInterface.Close();
		else
			g_amodCPanInterface.Open(this);
	}
	else if (!Q_strcmp(pcCommand, "SBxPanel"))
	{
#if !AMOD_DAYTIME_EDITION
		if (m_bIsSPanelOpen)
			g_amodSPanInterface.Close();
		else
			g_amodSPanInterface.Open(this);
#endif
	}
	else if (!Q_strcmp(pcCommand, "WPanel"))
	{
		if (m_bIsRPanelOpen)
			g_amodWPanInterface.Close();
		else
			g_amodWPanInterface.Open(this);
	}
	else if (!Q_strcmp(pcCommand, "EffectsPanel"))
	{
		engine->ClientCmd("toggleeffectspanel");
	}
}

//i am verry sorry im using sstream its the only way i could think of to save and read
//the settings but it works - waddelz 4:49 16/05/2024 
#include <sstream>

void COptionsPanel::InitConVars()
{
	FileHandle_t File = filesystem->Open("cfg/AloneMod_Config.txt", "r", "MOD");
	if (!File)
	{
		engine->ClientCmd_Unrestricted("Amod_WriteConfig");
		return;
	}

	int size = filesystem->Size(File);
	char* buffer = new char[size + 1];

	int read = filesystem->Read(buffer, size, File);
	buffer[read] = '\0';

	std::istringstream sstream(buffer);
	std::string Var, Val;
	while (sstream >> Var >> Val)
	{
		if (Var == "amod_viewbob_enabled")
		{
			engine->ClientCmd_Unrestricted(CFmtStr("%s %s", Var.c_str(), Val.c_str()));
			amod_viewbob_enabled.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "sv_rollangle")
		{
			engine->ClientCmd_Unrestricted(CFmtStr("%s %s", Var.c_str(), Val.c_str()));
			sv_rollangle.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "amod_jump_punch_enable")
		{
			engine->ClientCmd_Unrestricted(CFmtStr("%s %s", Var.c_str(), Val.c_str()));
			amod_jump_punch_enable.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "amod_land_punch_enable")
		{
			engine->ClientCmd_Unrestricted(CFmtStr("%s %s", Var.c_str(), Val.c_str()));
			amod_land_punch_enable.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "r_flashlightfar")
		{
			engine->ClientCmd_Unrestricted(CFmtStr("%s %s", Var.c_str(), Val.c_str()));
			r_flashlightfar.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "r_flashlightfov")
		{
			engine->ClientCmd_Unrestricted(CFmtStr("%s %s", Var.c_str(), Val.c_str()));
			r_flashlightfov.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "filter_brightness_on")
		{
			FilterOn = atoi(Val.c_str());
		}
		else if (Var == "filter_brightness_on_exp")
		{
			FilterOnExp = atoi(Val.c_str());
		}
		else if (Var == "filter_brightness_off")
		{
			FilterOff = atoi(Val.c_str());
		}
		else if (Var == "amod_flashlightlag")
		{
			engine->ClientCmd_Unrestricted(CFmtStr("%s %s", Var.c_str(), Val.c_str()));
			amod_flashlightlag.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "amod_flashlightflicker")
		{
			engine->ClientCmd_Unrestricted(CFmtStr("%s %s", Var.c_str(), Val.c_str()));
			amod_flashlightflicker.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "amod_mirrored")
		{
			engine->ClientCmd_Unrestricted(CFmtStr("%s %s", Var.c_str(), Val.c_str()));
			amod_mirrored.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "amod_soundscapes_disable")
		{
			ConVar* amod_soundscapes_disable = cvar->FindVar("amod_soundscapes_disable");
			if (amod_soundscapes_disable)
				amod_soundscapes_disable->SetValue(atoi(Val.c_str()));

			engine->ClientCmd_Unrestricted(CFmtStr("amod_soundscapes_disable %s", Val.c_str()));
		}
		else if (Var == "amod_music_disable")
		{
			ConVar* amod_music_disable_tmp = cvar->FindVar("amod_music_disable");
			if (amod_music_disable_tmp)
				amod_music_disable_tmp->SetValue(atoi(Val.c_str()));

			engine->ClientCmd_Unrestricted(CFmtStr("amod_music_disable %s", Val.c_str()));
		}
		else if (Var == "sv_footsteps")
		{
			ConVar* sv_footstep = cvar->FindVar("sv_footsteps");
			if (sv_footstep)
				sv_footstep->SetValue(atoi(Val.c_str()));

			engine->ClientCmd_Unrestricted(CFmtStr("sv_footsteps %s", Val.c_str()));
		}
		else if (Var == "hidehud")
		{
			ConVar* hidehud = cvar->FindVar("hidehud");
			if (hidehud)
				hidehud->SetValue(atoi(Val.c_str()));

			engine->ClientCmd_Unrestricted(CFmtStr("cl_drawhud %s", Val.c_str()));
		}
		else if (Var == "amod_fog_disabled")
		{
			amod_fog_disabled.SetValue(atoi(Val.c_str()));
			engine->ClientCmd_Unrestricted(CFmtStr("amod_fog_disabled %s", Val.c_str()));
		}
		else if (Var == "amod_enable_god")
		{
			ConVar* amod_enabl_god = cvar->FindVar("amod_enable_god");
			if (amod_enabl_god)
				amod_enabl_god->SetValue(atoi(Val.c_str()));

			engine->ClientCmd_Unrestricted(CFmtStr("amod_enable_god %s", Val.c_str()));
		}
		else if (Var == "amod_standbob_enabled")
		{
			ConVar* amod_standbob_enabled = cvar->FindVar("amod_standbob_enabled");
			if (amod_standbob_enabled)
				amod_standbob_enabled->SetValue(atoi(Val.c_str()));

			engine->ClientCmd_Unrestricted(CFmtStr("amod_standbob_enabled %s", Val.c_str()));
		}

#if !AMOD_DAYTIME_EDITION
		else if (Var == "amod_day")
		{
			amod_day.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "amod_day_ravenholm")
		{
			amod_day_ravenholm.SetValue(atoi(Val.c_str()));
		}
#endif

		else if (Var == "amod_do_citadel_timer" || Var == "amod_do_core_timer")
		{
			ConVar* var = cvar->FindVar(Var.c_str());
			if (var)
				var->SetValue(atoi(Val.c_str()));
		}
		else if (Var == "amod_vignette")
		{
			amod_vignette.SetValue(atoi(Val.c_str()));
		}
		else if (Var == "amod_epic_filter")
		{
			ConVar* var = cvar->FindVar(Var.c_str());
			if (var)
			{
				//fixes "No Edict's yet" error
				auto t = var->m_fnChangeCallback;
				var->m_fnChangeCallback = nullptr;
				var->SetValue(atoi(Val.c_str()));
				var->m_fnChangeCallback = t;
			}

			//engine->ClientCmd_Unrestricted(CFmtStr("amod_epic_filter %s", Val.c_str()));
		}
		Val = "";
		Var = "";
	}

	filesystem->Close(File);
} 

CON_COMMAND_F(amod_outrotest, "", FCVAR_HIDDEN)
{
	if (bUseNewEnding)
		engine->ClientCmd_Unrestricted("map credits_d");
}