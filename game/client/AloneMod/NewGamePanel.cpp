// aint no way i got this to work
// i had to make the new "NewGamePanel" because only 32 chapters are alowed on the base hl2 new game panel at a time
#include "cbase.h"
#include "fmtstr.h"
#include "filesystem.h"
#include "INewGamePanel.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Divider.h>
#include <vgui_controls/CheckButton.h>
#include <vgui/ISurface.h>
#include "fmtstr.h"
#include "AloneMod/Amod_SharedDefs.h"

using namespace vgui;

ConVar cl_drawnewgamepanel("cl_drawnewgamepanel", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN, "Sets the state of the alone mod New Game panel");
ConVar cl_drawoldnewgamepanel("cl_drawoldnewgamepanel", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN, "Sets the state of the alone mod New Game panel");

static int panx = 200, pany = 200;

#if !AMOD_DAYTIME_EDITION
extern ConVar amod_day;

#define IsDay(a) (amod_day.GetBool() && (ButtonExp + a > 48 && ButtonExp + a < 69))
#else
#define IsDay(a) false
#endif

void AmodOldPanelChangeCvar(IConVar* pConVar, const char* pOldString, float flOldValue)
{
	ConVarRef var(pConVar);
	if (var.GetBool())
	{
		if (cl_drawnewgamepanel.GetBool())
		{
			cl_drawnewgamepanel.SetValue(false);
			cl_drawoldnewgamepanel.SetValue(true);
			oldnewgamepanel->Activate();
		}
	}
	else
	{
		if (cl_drawoldnewgamepanel.GetBool())
		{
			cl_drawnewgamepanel.SetValue(true);
			cl_drawoldnewgamepanel.SetValue(false);
			newgamepanel->Activate();
		}
	}
}
ConVar amod_enable_old_new_game_panel("amod_enable_old_new_game_panel", "0", FCVAR_NONE, "", AmodOldPanelChangeCvar);

CheckButton* oldpanelbutfornew;
CheckButton* oldpanelbutforold;

class CNewGamePanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CNewGamePanel, vgui::Frame);

	CNewGamePanel(vgui::VPANEL parent);
	~CNewGamePanel() {};

	void DoDayCheck();

	void OnClose();
protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

private:
	int ButtonExp = 0;
	Button* chapter1b, * chapter2b, * chapter3b, * next, * prev;
	Label* chapter1l, * chapter2l, * chapter3l;
	ImagePanel* ImageC1, * ImageC2, * ImageC3;

	CUtlVector<const char*> ChapterNames;
	void Init();
};

CNewGamePanel::CNewGamePanel(vgui::VPANEL parent)
	: BaseClass(NULL, "NewGamePanel")
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

	vgui::ivgui()->AddTickSignal(GetVPanel(), 50);

	Init();
}

void CNewGamePanel::Init()
{
	KeyValues* kv = new KeyValues("");
	kv->LoadFromFile(filesystem, "resource/HL2_AloneMod_english.txt", "MOD");
	KeyValues* sub = kv->GetFirstTrueSubKey();

	for (int i = 1; i <= 101; i++)
	{
		if (!filesystem->FileExists(CFmtStr("cfg/chapter%d.cfg", i)))
			break;

		ChapterNames.AddToTail(sub->GetString(CFmtStr("HL2_Chapter%d_Title", i)));
	}

	SetTitle("Chapter Select", false);
	SetSize(520, 193);

	int screenWidth, screenHeight;
	vgui::surface()->GetScreenSize(screenWidth, screenHeight);

	int xPos = (screenWidth - 520) / 2;
	int yPos = (screenHeight - 193) / 2;

	SetPos(xPos, yPos);

	ImageC1 = new ImagePanel(this, "ImageC1");
	ImageC1->SetBounds(10, 40, 160, 90);
	ImageC1->SetImage("chapters/chapter1");

	ImageC2 = new ImagePanel(this, "ImageC2");
	ImageC2->SetBounds(180, 40, 160, 90);
	ImageC2->SetImage("chapters/chapter2");

	ImageC3 = new ImagePanel(this, "ImageC2");
	ImageC3->SetBounds(350, 40, 160, 90);
	ImageC3->SetImage("chapters/chapter3");

	chapter1l = new Label(this, "Chapter1l", ChapterNames[0]);
	chapter1l->SetBounds(10, 20, 170, 20);

	chapter2l = new Label(this, "Chapter2l", ChapterNames[1]);
	chapter2l->SetBounds(180, 20, 165, 20);

	chapter3l = new Label(this, "Chapter3l", ChapterNames[2]);
	chapter3l->SetBounds(350, 20, 165, 20);

	//Divider* div1 = new Divider(this, "div1");
	//div1->SetBounds(5, 23, 510, 2);

	prev = new Button(this, "prev", "Previous");
	prev->SetBounds(10, 165, 80, 25);
	prev->SetCommand("Prev");
	prev->SetEnabled(false);

	next = new Button(this, "next", "Next");
	next->SetBounds(430, 165, 80, 25);
	next->SetCommand("Next");

	Divider* div2 = new Divider(this, "div1");
	div2->SetBounds(5, 158, 510, 2);

	chapter1b = new Button(this, "Chapter1b", "Load Chapter 1");
	chapter1b->SetBounds(10, 135, 160, 20);
	chapter1b->SetCommand("chapter1");

	chapter2b = new Button(this, "Chapter2b", "Load Chapter 2");
	chapter2b->SetBounds(180, 135, 160, 20);
	chapter2b->SetCommand("chapter2");

	chapter3b = new Button(this, "Chapter3b", "Load Chapter 3");
	chapter3b->SetBounds(350, 135, 160, 20);
	chapter3b->SetCommand("chapter3");

	oldpanelbutfornew = new CheckButton(this, "oldbuttonfornewpanel", "Use Other Panel");
	oldpanelbutfornew->SetCommand("OBPress");
	oldpanelbutfornew->SetBounds(186, 165, 150, 25);
}

class CNGPanellInterface : public NewGamePanel
{
private:
	CNewGamePanel* NPanel;
public:
	CNGPanellInterface()
	{
		NPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		NPanel = new CNewGamePanel(parent);
	}
	void Destroy()
	{
		if (NPanel)
		{
			NPanel->SetParent((vgui::Panel*)NULL);
			delete NPanel;
		}
	}
	void Activate(void)
	{
		if (NPanel)
		{
			NPanel->Activate();
			NPanel->MoveToFront();
		}
	}

	void DayCheck()
	{
		if (NPanel)
		{
			NPanel->DoDayCheck();
		}
	}
};
static CNGPanellInterface g_NGanel;
NewGamePanel* newgamepanel = (NewGamePanel*)&g_NGanel;

void CNewGamePanel::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_drawnewgamepanel.GetBool());

	if (!amod_enable_old_new_game_panel.GetBool())
	{
		GetPos(panx, pany);
	}
	else
	{
		SetPos(panx - 185, pany - 8);
	}
}

CON_COMMAND_F(ToggleNewGamePanel, "Toggles The Alone Mod New Game Panel", FCVAR_HIDDEN)
{
	if (!amod_enable_old_new_game_panel.GetBool())
	{
		cl_drawnewgamepanel.SetValue(!cl_drawnewgamepanel.GetBool());
		newgamepanel->Activate();
	}
	else
	{
		cl_drawoldnewgamepanel.SetValue(!cl_drawoldnewgamepanel.GetBool());
		oldnewgamepanel->Activate();
	}
};

void CNewGamePanel::OnClose()
{
	cl_drawnewgamepanel.SetValue(0);
}

//i have absolutly 0 idea how to use % so i just did this
void CNewGamePanel::OnCommand(const char* pcCommand)
{
	static ConVar* commentary = cvar->FindVar("commentary");

	BaseClass::OnCommand(pcCommand);

	if (!Q_strcmp(pcCommand, "OBPress"))
	{
		if (oldpanelbutfornew->IsSelected())
		{
			if (oldpanelbutforold)
			{
				oldpanelbutforold->SetSelected(true);
			}

			amod_enable_old_new_game_panel.SetValue(1);
			engine->ClientCmd("amod_enable_old_new_game_panel 1");
		}
	}
	else if (!Q_strcmp(pcCommand, "Prev"))
	{
		if (ButtonExp == 0)
			return;

		chapter2b->SetSize(160, 20);
		chapter3b->SetSize(160, 20);

		ImageC2->SetSize(160, 90);
		ImageC3->SetSize(160, 90);

		chapter1b->SetText(CFmtStr("Load Chapter %d", ButtonExp - 2));
		chapter2b->SetText(CFmtStr("Load Chapter %d", ButtonExp - 1));
		chapter3b->SetText(CFmtStr("Load Chapter %d", ButtonExp));

		next->SetEnabled(true);

		if (ButtonExp == 3)
		{
			prev->SetEnabled(false);
		}

		next->SetEnabled(true);
		ButtonExp -= 3;
		chapter1l->SetText(ChapterNames[ButtonExp]);
		ImageC1->SetImage(CFmtStr("chapters/chapter%d%s", ButtonExp + 1, IsDay(1) ? "_day" : ""));

		if (ChapterNames.Size() > ButtonExp + 1)
		{
			chapter2l->SetText(ChapterNames[ButtonExp + 1]);
			ImageC2->SetImage(CFmtStr("chapters/chapter%d%s", ButtonExp + 2, IsDay(2) ? "_day" : ""));
		}
		else
		{
			chapter2l->SetText("");
		}

		if (ChapterNames.Size() > ButtonExp + 2)
		{
			chapter3l->SetText(ChapterNames[ButtonExp + 2]);
			ImageC3->SetImage(CFmtStr("chapters/chapter%d%s", ButtonExp + 3, IsDay(3) ? "_day" : ""));
		}
		else
		{
			chapter3l->SetText("");
		}
	}
	else if (!Q_strcmp(pcCommand, "Next"))
	{
		prev->SetEnabled(true);

		ButtonExp += 3;
		if (ButtonExp + 3 >= ChapterNames.Size())
			next->SetEnabled(false);

		chapter1b->SetText(CFmtStr("Load Chapter %d", ButtonExp + 1));

		if (ButtonExp + 1 < ChapterNames.Size())
		{
			chapter2b->SetText(CFmtStr("Load Chapter %d", ButtonExp + 2));
			if (ButtonExp + 2 < ChapterNames.Size())
			{
				chapter3b->SetText(CFmtStr("Load Chapter %d", ButtonExp + 3));
			}
			else
			{
				chapter3b->SetSize(0, 0);
				ImageC3->SetSize(0, 0);
			}
		}
		else
		{
			chapter2b->SetSize(0, 0);
			chapter3b->SetSize(0, 0);
			ImageC2->SetSize(0, 0);
			ImageC3->SetSize(0, 0);
		}

		chapter1l->SetText(ChapterNames[ButtonExp]);
		ImageC1->SetImage(CFmtStr("chapters/chapter%d%s", ButtonExp + 1, IsDay(1) ? "_day" : ""));

		if (ChapterNames.Size() > ButtonExp + 1)
		{
			chapter2l->SetText(ChapterNames[ButtonExp + 1]);
			ImageC2->SetImage(CFmtStr("chapters/chapter%d%s", ButtonExp + 2, IsDay(2) ? "_day" : ""));
		}
		else
		{
			chapter2l->SetText("");
		}

		if (ChapterNames.Size() > ButtonExp + 2)
		{
			chapter3l->SetText(ChapterNames[ButtonExp + 2]);
			ImageC3->SetImage(CFmtStr("chapters/chapter%d%s", ButtonExp + 3, IsDay(3) ? "_day" : ""));
		}
		else
		{
			chapter3l->SetText("");
		}
	}
	else if (!Q_strcmp(pcCommand, "chapter1"))
	{
		if (commentary) commentary->SetValue(0);
		engine->ClientCmd(CFmtStr("exec chapter%d", ButtonExp + 1));
	}
	else if (!Q_strcmp(pcCommand, "chapter2"))
	{
		if (commentary) commentary->SetValue(0);
		engine->ClientCmd(CFmtStr("exec chapter%d", ButtonExp + 2));
	}
	else if (!Q_strcmp(pcCommand, "chapter3"))
	{
		if (commentary) commentary->SetValue(0);
		engine->ClientCmd(CFmtStr("exec chapter%d", ButtonExp + 3));
	}
}

void CNewGamePanel::DoDayCheck()
{
	ImageC1->SetImage(CFmtStr("chapters/chapter%d%s", ButtonExp + 1, IsDay(1) ? "_day" : ""));

	if (ChapterNames.Size() > ButtonExp + 1)
		ImageC2->SetImage(CFmtStr("chapters/chapter%d%s", ButtonExp + 2, IsDay(2) ? "_day" : ""));

	if (ChapterNames.Size() > ButtonExp + 2)
		ImageC3->SetImage(CFmtStr("chapters/chapter%d%s", ButtonExp + 3, IsDay(3) ? "_day" : ""));
}

// this was the original new game panel. 
// i commented this out incase i couldnt get the new panel to work but i ended up getting it working
// im just gonna leave it here

typedef struct
{
	//Wowza thats alot of const char*'s
	const char* chaptertext;
	const char* maptext;
	const char* description;
	const char* imagefile;
	const char* cfgfile;
} Map_T;


class COldNewGamePanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(COldNewGamePanel, vgui::Frame);

	COldNewGamePanel(vgui::VPANEL parent);
	~COldNewGamePanel() {};

	void OnClose();
protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

private:
	CUtlVector<CUtlVector<Map_T*>*> m_vMaps;
	KeyValues* m_kvFile;

	ComboBox* m_cbMapList;
	Button* m_bPrev;
	Button* m_bNext;
	Button* m_bPlay;
	Divider* div1;
	Label* m_labelChapter;
	Label* m_labelDescription;
	ImagePanel* m_iIcon;

	unsigned int m_iCurr;
	int m_cbPrev;

	bool bDidLoad;
private:

	void LoadMaplist();
	void Init();
	void Format();
};

COldNewGamePanel::COldNewGamePanel(vgui::VPANEL parent)
	: BaseClass(NULL, "OldNewGamePanel")
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

	vgui::ivgui()->AddTickSignal(GetVPanel(), 20);

	bDidLoad = false;
	m_iCurr = 0;

	m_cbPrev = 0;

	Init();
}

void COldNewGamePanel::Init()
{
	m_kvFile = new KeyValues("MapList");
	if (!m_kvFile->LoadFromFile(filesystem, "resource/maplist.txt", "MOD"))
	{
		m_kvFile->deleteThis();
		ConWarning("Failed To Load resource/maplist.txt Aborting Panel Load!!!\n");
		return;
	}

	LoadMaplist();

	if (m_vMaps.Count() <= 0)
	{
		m_kvFile->deleteThis();
		ConWarning("Failed To Load resource/maplist.txt Aborting Panel Load!!!\n");
		return;
	}

	CUtlVector<Map_T*>* tmpvec = m_vMaps[0];
	if (!tmpvec)
	{
		m_kvFile->deleteThis();
		ConWarning("Failed To Load resource/maplist.txt Aborting Panel Load!!!\n");
		return;
	}
	Map_T* tmp = (*tmpvec)[0];

	SetTitle("Chapter Select", false);
	SetBounds(200, 200, 400, 285);

	m_bPrev = new Button(this, "", "Previous Chapter");
	m_bPrev->SetBounds(7, 185, 115, 25);
	m_bPrev->SetCommand("prev");
	m_bPrev->SetEnabled(false);

	m_bNext = new Button(this, "", "Next Chapter");
	m_bNext->SetBounds(296, 185, 100, 25);
	m_bNext->SetCommand("next");
	m_bNext->SetEnabled(m_vMaps.Count() > 1);

	m_bPlay = new Button(this, "", "Play Selected Map");
	m_bPlay->SetBounds(276, 255, 120, 25);
	m_bPlay->SetCommand("play");
	m_bPlay->SetEnabled(tmp);

	Label* maplab = new Label(this, "", "Current Map");
	maplab->SetBounds(61, 52, 120, 20);

	m_cbMapList = new ComboBox(this, 0, tmpvec->Count(), false);
	m_cbMapList->SetEnabled(true);
	m_cbMapList->SetBounds(10, 75, 200, 25);

	for (int i = 0; i < tmpvec->Size(); i++)
	{
		Map_T* tmpmap = (*tmpvec)[i];
		if (!tmpmap)
			continue;

		m_cbMapList->AddItem(tmpmap->maptext, nullptr);
	}

	m_cbMapList->ActivateItem(0);

	div1 = new Divider(this, "");
	div1->SetBounds(-2, 215, 500, 2);

	m_labelChapter = new Label(this, "", CFmtStr("Current Chapter: %s", (tmp) ? tmp->chaptertext : "Unknown Chapter"));
	m_labelChapter->SetBounds(8, 18, 400, 35);

	m_iIcon = new ImagePanel(this, "");
	m_iIcon->SetImage(tmp ? tmp->imagefile : "");
	m_iIcon->SetBounds(230, 65, 163, 93);

	vgui::HFont customFont = vgui::surface()->CreateFont();
	vgui::surface()->SetFontGlyphSet(customFont, "font", 24, 500, 0, 0, vgui::ISurface::FONTFLAG_ANTIALIAS);

	m_labelChapter->SetFont(customFont);

	m_labelDescription = new Label(this, "", (tmp) ? CFmtStr("Description:\n%s", tmp->description) : "");
	m_labelDescription->SetBounds(9, 210, 500, 50);

	oldpanelbutforold = new CheckButton(this, "", "Use Other Panel");
	oldpanelbutforold->SetBounds(130, 255, 135, 20);
	oldpanelbutforold->SetCommand("OBCheck");

	bDidLoad = true;
}

void COldNewGamePanel::LoadMaplist()
{
	FOR_EACH_TRUE_SUBKEY(m_kvFile, chapters)
	{
		CUtlVector<Map_T*>* tmp = new CUtlVector<Map_T*>;
		FOR_EACH_TRUE_SUBKEY(chapters, chapter)
		{
			Map_T* tmpmap = new Map_T;
			tmpmap->chaptertext = chapters->GetName();
			tmpmap->maptext = chapter->GetName();
			tmpmap->cfgfile = chapter->GetString("config", "chapter1");
			tmpmap->imagefile = chapter->GetString("image", "chapters/chapter1");
			tmpmap->description = chapter->GetString("description", "");
			tmp->AddToTail(tmpmap);
		}
		m_vMaps.AddToTail(tmp);
	}
}

class CONGPanellInterface : public OldNewGamePanel
{
private:
	COldNewGamePanel* NPanel;
public:
	CONGPanellInterface()
	{
		NPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		NPanel = new COldNewGamePanel(parent);
	}
	void Destroy()
	{
		if (NPanel)
		{
			NPanel->SetParent((vgui::Panel*)NULL);
			delete NPanel;
		}
	}
	void Activate(void)
	{
		if (NPanel)
		{
			NPanel->Activate();
			NPanel->MoveToFront();
		}
	}
};
static CONGPanellInterface g_ONGanel;
OldNewGamePanel* oldnewgamepanel = (OldNewGamePanel*)&g_ONGanel;

void COldNewGamePanel::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_drawoldnewgamepanel.GetBool());

	if (!bDidLoad)
		return;

	if (!m_cbMapList)
		return;

	if (m_cbMapList->GetActiveItem() != m_cbPrev)
	{
		CUtlVector<Map_T*>* tmpvec = m_vMaps[m_iCurr];
		if (!tmpvec)
			return;

		m_cbPrev = m_cbMapList->GetActiveItem();
		Map_T* tmp = (*tmpvec)[m_cbPrev];
		if (!tmp)
			return;

		m_labelDescription->SetText(CFmtStr("Description:\n%s", tmp->description));
		m_iIcon->SetImage(tmp->imagefile);

		if (m_iIcon->GetImage())
			m_iIcon->GetImage()->SetSize(267, 160);
	}
}

CON_COMMAND_F(ToggleOldNewGamePanel, "Toggles The Alone Mod New Game Panel", FCVAR_HIDDEN)
{
	cl_drawoldnewgamepanel.SetValue(!cl_drawoldnewgamepanel.GetBool());
	oldnewgamepanel->Activate();
};

void COldNewGamePanel::OnClose()
{
	cl_drawoldnewgamepanel.SetValue(0);
}

void COldNewGamePanel::Format()
{
	CUtlVector<Map_T*>* tmpvec = m_vMaps[m_iCurr];
	if (!tmpvec)
		return;

	m_cbPrev = m_cbMapList->GetActiveItem();
	Map_T* tmp = (*tmpvec)[0];

	m_labelDescription->SetText(tmp ? CFmtStr("Description:\n%s",  tmp->description) : "");
	m_iIcon->SetImage((tmp) ? tmp->imagefile : "");

	m_cbPrev = 0;

	delete m_cbMapList;

	m_cbMapList = new ComboBox(this, 0, tmpvec->Count(), false);
	m_cbMapList->SetEnabled(true);
	m_cbMapList->SetBounds(10, 75, 200, 25);

	for (int i = 0; i < tmpvec->Size(); i++)
	{
		Map_T* tmpmap = (*tmpvec)[i];
		if (!tmpmap)
			continue;

		m_cbMapList->AddItem(tmpmap->maptext, nullptr);
	}

	m_cbMapList->ActivateItem(0);

	m_labelChapter->SetText(tmp ? CFmtStr("Current Chapter: %s", tmp->chaptertext) : "");
}

void COldNewGamePanel::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);
	
	static ConVar* commentary = cvar->FindVar("commentary");

	if (!Q_strcmp(pcCommand, "OBCheck"))
	{
		if (!oldpanelbutforold->IsSelected())
		{
			if (oldpanelbutfornew)
			{
				oldpanelbutfornew->SetSelected(false);
			}

			amod_enable_old_new_game_panel.SetValue(0);
			engine->ClientCmd("amod_enable_old_new_game_panel 0");
		}
	}
	else if (!Q_strcmp(pcCommand, "play"))
	{
		CUtlVector<Map_T*>* tmpvec = m_vMaps[m_iCurr];
		if (!tmpvec)
			return;

		int ai = m_cbMapList->GetActiveItem();
		if (ai >= tmpvec->Count())
			return;

		Map_T* curr = (*tmpvec)[ai];
		if (!curr)
			return;

		if (commentary) commentary->SetValue(0);
		engine->ClientCmd_Unrestricted(CFmtStr("exec %s", curr->cfgfile));
	}
	else if (!Q_strcmp(pcCommand, "next"))
	{
		m_iCurr++;
		if ((int)m_iCurr >= m_vMaps.Count() - 1)
			m_bNext->SetEnabled(false);

		m_bPrev->SetEnabled(true);
		Format();
	}
	else if (!Q_strcmp(pcCommand, "prev"))
	{
		m_iCurr--;
		if (m_iCurr <= 0)
			m_bPrev->SetEnabled(false);

		m_bNext->SetEnabled(true);
		Format();
	}
}

void AmodDayChangeCallback(IConVar* var, const char*, float)
{
	g_NGanel.DayCheck();

	if (engine->IsInGame() && !engine->IsLevelMainMenuBackground() && IsCityMap(szMapName))
		engine->ExecuteClientCmd("save tmp_day001; load tmp_day001");
	else
	{
		if (engine->IsInGame() && engine->IsLevelMainMenuBackground() && !Q_strcmp(szMapName, "background06_d"))
			engine->ExecuteClientCmd("map_background background06_d");
		else if (engine->IsInGame() && engine->IsLevelMainMenuBackground() && !Q_strcmp(szMapName, "background07_d"))
			engine->ExecuteClientCmd("map_background background06_d");
	}

#if !AMOD_DAYTIME_EDITION
	if (!amod_day.GetBool())
		engine->ClientCmd_Unrestricted("tf1; alias Amod_ToggleFilter tf2");
#endif
}

void AmodDayRavChangeCallback(IConVar* var, const char*, float)
{
	g_NGanel.DayCheck();

	if (engine->IsInGame() && !engine->IsLevelMainMenuBackground() && Q_strcmp(szMapName, "d1_town_05_d") && Q_strstr(szMapName, "d1_town_"))
		engine->ExecuteClientCmd("save tmp_day001; load tmp_day001");

#if !AMOD_DAYTIME_EDITION
	if (!amod_day.GetBool())
		engine->ClientCmd_Unrestricted("tf1; alias Amod_ToggleFilter tf2");
#endif
}