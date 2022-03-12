/*
 *  Author: Vilmain Nicolas
 *  Contact: nicolas.vilmain@gmail.com
 *
 *  This file is part of lib sbuf.
 *
 *  lib sbuf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libsbuf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lib sbuf.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "src/sbuf.h"
#include "src/sbuf.c"

#define PRINT_VERSION #SBUF_VERSION

void test_sbuf_add(struct sbuf *s);
void test_sbuf_vadd(struct sbuf *s);
void test_sbuf_add_to_offset(struct sbuf *s);
void test_sbuf_set_size(struct sbuf *s);
void test_sbuf_len(struct sbuf *s);
void test_sbuf_search(struct sbuf *s);
void test_sbuf_to_lower(struct sbuf *s);
void test_sbuf_to_upper(struct sbuf *s);
void test_sbuf_trim_blank(struct sbuf *s);
void test_sbuf_trim_char(struct sbuf *s);
void test_sbuf_rm(struct sbuf *s);
void test_sbuf_rm_before_offset(struct sbuf *s);
void test_sbuf_rm_after_offset(struct sbuf *s);
void test_sbuf_replace(struct sbuf *s);
void test_sbuf_replace_all(struct sbuf *s);
void test_sbuf_read_file(struct sbuf *s);
void test_sbuf_string_copy(struct sbuf *s);
void test_sbuf_rdfile_get_func_fail(void);
void tests(struct sbuf *s);

int
main(int argc, char **argv)
{
    struct sbuf str;

    (void) argc;
    (void) argv;
    sbuf_init(&str);
    
    test_sbuf_add(&str);
    test_sbuf_vadd(&str);
    test_sbuf_add_to_offset(&str);
    test_sbuf_set_size(&str);
    test_sbuf_len(&str);
    test_sbuf_search(&str);
    test_sbuf_to_lower(&str);
    test_sbuf_to_upper(&str);
    test_sbuf_trim_blank(&str);
    test_sbuf_trim_char(&str);
    test_sbuf_rm(&str);
    test_sbuf_rm_before_offset(&str);
    test_sbuf_rm_after_offset(&str);
    test_sbuf_replace(&str);
    test_sbuf_replace_all(&str);
    test_sbuf_read_file(&str);
    test_sbuf_rdfile_get_func_fail();
    test_sbuf_string_copy(&str);
    sbuf_free(&str);
    assert(str.buf == NULL);

    sbuf_init(&str);
    tests(&str);
    sbuf_free(&str);
    
    return 0;
}

void
test_sbuf_add(struct sbuf *s)
{
    sbuf_reset(s);

    sbuf_add(s, "a");
    sbuf_add(s, "z");
    sbuf_add(s, "erty");
    assert(strcmp(s->buf, "azerty") == 0);
}

void
test_sbuf_vadd(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_vadd(s, "a%c%s %d", 'z', "erty", 1337);
    assert(!strcmp(s->buf, "azerty 1337"));

    sbuf_reset(s);
    sbuf_vadd(s, "n");
    sbuf_vadd(s, "=");
    sbuf_vadd(s, "%0.2f", 11.22222);
    assert(!strcmp(s->buf, "n=11.22"));
}

void
test_sbuf_add_to_offset(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_add(s, "azy");
    sbuf_add_to_offset(s, 2, "ert");
    assert(!strcmp(s->buf, "azerty"));

    sbuf_reset(s);
    sbuf_add_to_offset(s, 0, "azerty");
    assert(!strcmp(s->buf, "azerty"));
    
    sbuf_reset(s);
    sbuf_add(s, "aze");
    sbuf_add_to_offset(s, 3, "rty");
    assert(!strcmp(s->buf, "azerty"));

    sbuf_reset(s);
    sbuf_add(s, "azy");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    sbuf_add_to_offset(s, -1, "ert");
#pragma GCC diagnostic pop
    assert(!strcmp(s->buf, "azyert"));

    sbuf_reset(s);
    sbuf_add(s, "zerty");
    sbuf_add_to_offset(s, 0, "a");
    assert(!strcmp(s->buf, "azerty"));
}

void
test_sbuf_set_size(struct sbuf *s)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    assert(sbuf_set_size(s, -1) == -1);
#pragma GCC diagnostic pop

    sbuf_reset(s);
    assert(sbuf_set_size(s, 10) == 0);
}

void
test_sbuf_len(struct sbuf *s)
{
    sbuf_reset(s);
    assert(sbuf_len(s) == 0);

    sbuf_add(s, "azerty");
    assert(sbuf_len(s) == strlen("azerty"));
    assert(strlen(s->buf) == strlen("azerty"));
}

void
test_sbuf_search(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_add(s, "azerty azerty");
    assert(sbuf_search(s, "zejty") == -1);
    assert(sbuf_search(s, "azerty") == 0);
    assert(sbuf_search(s, "y") == 5);
    assert(sbuf_search(s, "a") == 0);
    assert(sbuf_search(s, " ") == 6);    
}

void
test_sbuf_to_lower(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_vadd(s, "aZeRty %s", "AZERTY");
    sbuf_to_lower(s);
    assert(!strcmp(s->buf, "azerty azerty"));
}

void
test_sbuf_to_upper(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_vadd(s, "aZeRty %s!", "azerty");
    sbuf_to_upper(s);
    assert(!strcmp(s->buf, "AZERTY AZERTY!"));
}

void
test_sbuf_trim_blank(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_vadd(s, "azerty");
    sbuf_trim_blank(s);
    assert(!strcmp(s->buf, "azerty"));
    sbuf_add(s, " azerty");
    assert(!strcmp(s->buf, "azerty azerty"));
    
    sbuf_reset(s);
    sbuf_vadd(s, "azerty ");
    sbuf_trim_blank(s);
    assert(!strcmp(s->buf, "azerty"));

    sbuf_reset(s);
    sbuf_vadd(s, "azerty \n");
    sbuf_trim_blank(s);
    assert(!strcmp(s->buf, "azerty"));

    sbuf_reset(s);
    sbuf_vadd(s, "azerty\t");
    sbuf_trim_blank(s);
    assert(!strcmp(s->buf, "azerty"));

    sbuf_reset(s);
    sbuf_vadd(s, "azerty \n \t");
    sbuf_trim_blank(s);
    assert(!strcmp(s->buf, "azerty"));

    sbuf_reset(s);
    sbuf_vadd(s, " ");
    sbuf_trim_blank(s);
    assert(strlen(s->buf) == 0 && s->buf[0] == 0);
    sbuf_add(s, "azerty");
    assert(strcmp(s->buf, "azerty") == 0);
    
    sbuf_reset(s);
    sbuf_vadd(s, " \n \t");
    sbuf_trim_blank(s);
    assert(sbuf_len(s) == 0 && s->buf[0] == 0);
    sbuf_add(s, "azerty");
    assert(strcmp(s->buf, "azerty") == 0);    
}

void
test_sbuf_trim_char(struct sbuf *s)
{
    int ret;
    
    sbuf_reset(s);
    sbuf_vadd(s, ",");
    ret = sbuf_trim_char(s, ' ');
    assert(ret == -1 && strlen(s->buf) == 1 && strcmp(s->buf, ",") == 0);

    sbuf_reset(s);
    sbuf_vadd(s, ",");
    ret = sbuf_trim_char(s, ',');
    assert(ret == 0 && strlen(s->buf) == 0 && s->buf[0] == 0);
    sbuf_add(s, "azerty");
    assert(strcmp(s->buf, "azerty") == 0);

    sbuf_reset(s);
    sbuf_vadd(s, "azerty,");
    ret = sbuf_trim_char(s, ',');
    assert(ret == 0  && strcmp(s->buf, "azerty") == 0);
    sbuf_add(s, "!");
    assert(strcmp(s->buf, "azerty!") == 0);
}

void
test_sbuf_rm(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_rm(s, 1337);
    assert(s->buf[0] == 0);

    sbuf_add(s, "azerty");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    sbuf_rm(s, -1);
#pragma GCC diagnostic pop
    assert(s->buf[0] == 0);

    sbuf_rm(s, 1337);
    assert(s->buf[0] == 0);

    sbuf_add(s, "azerty");
    sbuf_rm(s, 1);
    assert(strcmp(s->buf, "azert") == 0);

    sbuf_rm(s, 2);
    assert(strcmp(s->buf, "aze") == 0);

    sbuf_rm(s, 1);;
    assert(strcmp(s->buf, "az") == 0);

    sbuf_add(s, "erty");
    assert(strcmp(s->buf, "azerty") == 0);
}

void
test_sbuf_rm_before_offset(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_rm_before_offset(s, 2, 5);
    assert(strcmp(s->buf, "azey") == 0);

    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_rm_before_offset(s, 1337, 5);
    assert(strcmp(s->buf, "y") == 0);

    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_rm_before_offset(s, 1, 0);
    assert(strcmp(s->buf, "azerty") == 0);

    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_rm_before_offset(s, 1337, 1337);
    assert(s->buf[0] == 0 && s->offset == 0);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    sbuf_add(s, "azerty");
    sbuf_rm_before_offset(s, -1, -1);
    assert(strcmp(s->buf, "azerty") == 0);
#pragma GCC diagnostic pop
}

void
test_sbuf_rm_after_offset(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_rm_after_offset(s, 2, 0);
    assert(strcmp(s->buf, "erty") == 0);

    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_rm_after_offset(s, 1337, 4);
    assert(strcmp(s->buf, "azer") == 0);

    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_rm_after_offset(s, 1, 0);
    assert(strcmp(s->buf, "zerty") == 0);

    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_rm_after_offset(s, 1337, 0);
    assert(s->buf[0] == 0 && s->offset == 0);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
    sbuf_add(s, "azerty");
    sbuf_rm_after_offset(s, -1, -1);
    assert(strcmp(s->buf, "azerty") == 0);
#pragma GCC diagnostic pop
}

void
test_sbuf_replace(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_replace(s, "azerty", "");
    assert(strcmp(s->buf, "") == 0 && s->offset == 0);

    sbuf_add(s, "azerty azerty");
    sbuf_replace(s, "azerty", "ytreza");
    assert(strcmp(s->buf, "ytreza azerty") == 0);
    sbuf_add(s, " azerty");
    assert(strcmp(s->buf, "ytreza azerty azerty") == 0);

    sbuf_reset(s);
    sbuf_add(s, "azerty azerty");
    sbuf_replace(s, "az", "ZA");
    assert(strcmp(s->buf, "ZAerty azerty") == 0);
    sbuf_add(s, " azerty");
    assert(strcmp(s->buf, "ZAerty azerty azerty") == 0);

    sbuf_reset(s);
    sbuf_add(s, "azerty");
    sbuf_replace(s, "y", " test replace");
    assert(strcmp(s->buf, "azert test replace") == 0);
    sbuf_add(s, " azerty");
    assert(strcmp(s->buf, "azert test replace azerty") == 0);
}

void
test_sbuf_replace_all(struct sbuf *s)
{
    sbuf_reset(s);
    sbuf_add(s, "azerty azerty");
    sbuf_replace_all(s, "azerty", "");
    assert(strcmp(s->buf, " ") == 0 && s->offset == 1);
    sbuf_add(s, "azerty");
    assert(strcmp(s->buf, " azerty") == 0);
    
    sbuf_reset(s);
    sbuf_add(s, "azerty azerty");
    sbuf_replace_all(s, "zer", "ZER");
    assert(strcmp(s->buf, "aZERty aZERty") == 0);
    sbuf_add(s, " azerty");
    assert(strcmp(s->buf, "aZERty aZERty azerty") == 0);
}

void
test_sbuf_read_file(struct sbuf *s)
{
    FILE *file = NULL;
    
    sbuf_reset(s);

    assert(sbuf_read_file(s, "./COPYING") > -1);

    file = fopen("./test_rd_file", "w+");
    assert(file != NULL);

    fprintf(file, "%s", s->buf);
    fclose(file);
}

void
test_sbuf_string_copy(struct sbuf *s)
{
    char *cpy = NULL;
    
    sbuf_reset(s);
    sbuf_add(s, "azerty");
    cpy = sbuf_string_copy(s);
    assert(strcmp(cpy, s->buf) == 0);
    free(cpy);
}

void
test_sbuf_rdfile_get_func_fail(void)
{
    assert(strcmp(sbuf_rdfile_get_func_fail(-SBUF_ERR_OPEN), "open") == 0);
    assert(strcmp(sbuf_rdfile_get_func_fail(-SBUF_ERR_READ), "read") == 0);
    assert(strcmp(sbuf_rdfile_get_func_fail(1337), "nop") == 0);
}

void
tests(struct sbuf *s)
{
    int ret;
    char *copy = NULL;
    
    sbuf_add(s, "tests");
    sbuf_vadd(s, " just %s %c%c%c", "for", 'f', 'u', 'n');
    assert(strcmp(s->buf, "tests just for fun") == 0);

    assert(sbuf_set_size(s, 0) == -1);

    sbuf_rm(s, 3);
    assert(strcmp(s->buf, "tests just for ") == 0);

    sbuf_trim_blank(s);
    assert(strcmp(s->buf, "tests just for") == 0);

    sbuf_add_to_offset(s, 0, "Azerty ");
    assert(strcmp(s->buf, "Azerty tests just for") == 0);
    assert(sbuf_len(s) == strlen("Azerty tests just for"));

    sbuf_to_upper(s);
    assert(strcmp(s->buf, "AZERTY TESTS JUST FOR") == 0);

    sbuf_add_to_offset(s, 6, "!");
    sbuf_to_lower(s);
    assert(strcmp(s->buf, "azerty! tests just for") == 0);

    sbuf_trim_char(s, 'r');
    sbuf_trim_char(s, 'u');
    sbuf_trim_char(s, 'o');
    sbuf_trim_char(s, 'f');
    sbuf_trim_blank(s);
    sbuf_trim_blank(s);
    assert(strcmp(s->buf, "azerty! tests just") == 0);

    sbuf_add(s, " fun !!");
    assert(strcmp(s->buf, "azerty! tests just fun !!") == 0);

    ret = (int) sbuf_search(s, "tests");
    assert(ret > 0);
    sbuf_replace(s, "tests ", "");
    assert(strcmp(s->buf, "azerty! just fun !!") == 0);

    sbuf_replace_all(s, "t", "T");
    assert(strcmp(s->buf, "azerTy! jusT fun !!") == 0);

    sbuf_rm_before_offset(s, 8, 8);
    assert(strcmp(s->buf, "jusT fun !!") == 0);


    sbuf_trim_char(s, '!');
    assert(strcmp(s->buf, "jusT fun ") == 0);
    sbuf_add_to_offset(s, 5, "for ");
    sbuf_trim_blank(s);
    sbuf_to_lower(s);
    assert(strcmp(s->buf, "just for fun") == 0);

    sbuf_vadd(s, ", %s !", "for sure");
    assert(strcmp(s->buf, "just for fun, for sure !") == 0);

    ret = (int) sbuf_search(s, ", for");
    assert(ret > 0);
    sbuf_rm_after_offset(s, 28, (size_t) ret);
    assert(strcmp(s->buf, "just for fun") == 0);
    
    sbuf_add(s, ", is end");
    assert(strcmp(s->buf, "just for fun, is end") == 0);
    
    copy = sbuf_string_copy(s);
    assert(strcmp(s->buf, copy) == 0);
    free(copy);
}
