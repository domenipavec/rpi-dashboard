/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "rpi_security.h"

#include <time.h>

int rpi_validate_user(duda_request_t *dr, rpi_module_t * module)
{
    struct mk_list *entry;
    struct mk_string_line *sl;

    /* testing */
    response->printf(dr, "Allow flag: %d\n\n", module->allow_flag);
    if (module->allow_flag == RPI_ALLOW_LIST) {
        mk_list_foreach(entry, module->allowed_users) {
            sl = mk_list_entry(entry, struct mk_string_line, _head);
            response->printf(dr, "User: ");
            response->print(dr, sl->val, sl->len);
            response->printf(dr, "\n");
        }
    }
    /* end testing */
    
    if (module->allow_flag == RPI_ALLOW_GUESTS) {
        return 0;
    }
    
    char *logged_user = session->get(dr, "user");
    if (logged_user != NULL) {
        response->printf(dr, "Logged in user: %s\n\n", session->get(dr, "user"));
        if (module->allow_flag == RPI_ALLOW_ALLUSERS) {
            return 0;
        }
        
        if (module->allow_flag == RPI_ALLOW_LIST) {
            mk_list_foreach(entry, module->allowed_users) {
                sl = mk_list_entry(entry, struct mk_string_line, _head);
                if (strncmp(logged_user, sl->val, sl->len) == 0) {
                    return 0;
                }
            }
        }
        
        return -1;
    } else {
        /* create user for testing */
        session->create(dr, "user", "john", time(NULL) + 3600);
        return -1;
    }
}

void rpi_send_auth_request(duda_request_t *dr)
{
    response->http_status(dr, 403);

    response->printf(dr, "Forbidden!");

    response->end(dr, NULL);
}