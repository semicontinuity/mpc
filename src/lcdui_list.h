#ifndef __LCDUI_LIST_H__
#define __LCDUI_LIST_H__

#define LCDUI_LIST__ITEM_HEIGHT (10)
#define LCDUI_LIST__ITEM_TOP_MARGIN (1)
#define LCDUI_LIST__ITEM_LEFT_MARGIN (1)

struct lcdui__list {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int visible_items;
    unsigned int first_visible_item;
    unsigned int selected_item;
    unsigned int (*size)();
    const char* (*item)(unsigned int index);
    char (*icon)(unsigned int index);
};

void lcdui_list__paint(struct lcdui__list *list);

unsigned char lcdui_list__handleEvent(struct lcdui__list *list, unsigned char event);

#endif
