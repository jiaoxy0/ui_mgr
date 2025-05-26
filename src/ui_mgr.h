#ifndef _UI_MGR_H
#define _UI_MGR_H

#include "lvgl.h"

#define UI_MGR_SWITCH_PAGE_DBG      0

#define UI_MGR_MAX_ALIVE_PAGES      2 // 最大活跃页面数量，为0时不做限制
#define UI_MGR_MEMORY_THRESHOLD     0  // 内存占用阈值（百分比），为0时不做限制
#if UI_MGR_MAX_ALIVE_PAGES || UI_MGR_MEMORY_THRESHOLD
#include "my_queue.h"
#endif

typedef enum {
    UI_MGR_PAGE_NULL = -1,

    // PAGE_LOGO,
    UI_MGR_PAGE_MAIN,
    UI_MGR_PAGE_SETTINGS,
    UI_MGR_PAGE_IMAGE,

    UI_MGR_PAGE_TOTAL, // Total number of pages
}ui_mgr_page_id_t;

typedef void (*page_init_cb_t)(lv_obj_t *root);
typedef void (*page_destroy_cb_t)(lv_obj_t *root);
typedef struct {
    ui_mgr_page_id_t page_id;
    lv_obj_t *root;
    page_init_cb_t page_init_cb; // The callback function to init all widgets of a page whose parent is root
    page_destroy_cb_t page_destroy_cb;
}ui_mgr_page_t;

typedef struct {
    ui_mgr_page_t *current_page;
    lv_group_t *input_group;
#if UI_MGR_MAX_ALIVE_PAGES || UI_MGR_MEMORY_THRESHOLD
    my_queue_t alive_pages_queue;
#endif
}ui_mgr_t;

/**
 * @brief Switch to a page
 */
void ui_mgr_switch_to_page(ui_mgr_page_id_t page_id);

/**
 * @brief Initialize the UI manager, whitch include initializing default group and creating all pages
 */
int ui_mgr_init(void);

#endif /* _UI_MGR_H */
