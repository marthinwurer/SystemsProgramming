#ifndef _CTYPE_H
#define _CTYPE_H

#define isalnum(ch)
#define isalpha(ch)

#define isblank(ch)
#define iscntrl(ch)
#define isdigit(ch)
#define isgraph(ch)
#define islower(ch) (ch >= 'a' && ch <= 'z')
#define isprint(ch)
#define ispunct(ch)
#define isspace(ch)
#define isupper(ch) (ch >= 'A' && ch <= 'Z')
#define isxdigit(ch)

#define tolower(ch) (isupper(ch) ? ch + ('a' - 'A') : ch)
#define toupper(ch) (islower(ch) ? ch - ('a' - 'A') : ch)

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#endif