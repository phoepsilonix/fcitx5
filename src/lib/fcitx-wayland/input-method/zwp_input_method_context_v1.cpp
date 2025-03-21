#include "zwp_input_method_context_v1.h"
#include <cassert>
#include "wayland-input-method-unstable-v1-client-protocol.h"
#include "wl_keyboard.h"

namespace fcitx::wayland {
const struct zwp_input_method_context_v1_listener
    ZwpInputMethodContextV1::listener = {
        .surrounding_text =
            [](void *data, zwp_input_method_context_v1 *wldata,
               const char *text, uint32_t cursor, uint32_t anchor) {
                auto *obj = static_cast<ZwpInputMethodContextV1 *>(data);
                assert(*obj == wldata);
                {
                    obj->surroundingText()(text, cursor, anchor);
                }
            },
        .reset =
            [](void *data, zwp_input_method_context_v1 *wldata) {
                auto *obj = static_cast<ZwpInputMethodContextV1 *>(data);
                assert(*obj == wldata);
                {
                    obj->reset()();
                }
            },
        .content_type =
            [](void *data, zwp_input_method_context_v1 *wldata, uint32_t hint,
               uint32_t purpose) {
                auto *obj = static_cast<ZwpInputMethodContextV1 *>(data);
                assert(*obj == wldata);
                {
                    obj->contentType()(hint, purpose);
                }
            },
        .invoke_action =
            [](void *data, zwp_input_method_context_v1 *wldata, uint32_t button,
               uint32_t index) {
                auto *obj = static_cast<ZwpInputMethodContextV1 *>(data);
                assert(*obj == wldata);
                {
                    obj->invokeAction()(button, index);
                }
            },
        .commit_state =
            [](void *data, zwp_input_method_context_v1 *wldata,
               uint32_t serial) {
                auto *obj = static_cast<ZwpInputMethodContextV1 *>(data);
                assert(*obj == wldata);
                {
                    obj->commitState()(serial);
                }
            },
        .preferred_language =
            [](void *data, zwp_input_method_context_v1 *wldata,
               const char *language) {
                auto *obj = static_cast<ZwpInputMethodContextV1 *>(data);
                assert(*obj == wldata);
                {
                    obj->preferredLanguage()(language);
                }
            },
};

ZwpInputMethodContextV1::ZwpInputMethodContextV1(
    zwp_input_method_context_v1 *data)
    : version_(zwp_input_method_context_v1_get_version(data)), data_(data) {
    zwp_input_method_context_v1_set_user_data(*this, this);
    zwp_input_method_context_v1_add_listener(
        *this, &ZwpInputMethodContextV1::listener, this);
}

void ZwpInputMethodContextV1::destructor(zwp_input_method_context_v1 *data) {
    const auto version = zwp_input_method_context_v1_get_version(data);
    if (version >= 1) {
        zwp_input_method_context_v1_destroy(data);
        return;
    }
}
void ZwpInputMethodContextV1::commitString(uint32_t serial, const char *text) {
    zwp_input_method_context_v1_commit_string(*this, serial, text);
}
void ZwpInputMethodContextV1::preeditString(uint32_t serial, const char *text,
                                            const char *commit) {
    zwp_input_method_context_v1_preedit_string(*this, serial, text, commit);
}
void ZwpInputMethodContextV1::preeditStyling(uint32_t index, uint32_t length,
                                             uint32_t style) {
    zwp_input_method_context_v1_preedit_styling(*this, index, length, style);
}
void ZwpInputMethodContextV1::preeditCursor(int32_t index) {
    zwp_input_method_context_v1_preedit_cursor(*this, index);
}
void ZwpInputMethodContextV1::deleteSurroundingText(int32_t index,
                                                    uint32_t length) {
    zwp_input_method_context_v1_delete_surrounding_text(*this, index, length);
}
void ZwpInputMethodContextV1::cursorPosition(int32_t index, int32_t anchor) {
    zwp_input_method_context_v1_cursor_position(*this, index, anchor);
}
void ZwpInputMethodContextV1::modifiersMap(wl_array *map) {
    zwp_input_method_context_v1_modifiers_map(*this, map);
}
void ZwpInputMethodContextV1::keysym(uint32_t serial, uint32_t time,
                                     uint32_t sym, uint32_t state,
                                     uint32_t modifiers) {
    zwp_input_method_context_v1_keysym(*this, serial, time, sym, state,
                                       modifiers);
}
WlKeyboard *ZwpInputMethodContextV1::grabKeyboard() {
    return new WlKeyboard(zwp_input_method_context_v1_grab_keyboard(*this));
}
void ZwpInputMethodContextV1::key(uint32_t serial, uint32_t time, uint32_t key,
                                  uint32_t state) {
    zwp_input_method_context_v1_key(*this, serial, time, key, state);
}
void ZwpInputMethodContextV1::modifiers(uint32_t serial, uint32_t modsDepressed,
                                        uint32_t modsLatched,
                                        uint32_t modsLocked, uint32_t group) {
    zwp_input_method_context_v1_modifiers(*this, serial, modsDepressed,
                                          modsLatched, modsLocked, group);
}
void ZwpInputMethodContextV1::language(uint32_t serial, const char *language) {
    zwp_input_method_context_v1_language(*this, serial, language);
}
void ZwpInputMethodContextV1::textDirection(uint32_t serial,
                                            uint32_t direction) {
    zwp_input_method_context_v1_text_direction(*this, serial, direction);
}

} // namespace fcitx::wayland
