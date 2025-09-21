class SongPanel
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
	virtual void		Activate(void) = 0;
	virtual void		OnLevelChange(void) = 0;
	virtual void		OnLevelShutdown(void) = 0;
	virtual void		OnNewLevel(void) = 0;
};

extern SongPanel* songpanel;