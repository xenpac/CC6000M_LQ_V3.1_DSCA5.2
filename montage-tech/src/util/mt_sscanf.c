/********************************************************************************************/
/* Montage Technology (Shanghai) Co., Ltd.                                                  */
/* Montage Proprietary and Confidential                                                     */
/* Copyright (c) 2014 Montage Technology Group Limited and its affiliated companies         */
/********************************************************************************************/

#include <ctype.h>
#include <string.h>
#include "mt_sscanf.h"

/*!
    int max
 */
#define INT_MAX  ((int)(~0U >> 1))

/*!
    native int
 */
typedef unsigned int    MT_NATIVE_INT;
/*!
    va list
 */
typedef char        *mt_va_list;

/*!
    Storage alignment properties
 */
#define  MT_AUPBND         (sizeof (MT_NATIVE_INT) - 1)
/*!
    Storage alignment properties
 */
#define  MT_ADNBND         (sizeof (MT_NATIVE_INT) - 1)

/*!
    Variable argument list macro definitions
 */
#define MT_BND(X, bnd)        (((sizeof (X)) + (bnd)) & (~(bnd)))
/*!
    Variable argument list macro definitions
 */
#define MT_VA_ARG(ap, T)      (*(T *)(((ap) += (MT_BND (T, MT_AUPBND))) - (MT_BND (T,MT_ADNBND))))
/*!
    Variable argument list macro definitions
 */
#define MT_VA_END(ap)         (void)0    /*ap = (mt_va_list)0 */
/*!
    Variable argument list macro definitions
 */
#define MT_VA_START(ap, A)    (void) ((ap) = (((char *) &(A)) + (MT_BND (A,MT_AUPBND))))

/*!
  skip_atoi

  \param[in] p_s
  \return :  int value.
*/
static int skip_atoi(const char **p_s)
{
    int i = 0;

    while (isdigit(**p_s))
        i = i * 10 + *((*p_s) ++) - '0';
    return i;
}

/*!
    Works only for digits and letters, but small and fast
*/
#define TOLOWER(x) ((x) | 0x20)

/*!
  skip_atoi

  \param[in] p_cp
  \return :  int value.
*/
static unsigned int mt_guess_base(const char *p_cp)
{
    if (p_cp[0] == '0')
    {
        if (TOLOWER(p_cp[1]) == 'x' && isxdigit(p_cp[2]))
            return 16;
        else
            return 8;
    }
    else
    {
        return 10;
    }
}


/*!
    mt_strtoull - convert a string to an unsigned long long
    param[in]   p_cp    The start of the string
    param[in]   p_endp  A pointer to the end of the parsed string will be placed here
    param[in]   base    The number base to use
 */
unsigned long long mt_strtoull(const char *p_cp, char **p_endp, unsigned int base)
{
    unsigned long long result = 0;

    if (!base)
        base = mt_guess_base(p_cp);

    if (base == 16 && p_cp[0] == '0' && TOLOWER(p_cp[1]) == 'x')
        p_cp += 2;

    while (isxdigit(*p_cp))
    {
        unsigned int value = 0;

        value = isdigit(*p_cp) ? *p_cp - '0' : TOLOWER(*p_cp) - 'a' + 10;
        if (value >= base)
            break;
        result = result * base + value;
        p_cp++;
    }

    if (p_endp)
        *p_endp = (char *)p_cp;
    return result;
}


/*!
    mt_strtoull - convert a string to an unsigned long long
    param[in]   p_cp    The start of the string
    param[in]   p_endp  A pointer to the end of the parsed string will be placed here
    param[in]   base    The number base to use
 */
long long mt_strtoll(const char *p_cp, char **p_endp, unsigned int base)
{
    if (*p_cp == '-')
        return - mt_strtoull(p_cp + 1, p_endp, base);
    return mt_strtoull(p_cp, p_endp, base);
}


/*!
    mt_strtoull - convert a string to an unsigned long long
    param[in]   p_cp    The start of the string
    param[in]   p_endp  A pointer to the end of the parsed string will be placed here
    param[in]   base    The number base to use
 */
unsigned long mt_strtoul(const char *p_cp, char **p_endp, unsigned int base)
{
    unsigned long result = 0;

    if (!base)
        base = mt_guess_base(p_cp);

    if (base == 16 && p_cp[0] == '0' && TOLOWER(p_cp[1]) == 'x')
        p_cp += 2;

    while (isxdigit(*p_cp))
    {
        unsigned int value = 0;

        value = isdigit(*p_cp) ? *p_cp - '0' : TOLOWER(*p_cp) - 'a' + 10;
        if (value >= base)
            break;
        result = result * base + value;
        p_cp++;
    }

    if (p_endp)
        *p_endp = (char *)p_cp;
    return result;
}


/*!
    mt_strtoull - convert a string to an unsigned long long
    param[in]   p_cp    The start of the string
    param[in]   p_endp  A pointer to the end of the parsed string will be placed here
    param[in]   base    The number base to use
 */
long mt_strtol(const char *p_cp, char **p_endp, unsigned int base)
{
    if(*p_cp == '-')
        return - mt_strtoul(p_cp + 1, p_endp, base);
    return mt_strtoul(p_cp, p_endp, base);
}

/*!
    mt_vsscanf - Unformat a buffer into a list of arguments
    param[in]   p_buf   input buffer
    param[in]   p_fmt   format of buffer
    param[in]   args    arguments
 */
int mt_vsscanf(const char * p_buf, const char * p_fmt, mt_va_list args)
{
    const char *p_str = p_buf;
    char *p_next = NULL;
    char digit = 0;
    int num = 0;
    int qualifier = 0;
    int base = 0;
    int field_width = 0;
    int is_sign = 0;

    while(*p_fmt && *p_str)
    {
        /* skip any white space in format */
        /* white space in format matchs any amount of
         * white space, including none, in the input.
         */
        if (isspace(*p_fmt))
        {
            while (isspace(*p_fmt))
                ++p_fmt;
            while (isspace(*p_str))
                ++p_str;
        }

        /* anything that is not a conversion must match exactly */
        if (*p_fmt != '%' && *p_fmt)
        {
            if (*p_fmt++ != *p_str++)
                break;
            continue;
        }

        if (!*p_fmt)
            break;
        ++p_fmt;

        /* skip this conversion.
         * advance both strings to next white space
         */
        if (*p_fmt == '*')
        {
            while (!isspace(*p_fmt) && *p_fmt)
                p_fmt++;
            while (!isspace(*p_str) && *p_str)
                p_str++;
            continue;
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*p_fmt))
            field_width = skip_atoi(&p_fmt);

        /* get conversion qualifier */
        qualifier = -1;
        if (*p_fmt == 'h' || *p_fmt == 'l' || *p_fmt == 'L'
            || *p_fmt == 'Z' || *p_fmt == 'z')
        {
            qualifier = *p_fmt++;
            if ((qualifier == *p_fmt))
            {
                if (qualifier == 'h')
                {
                    qualifier = 'H';
                    p_fmt++;
                }
                else if (qualifier == 'l')
                {
                    qualifier = 'L';
                    p_fmt++;
                }
            }
        }
        base = 10;
        is_sign = 0;

        if (!*p_fmt || !*p_str)
            break;

        switch(*p_fmt++)
        {
            case 'c':
            {
                char *p_s = (char *) MT_VA_ARG(args,char *);
                if (field_width == -1)
                    field_width = 1;
                do {
                    *p_s++ = *p_str++;
                } while (--field_width > 0 && *p_str);
                num++;
            }
            continue;
            case 's':
            {
                char *p_s = (char *) MT_VA_ARG(args, char *);
                if(field_width == -1)
                    field_width = INT_MAX;
                /* first, skip leading white space in buffer */
                while (isspace(*p_str))
                    p_str++;

                /* now copy until next white space */
                while (*p_str && !isspace(*p_str) && field_width--)
                {
                    *p_s++ = *p_str++;
                }
                *p_s = '\0';
                num++;
            }
            continue;
            case 'n':
                /* return number of characters read so far */
            {
                int *p_i = (int *) MT_VA_ARG(args, int *);
                *p_i = p_str - p_buf;
            }
            continue;
            case 'o':
                base = 8;
                break;
            case 'x':
            case 'X':
                base = 16;
                break;
            case 'i':
                base = 0;
            case 'd':
                is_sign = 1;
            case 'u':
                break;
            case '%':
                /* looking for '%' in p_str */
                if (*p_str++ != '%')
                    return num;
                continue;
            default:
                /* invalid format; stop here */
                return num;
        }

        /* have some sort of integer conversion.
         * first, skip white space in buffer.
         */
        while (isspace(*p_str))
            p_str++;

        digit = *p_str;
        if (is_sign && digit == '-')
            digit = *(p_str + 1);

        if (!digit
                    || (base == 16 && !isxdigit(digit))
                    || (base == 10 && !isdigit(digit))
                    || (base == 8 && (!isdigit(digit) || digit > '7'))
                    || (base == 0 && !isdigit(digit)))
                break;

        switch(qualifier)
        {
            case 'H':    /* that's 'hh' in format */
                if (is_sign)
                {
                    signed char *p_s = (signed char *) MT_VA_ARG(args,signed char *);
                    *p_s = (signed char) mt_strtol(p_str,&p_next,base);
                }
                else
                {
                    unsigned char *p_s = (unsigned char *) MT_VA_ARG(args, unsigned char *);
                    *p_s = (unsigned char) mt_strtoul(p_str, &p_next, base);
                }
                break;
            case 'h':
                if (is_sign)
                {
                    short *p_s = (short *) MT_VA_ARG(args,short *);
                    *p_s = (short) mt_strtol(p_str, &p_next,base);
                }
                else
                {
                    unsigned short *p_s = (unsigned short *) MT_VA_ARG(args, unsigned short *);
                    *p_s = (unsigned short) mt_strtoul(p_str, &p_next, base);
                }
                break;
            case 'l':
                if (is_sign)
                {
                    long *p_l = (long *) MT_VA_ARG(args,long *);
                    *p_l = mt_strtol(p_str, &p_next, base);
                }
                else
                {
                    unsigned long *p_l = (unsigned long *) MT_VA_ARG(args,unsigned long *);
                    *p_l = mt_strtoul(p_str, &p_next, base);
                }
                break;
            case 'L':
                if (is_sign)
                {
                    long long *p_l = (long long *) MT_VA_ARG(args, long long *);
                    *p_l = mt_strtoll(p_str, &p_next,base);
                }
                else
                {
                    unsigned long long *p_l
                        = (unsigned long long *) MT_VA_ARG(args, unsigned long long *);
                    *p_l = mt_strtoull(p_str, &p_next,base);
                }
                break;
            case 'Z':
            case 'z':
            {
                size_t *p_s = (size_t *) MT_VA_ARG(args, size_t *);
                *p_s = (size_t) mt_strtoul(p_str, &p_next,base);
            }
            break;
            default:
                if (is_sign)
                {
                    int *p_i = (int *) MT_VA_ARG(args, int *);
                    *p_i = (int) mt_strtol(p_str,&p_next,base);
                }
                else
                {
                    unsigned int *p_i = (unsigned int *) MT_VA_ARG(args, unsigned int *);
                    *p_i = (unsigned int) mt_strtoul(p_str, &p_next, base);
                }
                break;
        }
        num++;

        if (!p_next)
            break;
        p_str = p_next;
    }

    /*
     * Now we've come all the way through so either the input string or the
     * format ended. In the former case, there can be a %n at the current
     * position in the format that needs to be filled.
     */
    if (*p_fmt == '%' && *(p_fmt + 1) == 'n')
    {
        int *p_t = (int *)MT_VA_ARG(args, int *);
        *p_t = p_str - p_buf;
    }

    return num;
}


/*!
    mt_sscanf - Unformat a buffer into a list of arguments
    param[in]   p_buf   input buffer
    param[in]   p_fmt   formatting of buffer
    param[in]   ...     resulting arguments
 */
int mt_sscanf(const char * p_buf, const char * p_fmt, ...)
{
    mt_va_list p_args = NULL;
    int i = 0;

    MT_VA_START(p_args,p_fmt);
    i = mt_vsscanf(p_buf, p_fmt, p_args);
    MT_VA_END(p_args);
    return i;
}
