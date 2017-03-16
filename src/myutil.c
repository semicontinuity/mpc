#include <mpd/client.h>
#include <assert.h>
#include <stdlib.h>
#include "lcd_graphics.h"
#include "model.h"

void
printErrorAndExit(struct mpd_connection *conn)
{
    shutting_down = 1;

    lcd_graphics__clear();

    const char *message;

    assert(mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS);
    message = mpd_connection_get_error_message(conn);

    lcd_graphics__setColor(0);
    lcd_graphics__fillText(message, 0, 0);
    lcd_graphics__flush_all();

    mpd_connection_free(conn);

    exit(EXIT_FAILURE);
}
