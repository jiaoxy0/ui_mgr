#include "ui_mgr.h"
// #include "page_logo.h"
#include "page_main.h"
#include "page_settings.h"
#include "page_image.h"

#define METHOD_RECURSIVE    1
#define METHOD_LOOP         0
#if METHOD_RECURSIVE && METHOD_LOOP
#error "METHOD_RECURSIVE and METHOD_LOOP can not be open at same time"
#endif

// #define UI_MGR_PAGE_TOTAL (sizeof(pages) / sizeof(pages[0]))

extern lv_indev_t * indev_keypad;

static ui_mgr_t ui_mgr = {
    .current_page = NULL,
    .input_group = NULL
};

// Array of pages
static ui_mgr_page_t pages[] = {
    // { PAGE_LOGO, NULL, page_logo_init },
    { UI_MGR_PAGE_MAIN, NULL, page_main_init, page_main_destroy },
    { UI_MGR_PAGE_SETTINGS, NULL, page_settings_init, NULL },
    { UI_MGR_PAGE_IMAGE, NULL, page_image_init, NULL },
};

/**
 * @brief Get current page of UI manager
 * @return the pointer of current page
 */
static ui_mgr_page_t *ui_mgr_get_current_page() {
    return ui_mgr.current_page;
}

/**
 * @brief Find page by page ID
 */
static ui_mgr_page_t *ui_mgr_find_page_by_id(ui_mgr_page_id_t page_id) {
    if((page_id < 0) || (page_id >= UI_MGR_PAGE_TOTAL)) return NULL;

    return &pages[page_id];
}

#if UI_MGR_MEMORY_THRESHOLD
/**
 * @brief Check current memory usage
 * @return true if memory usage is below threshold, false otherwise
 */
static bool ui_mgr_check_memory_usage(void) {
    uint32_t free_memory = lv_mem_get_free();
    uint32_t total_memory = lv_mem_get_size();
    uint32_t used_memory = total_memory - free_memory;
    uint32_t usage_percentage = (used_memory * 100) / total_memory;

    return usage_percentage < UI_MGR_MEMORY_THRESHOLD;
}
#endif

#if UI_MGR_MAX_ALIVE_PAGES || UI_MGR_MEMORY_THRESHOLD
/**
 * @brief Find the oldest page from queue and delete it
 * @return 0 Success
 *          -2 Queue API error
 *          -3 ui_mgr_find_page_by_id error
 *          -4 Already deleted
 */
static int ui_mgr_delete_oldest_page(void) {
    int err;

    int oldest_id = my_queue_peek(&ui_mgr.alive_pages_queue, &err);
    if(err) {
        LV_LOG_USER("my_queue_peek err: %d\n", err);
        return -2;
    }

    ui_mgr_page_t *oldest_page = ui_mgr_find_page_by_id(oldest_id);
    if(oldest_page != NULL) {
        if(oldest_page->root != NULL) {
            /* Hide the oldest page before delete */
            lv_obj_add_flag(oldest_page->root, LV_OBJ_FLAG_HIDDEN);
            
            /* Remove all objs from default group before delete */
            lv_group_t *g = lv_indev_get_group(indev_keypad);
            if(g) lv_group_remove_all_objs(g);
            
            /* Delete the oldest page */
            if(oldest_page->page_destroy_cb != NULL) oldest_page->page_destroy_cb(oldest_page->root); // 调用回调函数
            lv_obj_del(oldest_page->root);  // 删除页面对象
            // lv_obj_del_async(oldest_page->root);
            oldest_page->root = NULL;      // 清空页面对象指针

            /* Dequeue */
            my_queue_dequeue(&ui_mgr.alive_pages_queue, &err);
            if(err) {
                LV_LOG_USER("my_queue_dequeue err: %d\n", err);
                return -2;
            }
            LV_LOG_USER("Delete page %d\n", oldest_id);

            return 0;
        } else {
            return -4;
        }
    } else {
        return -3;
    }
}
#endif

/**
 * @brief Create a new page
 */
static ui_mgr_page_t *ui_mgr_create_page(ui_mgr_page_id_t page_id) {
    ui_mgr_page_t *page = ui_mgr_find_page_by_id(page_id);
    if(page == NULL) return NULL;

    /* To insure that the page only be create once */
    if(page->root == NULL) {
#if UI_MGR_MAX_ALIVE_PAGES || UI_MGR_MEMORY_THRESHOLD
        /* Delete the oldest page if queue is full */
        int err;

        int now_size = my_queue_get_size(&ui_mgr.alive_pages_queue, &err);
        if(err) {
            LV_LOG_USER("my_queue_get_size err: %d\n", err);
            return NULL;
        }
#if UI_MGR_MAX_ALIVE_PAGES && UI_MGR_MEMORY_THRESHOLD
        while((now_size >= UI_MGR_MAX_ALIVE_PAGES) || (!ui_mgr_check_memory_usage() && (now_size > 0))) {
#elif UI_MGR_MEMORY_THRESHOLD
        while(!ui_mgr_check_memory_usage() && (now_size > 0)) {
#else
        while(now_size >= UI_MGR_MAX_ALIVE_PAGES) {
#endif
            err = ui_mgr_delete_oldest_page();
            if(err) {
                LV_LOG_USER("ui_mgr_delete_oldest_page err: %d\n", err);
                return NULL;
            }

            now_size = my_queue_get_size(&ui_mgr.alive_pages_queue, &err);
            if(err) {
                LV_LOG_USER("my_queue_get_size err: %d\n", err);
                return NULL;
            }
        }
#endif /* UI_MGR_MAX_ALIVE_PAGES || UI_MGR_MEMORY_THRESHOLD */

        /* Create the page */
        page->root = lv_obj_create(NULL);
        if(page->root == NULL) {
            LV_LOG_USER("Failed to create page %d\n", page_id);
            return NULL;
        }
        if(page->page_init_cb != NULL) page->page_init_cb(page->root);

#if UI_MGR_MAX_ALIVE_PAGES || UI_MGR_MEMORY_THRESHOLD
        /* Enqueue */
        err = my_queue_enqueue(&ui_mgr.alive_pages_queue, page->page_id);
        if(err) {
            LV_LOG_USER("my_queue_enqueue err: %d\n", err);
            lv_obj_del(page->root);  // 删除页面对象
            // lv_obj_del_async(page->root);
            page->root = NULL;      // 清空页面对象指针
            return NULL;
        }
#endif
    }

    return page;
}

#if METHOD_RECURSIVE
static void add_focusable_objs(lv_obj_t * obj, lv_group_t * g, lv_obj_t ** first_focusable) {
    int child_idx = 0;
    LV_LOG_USER("Enter add_focusable_objs, obj = %p, *first_focusable = %p\n", obj, *first_focusable);

    /*Recursively iterate the children*/
    lv_obj_t * child = lv_obj_get_child(obj, child_idx++);

    while(child) {
        LV_LOG_USER("child %d\n", child_idx - 1);

        if(lv_obj_has_flag(child, LV_OBJ_FLAG_CLICKABLE) || 
        lv_obj_has_flag(child, LV_OBJ_FLAG_CHECKABLE)) {
            LV_LOG_USER("Focusable\n");
            // 添加到输入组
            lv_group_add_obj(g, child);
            
            // 记录第一个可聚焦对象
            if(*first_focusable == NULL) {
                *first_focusable = child;
            }
        }

        add_focusable_objs(child, g, first_focusable);
        child = lv_obj_get_child(obj, child_idx++);
    }
}
#endif

/**
 * @brief Switch to a page
 */
void ui_mgr_switch_to_page(ui_mgr_page_id_t page_id) {
    /* If the target page is current page, don not switch */
    // if(ui_mgr.current_page && (ui_mgr.current_page->page_id == page_id)) return;
    if(ui_mgr.current_page != NULL) {
        if(ui_mgr.current_page->page_id == page_id) return;
    }

    ui_mgr_page_t *target_page = ui_mgr_find_page_by_id(page_id);
    if(target_page == NULL) return;

    /* If the target has not been created, create it */
    if(target_page->root == NULL) {
        target_page = ui_mgr_create_page(page_id);
        if(target_page == NULL) return;
    }

    /* Get and hide current page */
    ui_mgr_page_t *current_page = ui_mgr_get_current_page();
    if(current_page != NULL) {
        if(current_page->root != NULL) lv_obj_add_flag(current_page->root, LV_OBJ_FLAG_HIDDEN);
    }

    /* Unhide and load target page */
    if (target_page->root == NULL || !lv_obj_is_valid(target_page->root)) {
        LV_LOG_ERROR("Target page root is invalid!");
        return;
    }
    lv_obj_clear_flag(target_page->root, LV_OBJ_FLAG_HIDDEN);
    lv_scr_load(target_page->root);
#if UI_MGR_SWITCH_PAGE_DBG
    LV_LOG_USER("cur id %d, root %p, cb %p\n", current_page ? current_page->page_id : -1, current_page ? current_page->root : NULL, current_page ? current_page->page_init_cb : NULL);
    LV_LOG_USER("tar id %d, root %p, cb %p\n", target_page->page_id, target_page->root, target_page->page_init_cb);
#endif

    /* Refresh the focus */
    lv_group_t *g = lv_indev_get_group(indev_keypad);
    if(g) {
        // 1. 清除组内所有旧对象
        lv_group_remove_all_objs(g);

        // 2. 遍历目标页面，添加所有可聚焦对象到组中
        lv_obj_t * first_focusable = NULL;
#if METHOD_LOOP
        uint32_t i;
        
        for (i = 0; i < lv_obj_get_child_count(target_page->root); i++) {
            lv_obj_t *child = lv_obj_get_child(target_page->root, i);
            // 判断对象是否可聚焦
            if(lv_obj_has_flag(child, LV_OBJ_FLAG_CLICKABLE) || 
            lv_obj_has_flag(child, LV_OBJ_FLAG_CHECKABLE)) {
                
                // 添加到输入组
                lv_group_add_obj(g, child);
                
                // 记录第一个可聚焦对象
                if(first_focusable == NULL) {
                    first_focusable = child;
                }
            }
        }
#elif METHOD_RECURSIVE
        add_focusable_objs(target_page->root, g, &first_focusable);
#endif

        // 3. 聚焦到第一个可聚焦对象
        if(first_focusable) {
            lv_group_focus_obj(first_focusable);
        }
    }

    ui_mgr.current_page = target_page;

    LV_LOG_USER("Switch from page %d to page %d\n", current_page ? current_page->page_id : -1, target_page->page_id);
}

/**
 * @brief Initialize the UI manager, whitch include initializing default group and creating all pages
 */
int ui_mgr_init(void) {
#if UI_MGR_MAX_ALIVE_PAGES || UI_MGR_MEMORY_THRESHOLD
    /* Initialize page queue */
#if UI_MGR_MEMORY_THRESHOLD
    int err = my_queue_init(&ui_mgr.alive_pages_queue, UI_MGR_PAGE_TOTAL);
#else
    int err = my_queue_init(&ui_mgr.alive_pages_queue, UI_MGR_MAX_ALIVE_PAGES);
#endif
    if(err) {
        LV_LOG_USER("my_queue_init err: %d\n", err);
        return -1;
    }
#endif

    /* Initialize default group */
    ui_mgr.input_group = lv_group_create();
    lv_group_set_default(ui_mgr.input_group);
    lv_indev_set_group(indev_keypad, ui_mgr.input_group);

    /* Creating all pages */
    // for(int i = 0; i < UI_MGR_PAGE_TOTAL; i++) {
    //     ui_mgr_page_t *page = ui_mgr_create_page((ui_mgr_page_id_t)i);
    //     if(page == NULL) LV_LOG_USER("Failed to create page %d\n", i);
    // }

    /* Switch to main page */
    ui_mgr_switch_to_page(UI_MGR_PAGE_MAIN);

    return 0;
}
