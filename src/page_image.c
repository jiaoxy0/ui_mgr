#include "lvgl.h"
#include "ui_mgr.h"
#include "page_image.h"

// LV_FONT_DECLARE(lv_font_source_han_sans_medium_20);
LV_IMAGE_DECLARE(weixin_20250423064936_18);

static lv_style_t style_btn;

static void button_event_cb(lv_event_t *e) {
    lv_obj_t *button = lv_event_get_target(e);
    // 处理按钮点击事件
    ui_mgr_switch_to_page(UI_MGR_PAGE_MAIN); // 切换到设置页面
}

void page_image_init(lv_obj_t *root) {
    lv_style_init(&style_btn);
    lv_style_set_size(&style_btn, 64, 32);
    lv_style_set_bg_color(&style_btn, lv_color_make(31, 31, 31));

    lv_obj_t *img1 = lv_image_create(root);
    lv_image_set_src(img1, &weixin_20250423064936_18);
    lv_image_set_scale(img1, LV_SCALE_NONE * 4);
    lv_obj_center(img1);

    lv_obj_t *button = lv_button_create(root);
    lv_obj_align(button, LV_ALIGN_TOP_LEFT, 10, 10);
    lv_obj_add_style(button, &style_btn, 0);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, LV_SYMBOL_LEFT);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(button, button_event_cb, LV_EVENT_CLICKED, NULL);
}
