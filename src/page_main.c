#include "lvgl.h"
#include "ui_mgr.h"
#include "page_main.h"

#define MAIN_PAGE_NOTIFY_BAR_FROM_TOP       10
#define MAIN_PAGE_NOTIFY_BAR_FROM_L         5

// static lv_style_t style_btn;
static lv_style_t style_lable_notify_bar;
static lv_style_t style_btnm_icons;

static uint32_t last_pressed_id = 65535;

// static void button_event_cb(lv_event_t *e) {
//     lv_obj_t *button = lv_event_get_target(e);
//     // 处理按钮点击事件
//     ui_mgr_switch_to_page(UI_MGR_PAGE_SETTINGS); // 切换到设置页面
// }

static void btnm_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        // 获取当前被点击的按钮的ID
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        // 根据按钮ID获取对应按钮上的文本
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);

        LV_LOG_USER("(%d, %s) was pressed\n", id, txt);

        // 保存当前选中的按钮ID
        last_pressed_id = id;
    }
    else if(code == LV_EVENT_CLICKED) {
        // 使用保存的按钮ID
        const char * txt = lv_btnmatrix_get_btn_text(obj, last_pressed_id);

        LV_LOG_USER("(%d, %s) was clicked\n", last_pressed_id, txt);

        if(last_pressed_id == 0) {
            ui_mgr_switch_to_page(UI_MGR_PAGE_SETTINGS); // 切换到设置页面
        } else if(last_pressed_id == 1) {
            ui_mgr_switch_to_page(UI_MGR_PAGE_IMAGE); // 切换到设置页面
        }
    }
}

void page_main_init(lv_obj_t *root) {
    lv_style_init(&style_lable_notify_bar);
    lv_style_set_text_font(&style_lable_notify_bar, &lv_font_montserrat_20);
    lv_style_init(&style_btnm_icons);
    lv_style_set_text_font(&style_btnm_icons, &lv_font_montserrat_28);
    // lv_style_set_bg_color(&style_btnm_icons, lv_color_make(0, 120, 212));
    lv_style_set_bg_color(&style_btnm_icons, lv_color_make(255, 207, 74));

    /* Notify bar left */
    lv_obj_t *label_notify_bar_l = lv_label_create(root);
    // lv_label_set_recolor(label_notify_bar_l, true);
    // lv_label_set_text(label_notify_bar_l, "13:45:#ff0000 02#");
    lv_label_set_text(label_notify_bar_l, "13:45");
    lv_obj_add_style(label_notify_bar_l, &style_lable_notify_bar, 0);
    lv_obj_align(label_notify_bar_l, LV_ALIGN_TOP_LEFT, MAIN_PAGE_NOTIFY_BAR_FROM_L, MAIN_PAGE_NOTIFY_BAR_FROM_TOP);

    /* Notify bar right */
    lv_obj_t *label_notify_bar_r = lv_label_create(root);
    lv_label_set_text(label_notify_bar_r, LV_SYMBOL_WIFI " " LV_SYMBOL_UPLOAD " " LV_SYMBOL_VOLUME_MAX " " LV_SYMBOL_BATTERY_3 " " LV_SYMBOL_CHARGE);
    lv_obj_add_style(label_notify_bar_r, &style_lable_notify_bar, 0);
    lv_obj_align(label_notify_bar_r, LV_ALIGN_TOP_RIGHT, -MAIN_PAGE_NOTIFY_BAR_FROM_L, MAIN_PAGE_NOTIFY_BAR_FROM_TOP);

    /* Icons */
    // static const char * map[] = {"#7fba00 " LV_SYMBOL_SETTINGS "#", "#ffb900 " LV_SYMBOL_IMAGE "#", NULL};
    static const char * map[] = {LV_SYMBOL_SETTINGS, LV_SYMBOL_IMAGE, NULL};
    // static lv_btnmatrix_ctrl_t ctrl_map[] = {LV_BTNMATRIX_CTRL_RECOLOR, LV_BTNMATRIX_CTRL_RECOLOR};

    lv_obj_t * btnm = lv_btnmatrix_create(root);
    lv_btnmatrix_set_map(btnm, map);
    // lv_btnmatrix_set_ctrl_map(btnm, ctrl_map);
    lv_obj_add_style(btnm, &style_btnm_icons, LV_PART_ITEMS);
    lv_obj_align(btnm, LV_ALIGN_CENTER, 0, 0);
    // lv_btnmatrix_set_btn_ctrl_all(btnm, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_one_checked(btnm, true); // 允许单选
    lv_obj_add_event_cb(btnm, btnm_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(btnm, btnm_event_handler, LV_EVENT_CLICKED, NULL);
    
    // lv_style_init(&style_btn);
    // lv_style_set_size(&style_btn, 64, 32);

    // lv_obj_t *label = lv_label_create(root);
    // lv_label_set_text(label, "Main Page");
    // lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    // lv_obj_t *button = lv_button_create(root);
    // lv_obj_align(button, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_add_style(button, &style_btn, 0);
    // lv_obj_add_event_cb(button, button_event_cb, LV_EVENT_CLICKED, NULL);
}

void page_main_destroy(lv_obj_t *root) {
    LV_LOG_USER("%s called\n", __func__);
}
