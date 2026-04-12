#pragma once

#include <SMS/MapObj/MapObjGeneral.hxx>
#include <BetterSMS/module.hxx>

class TEMarioPortal : public TMapObjGeneral {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TEMarioPortal("TEMarioPortal");
    }

    TEMarioPortal(const char *name);
    ~TEMarioPortal() override = default;

    void linkTo(TEMarioPortal *other) { m_linked_portal = other; }
    void openPortal(const TVec3f &at, const TVec3f &look_nrm);
    void closePortal();

    void control() override;
    void initMapObj() override;
    bool receiveMessage(THitActor *sender, u32 message) override;

protected:
    void transportActor(TLiveActor *actor);
    void transportPlayer(TMario *player);

private:
    bool m_is_closed;
    bool m_is_closing;
    bool m_is_opening;

    TVec3f m_init_scale;
    TVec3f m_target_scale;
    TVec3f m_look_nrm;

    float m_interp;

    TEMarioPortal *m_linked_portal;
};

extern ObjData emarioPortalData;