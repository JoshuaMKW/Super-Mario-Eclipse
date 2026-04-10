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

    void openPortal(const TVec3f &at, const TVec3f &look_nrm);
    void closePortal();

    void control() override;
    void initMapObj() override;
    bool receiveMessage(THitActor *sender, u32 message) override;

protected:
    TBGCheckData *getNearestSurface(const TVec3f &at, const TVec3f &look_nrm);
    TVec3f getRotationFromNormal(const TVec3f &nrm);

private:
    bool m_is_closed;
    bool m_is_closing;
    bool m_is_opening;
};

extern ObjData emarioPortalData;