#pragma once

#include <Dolphin/types.h>
#include <SMS/GC2D/GCConsole2.hxx>
#include <SMS/System/MarDirector.hxx>

class AtomBalloonMessageViewer {
public:
    AtomBalloonMessageViewer() = delete;
    AtomBalloonMessageViewer(u32 messageID, s32 duration)
        : m_message_id(messageID), m_duration(duration), m_is_used(false) {}

    u32 id() const { return m_message_id; }
    s32 duration() const { return m_duration; }

    void reset() { m_is_used = false; }

    void show() {
        if (m_is_used) {
            return;
        }
        pushMessage(this);
        m_is_used = true;
    }

protected:
    friend void checkBalloonMessagePool(TMarDirector *director);

    static void pushMessage(AtomBalloonMessageViewer *message);
    static AtomBalloonMessageViewer *peekMessage();
    static void popMessage();

private:
    u32 m_message_id;
    s32 m_duration;
    bool m_is_used;
};