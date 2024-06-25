#include <SMS/NPC/NpcBase.hxx>
#include <BetterSMS/module.hxx>
#include <Dolphin/types.h>


class TMafiaPianta : public TBaseNPC {
 
 public:
    
    TMafiaPianta(const char *name);
    ~TMafiaPianta() override;

    void control() override;
    void loadAfter() override;
    void perform(u32 flags, JDrama::TGraphics *graphics) override;
    bool receiveMessage(TActor *sender, u32 message) override;

 
};