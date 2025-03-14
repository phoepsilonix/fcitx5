/*
 * SPDX-FileCopyrightText: 2016-2016 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#ifndef _FCITX_UTILS_DBUS_OBJECTVTABLE_P_SDBUS_H_
#define _FCITX_UTILS_DBUS_OBJECTVTABLE_P_SDBUS_H_

#include <map>
#include <memory>
#include <string>
#include "../objectvtable.h"
#include "sd-bus-wrap.h"

namespace fcitx::dbus {
class SDVTableSlot;

class ObjectVTableBasePrivate {
public:
    ~ObjectVTableBasePrivate();

    const sd_bus_vtable *toSDBusVTable(ObjectVTableBase *q);

    std::map<std::string, ObjectVTableMethod *> methods_;
    std::map<std::string, ObjectVTableProperty *> properties_;
    std::map<std::string, ObjectVTableSignal *> sigs_;
    std::unique_ptr<SDVTableSlot> slot_;
    Message *msg_ = nullptr;
};

} // namespace fcitx::dbus

#endif // _FCITX_UTILS_DBUS_OBJECTVTABLE_SDBUS_P_H_
