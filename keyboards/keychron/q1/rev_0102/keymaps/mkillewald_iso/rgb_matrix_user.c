/* Copyright 2021 @ Mike Killewald
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "rgb_matrix_user.h"
#include "keymap_user.h"

//extern rgb_config_t rgblight_config;

bool is_arrow_key(uint16_t keycode);
bool is_special_key(uint16_t keycode);
bool is_modifier_key(uint16_t keycode);
bool is_spacebar(uint16_t keycode);

keypos_t led_index_key_position[DRIVER_LED_TOTAL];

void rgb_matrix_init_user(void) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led_index = g_led_config.matrix_co[row][col];
            if (led_index != NO_LED) {
                led_index_key_position[led_index] = (keypos_t){.row = row, .col = col};
            }
        }
    }
}

void rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t current_layer = get_highest_layer(layer_state);

    if(get_arrow_color_enable())
    {
        HSV hsv = rgb_matrix_get_hsv();
        hsv.h += get_arrow_hueshift();
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color_by_keycode(led_min, led_max, WIN_BASE, is_arrow_key, rgb.r, rgb.g, rgb.b);
    }
    if(get_special_color_enable())
    {
        HSV hsv = rgb_matrix_get_hsv();
        hsv.h += get_special_hueshift();
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color_by_keycode(led_min, led_max, WIN_BASE, is_special_key, rgb.r, rgb.g, rgb.b);
    }
    if(get_modifier_color_enable())
    {
        HSV hsv = rgb_matrix_get_hsv();
        hsv.h += get_modifier_hueshift();
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color_by_keycode(led_min, led_max, WIN_BASE, is_modifier_key, rgb.r, rgb.g, rgb.b);
    }

    switch (current_layer) {
        case MAC_BASE:
        case WIN_BASE:
#ifdef CAPS_LOCK_INDICATOR_COLOR
            if (host_keyboard_led_state().caps_lock) {
                rgb_matrix_set_color_by_keycode(led_min, led_max, current_layer, is_caps_lock_indicator, CAPS_LOCK_INDICATOR_COLOR);
            }
#endif
            if(!get_space_illum_enable())
                rgb_matrix_set_color_by_keycode(led_min, led_max, current_layer, is_spacebar, RGB_OFF);

            break;
        case MAC_FN:
        case WIN_FN:
#ifdef FN_LAYER_COLOR
            if (get_fn_layer_color_enable()) {
                rgb_matrix_set_color_by_keycode(led_min, led_max, current_layer, is_not_transparent, FN_LAYER_COLOR);
            }
#endif
            if (get_fn_layer_transparent_keys_off()) {
                rgb_matrix_set_color_by_keycode(led_min, led_max, current_layer, is_transparent, RGB_OFF);
            }
            break;
    }
}

void rgb_matrix_set_color_by_keycode(uint8_t led_min, uint8_t led_max, uint8_t layer, bool (*is_keycode)(uint16_t), uint8_t red, uint8_t green, uint8_t blue) {
    for (uint8_t i = led_min; i < led_max; i++) {
        uint16_t keycode = keymap_key_to_keycode(layer, led_index_key_position[i]);
        if ((*is_keycode)(keycode)) {
            rgb_matrix_set_color(i, red, green, blue);
        }
    }
}

bool is_caps_lock_indicator(uint16_t keycode) {
    bool indicator = keycode == KC_CAPS;
    
    if (get_caps_lock_light_tab()) {
        indicator = keycode == KC_TAB || keycode == KC_CAPS;
    }
    
    if (get_caps_lock_light_alphas()) {
        return (KC_A <= keycode && keycode <= KC_Z) || indicator;
    } else {
        return indicator;
    }
}

bool is_transparent(uint16_t keycode) { return keycode == KC_TRNS; }
bool is_not_transparent(uint16_t keycode) { return keycode != KC_TRNS; }

bool is_arrow_key(uint16_t keycode)
{
     return ( keycode == KC_UP || keycode == KC_DOWN || keycode == KC_LEFT || keycode == KC_RGHT );
}

bool is_special_key(uint16_t keycode)
{
    return keycode == KC_HOME || keycode == KC_END || keycode == KC_DELETE || keycode == KC_PGUP || keycode == KC_PGDN;
}

bool is_modifier_key(uint16_t keycode)
{
    return keycode == KC_TAB || keycode == KC_CAPS || keycode == KC_LSFT || keycode == KC_LCTL || keycode == KC_RCTL || keycode == KC_LALT || keycode == KC_LGUI || keycode == KC_RALT || keycode == WIN_FN || keycode == KC_RSFT;
}

bool is_spacebar(uint16_t keycode)
{
    return keycode == KC_SPC;
}