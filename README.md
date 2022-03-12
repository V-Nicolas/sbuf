
# LIB STRING BUFFER sbuf

## Description:
Library for string operations.
<br/>
<br/>

#### Why ?
For fun, sure.
<br/>
<br/>

## Tricks:
Add to compil flags:<br/>
Change default allocation size:<br />
-DSBUF_ALLOC_SIZE=1024<br />
By default alloc size = 128<br />
If you know you'r add always big string in buffer, change value for optimize.
<br/>
<br/>
-DSBUF_THREAD_SAFE for using a same structure with many threads.
<br/>
<br/>

## library require:
pthread if you compil with SBUF_THREAD_SAFE.
<br/>
<br/>

## Compil test / example:
```
./tests.sh
```
<br/>


## Man
```
void sbuf_init(struct sbuf *str);
```
Description: Initialize sbuf structure, or use SBUF_INIT: `struct sbuf str
 = SBUF_INIT;`
<br/>
<br/>


```
void sbuf_reset(struct sbuf *str);
```
Description: Reset buffer at 0
<br/>
<br/>


```
void sbuf_free(struct sbuf *str);
```
Description: Free memory used by struct
<br/>
<br/>


```
int sbuf_add(struct sbuf *str, const char *stradd);
```
Description:Add simple string to buffer.
Return: Always 0
<br/>
<br/>


```
void sbuf_vadd(struct sbuf *str, const char *fmt, ...);
```
Description: Add complex string format with arguments.<br />
example:
```
int i = 1, j = 2;
sbuf_vadd(&str, "%d+%d=%d", i, j, (i + j));
```
Return: -1: error, success: number of bytes copyed.
<br/>
<br/>


```
int sbuf_add_to_offset(struct sbuf *str, size_t offset, const char *addstr);
```
Description:Add simple string to buffer in offset.
<br/>
<br/>


```
int sbuf_set_size(struct sbuf *str, size_t newsize);
```
Description: Alloc buffer to newsize.
Return: -1 error: newsize lower to current string len, success: 0
<br/>
<br/>


```
size_t sbuf_len(struct sbuf *str);
```
Description: Return buffer length.
<br/>
<br/>


```
long sbuf_search(struct sbuf *str, const char *substr);
```
Description: Return offset to find string or -1 if not found.
<br/>
<br/>


```
char * sbuf_string_copy(struct sbuf *str);
```
Description: Return a copy of buffer or NULL if failed (errno ENOMEM)<br/>
Need free the duplicat.
<br/>
<br/>


```
void sbuf_trim_blank(struct sbuf *str);
```
Description: Delete blanks char at end buffer. (\t, ' ', \n)
<br/>
<br/>


```
int sbuf_trim_char(struct sbuf *str, char c);
```
Description: Delete all last char in string if equal to <c> param<br/>
Return: -1 not delete, else <c> is delete
<br/>
<br/>


```
void sbuf_rm(struct sbuf *str, size_t rm_size);
```
Description: Delete X char to end buffer.
<br/>
<br/>


```
int sbuf_rm_before_offset(struct sbuf *str, size_t rm_size, size_t offset);
```
Description: Delete X char before offset.<br/>
Return: -1 error: offset overflow, success: 0
<br/>
<br/>


```
int sbuf_rm_after_offset(struct sbuf *str, size_t rm_size, size_t offset);
```
Description: Delete X char after offset.<br/>
Return: -1 error: offset overflow, success: 0
<br/>
<br/>


```
void sbuf_to_lower(struct sbuf *str);
```
Description: Convert string to lower.
<br/>
<br/>


```
void sbuf_to_upper(struct sbuf *str);
```
Description: Convert string to upper.
<br/>
<br/>


```
void sbuf_replace(struct sbuf *str, const char *oldstr, const char *newstr);
```
Description: Replace first oldstr occurency by newstr<br/>
Return: -1 if oldstr not found or other error :)<br/>
        0 replace success.<br/>
<br/>
<br/>


```
void sbuf_replace_all(struct sbuf *str, const char *oldstr, const char *newstr);
```
Description: Replace all oldstr occurency by newstr<br/>
<br/>
<br/>


```
int sbuf_read_file(struct sbuf *str, const char *path);
```
Description: Read file.<br/>
Return: error < 0, success 0<br/>
Note: see sbuf_rdfile_get_func_fail
      
Example with bad path:
```
    int errnum;
    if ((errnum = sbuf_read_file(str, "bad-path-file-no-exist")) < 0) {
        fprintf(stderr, "%s: %s\n", sbuf_rdfile_get_func_fail(errnum),
                strerror(errno));
        return;
    }
```
Output: `open: No such file or directory`
<br/>
<br/>


```
const char * sbuf_rdfile_get_func_fail(int errnum);
```
Return: Function name fail in sbuf_read_file() or "nop"<br/>
Note: If you call a function which modifie errno after
 sbuf_read_file() and before sbuf_rdfile_get_func_fail(), errno contains a bad
  code.
<br/>
<br/>
