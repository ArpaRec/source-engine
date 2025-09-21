this file is for me so if the random song stuff breaks or i mess it up i have a backup
//#include "cbase.h"
//#include "fmtstr.h"
//#include "filesystem.h"
//#include "ISongPanel.h"
//#include <vgui/IVGui.h>
//#include <vgui_controls/Frame.h>
//#include <vgui_controls/ComboBox.h>
//#include <vgui_controls/Button.h>
//#include <string>
//
//using namespace vgui;
//
//bool FindSubStr(const char* str, const char* subStr) {
//	int strLen = Q_strlen(str);
//	int subStrLen = Q_strlen(subStr);
//
//	if (subStrLen == 0 || subStrLen > strLen) {
//		return false;
//	}
//
//	for (int i = 0; i <= strLen - subStrLen; ++i) {
//		bool match = true;
//		for (int j = 0; j < subStrLen; ++j) {
//			if (str[i + j] != subStr[j]) {
//				match = false;
//				break;
//			}
//		}
//
//		if (match) {
//			return true;
//		}
//	}
//
//	return false;
//}
//
//class CSongPanel : public vgui::Frame
//{
//	DECLARE_CLASS_SIMPLE(CSongPanel, vgui::Frame);
//
//	CSongPanel(vgui::VPANEL parent); 	
//	~CSongPanel() {};				
//
//protected:
//	//VGUI overrides:
//	virtual void OnTick();
//	virtual void OnCommand(const char* pcCommand);
//
//	void OnKeyCodePressed(KeyCode code);
//	void OnClose();
//private:
//	KeyValues* kvMain;
//	ComboBox* CBoxSongs;
//	Button* bPlay,* bStop;
//
//	CUtlVector<const char*> v_Songs, v_Names, m_vDiscardSongs;
//
//	void Init();
//};
//
//CSongPanel::CSongPanel(vgui::VPANEL parent)
//	: BaseClass(NULL, "SongPanel")
//{
//	SetParent(parent);
//
//	SetKeyBoardInputEnabled(true);
//	SetMouseInputEnabled(true);
//
//	SetProportional(false);
//	SetTitleBarVisible(true);
//	SetMinimizeButtonVisible(false);
//	SetMaximizeButtonVisible(false);
//	SetCloseButtonVisible(true);
//	SetSizeable(false);
//	SetMoveable(true);
//	SetVisible(true);
//
//	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
//
//	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
//
//	kvMain = new KeyValues("SongPanel");
//	if (!kvMain->LoadFromFile(filesystem, "resource/panels/SongPanel.txt", "MOD"))
//	{
//		ConWarning("Error Opening SongPanel.txt Aborting Panel Load\n");
//		return;
//	}
//
//	Init();
//}
//
//void CSongPanel::Init()
//{
//	KeyValues* SongListKv = kvMain->FindKey("SongList");
//	if (!SongListKv)
//	{
//		ConWarning("Got NULL Subkey: \"SongList\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
//		return;
//	}
//
//	KeyValues* CSongsSub = SongListKv->FindKey("Songs");
//	if (!CSongsSub)
//	{
//		ConWarning("Got NULL Subkey: \"Songs\" For SongList In File: \"SongPanel.txt\" Aborting Panel Load!\n");
//		return;
//	}
//	
//	if (!CSongsSub->GetFirstSubKey())
//	{
//		ConWarning("Got No keyvalue Pair In Subkey: \"Songs\" For Songs In File: \"SongPanel.txt\" Aborting Panel Load!\n");
//		return;
//	}
//
//	FOR_EACH_VALUE(CSongsSub, CSongs)
//	{
//		if (v_Names.Find(CSongs->GetName()) != -1)
//		{
//			ConWarning("Got Multiple Of The Same Key In Subkey: \"Songs\" For Songs In File: \"SongPanel.txt\" Aborting Panel Load!\n");
//			return;
//		}
//
//		v_Names.AddToTail(CSongs->GetName());
//		v_Songs.AddToTail(CSongs->GetString());
//	}
//
//	KeyValues* SPPlayBKv = kvMain->FindKey("PlayButton");
//	if (!SPPlayBKv)
//	{
//		ConWarning("Got NULL Subkey: \"PlayButton\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
//		return;
//	}
//
//	KeyValues* SPStopBKv = kvMain->FindKey("StopButton");
//	if (!SPStopBKv)
//	{
//		ConWarning("Got NULL Subkey: \"StopButton\" In File: \"SongPanel.txt\" Aborting Panel Load!\n");
//		return;
//	}
//
//	SetTitle(kvMain->GetString("Title", "Title"), false);
//	SetBounds(kvMain->GetInt("XPos"), kvMain->GetInt("YPos"), kvMain->GetInt("Wide"), kvMain->GetInt("Tall"));
//
//	CBoxSongs = new ComboBox(this, "MapCBox", v_Names.Size(), false);
//	for (int i = 0; i < v_Names.Size(); i++)
//	{
//		CBoxSongs->AddItem(v_Names[i], nullptr);
//		if (!Q_strcmp(v_Names[i], "Random"))
//		{
//			for (int j = 0; j < v_Names.Size(); j++)
//			{
//				if (FindSubStr(v_Songs[i], std::string(std::string("[discard]") + v_Names[j]).c_str()))
//					m_vDiscardSongs.AddToTail(v_Names[j]);
//			}
//		}
//	}
//
//	CBoxSongs->SetBounds(SongListKv->GetInt("XPos"), SongListKv->GetInt("YPos"), SongListKv->GetInt("Wide"), SongListKv->GetInt("Tall"));
//	CBoxSongs->ActivateItem(SongListKv->GetInt("ActiveItem"));
//		
//	bPlay = new Button(this, "PlayButton", SPPlayBKv->GetString("Text", "Button"));
//	bPlay->SetBounds(SPPlayBKv->GetInt("XPos"), SPPlayBKv->GetInt("YPos"), SPPlayBKv->GetInt("Wide"), SPPlayBKv->GetInt("Tall"));
//	bPlay->SetCommand("PlaySong");
//
//	bStop = new Button(this, "StopButton", SPStopBKv->GetString("Text", "Button"));
//	bStop->SetBounds(SPStopBKv->GetInt("XPos"), SPStopBKv->GetInt("YPos"), SPStopBKv->GetInt("Wide"), SPStopBKv->GetInt("Tall"));
//	bStop->SetCommand("Stop");
//}
//
//class CSPanellInterface : public SongPanel
//{
//private:
//	CSongPanel* SPanell;
//public:
//	CSPanellInterface()
//	{
//		SPanell = NULL;
//	}
//	void Create(vgui::VPANEL parent)
//	{
//		SPanell = new CSongPanel(parent);
//	}
//	void Destroy()
//	{
//		if (SPanell)
//		{
//			SPanell->SetParent((vgui::Panel*)NULL);
//			delete SPanell;
//		}
//	}
//	void Activate(void)
//	{
//		if (SPanell)
//		{
//			SPanell->Activate();
//		}
//	}
//};
//static CSPanellInterface g_SPanel;
//SongPanel* songpanel = (SongPanel*)&g_SPanel;
//
//ConVar cl_drawsongpanel("cl_drawsongpanel", "0", FCVAR_CLIENTDLL, "Sets the state of the alone mod Song panel");
//
//void CSongPanel::OnTick()
//{
//	BaseClass::OnTick();
//	SetVisible(cl_drawsongpanel.GetBool());
//}
//
//CON_COMMAND_F(ToggleSongPanel, "Toggles The Alone Mod Song Panel", FCVAR_HIDDEN)
//{
//	cl_drawsongpanel.SetValue(!cl_drawsongpanel.GetBool());
//	songpanel->Activate();
//};
//
//char ToUpper(char c)
//{
//	if (c >= 'a' && c <= 'z')
//		return c - 32;
//
//	return c;
//}
//
//const char* UppercaseLowerCase(const char* input) {
//	if (Q_strlen(input) > 0)
//	{
//		char* output = new char[2];
//		output[0] = ToUpper(input[0]);
//		output[1] = '\0';
//
//		return output;
//	}
//	return nullptr;
//}
//
//void CSongPanel::OnKeyCodePressed(KeyCode code)
//{
//	const char* bind = engine->Key_LookupBinding("ToggleSongPanel");
//	if (bind)
//	{
//		const char* lcase = UppercaseLowerCase(bind);
//		const char* cdtos = (const char*)KeyCodeToDisplayString(code);
//		if (cdtos && lcase)
//		{
//			if (!Q_strcmp(lcase, cdtos))
//				cl_drawsongpanel.SetValue(0);
//		}
//	}
//	
//}
//
//void CSongPanel::OnClose()
//{
//	cl_drawsongpanel.SetValue(0);
//}
//
//void CSongPanel::OnCommand(const char* pcCommand)
//{
//	BaseClass::OnCommand(pcCommand);
//
//	if (!Q_strcmp(pcCommand, "Stop"))
//	{
//		engine->ClientCmd("play common/null");
//	}
//	else if (!Q_strcmp(pcCommand, "PlaySong"))
//	{
//		//this can crash if you discard every song so dont do that
//		if (!Q_strcmp(v_Names[CBoxSongs->GetActiveItem()], "Random"))
//		{
//			int num = 0;
//			while (true)
//			{
//				num = random->RandomInt(0, v_Names.Size() - 1);
//
//				bool found = false;
//				for (int i = 0; i < m_vDiscardSongs.Size(); i++)
//				{
//					if (!Q_strcmp(m_vDiscardSongs[i], v_Names[num]))
//					{
//						found = true;
//						break;
//					}
//				}
//				if (found)
//					continue;
//
//				if (Q_strcmp(v_Names[num], "Random"))
//				{
//					break;
//				}
//			}
//			engine->ClientCmd(CFmtStr("play %s", v_Songs[num]));
//		}
//		else
//		{
//			engine->ClientCmd(CFmtStr("play %s", v_Songs[CBoxSongs->GetActiveItem()]));
//		}
//	}
//}