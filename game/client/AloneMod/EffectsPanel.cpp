#include "cbase.h"
#include "fmtstr.h"
#include "filesystem.h"
#include "IEffectsPanel.h"
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Slider.h>
#include <vgui_controls/Divider.h>	
#include <vgui_controls/TextEntry.h>	
#include <vgui_controls/FileOpenDialog.h>	
#include <vgui/ISurface.h>
#include "AloneMod/AmodCvars.h"

using namespace vgui;

extern ConVar fov_desired;
extern ConVar v_viewmodel_fov;
extern ConVar cl_pitchdown;
extern ConVar cl_pitchup;
extern ConVar r_drawviewmodel;

#define MAKEFILTER_START() \
{\
int py = 46;

#define MAKEFILTER_END() \
}

#define MAKEFILTER_INTERNAL(i) \
m_CBFilter##i = new CheckButton(this, "", CFmtStr("Dark Filter - Brightness Level: %d", i));\
m_CBFilter##i->SetBounds(255, py, 250, 25); \
py += 20;

#define MAKEFILTER(i) \
CheckButton* m_CBFilter##i;\

#define FILTERCHECK(i) \
amod_view_filter_video##i.SetValue(m_CBFilter##i->IsSelected());

class CEffectsPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CEffectsPanel, vgui::Frame);

	CEffectsPanel(vgui::VPANEL parent);
	~CEffectsPanel() {};

	MESSAGE_FUNC_CHARPTR(OnFileSelected, "FileSelected", fullpath);

	void OnClose();
protected:
	virtual void OnTick();
	virtual void OnCommand(const char* pcCommand);

	void OnKeyCodePressed(KeyCode code);
private:
	void Init();

	CheckButton* m_cbDDVModel;
	CheckButton* m_cbBAWhite;
	CheckButton* m_cbLenseDirt;
	CheckButton* m_cbBCam;
	CheckButton* m_cbCBlur;
	CheckButton* m_cbBinoculars;
	CheckButton* m_cbSquare;
	CheckButton* m_cbCPhobia;
	CheckButton* m_cbVFov;

	CheckButton* m_cbCCamera;
	CheckButton* m_cbCCameraFix;

	MAKEFILTER(1);
	MAKEFILTER(2);
	MAKEFILTER(3);
	MAKEFILTER(4);
	MAKEFILTER(5);
	MAKEFILTER(6);
	MAKEFILTER(7);
	MAKEFILTER(8);

	Slider* m_sSquare;
	Slider* m_sSquareHeight;
	Slider* m_sCPhobia;
	Slider* m_sCFov;
	Slider* m_sViewmodelFov;
	Slider* m_sCCamAngles;
	Slider* m_sCCamOrigin;

	Slider* m_sMinAngles;
	Slider* m_sMaxAngles;


	TextEntry* m_teXYZ;
	TextEntry* m_tePYR;

	FileOpenDialog* m_FOLoad;
	FileOpenDialog* m_FSave;
	bool bWasLoad;
};					

CEffectsPanel::CEffectsPanel(vgui::VPANEL parent)
	: BaseClass(NULL, "EffectsPanel")
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

	vgui::ivgui()->AddTickSignal(GetVPanel(), 30);

	Init();	
}

void CEffectsPanel::Init()
{
	int screenWidth, screenHeight;
	vgui::surface()->GetScreenSize(screenWidth, screenHeight);

	int xPos = (screenWidth - 505) / 2;
	int yPos = (screenHeight - 487) / 2;

	SetSize(505, 487);
	SetPos(xPos, yPos);

	SetTitle("Effects Panel", false);

	m_cbDDVModel = new CheckButton(this, "", "Dont Draw Weapon Viewmodel");
	m_cbDDVModel->SetBounds(10, 25, 250, 25);
	
	m_cbBAWhite = new CheckButton(this, "", "Enable Black And White View");
	m_cbBAWhite->SetBounds(10, 45, 250, 25);

	m_cbLenseDirt = new CheckButton(this, "", "Enable Lense Dirt On Screen");
	m_cbLenseDirt->SetBounds(10, 65, 250, 25);

	m_cbBCam = new CheckButton(this, "", "Enable Tv Styled View");
	m_cbBCam->SetBounds(10, 85, 250, 25);

	m_cbBinoculars = new CheckButton(this, "", "Enable Colored Tv Styled View");
	m_cbBinoculars->SetBounds(10, 105, 250, 25);

	m_cbCBlur = new CheckButton(this, "", "Enable Blured View");
	m_cbCBlur->SetBounds(10, 125, 250, 25);

	m_cbSquare = new CheckButton(this, "", "Show Cinematic Black Boxes");
	m_cbSquare->SetBounds(10, 145, 250, 25);

	m_cbCPhobia = new CheckButton(this, "", "Enable Claustrophobia View");
	m_cbCPhobia->SetBounds(10, 165, 250, 25);
	m_cbCPhobia->SetCommand("cphobia");

	m_cbVFov = new CheckButton(this, "", "Override Viewmodel Fov");
	m_cbVFov->SetBounds(10, 185, 250, 25);
	m_cbVFov->SetCommand("vfov");

	Label* lSSquare = new Label(this, "", "Black Edge Boxes Width");
	lSSquare->SetBounds(35, 205, 200, 20);

	m_sSquare = new Slider(this, "m_sSquare");
	m_sSquare->SetBounds(15, 222, 190, 25);
	m_sSquare->SetRange(0, 20);
	m_sSquare->SetValue(5);

	Label* lSSquareH = new Label(this, "", "Black Edge Boxes Height");
	lSSquareH->SetBounds(35, 245, 200, 20);

	m_sSquareHeight = new Slider(this, "m_sSquare");
	m_sSquareHeight->SetBounds(15, 262, 190, 25);
	m_sSquareHeight->SetRange(0, 20);
	m_sSquareHeight->SetValue(0);

	Label* lCPhobia = new Label(this, "", "Claustraphobia Amount");
	lCPhobia->SetBounds(35, 284, 200, 20);

	m_sCPhobia = new Slider(this, "m_sCPhobia");
	m_sCPhobia->SetBounds(15, 301, 190, 25);
	m_sCPhobia->SetRange(1, 20);
	m_sCPhobia->SetValue(4);

	Label* lFov = new Label(this, "", "Claustraphobia Fov Override");
	lFov->SetBounds(25, 325, 200, 20);

	m_sCFov = new Slider(this, "m_sCFov");
	m_sCFov->SetBounds(15, 345, 190, 25);
	m_sCFov->SetRange(75, 170);
	m_sCFov->SetValue(fov_desired.GetInt());
	m_sCFov->SetEnabled(false);

	Label* Vfo = new Label(this, "", "Viewmodel Fov Override");
	Vfo->SetBounds(38, 360, 250, 35);

	m_sViewmodelFov = new Slider(this, "m_sCFov");
	m_sViewmodelFov->SetBounds(15, 385, 190, 25);
	m_sViewmodelFov->SetRange(5, 360);
	m_sViewmodelFov->SetValue(v_viewmodel_fov.GetInt());
	m_sViewmodelFov->SetEnabled(false);

	Label* Sov = new Label(this, "", "This Was Mainly Made For People To\nCreate Cinematic Style Videos");
	Sov->SetBounds(10, 445, 250, 35);

	Divider* div = new Divider(this, "div");
	div->SetBounds(235, -2, 2, 500);

	Label* lFilters = new Label(this, "lFilters", "Dark Filters That Will Show When\n\t\t\tScreen Recording");
	lFilters->SetBounds(255, 10, 195, 40);

	MAKEFILTER_START();
		
		MAKEFILTER_INTERNAL(1);
		MAKEFILTER_INTERNAL(2);
		MAKEFILTER_INTERNAL(3);
		MAKEFILTER_INTERNAL(4);
		MAKEFILTER_INTERNAL(5);
		MAKEFILTER_INTERNAL(6);
		MAKEFILTER_INTERNAL(7);
		MAKEFILTER_INTERNAL(8);

	MAKEFILTER_END();

	Divider* div2 = new Divider(this, "div");
	div2->SetBounds(236, 212, 300, 2);

	m_cbCCamera = new CheckButton(this, "m_cbCCamera", "Smooth/Cinematic Camera + Cam Editor");
	m_cbCCamera->SetBounds(235, 214, 375, 20);

	m_cbCCameraFix = new CheckButton(this, "m_cbCCamera", "Enable Smooth Camera Viewmodel Fix");
	m_cbCCameraFix->SetBounds(235, 234, 375, 20);

	Label* lCameraSmoothAngles = new Label(this, "", "Angle Smooth Amount (0 = none)");
	lCameraSmoothAngles->SetBounds(278, 252, 200, 20);

	m_sCCamAngles = new Slider(this, "");
	m_sCCamAngles->SetRange(0, 20);
	m_sCCamAngles->SetValue(0);
	m_sCCamAngles->SetBounds(270, 273, 200, 20);

	Label* lCameraSmoothOrigin = new Label(this, "", "Origin Smooth Amount (0 = none)");
	lCameraSmoothOrigin->SetBounds(278, 293, 200, 20);

	m_sCCamOrigin = new Slider(this, "");
	m_sCCamOrigin->SetRange(0, 20);
	m_sCCamOrigin->SetValue(0);
	m_sCCamOrigin->SetBounds(270, 314, 200, 20);

	Label* lCameraOO = new Label(this, "", "   Origin Override\n(x y z)(0 0 0 = none)");
	lCameraOO->SetBounds(242, 334, 130, 40);

	m_teXYZ = new TextEntry(this, "");
	m_teXYZ->SetText("0 0 0");
	m_teXYZ->SetBounds(239, 377, 130, 20);

	Label* lCameraAO = new Label(this, "", "   Angles Override\n(p y r)(0 0 0 = none)");
	lCameraAO->SetBounds(375, 334, 130, 40);

	m_tePYR = new TextEntry(this, "");
	m_tePYR->SetText("0 0 0");
	m_tePYR->SetBounds(372, 377, 130, 20);

	Label* lCameraPMin = new Label(this, "", "   Minimum Pitch\n(0 = default)");
	lCameraPMin->SetBounds(247, 393, 145, 40);

	m_sMinAngles = new Slider(this, "");
	m_sMinAngles->SetRange(181, 0);
	m_sMinAngles->SetValue(89);
	m_sMinAngles->SetBounds(240, 429, 130, 20);

	Label* lCameraPMax = new Label(this, "", "   Maximum Pitch\n(0 = default)");
	lCameraPMax->SetBounds(375, 393, 145, 40);

	m_sMaxAngles = new Slider(this, "");
	m_sMaxAngles->SetRange(181, 0);
	m_sMaxAngles->SetValue(89);
	m_sMaxAngles->SetBounds(376, 429, 130, 20);

	Divider* div3 = new Divider(this, "div");
	div3->SetBounds(235, 452, 300, 2);

	Button* LoadPreset = new Button(this, "", "Load Preset");
	LoadPreset->SetCommand("Load");
	LoadPreset->SetBounds(245, 459, 120, 25);

	Button* SavePreset = new Button(this, "", "Save Preset");
	SavePreset->SetCommand("Save");
	SavePreset->SetBounds(375, 459, 120, 25);
}

class CEPanellInterface : public IEffectsPanel
{
private:
	CEffectsPanel* EPanel;
public:
	CEPanellInterface()
	{
		EPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		EPanel = new CEffectsPanel(parent);
	}
	void Destroy()
	{
		if (EPanel)
		{
			EPanel->SetParent((vgui::Panel*)NULL);
			delete EPanel;
		}
	}
	void Activate(void)
	{
		if (EPanel)
		{
			EPanel->Activate();
		}
	}
};
static CEPanellInterface g_EPanel;
IEffectsPanel* effectspanel = (IEffectsPanel*)&g_EPanel;

ConVar cl_draweffectspanel("cl_draweffectspanel", "0", FCVAR_CLIENTDLL, "Sets the state of the alone mod Effects panel");

void CEffectsPanel::OnTick()
{
	BaseClass::OnTick();
	SetVisible(cl_draweffectspanel.GetBool());

	if (!this->IsVisible())
		return;

	static ConVar* mat_yuv = cvar->FindVar("mat_yuv");
	if (mat_yuv)
		mat_yuv->SetValue(m_cbBAWhite->IsSelected());

	amod_view_lense_dirt.SetValue(m_cbLenseDirt->IsSelected());
	amod_view_bodycam.SetValue(m_cbBCam->IsSelected());
	amod_view_binoculars.SetValue(m_cbBinoculars->IsSelected());
	amod_view_blur.SetValue(m_cbCBlur->IsSelected());
	amod_view_square.SetValue(m_cbSquare->IsSelected());
	amod_view_square_width.SetValue((float)(m_sSquare->GetValue()) / 20);
	amod_view_square_height.SetValue((float)(m_sSquareHeight->GetValue()) / 20);
	amod_view_claustrophobia.SetValue(m_cbCPhobia->IsSelected());
	amod_view_claustrophobia_amt.SetValue((float)(m_sCPhobia->GetValue()) / 2);
	amod_camera_cinematic.SetValue(m_cbCCamera->IsSelected());
	amod_camera_cinematic_fix.SetValue(m_cbCCameraFix->IsSelected());
	r_drawviewmodel.SetValue(!m_cbDDVModel->IsSelected());

	bool bState = m_cbCCamera->IsSelected();
	m_cbCCameraFix->SetEnabled(bState);
	m_sCCamAngles->SetEnabled(bState);
	m_sCCamOrigin->SetEnabled(bState);
	m_teXYZ->SetEnabled(bState);
	m_tePYR->SetEnabled(bState);
	m_sMinAngles->SetEnabled(bState);
	m_sMaxAngles->SetEnabled(bState);

	int mval = m_sMinAngles->GetValue() == 181 ? 89 : m_sMinAngles->GetValue();
	int maval = m_sMaxAngles->GetValue() == 181 ? 89 : m_sMaxAngles->GetValue();
	cl_pitchdown.SetValue((bState) ? mval : 89);
	cl_pitchup.SetValue((bState) ? maval : 89);

	if (bState)
	{
		char buf1[128];
		char buf2[128];

		m_teXYZ->GetText(buf1, sizeof(buf1));
		m_tePYR->GetText(buf2, sizeof(buf2));

		amod_view_override_xyz_amt.SetValue(buf1);
		amod_view_override_pyr_amt.SetValue(buf2);
	}

	int cca = m_sCCamAngles->GetValue();
	if (cca != 0)
	{
		amod_camera_cinematic_lag_angles.SetValue(1);
		amod_camera_cinematic_lag_angles_amt.SetValue((float)(0.21 - ((float)cca / 100)));
	}
	else
		amod_camera_cinematic_lag_angles.SetValue(0);

	int cco = m_sCCamOrigin->GetValue();
	if (cco != 0)
	{
		amod_camera_cinematic_lag_origin.SetValue(1);
		amod_camera_cinematic_lag_origin_amt.SetValue((float)(0.21 - ((float)cco / 100)));
	}
	else
		amod_camera_cinematic_lag_origin.SetValue(0);

	FILTERCHECK(1);
	FILTERCHECK(2);
	FILTERCHECK(3);
	FILTERCHECK(4);
	FILTERCHECK(5);
	FILTERCHECK(6);
	FILTERCHECK(7);
	FILTERCHECK(8);

	m_sCFov->SetEnabled(m_cbCPhobia->IsSelected());
	if (m_cbCPhobia->IsSelected())
		fov_desired.SetValue(m_sCFov->GetValue());

	m_sViewmodelFov->SetEnabled(m_cbVFov->IsSelected());
	if (m_cbVFov->IsSelected())
		v_viewmodel_fov.SetValue(m_sViewmodelFov->GetValue());
}

CON_COMMAND_F(ToggleEffectsPanel, "Toggles The Alone Mod Effects Panel", FCVAR_HIDDEN)
{
	cl_draweffectspanel.SetValue(!cl_draweffectspanel.GetBool());
	effectspanel->Activate();
};

void CEffectsPanel::OnClose()
{
	cl_draweffectspanel.SetValue(0);
}

void CEffectsPanel::OnCommand(const char* pcCommand)
{
	BaseClass::OnCommand(pcCommand);
	if (!Q_strcmp(pcCommand, "cphobia"))
	{
		if (!m_cbCPhobia->IsSelected())
		{
			if (fov_desired.GetInt() >= 110)
				fov_desired.SetValue(110);
		}
	}
	if (!Q_strcmp(pcCommand, "vfov"))
	{
		if (!m_cbVFov->IsSelected())
		{
			v_viewmodel_fov.SetValue(54);
		}
	}
	else if (!Q_strcmp(pcCommand, "Load"))
	{
		bWasLoad = true;
		if (!m_FOLoad)
		{
			m_FOLoad = new FileOpenDialog(this, "Load Preset", true);
			m_FOLoad->AddFilter("*.amf", "Amod Filter Files", true);
			m_FOLoad->AddFilter("*.*", "All Files", false);
			m_FOLoad->AddActionSignalTarget(this);
		}
		m_FOLoad->DoModal(false);
		m_FOLoad->Activate();
	}
	else if (!Q_strcmp(pcCommand, "Save"))
	{
		bWasLoad = false;
		if (!m_FSave)
		{
			m_FSave = new FileOpenDialog(this, "Save Preset", false);
			m_FSave->AddFilter("*.amf", "Amod Filter Files (*.amf)", true);
			m_FSave->AddFilter("*.*", "All Files (*.*)", false);
			m_FSave->AddActionSignalTarget(this);
		}
		m_FSave->DoModal(false);
		m_FSave->Activate();
	}
}

void UppercaseLowerCase(const char* input, char** output);

void CEffectsPanel::OnKeyCodePressed(KeyCode code)
{
	const char* bind = engine->Key_LookupBinding("ToggleEffectsPanel");
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
				cl_draweffectspanel.SetValue(0);

			delete (*lcase);
			delete[] lcase;
			lcase = nullptr;
		}
	}
}
void CEffectsPanel::OnFileSelected(const char* pszFileName)
{
	if (!pszFileName || pszFileName[0] == '\0')
		return;

	if (bWasLoad)
	{
		KeyValues* kv = new KeyValues("AmodFilterPreset");
		KeyValues::AutoDelete kad(kv);

		if (!kv->LoadFromFile(filesystem, pszFileName))
			return;

		m_cbBAWhite->SetSelected(kv->GetBool("Filter:BlackAndWhite", false));
		m_cbBCam->SetSelected(kv->GetBool("Filter:BodyCam", false));
		m_cbBinoculars->SetSelected(kv->GetBool("Filter:Binoculars", false));
		m_cbCBlur->SetSelected(kv->GetBool("Filter:Blur", false));
		m_cbLenseDirt->SetSelected(kv->GetBool("Filter:LenseDirt", false));
		m_cbCPhobia->SetSelected(kv->GetBool("Filter:Claustrophobia", false));
		m_sCPhobia->SetValue(kv->GetInt("Filter:ClaustrophobiaAmmount", 4));
		m_sCFov->SetValue(kv->GetInt("Filter:ClaustrophobiaFovOverride", 100));
		m_cbSquare->SetSelected(kv->GetBool("Filter:Square", false));
		m_sSquare->SetValue(kv->GetInt("Filter:SquareWidth", 8));
		m_sSquareHeight->SetValue(kv->GetInt("Filter:SquareHeight", 0));

		m_CBFilter1->SetSelected(kv->GetBool("Filter:Dark:1", false));
		m_CBFilter2->SetSelected(kv->GetBool("Filter:Dark:2", false));
		m_CBFilter3->SetSelected(kv->GetBool("Filter:Dark:3", false));
		m_CBFilter4->SetSelected(kv->GetBool("Filter:Dark:4", false));
		m_CBFilter5->SetSelected(kv->GetBool("Filter:Dark:5", false));
		m_CBFilter6->SetSelected(kv->GetBool("Filter:Dark:6", false));
		m_CBFilter7->SetSelected(kv->GetBool("Filter:Dark:7", false));
		m_CBFilter8->SetSelected(kv->GetBool("Filter:Dark:8", false));
		
		m_cbVFov->SetSelected(kv->GetBool("Camera:ViewmodelFov", false));
		m_sViewmodelFov->SetValue(kv->GetInt("Camera:ViewmodelOverride", 54));

		m_cbCCamera->SetSelected(kv->GetBool("Camera:EnableSmoothing", false));
		m_cbCCameraFix->SetSelected(kv->GetBool("Camera:Viewmodel:Fix", false));
		m_cbDDVModel->SetSelected(kv->GetBool("Camera:Viewmodel:DontDraw", false));
		m_sCCamAngles->SetValue(kv->GetInt("Camera:AngleAmount", 5));
		m_sCCamOrigin->SetValue(kv->GetInt("Camera:OriginAmount", 5));

		m_teXYZ->SetText(kv->GetString("Camera:OriginOverride:Amount", "0 0 0"));
		m_tePYR->SetText(kv->GetString("Camera:AngleOverride:Amount", "0 0 0"));

		m_sMinAngles->SetValue(kv->GetInt("Camera:AngleOverride:Min", 89));
		m_sMaxAngles->SetValue(kv->GetInt("Camera:AngleOverride:Max", 89));
	}
	else
	{
		KeyValues* kv = new KeyValues("AmodFilterPreset");
		KeyValues::AutoDelete kad(kv);

		kv->SetBool("Filter:BlackAndWhite", m_cbBAWhite->IsSelected());
		kv->SetBool("Filter:BodyCam", m_cbBCam->IsSelected());
		kv->SetBool("Filter:Binoculars", m_cbBinoculars->IsSelected());
		kv->SetBool("Filter:Blur", m_cbCBlur->IsSelected());
		kv->SetBool("Filter:LenseDirt", m_cbLenseDirt->IsSelected());
		kv->SetBool("Filter:Claustrophobia", m_cbCPhobia->IsSelected());
		kv->SetInt("Filter:ClaustrophobiaAmmount", m_sCPhobia->GetValue());
		kv->SetInt("Filter:ClaustrophobiaFovOverride", m_sCFov->GetValue());
		kv->SetBool("Filter:Square", m_cbSquare->IsSelected());
		kv->SetInt("Filter:SquareWidth", m_sSquare->GetValue());
		kv->SetInt("Filter:SquareHeight", m_sSquareHeight->GetValue());

		kv->SetBool("Filter:Dark:1", m_CBFilter1->IsSelected());
		kv->SetBool("Filter:Dark:2", m_CBFilter2->IsSelected());
		kv->SetBool("Filter:Dark:3", m_CBFilter3->IsSelected());
		kv->SetBool("Filter:Dark:4", m_CBFilter4->IsSelected());
		kv->SetBool("Filter:Dark:5", m_CBFilter5->IsSelected());
		kv->SetBool("Filter:Dark:6", m_CBFilter6->IsSelected());
		kv->SetBool("Filter:Dark:7", m_CBFilter7->IsSelected());
		kv->SetBool("Filter:Dark:8", m_CBFilter8->IsSelected());

		kv->SetBool("Camera:ViewmodelFov", m_cbVFov->IsSelected());
		kv->SetInt("Camera:ViewmodelOverride", m_sViewmodelFov->GetValue());

		kv->SetBool("Camera:EnableSmoothing", m_cbCCamera->IsSelected());
		kv->SetBool("Camera:Viewmodel:Fix", m_cbCCameraFix->IsSelected());
		kv->SetBool("Camera:Viewmodel:DontDraw", m_cbDDVModel->IsSelected());
		kv->SetInt("Camera:AngleAmount", m_sCCamAngles->GetValue());
		kv->SetInt("Camera:OriginAmount", m_sCCamOrigin->GetValue());

		char buf1[128];
		char buf2[128];

		m_teXYZ->GetText(buf1, sizeof(buf1));
		m_tePYR->GetText(buf2, sizeof(buf2));

		kv->SetString("Camera:OriginOverride:Amount", buf1);
		kv->SetString("Camera:AngleOverride:Amount", buf2);
		
		kv->SetInt("Camera:AngleOverride:Min", m_sMinAngles->GetValue());
		kv->SetInt("Camera:AngleOverride:Max", m_sMaxAngles->GetValue());

		kv->SaveToFile(filesystem, pszFileName);
	}
}