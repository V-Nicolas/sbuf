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

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sbuf.h"

static void test_sbuf_replace_file(struct sbuf *str);
static void test_sbuf_replace(struct sbuf *str);
static void test_sbuf_more(struct sbuf *str);
static void test_sbuf_file_copy(struct sbuf *str);
static void test_sbuf_basic(void);
static void test_sbuf_remove(struct sbuf *str);
static void parse_opt(int argc, char **argv);
static void usage(void);

static int test_file = 1;
static int test_more = 1;
static int test_basic = 1;
static int test_remove = 1;
static int test_replace = 1;
static int test_replace_file = 1;

#define PRINT_VERSION #SBUF_VERSION

int
main(int argc, char **argv)
{
    struct sbuf str;

    if (argc > 1)
        parse_opt(argc, argv);

    sbuf_init(&str);
    if (test_basic)
        test_sbuf_basic();
    if (test_more)
        test_sbuf_more(&str);
    if (test_file)
        test_sbuf_file_copy(&str);
    if (test_remove)
        test_sbuf_remove(&str);
    if (test_replace)
        test_sbuf_replace(&str);
    if (test_replace_file)
        test_sbuf_replace_file(&str);

    sbuf_free(&str);
    return 0;
}

static void
test_sbuf_replace_file(struct sbuf *str)
{
    int errnum;

    sbuf_reset(str);
    if ((errnum = sbuf_read_file(str, "./COPYING")) < 0) {
        fprintf(stderr, "%s: %s\n", sbuf_rdfile_get_func_fail(errnum),
                strerror(errno));
        /* maybe read error and sbuf already allocated */
        sbuf_free(str);
        return;
    }

    sbuf_replace_all(str, "e", "E");
    sbuf_replace_all(str, "E", "e");
    sbuf_replace_all(str, "a", "AZERTY");
    sbuf_replace_all(str, "AZERTY", "a");
    printf("%s\n", str->buf);
}

static void
test_sbuf_replace(struct sbuf *str)
{
    sbuf_reset(str);
    printf("\nREPLACE TEST:\n");
    sbuf_add(str, "0azerty1");
    printf("replace original: %s\n", str->buf);

    sbuf_replace(str, "azerty", "qwerty");
    printf("replace azerty by qwerty: %s\n", str->buf);

    sbuf_replace(str, "qwerty", "AAZZEERRTTYY");
    printf("replace by more char: %s\n", str->buf);

    sbuf_replace(str, "AAZZEERRTTYY", "qq");
    printf("replace by small char: %s\n", str->buf);

    sbuf_replace(str, "qq", "");
    printf("replace by nothing char: %s\n", str->buf);

    sbuf_add(str, "- finish - ");
    printf("replace finish: %s\n", str->buf);

    sbuf_replace_all(str, "i", "E");
    printf("replace all i by E: %s\n", str->buf);

    sbuf_to_lower(str);
    printf("replace to lower: %s\n", str->buf);

    sbuf_vadd(str, "\n");
    printf("replace add retline: %s", str->buf);
}

static void
test_sbuf_remove(struct sbuf *str)
{
    long offset;
    size_t azerty_size = strlen("azerty");

    sbuf_reset(str);
    for (int i = 0; i < 5; i++) {
        sbuf_add(str, "0");
    }
    sbuf_add(str, "azerty");
    printf("test_remove original string: %s\n", str->buf);

    offset = sbuf_search_str_offset(str, "azerty");
    sbuf_rm_before_offset(str, azerty_size, offset+azerty_size);
    sbuf_add(str, "qwerty");
    printf("test_remove \"qwerty\": %s\n", str->buf);


    sbuf_rm_after_offset(str, strlen("qwertyyyyyy"), offset);
    sbuf_add(str, "azerty");
    printf("test_remove \"azerty\": %s\n", str->buf);

    sbuf_rm_after_offset(str, 5, 0);
    sbuf_add(str, "11111");
    printf("test_remove \"00000\": %s\n", str->buf);

    sbuf_reset(str);

    sbuf_add(str, "00000azerty11111");
    offset = sbuf_search_str_offset(str, "azerty");
    sbuf_rm_after_offset(str, azerty_size, 5);
    printf("test_remove azerty to \"00000azerty11111\": %s\n", str->buf);

    sbuf_add(str, " - finish -\n \n\t\n  ");
    sbuf_trim_blank(str);
    printf("test_remove finish: %s\n", str->buf);
}

static void
test_sbuf_more(struct sbuf *str)
{
    int i = 0;
    const char *vartest = "test";

    for (i = 0; i < 128; i++)
        sbuf_add(str, "a");

    sbuf_add(str, "n");
    printf("%s\n", str->buf);

    sbuf_rm(str, 10);
    printf("%s\n", str->buf);

    for (i = 0; i < 6; i++)
        sbuf_vadd(str, "a");
    printf("%s\n", str->buf);

    sbuf_reset(str);

    sbuf_add(str, "<in>test add<out>\n");
    sbuf_vadd(str, "<test string>%s</text string>\n", vartest);
    sbuf_vadd(str, "<test char>%c</text char>\n", 'u');
    sbuf_vadd(str, "<test int>%d</text int>\n", 2147483647);
    sbuf_vadd(str, "<test long>%ld</text long>\n", -2147483648);
    sbuf_vadd(str, "<test long max>%ld</text long max>\n", 9223372036854775807);
    printf("%s\n", str->buf);
    sbuf_reset(str);
}

static void
test_sbuf_file_copy(struct sbuf *str)
{
    FILE *file;
    char buffer[1024];

    if (!(file = fopen("./COPYING", "r"))) {
        perror("fopen nread");
        return;
    }

    while (!feof(file)) {
        if (fgets(buffer, 1024, file)) {
            sbuf_add(str, buffer);
        }
    }
    fclose(file);

    if (!(file = fopen("copy_file_result.txt", "w"))) {
        perror("fopen write");
        return;
    }
    fprintf(file, "%s", str->buf);
    fclose(file);
}

static void
test_sbuf_basic(void)
{
    struct sbuf str = SBUF_INIT;

    sbuf_add(&str, "test");
    printf("first add: %s\n", str.buf);
    sbuf_rm(&str, strlen("test"));
    printf("after delete: %s\n", str.buf);
    sbuf_add(&str, "test");
    printf("add after delete: %s\n", str.buf);
    sbuf_to_upper(&str);
    printf("to upper: %s\n", str.buf);
    sbuf_to_lower(&str);
    printf("to lower: %s\n", str.buf);

    sbuf_reset(&str);
    sbuf_vadd(&str, "test bad %s", "10");
    printf("after reset: %s\n", str.buf);

    sbuf_reset(&str);
    sbuf_vadd(&str, "teeeeest");
    sbuf_rm_before_offset(&str, 4, 5);
    printf("rm offset: %s\n", str.buf);


    sbuf_vadd(&str, " vadd %s", "1234567890");
    sbuf_add(&str, "\n---EOF---");
    printf("add after rm_offset: %s\n", str.buf);

    char *duplicat = sbuf_string_copy(&str);
    sbuf_free(&str);

    printf("duplicat: %s\n", duplicat);
    free(duplicat);
}

static void
parse_opt(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--no-file"))
            test_file = 0;
        else if (!strcmp(argv[i], "--no-simple"))
            test_more = 0;
        else if (!strcmp(argv[i], "--no-basic"))
            test_basic = 0;
        else if (!strcmp(argv[i], "--no-remove"))
            test_remove = 0;
        else if (!strcmp(argv[i], "--no-replace"))
            test_replace = 0;
        else if (!strcmp(argv[i], "--no-replace_file"))
            test_replace_file = 0;
        else if (!strcmp(argv[i], "-h"))
            usage();
        else
            usage();
    }
}

static void
usage(void)
{
    puts("sbuf test usage: ./sbuf_test ["
         "--no-file "
         "--no-simple "
         "--no-basic "
         "--no-remove "
         "--no-replace]");
    exit(0);
}
