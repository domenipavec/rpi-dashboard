/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Raspberry Pi Dasboard
 *  =====================
 *  Copyright 2014 Domen Ipavec <domen.ipavec@z-v.si>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "webservice.h"

#include <stdarg.h>

char * rpi_string_concat(const char *s1, const char *s2)
{
    int l1 = strlen(s1);
    int l2 = strlen(s2);
    char * path = (char *)mem->alloc(l1 + l2 + 1);
    memcpy(path, s1, l1);
    memcpy(path + l1, s2, l2);
    path[l1 + l2] = '\0';
    return path;
}

char * rpi_string_concatN(int N, ...)
{
    int i;
    const char *s[N];
    int len[N];
    int total_len = 0;
    va_list ap;
    
    va_start(ap, N);
    for (i = 0; i < N; i++) {
        s[i] = va_arg(ap, const char *);
        len[i] = strlen(s[i]);
        total_len += len[i];
    }
    va_end(ap);
    
    char *ret = (char *)mem->alloc(total_len + 1);
    char *copy = ret;
    for (i = 0; i < N; i++) {
        memcpy(copy, s[i], len[i]);
        copy += len[i];
    }
    *copy = '\0';
    return ret;
}
