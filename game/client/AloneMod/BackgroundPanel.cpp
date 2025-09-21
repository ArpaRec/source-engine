#include "cbase.h"
#include "fmtstr.h"
#include "filesystem.h"
#include "IBackgroundPanel.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/ISurface.h>
#include "AloneMod/Amod_SharedDefs.h"

using namespace vgui;

class CBackgroundPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CBackgroundPanel, vgui::Frame);

	CBackgroundPanel(vgui::VPANEL parent); 	
	~CBackgroundPanel() {};	

	void OnClose();
protected:
	//VGUI overrides:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

private:
	void Init();

	ComboBox* MapNames;
	Button* LoadButton;
	ImagePanel* BackgroundPic;
	KeyValues* kvMain;
	CUtlVector<const char*> m_vMaps, v_MapNames, v_MapPics;
	bool m_bInit = false;
};

CBackgroundPanel::CBackgroundPanel(vgui::VPANEL parent)
	: BaseClass(NULL, "BackgroundPanel")
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

	kvMain = new KeyValues("BackgroundPanel");
	if (!kvMain->LoadFromFile(filesystem, "resource/panels/BackgroundPanel.txt", "MOD"))
	{
		ConWarning("Error Opening BackgroundPanel.txt Aborting Panel Load\n");
		return;
	}

	Init();
}

void CBackgroundPanel::Init()
{
	KeyValues* CBoxKv = kvMain->FindKey("ChapterSelect");
	if (!kvMain)
	{
		ConWarning("Got NULL Subkey: \"ChapterSelect\" In File: \"BackgroundPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* CBoxDataSub = CBoxKv->FindKey("Data");
	if (!CBoxKv)
	{
		ConWarning("Got NULL Subkey: \"Data\" For ChapterSelect In File: \"BackgroundPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	if (!CBoxDataSub->GetFirstSubKey())
	{
		ConWarning("Got Empty Data In Subkey: \"Data\" For \"ChapterSelect\" In File: \"BackgroundPanel.txt\" Aborting Panel Load!\n");
		return;	
	}

	FOR_EACH_TRUE_SUBKEY(CBoxDataSub, CBoxDat)
	{
		if (m_vMaps.Find(CBoxDat->GetName()) != -1)
		{
			ConWarning("Got Multiple Of The Same Subkey In Subkey: \"Data\" For ChapterSelect In File: \"BackgroundPanel.txt\" Aborting Panel Load!\n");
			return;
		}

		m_vMaps.AddToTail(CBoxDat->GetName());
		v_MapNames.AddToTail(CBoxDat->GetString("Map", "background01_d"));
		v_MapPics.AddToTail(CBoxDat->GetString("Image", "backgrounds/background01"));
	}

	KeyValues* LMapBKv = kvMain->FindKey("LoadButton");
	if (!LMapBKv)
	{
		ConWarning("Got NULL Subkey: \"LoadButton\" In File: \"BackgroundPanel.txt\" Aborting Panel Load!\n");
		return;
	}

	KeyValues* ImageKV = kvMain->FindKey("BackgroundImage");
	if (!ImageKV)
	{
		ConWarning("Got NULL Subkey: \"BackgroundImage\" In File: \"BackgroundPanel.txt\" Aborting Panel Load!\n");
		return;
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

	MapNames = new ComboBox(this, "MapCBox", m_vMaps.Size(), false);
	for (int i = 0; i < m_vMaps.Size(); i++)
	{
		MapNames->AddItem(m_vMaps[i], nullptr);
	}

	MapNames->SetBounds(CBoxKv->GetInt("XPos"), CBoxKv->GetInt("YPos"), CBoxKv->GetInt("Wide"), CBoxKv->GetInt("Tall"));
	MapNames->ActivateItem(0);

	LoadButton = new Button(this, "LoadButton", LMapBKv->GetString("Text", "Button"));
	LoadButton->SetBounds(LMapBKv->GetInt("XPos"), LMapBKv->GetInt("YPos"), LMapBKv->GetInt("Wide"), LMapBKv->GetInt("Tall"));
	LoadButton->SetCommand("LoadMap");

	BackgroundPic = new ImagePanel(this, "BackgroundPic");
	BackgroundPic->SetSize(ImageKV->GetInt("Wide"), ImageKV->GetInt("Tall"));
	BackgroundPic->SetPos(ImageKV->GetInt("XPos"), ImageKV->GetInt("YPos"));
	BackgroundPic->SetImage(v_MapPics[MapNames->GetActiveItem()]);

	m_bInit = true;
}

class CBPanellInterface : public BackgroundPanel
{
private:
	CBackgroundPanel* BPanel;
public:
	CBPanellInterface()
	{
		BPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		BPanel = new CBackgroundPanel(parent);
	}
	void Destroy()
	{
		if (BPanel)
		{
			BPanel->SetParent((vgui::Panel*)NULL);
			delete BPanel;
		}
	}
	void Activate(void)
	{
		if (BPanel)
		{
			BPanel->Activate();
		}
	}
};
static CBPanellInterface g_BPanel;
BackgroundPanel* backgroundpanel = (BackgroundPanel*)&g_BPanel;

ConVar cl_drawbackgroundpanel("cl_drawbackgroundpanel", "0", FCVAR_CLIENTDLL, "Sets the state of the alone mod background panel");

#if !AMOD_DAYTIME_EDITION
extern ConVar amod_day;
#endif

void CBackgroundPanel::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_drawbackgroundpanel.GetBool());

	if (!m_bInit)
		return;

	static int prev = 0;

	if (!Q_strcmp(v_MapPics[MapNames->GetActiveItem()], "PrevImage"))
	{
		BackgroundPic->SetImage(v_MapPics[prev]);
		return;
	}

#if !AMOD_DAYTIME_EDITION
	if (!amod_day.GetBool())
#else
	if (1)
#endif
		BackgroundPic->SetImage(v_MapPics[MapNames->GetActiveItem()]);
	else
	{
		if (!Q_strcmp(v_MapNames[MapNames->GetActiveItem()], "background06_d"))
			BackgroundPic->SetImage(CFmtStr("%s_day", v_MapPics[MapNames->GetActiveItem()]));
		else
			BackgroundPic->SetImage(v_MapPics[MapNames->GetActiveItem()]);
	}

	prev = MapNames->GetActiveItem();
}

CON_COMMAND_F(ToggleBackgroundPanel, "Toggles The Alone Mod Background Panel", FCVAR_HIDDEN)
{
	cl_drawbackgroundpanel.SetValue(!cl_drawbackgroundpanel.GetBool());
	backgroundpanel->Activate();
};

void CBackgroundPanel::OnClose()
{
	cl_drawbackgroundpanel.SetValue(0);
}

void CBackgroundPanel::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);

	if (!Q_strcmp(pcCommand, "LoadMap"))
	{
		if (!Q_strcmp(v_MapNames[MapNames->GetActiveItem()], "Random"))
		{
			int num = 0;
			while (true)
			{
				num = random->RandomInt(0, v_MapNames.Size() - 1);
				if (Q_strcmp(v_MapNames[num], "Random"))
				{
					break;
				}
			}

			char buf[215];
			Q_snprintf(buf, sizeof(buf), "map_background %s", v_MapNames[num]);
			engine->ClientCmd_Unrestricted(buf);
			engine->ClientCmd_Unrestricted("hideconsole");
			engine->ClientCmd_Unrestricted("cl_drawoptionspanel 0");
			engine->ClientCmd_Unrestricted("cl_drawsongpanel 0");
			engine->ClientCmd_Unrestricted("cl_drawnewgamepanel 0");
			cl_drawbackgroundpanel.SetValue(0);
		}
		else
		{
			char buf[215];
			Q_snprintf(buf, sizeof(buf), "map_background %s", v_MapNames[MapNames->GetActiveItem()]);
			engine->ClientCmd_Unrestricted(buf);
			engine->ClientCmd_Unrestricted("hideconsole");
			engine->ClientCmd_Unrestricted("cl_drawoptionspanel 0");
			engine->ClientCmd_Unrestricted("cl_drawsongpanel 0");
			engine->ClientCmd_Unrestricted("cl_drawnewgamepanel 0");
			cl_drawbackgroundpanel.SetValue(0);
		}
	}
} 