#include "cbase.h"
#include "fmtstr.h"
#include "filesystem.h"
#include "ISongPanel.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Slider.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/CheckButton.h>
#include <engine/IEngineSound.h>
#include <string>
#include <map>

using namespace vgui;

char* ConvertEscapeChars(const char* input);

int FindSubStr(const char* str, const char* subStr) {
	int strLen = strlen(str);
	int subStrLen = strlen(subStr);

	if (subStrLen == 0 || subStrLen > strLen) {
		return -1;
	}

	for (int i = 0; i <= strLen - subStrLen; ++i) {
		bool match = true;
		for (int j = 0; j < subStrLen; ++j) {
			if (str[i + j] != subStr[j]) {
				match = false;
				break;
			}
		}

		if (match) {
			if ((int)strlen(str) <= i + 8)
				return -1;

			return i;
		}
	}

	return -1;
}

std::string RemoveRandom(const char* str)
{
	int at = FindSubStr(str, "[random]");
	if (at != -1)
	{
		for (int i = 0; i < at; i++)
		{
			if (!isspace(str[i]))
				return str;
		}

		return std::string(str).erase(0, at + 8);
	}
	return str;
}


class CSongPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CSongPanel, vgui::Frame);

	CSongPanel(vgui::VPANEL parent);
	~CSongPanel() {};

	void OnLevelChange();
	void OnNewLevel();

	int m_iSongGuid;
	int m_iStartPitch;
	float m_fStartTime;
	float m_fStartTmp;
	const char* m_sCurrPlaying;
protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

	void OnKeyCodePressed(KeyCode code);
	void OnClose();

private:
	KeyValues* kvMain;
	ComboBox* CBoxSongs;

	Label* volLabel;
	Slider* sVolume;

	Label* PitchLabel;
	Slider* sPitch;
	
	CheckButton* cbTransThroughLevel;

	Button* bPlay, * bStop, * bResetPitch;

	CUtlVector<const char*> v_Songs, v_Names;
	CUtlVector<std::string> v_RandomSongs;
	std::map<std::string, CUtlVector<const char*>> v_DiscardSongsMap;

	bool bInit;

	void Init();
};

CON_COMMAND(amod_songpanel_changelevel, "I had to make this a hack cause you cant get the gpGlobals->eLoadType on the client")
{
	songpanel->OnLevelChange();
}

CON_COMMAND(amod_songpanel_newlevel, "I had to make this a hack cause you cant get the gpGlobals->eLoadType on the client")
{
	songpanel->OnNewLevel();
}

CSongPanel::CSongPanel(vgui::VPANEL parent)
	: BaseClass(NULL, "SongPanel")
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

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	kvMain = new KeyValues("SongPanel");
	if (!kvMain->LoadFromFile(filesystem, "resource/panels/SongPanel.txt", "MOD"))
	{
		ConWarning("Error Opening SongPanel.txt Aborting Panel Load\n");
		return;
	}

	m_iSongGuid = -1;
	m_iStartPitch = 0;
	m_fStartTime = 0;
	m_fStartTmp = 0;
	m_sCurrPlaying = nullptr;
	bInit = false;

	Init();
}

void CSongPanel::Init()
{
	KeyValues* SongListKv = kvMain->FindKey("SongList");
	if (!SongListKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"SongList\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* CSongsSub = SongListKv->FindKey("Songs");
	if (!CSongsSub)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"Songs\" For SongList In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	if (!CSongsSub->GetFirstSubKey())
	{
		kvMain->deleteThis();
		ConWarning("Got No keyvalue Pair In Subkey: \"Songs\" For Songs In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	FOR_EACH_VALUE(CSongsSub, CSongs)
	{
		if (v_Names.Find(CSongs->GetName()) != -1)
		{
			kvMain->deleteThis();
			ConWarning("Got Multiple Of The Same Key In Subkey: \"Songs\" For Songs In File: \"SongPanel.txt\" Aborting Panel Load!\n");
			return;
		}

		v_Names.AddToTail(CSongs->GetName());
		v_Songs.AddToTail(CSongs->GetString());
	}

	KeyValues* SPPlayBKv = kvMain->FindKey("PlayButton");
	if (!SPPlayBKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"PlayButton\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* SPStopBKv = kvMain->FindKey("StopButton");
	if (!SPStopBKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"StopButton\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* RPButtonKv = kvMain->FindKey("ResetPitch");
	if (!RPButtonKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"ResetPitch\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* SVolKv = kvMain->FindKey("SliderVol");
	if (!SVolKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"SliderVol\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* sVolTextKv = kvMain->FindKey("VolumeText");
	if (!sVolTextKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"VolumeText\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* SPitchKv = kvMain->FindKey("SliderPitch");
	if (!SPitchKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"SliderPitch\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* sPitchTextKv = kvMain->FindKey("PitchText");
	if (!sPitchTextKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"PitchText\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* TransCBKv = kvMain->FindKey("TransCB");
	if (!TransCBKv)
	{
		kvMain->deleteThis();
		ConWarning("Got NULL Subkey: \"TransCB\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	SetTitle(kvMain->GetString("Title", "Title"), false);
	SetBounds(kvMain->GetInt("XPos"), kvMain->GetInt("YPos"), kvMain->GetInt("Wide"), kvMain->GetInt("Tall"));

	CBoxSongs = new ComboBox(this, "MapCBox", v_Names.Size(), false);
	for (int i = 0; i < v_Names.Size(); i++)
	{
		if (FindSubStr(v_Names[i], "[random]") != -1)
		{
			std::string name = RemoveRandom(v_Names[i]).c_str();
			CBoxSongs->AddItem(name.c_str(), nullptr);
			v_RandomSongs.AddToTail(name);
			for (int j = 0; j < v_Names.Size(); j++)
			{
				if (FindSubStr(v_Songs[i], std::string(std::string("[discard]") + v_Names[j]).c_str()) != -1)
				{
					v_DiscardSongsMap[name.c_str()].AddToTail(v_Names[j]);
				}
			}
		}
		else
			CBoxSongs->AddItem(v_Names[i], nullptr);
	}

	CBoxSongs->SetBounds(SongListKv->GetInt("XPos"), SongListKv->GetInt("YPos"), SongListKv->GetInt("Wide"), SongListKv->GetInt("Tall"));
	CBoxSongs->ActivateItem(SongListKv->GetInt("ActiveItem"));

	bPlay = new Button(this, "PlayButton", SPPlayBKv->GetString("Text", "Button"));
	bPlay->SetBounds(SPPlayBKv->GetInt("XPos"), SPPlayBKv->GetInt("YPos"), SPPlayBKv->GetInt("Wide"), SPPlayBKv->GetInt("Tall"));
	bPlay->SetCommand("PlaySong");

	bStop = new Button(this, "StopButton", SPStopBKv->GetString("Text", "Button"));
	bStop->SetBounds(SPStopBKv->GetInt("XPos"), SPStopBKv->GetInt("YPos"), SPStopBKv->GetInt("Wide"), SPStopBKv->GetInt("Tall"));
	bStop->SetCommand("Stop");

	bResetPitch = new Button(this, "R", RPButtonKv->GetString("Text", "Button"));
	bResetPitch->SetBounds(RPButtonKv->GetInt("XPos"), RPButtonKv->GetInt("YPos"), RPButtonKv->GetInt("Wide"), RPButtonKv->GetInt("Tall"));
	bResetPitch->SetCommand("RPitch");

	sVolume = new Slider(this, "");
	sVolume->SetBounds(SVolKv->GetInt("XPos"), SVolKv->GetInt("YPos"), SVolKv->GetInt("Wide"), SVolKv->GetInt("Tall"));
	sVolume->SetRange(SVolKv->GetInt("Min"), SVolKv->GetInt("Max"));
	sVolume->SetValue(100);

	volLabel = new Label(this, "", sVolTextKv->GetString("Text"));
	volLabel->SetBounds(sVolTextKv->GetInt("XPos"), sVolTextKv->GetInt("YPos"), sVolTextKv->GetInt("Wide"), sVolTextKv->GetInt("Tall"));

	sPitch = new Slider(this, "");
	sPitch->SetBounds(SPitchKv->GetInt("XPos"), SPitchKv->GetInt("YPos"), SPitchKv->GetInt("Wide"), SPitchKv->GetInt("Tall"));
	sPitch->SetRange(SPitchKv->GetInt("Min"), SPitchKv->GetInt("Max"));
	sPitch->SetValue(100);

	PitchLabel = new Label(this, "", ConvertEscapeChars(sPitchTextKv->GetString("Text")));
	PitchLabel->SetBounds(sPitchTextKv->GetInt("XPos"), sPitchTextKv->GetInt("YPos"), sPitchTextKv->GetInt("Wide"), sPitchTextKv->GetInt("Tall"));

	cbTransThroughLevel = new CheckButton(this, "", ConvertEscapeChars(TransCBKv->GetString("Text")));
	cbTransThroughLevel->SetBounds(TransCBKv->GetInt("XPos"), TransCBKv->GetInt("YPos"), TransCBKv->GetInt("Wide"), TransCBKv->GetInt("Tall"));
	cbTransThroughLevel->SetSelected(false);

	bInit = true;
}

class CSPanellInterface : public SongPanel
{
private:
	CSongPanel* SPanell;
public:
	CSPanellInterface()
	{
		SPanell = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		SPanell = new CSongPanel(parent);
	}
	void Destroy()
	{
		if (SPanell)
		{
			SPanell->SetParent((vgui::Panel*)NULL);
			delete SPanell;
		}
	}
	void Activate(void)
	{
		if (SPanell)
		{
			SPanell->Activate();
		}
	}

	void OnLevelChange()
	{
		SPanell->OnLevelChange();
	}

	void OnLevelShutdown() 
	{
		if (!SPanell->m_sCurrPlaying)
			return;

		enginesound->StopSoundByGuid(SPanell->m_iSongGuid);
		SPanell->m_fStartTime = SPanell->m_fStartTime - (gpGlobals->curtime - SPanell->m_fStartTmp);
		SPanell->m_iSongGuid = -1;
	}

	void OnNewLevel()
	{
		SPanell->OnNewLevel();
	}
};
static CSPanellInterface g_SPanel;
SongPanel* songpanel = (SongPanel*)&g_SPanel;

ConVar cl_drawsongpanel("cl_drawsongpanel", "0", FCVAR_CLIENTDLL, "Sets the state of the alone mod Song panel");

void CSongPanel::OnLevelChange()
{
	m_fStartTmp = gpGlobals->curtime;

	if (!m_sCurrPlaying)
		return;

	if (!cbTransThroughLevel->IsSelected())
		return;

	float vol = ((float)sVolume->GetValue() / 100);
	if (!enginesound->IsSoundPrecached(m_sCurrPlaying))
		enginesound->PrecacheSound(m_sCurrPlaying);

	enginesound->EmitAmbientSound(m_sCurrPlaying, vol, m_iStartPitch, SND_SHOULDPAUSE, m_fStartTime);
	m_iSongGuid = enginesound->GetGuidForLastSoundEmitted();
	if (m_iSongGuid == 0)
		m_iSongGuid = -1;
}

void CSongPanel::OnNewLevel()
{
	m_iSongGuid = -1;
	m_sCurrPlaying = nullptr;
}

void CSongPanel::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_drawsongpanel.GetBool());

	if (!bInit)
		return;

	static int m_prevSliderVal = 0;
	int sval = sVolume->GetValue();

	if (m_iSongGuid != -1 && sval != m_prevSliderVal)
	{
		float flVol = ((float)sval / 100);
		enginesound->SetVolumeByGuid(m_iSongGuid, flVol);
	}
	m_prevSliderVal = sval;

	if (m_iSongGuid != -1)
	{
		if (!enginesound->IsSoundStillPlaying(m_iSongGuid))
		{
			m_sCurrPlaying = nullptr;
			m_iSongGuid = -1;
		}
	}
}

CON_COMMAND_F(ToggleSongPanel, "Toggles The Alone Mod Song Panel", FCVAR_HIDDEN)
{
	cl_drawsongpanel.SetValue(!cl_drawsongpanel.GetBool());
	songpanel->Activate();
};

void UppercaseLowerCase(const char* input, char** ptr) {
	int len = Q_strlen(input);
	if (len > 0)
	{
		char* output = new char[len + 1];
		for (int i = 0; i < len; i++)
		{
			output[i] = toupper(input[i]);
		}
		output[len] = '\0';	

		*ptr = output;
	};
	ptr = nullptr;
}

void CSongPanel::OnKeyCodePressed(KeyCode code)
{
	const char* bind = engine->Key_LookupBinding("ToggleSongPanel");
	if (bind)
	{
		char** lcase = new char*; 
		UppercaseLowerCase(bind, lcase);
		const char* cdtos = (const char*)KeyCodeToDisplayString(code);
		if (cdtos && lcase)
		{
			if (!(*lcase))
				return;

			if (!Q_strcmp(*lcase, cdtos))
				cl_drawsongpanel.SetValue(0);

			delete (*lcase);
			delete[] lcase;
			lcase = nullptr;
		}
	}
}

void CSongPanel::OnClose()
{
	cl_drawsongpanel.SetValue(0);
}

void CSongPanel::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);

	if (!Q_strcmp(pcCommand, "RPitch"))
	{
		sPitch->SetValue(100);
	}
	else if (!Q_strcmp(pcCommand, "Stop"))
	{
		if (m_iSongGuid != -1)
			enginesound->StopSoundByGuid(m_iSongGuid);

		m_sCurrPlaying = nullptr;
	}
	else if (!Q_strcmp(pcCommand, "PlaySong"))
	{
		int index = -1;
		for (int i = 0; i < v_RandomSongs.Size(); i++)
		{
			if (v_RandomSongs[i] == RemoveRandom(v_Names[CBoxSongs->GetActiveItem()]))
			{
				index = i;
				break;
			}
		}
		if (index != -1)
		{
			int num = 0;
			while (true)
			{
				num = random->RandomInt(0, v_Names.Size() - 1);

				bool found = false;
				for (int i = 0; i < v_DiscardSongsMap[v_RandomSongs[index]].Size(); i++)
				{
					if (!Q_strcmp(v_DiscardSongsMap[v_RandomSongs[index]][i], v_Names[num]))
					{
						found = true;
						break;
					}
				}
				if (found)
					continue;

				bool f = false;
				for (int i = 0; i < v_RandomSongs.Size(); i++)
				{
					if (v_RandomSongs[i] == RemoveRandom(v_Names[num]))
					{
						f = true;
						break;
					}
				}
				if (f)
					continue;

				break;
			}
			if (m_iSongGuid != -1)
				enginesound->StopSoundByGuid(m_iSongGuid);

			m_sCurrPlaying = v_Songs[num];
			if (enginesound->PrecacheSound(m_sCurrPlaying))
			{
				float vol = ((float)sVolume->GetValue() / 100);
				m_iStartPitch = sPitch->GetValue();

				enginesound->EmitAmbientSound(m_sCurrPlaying, vol, m_iStartPitch, SND_SHOULDPAUSE);
				m_iSongGuid = enginesound->GetGuidForLastSoundEmitted();
				if (m_iSongGuid == 0)
					m_iSongGuid = -1;
				else
					m_fStartTime = gpGlobals->curtime;
			}
			else
				m_sCurrPlaying = nullptr;

			m_fStartTmp = 1;
		}
		else
		{
			if (m_iSongGuid != -1)
				enginesound->StopSoundByGuid(m_iSongGuid);

			m_sCurrPlaying = v_Songs[CBoxSongs->GetActiveItem()];
			if (enginesound->PrecacheSound(m_sCurrPlaying))
			{
				float vol = ((float)sVolume->GetValue() / 100);
				m_iStartPitch = sPitch->GetValue();

				enginesound->EmitAmbientSound(m_sCurrPlaying, vol, m_iStartPitch, SND_SHOULDPAUSE);
				m_iSongGuid = enginesound->GetGuidForLastSoundEmitted();
				if (m_iSongGuid == 0)
					m_iSongGuid = -1;
				else
					m_fStartTime = gpGlobals->curtime;
			}
			else
				m_sCurrPlaying = nullptr;

			m_fStartTmp = 1;
		}
	}
}