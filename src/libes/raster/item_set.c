
#include "graph.h"

R_pad_set_item (item, value)
    char *item;
    char *value;
{
    char result;

    _hold_signals(1);

    _send_ident (PAD_SET_ITEM);
    _send_text (item);
    _send_text (value);
    _get_char (&result);

    _hold_signals(0);

    return result;
}
