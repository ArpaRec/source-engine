#ifndef __C_AMOD_ENTIEIS_H
#define __C_AMOD_ENTIEIS_H

//so its easier im making this a define
#define DLIGHT_ENTITY(classname, dtname, servername, radmin, radmax, cr, cg, cb) \
class classname : public C_BaseAnimating                                      \
{                                                                             \
public:                                                                       \
    DECLARE_CLASS(classname, C_BaseAnimating);                                \
    DECLARE_CLIENTCLASS();                                                    \
                                                                              \
    void Simulate();                                                          \
};                                                                            \
                                                                              \
IMPLEMENT_CLIENTCLASS_DT(classname, dtname, servername)                       \
END_RECV_TABLE();                                                             \
                                                                              \
void classname::Simulate()                                                    \
{                                                                             \
    dlight_t* el = effects->CL_AllocDlight(LIGHT_INDEX_TE_DYNAMIC + index);   \
    el->origin = GetRenderOrigin();                                           \
    el->radius = random->RandomInt(radmin, radmax);                           \
    el->decay = el->radius / 0.35f;                                           \
    el->die = gpGlobals->curtime + 0.25f;                                     \
    el->color.r = cr;                                                         \
    el->color.g = cg;                                                         \
    el->color.b = cb;                                                         \
    el->color.exponent = 3;                                                   \
}																	  

#endif