/*
int list_menu(char **caption_list, int ox, int oy) {
    int i = 0;
    int index = 0;
    char *foo = NULL;
    int m_width = 0;
    int m_height = 0;
    int item_count = 0;
    int fgcolor = makecol(64, 64, 255);
    int bgcolor = makecol(0, 0, 0);
    int txcolor = makecol(255, 255, 255);
    int inkey = 0;
    int keyval = 0;
    int keycode = 0;
    int selected = 0;
    int start = 0;
    BITMAP *bmenu = NULL;
    char **lame = NULL; // this is lame
    
    while(caption_list[i] != NULL) {
	i += 1;
	item_count += 1;
    }
	for(i = 0; i < item_count; i += 1) {
	    if (text_length(f_modern, caption_list[i]) > m_width) {
		m_width = text_length(f_modern, caption_list[i]);
	    }
	}
	m_width += 8;
	m_height = text_height(f_modern) * item_count;
	if (oy + m_height > bmenu->h) {
	    m_height = bmenu->h - oy;
	}
	if (ox == -1) {
	    ox = 320 - (m_width * .5);
	    oy = 240 - (m_height * .5);
	}
	while (1 == 1) {
	    draw_list(bmenu, caption_list, selected, &start, ox, oy, ox + m_width, oy + m_height, 0); //ox, oy, ox + m_width, oy + m_height, 0);
	    pthread_yield();
	    inkey = readkey();
	    keycode = inkey >> 8;
	    keyval = inkey & 0xff;
	    //printf("Selected: %i, You pressed %i\n", selected, keycode);
	    
	    //
	    if (keycode == 85) { // arrow down
		selected += 1;
	    }
	    if (keycode == 84) { // arrow up
		selected -= 1;
	    }
	    if (keycode == 59) { // arrow up
		selected = -1;
		break;
	    }

	    if (keycode == 67) { // enter
		break;
	    }
	    if (selected >= item_count) {
		selected = 0;
	    }
	    if (selected < 0) {
		selected = item_count - 1;
	    }	    
	}
//	clear_to_color(bmenu, trans_color);
	return selected;
}

int draw_list(BITMAP *bmp, char **caption_list, int selected, int *start, int x1, int y1, int x2, int y2, int no_scrollbar) {
    int i = 0;
    int index = 0;
    char *foo = NULL;
    int m_width = x2 - x1;
    int m_height = y2 - y1;
    int item_count = 0;
    int fgcolor = makecol(64, 64, 255);
    int bgcolor = makecol(0, 0, 0);
    int txcolor = makecol(255, 255, 255);
    int inkey = 0;
    int keyval = 0;
    int keycode = 0;
    int max_items = 0;
    int total_height = 0;
    int scroll_height = 0;
    int scroll_pos = 0;
    
    i = 0;
    while (caption_list[i] != NULL) {
	item_count += 1;
	i += 1;
    }

    total_height = text_height(f_modern) * item_count;
    if (total_height != m_height) {
	scroll_height = (float)m_height * (float)((float)(1 / (float)total_height) * (float)m_height);
    } else {
	no_scrollbar = 1;
    }
    if (item_count == 0) {
	no_scrollbar = 1;
    }

    max_items = (m_height / text_height(f_modern));
    if (selected - *start >= max_items - 1) {
	*start = selected - (max_items - 1);
    }
    if (selected - *start < 0) {
	*start = selected;
    }
    if (max_items == item_count) {
	no_scrollbar = 1;
    }
    if (no_scrollbar == 0) {
	scroll_pos = (float)((m_height - scroll_height) / (item_count - max_items)) * *start;
    }
    draw_gradient(bmp, x1, y1, x2, y2, 128, 128, 128, 64, 64, 64);
    draw_gradient(bmp, x1, y1 + (text_height(f_modern) * (selected - *start)), x2, y1 + (text_height(f_modern) * ((selected - *start) + 1)), 128, 128, 190, 64, 64, 128);

    if (no_scrollbar == 0) {    
	draw_gradient(bmp, x2 - 8, y1, x2, y2, 64, 64, 64, 32, 32, 32);
	draw_gradient(bmp, x2 - 8, y1 + scroll_pos, x2, y1 + scroll_pos + scroll_height, 64, 64, 128, 32, 32, 128);
    }
    
    i = *start;
    
    while ((caption_list[i] != NULL) && (i < max_items + *start)) {	
	textprintf_ex(bmp, f_modern, x1, y1 + (text_height(f_modern) * (i - *start)), txcolor, -1, "%s", caption_list[i]);
	i += 1;
    }
}


int draw_message(BITMAP *bmp, char *title, char *text, int waitforkey) {
    int i = 0;
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;
    int th = text_height(f_modern);
    int m_width = 0;
    int m_height = 0;
    
    m_width = text_length(f_modern, title);
    if (text_length(f_modern, text) > m_width) {
	m_width = text_length(f_modern, text);
    }
    m_width += 16;
    m_height = th * 3 + 16;
    
    x1 = (bmp->w * .5) - (m_width * .5);
    y1 = (bmp->h * .5) - (m_height * .5);
    x2 = x1 + m_width;
    y2 = y1 + m_height;
    
    draw_gradient(bmp, x1, y1, x2, y2, 128, 128, 128, 64, 64, 64);
    draw_gradient(bmp, x1, y1, x2, y1 + th + 8, 128, 128, 190, 64, 64, 128);

    textprintf_ex(bmp, f_modern, x1, y1 + 4, 0, -1, "%s", title);
    textprintf_ex(bmp, f_modern, x1, y1 + 8 + (th * 1.5), 0, -1, "%s", text);
    
    if (waitforkey) {
	readkey();
    }
    return 0;
}

*/
int draw_gradient(BITMAP *bmp, int x1, int y1, int x2, int y2, int r1, int g1, int b1, int r2, int g2, int b2, int border) {
    int x = 0;
    int y = 0;
    float dist = y2 - y1;
    float rs = (r2 - r1) / dist;
    float gs = (g2 - g1) / dist;
    float bs = (b2 - b1) / dist;
    float r = r1;
    float g = g1;
    float b = b1;
    
    int color;
    for (y = y1; y <= y2; y += 1) {
	color = makecol(r, g, b);
	fastline(bmp, x1, y, x2, y, color);
	r += rs;
	g += gs;
	b += bs;
    }
    if (border == 1) {
	rect(bmp, x1, y1, x2, y2, makecol(0, 0, 0));
	fastline(bmp, x1 + 1, y1 + 1, x2 - 1, y1 + 1, makecol(255, 255, 255));
	fastline(bmp, x1 + 1, y1 + 1, x1 + 1, y2 - 1, makecol(255, 255, 255));    
	fastline(bmp, x1 + 1, y2 - 1, x2 - 1, y2 - 1, 0);
	fastline(bmp, x2 - 1, y1 + 1, x2 - 1, y2 - 1, 0);    
    }
    return 0;
}

