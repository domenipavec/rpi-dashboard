/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "webservice.h"
#include "packages/base64/base64.h"
#include "packages/sha1/sha1.h"

#include "rpi_security.h"
#include "rpi_config.h"

#include <time.h>

static struct mk_list users;

/* check http authorization */
/* based on monkey/plugins/auth/auth.c */
char * rpi_security_get_user(duda_request_t *dr)
{
    char *res;
    char *sep_pointer;
    int len, sep, basic_len;
    size_t auth_len;
    unsigned char *decoded = NULL;
    unsigned char digest[SHA1_DIGEST_LEN];
    struct mk_list *head;
    struct user *entry;
    const char * basic = "Basic ";
    basic_len = strlen(basic);

    res = request->header_get(dr, "Authorization:");
    if (res == NULL) {
        return NULL;
    }

    len = strlen(res);

    /* Validate value length */
    if (len <= basic_len + 1) {
        goto error;
    }

    /* Validate 'basic' credential type */
    if (strncmp(res, basic, basic_len) != 0) {
        goto error;
    }

    decoded = base64->decode((unsigned char *) res + basic_len, len - basic_len, &auth_len);

    if (decoded == NULL) {
        goto error;
    }
    
    if (auth_len <= 3) {
        goto error;
    }
    
    sep_pointer = memchr(decoded, ':', auth_len);
    sep = ((unsigned long)sep_pointer - (unsigned long)decoded);
    if (sep_pointer == NULL || sep == 0 || sep == auth_len - 1) {
        goto error;
    }
    
    sha1->encode(sep_pointer + 1, digest, auth_len - (sep + 1));
    
    mk_list_foreach(head, &users) {
        entry = mk_list_entry(head, struct user, _head);
        /* match user */
        if (strlen(entry->user) != (unsigned int) sep) {
            continue;
        }
        if (strncmp(entry->user, (char *) decoded, sep) != 0) {
            continue;
        }

        /* match password */
        if (memcmp(entry->passwd_decoded, digest, SHA1_DIGEST_LEN) == 0) {
            
            mem->free(decoded);
            mem->free(res);

            return entry->user;
        }

        break;
    }
    
    error:
    if (decoded != NULL) {
        mem->free(decoded);
    }
    mem->free(res);
    return NULL;
}

/* check if user has permission to access the module */
int rpi_security_check_permission(const char *user, rpi_module_t * module)
{
    struct mk_list *entry;
    struct mk_string_line *sl;

    if (module->allow_flag == RPI_ALLOW_GUESTS) {
        return 0;
    }

    if (user == NULL) {
        return -1;
    }

    if (module->allow_flag == RPI_ALLOW_ALLUSERS) {
        return 0;
    }

    if (module->allow_flag == RPI_ALLOW_LIST) {
        mk_list_foreach(entry, module->allowed_users) {
            sl = mk_list_entry(entry, struct mk_string_line, _head);
            if (strncmp(user, sl->val, sl->len) == 0) {
                return 0;
            }
        }
    }
    
    return -2;
}

/* initialize users list from password file */
/* based on monkey/plugins/auth/conf.c */
void rpi_security_init(void)
{
    char *buf;
    char *sep_pointer;
    int len, i, sep;
    struct user *cred;
    int offset = 0;
    size_t decoded_len;

    mk_list_init(&users);
    
    /* concat fiepath and filename */
    const char * filename = "rpi.users";
    const char * filepath = fconf->get_path();
    int lenpath = strlen(filepath);
    int lenname = strlen(filename);
    char * path = (char *)mem->alloc(lenpath + lenname + 1);
    memcpy(path, filepath, lenpath);
    memcpy(path + lenpath, filename, lenname);
    path[lenpath + lenname] = '\0';

    buf = (fconf->read_file)(path);
    if (buf == NULL) {
        return;
    }

    len = strlen(buf);
    for (i = 0; i < len; i++) {
        if (buf[i] == '\n' || (i) == len - 1) {
            sep_pointer = strchr(buf + offset, ':');
            sep = ((unsigned long)sep_pointer - (unsigned long)(buf + offset));
            
            if (sep >= (int)sizeof(cred->user)) {
                offset = i+1;
                continue;
            }
            
            if (i - offset - sep - 1 - 5 >= (int)sizeof(cred->passwd_raw)) {
                offset = i+1;
                continue;
            }
            
            cred = mem->alloc(sizeof(struct user));
            
            /* Copy username */
            strncpy(cred->user, buf + offset, sep);
            cred->user[sep] = '\0';
            
            /* Copy raw password */
            offset += sep + 1 + 5;
            strncpy(cred->passwd_raw,
                    buf + offset,
                    i - (offset));
            cred->passwd_raw[i - offset] = '\0';
            
            /* Decode raw password */
            cred->passwd_decoded = base64->decode((unsigned char *)(cred->passwd_raw),
                                                  strlen(cred->passwd_raw),
                                                  &decoded_len);
            
            offset = i + 1;
            
            if (cred->passwd_decoded == NULL) {
                mem->free(cred);
                continue;
            }
            mk_list_add(&cred->_head, &users);
        }
    }
    
    mem->free(buf);
}