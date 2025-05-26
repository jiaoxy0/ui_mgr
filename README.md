# ui_mgr

A simple UI manager based on LVGL.

## Usage

1. Add these files into your project;
2. Design your pages like those in folder ./pages;
3. Add your pages into the array of pages in ui_mgr.c;
4. Call ui_mgr_init to init UI manager;
5. Call ui_mgr_switch_to_page to switch to a new page by page ID.

## Additional features

You could control the maxmum number of active pages by modifying the macro UI_MGR_MAX_ALIVE_PAGES in ui_mgr.h
