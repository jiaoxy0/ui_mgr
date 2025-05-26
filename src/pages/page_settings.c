#include "lvgl.h"
#include "ui_mgr.h"
#include "page_settings.h"

LV_FONT_DECLARE(lv_font_source_han_sans_medium_20);

#define SETTINGS_PAGE_NOTIFY_BAR_FROM_TOP   10

// static lv_style_t style_btn;
static lv_style_t style_lable_notify_bar;
static lv_style_t style_roller;
static lv_style_t style_roller_sel;
// static lv_style_t style_setings_selections;

// static void button_event_cb(lv_event_t *e) {
//     lv_obj_t *button = lv_event_get_target(e);
//     // 处理按钮点击事件
//     ui_mgr_switch_to_page(UI_MGR_PAGE_MAIN); // 切换到设置页面
// }

static void roller_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *roller = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED)
    {
        LV_LOG_USER("%d", lv_roller_get_selected(roller));             // 获取被选中的选项的索引值，从0开始
        char tmp_buf[8];
        lv_roller_get_selected_str(roller, tmp_buf, sizeof(tmp_buf));  // 获取被选中的选项的内容
        LV_LOG_USER("%s", tmp_buf);
    }
    else if(code == LV_EVENT_CLICKED)
    {
        ui_mgr_switch_to_page(UI_MGR_PAGE_MAIN); // 切换到设置页面
    }
}

void page_settings_init(lv_obj_t *root) {
    lv_style_init(&style_lable_notify_bar);
    lv_style_set_text_font(&style_lable_notify_bar, &lv_font_source_han_sans_medium_20);
    // lv_style_init(&style_setings_selections);
    lv_style_init(&style_roller);
    lv_style_set_bg_color(&style_roller, lv_color_make(32, 32, 32));
    lv_style_set_text_color(&style_roller, lv_color_make(148, 148, 148));
    lv_style_set_text_font(&style_roller, &lv_font_source_han_sans_medium_20);
    lv_style_init(&style_roller_sel);
    lv_style_set_bg_color(&style_roller_sel, lv_color_make(209, 16, 59));
    lv_style_set_text_color(&style_roller_sel, lv_color_make(255, 255, 255));
    lv_style_set_text_font(&style_roller_sel, &lv_font_source_han_sans_medium_20);

    /* Notify bar left */
    lv_obj_t *label_title = lv_label_create(root);
    lv_label_set_text(label_title, "设置");
    lv_obj_add_style(label_title, &style_lable_notify_bar, 0);
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, SETTINGS_PAGE_NOTIFY_BAR_FROM_TOP);

    lv_obj_t * roller = lv_roller_create(root);    // 创建一个 lv_roller 部件(对象),他的父对象是活动屏幕对象
    lv_roller_set_options(
        roller, 
        LV_SYMBOL_WIFI "  WLAN\n" 
        LV_SYMBOL_BLUETOOTH "  蓝牙\n" 
        LV_SYMBOL_SD_CARD "  存储\n" 
        LV_SYMBOL_COPY "  其他\n" 
        LV_SYMBOL_REFRESH "  ...\n" 
        LV_SYMBOL_REFRESH "  ...\n" 
        LV_SYMBOL_REFRESH "  ...\n" 
        LV_SYMBOL_REFRESH "  ...\n", 
        LV_ROLLER_MODE_NORMAL
    );

    lv_roller_set_visible_row_count(roller, 6);

    lv_obj_set_size(roller, 280, 160);
    lv_obj_align(roller, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(roller, &style_roller, 0);
    lv_obj_add_style(roller, &style_roller_sel, LV_PART_SELECTED);
    // lv_obj_set_style_bg_color(roller, lv_color_hex(0x15528e), 0);
    // lv_obj_set_style_bg_color(roller, lv_color_hex(0x000000), LV_PART_SELECTED);
    // lv_obj_set_style_text_color(roller, lv_color_hex(0xffffff), 0);
    // lv_obj_set_style_text_color(roller, lv_color_hex(0xff3272), LV_PART_SELECTED);

    lv_obj_add_event_cb(roller, roller_event_handler, LV_EVENT_ALL, NULL);

    // lv_style_init(&style_btn);
    // lv_style_set_size(&style_btn, 64, 32);
    // lv_style_set_bg_color(&style_btn, lv_color_make(233, 84, 32));
    
    // lv_obj_t *label = lv_label_create(root);
    // lv_label_set_text(label, "Settings Page");
    // lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    // lv_obj_t *button = lv_button_create(root);
    // lv_obj_align(button, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_add_style(button, &style_btn, 0);
    // lv_obj_add_event_cb(button, button_event_cb, LV_EVENT_CLICKED, NULL);
}
