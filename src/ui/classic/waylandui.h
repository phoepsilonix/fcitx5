/*
 * SPDX-FileCopyrightText: 2016-2016 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UI_CLASSIC_WAYLANDUI_H_
#define _FCITX_UI_CLASSIC_WAYLANDUI_H_

#include <memory>
#include <string>
#include "fcitx-utils/eventloopinterface.h"
#include "fcitx-utils/signals.h"
#include "fcitx/userinterface.h"
#include "classicui.h"
#include "display.h"
#include "waylandcursortheme.h"
#include "waylandpointer.h"

namespace fcitx::classicui {

class WaylandWindow;
class WaylandInputWindow;

class WaylandUI : public UIInterface {
public:
    WaylandUI(ClassicUI *parent, const std::string &name, wl_display *display);
    ~WaylandUI();

    ClassicUI *parent() const { return parent_; }
    wayland::Display *display() const { return display_; }
    void update(UserInterfaceComponent component,
                InputContext *inputContext) override;
    void suspend() override;
    void resume() override;
    void setEnableTray(bool /*unused*/) override {}

    std::unique_ptr<WaylandWindow> newWindow();

    WaylandCursorTheme *cursorTheme() { return cursorTheme_.get(); }

private:
    void setupInputWindow();

    ClassicUI *parent_;
    wayland::Display *display_;
    ScopedConnection panelConn_, panelRemovedConn_;
    std::unique_ptr<WaylandCursorTheme> cursorTheme_;
    std::unique_ptr<WaylandPointer> pointer_;
    std::unique_ptr<WaylandInputWindow> inputWindow_;
    std::unique_ptr<EventSource> defer_;
};

} // namespace fcitx::classicui

#endif // _FCITX_UI_CLASSIC_WAYLANDUI_H_
