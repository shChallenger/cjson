#include "../cjson.h"
#include <stdlib.h>
#define __USE_MISC 1
#include <string.h>

#define toval(c) (c - '0')
#define SPACE_CASE case '\n': case '\t': case '\f': case '\r': case '\v': case ' '

void	jdecode_trim(const char **str, const char **end)
{
	for (; *str != *end; (*str)++)
	{
		switch (**str)
		{
			SPACE_CASE:
				break;
			default:
				goto step2;
		}
	}

	step2:
	for (; *str != *end; (*end)--)
	{
		switch (*((*end) - 1))
		{
			SPACE_CASE:
				break;
			default:
				return ;
		}
	}
}

char	*jdecode_resize(const char *str, const char *end)
{
	const size_t length = end - str;
	char *resized = malloc(length + 1);

	if (!resized)
		return (NULL);
	
	memcpy(mempcpy(resized, str, length), "", 1);
	return (resized);
}

const char	*jdecode_next(const char *str, size_t *size, const char c)
{
	bool quotes = false;
	bool array = false;
	bool json = false;
	const char *ptr = str;

	for (; *size; ptr++, (*size)--)
	{
		switch (*ptr)
		{
			case '\\':
				if (quotes)
					ptr++, (*size)--;
				break;
			case '"':
				if (!array && !json)
					quotes = !quotes;
				break;
			case '{': case '}':
				if (!array && !quotes)
					json = !json;
				break;
			case '[': case ']':
				if (!json && !quotes)
					array = !array;
				break;
			default:
				if (*ptr == c && !quotes && !array && !json)
					return (ptr);
				break ;
		}
	}
	return (ptr);
}

JType	jdecode_type(const char *str, const char *end)
{
	switch (*str)
	{
		case '{':
			return (J_SELF);
		case '[':
			return (J_ARRAY);
		case '"':
			return (J_STRING);
		case 't': case 'f':
			return (J_BOOL);
		case 'n':
			return (J_NULL);
		default:
		{
			size_t size = end - str;
			return (J_DECIMAL + (jdecode_next(str, &size, '.') == end));
		}
	}
}

char	*jdecode_str(const char *str, size_t size)
{
	size_t extra_chars = 0;

	for (const char *end = str + size - 1, *ptr = str + 1; ptr != end;)
	{
		switch (*ptr++)
		{
			EXTRA_CHARS_CASE:
			{
				extra_chars++;
				ptr++;
				break;
			}
			default:
				break;
		}
	}

	size -= 2;
	char *decoded = malloc(size - extra_chars + 1);

	if (!decoded)
		return (NULL);
	
	char *ptr = decoded;

	for (; size; size--)
	{
		switch (*++str)
		{
			EXTRA_CHARS_CASE:
			{
				str++;
				size--;
				break;
			}
			default:
				break;
		}
		*ptr++ = *str;
	}
	*ptr = '\0';
	return (decoded);
}

double	jdecode_decimal(const char *str, size_t size)
{
	double nb = 0;
	double decimal = 0;
	double sign;

	if (*str == '-')
	{
		sign = -1;
		str++;
		size--;
	}
	else
		sign = 1;
	
	for (; size; size--, str++)
	{
		if (*str == '.')
			decimal = 0.1;
		else if (decimal)
		{
			nb += toval(*str) * decimal;
			decimal *= 0.1;
		}
		else
			nb = nb * 10 + toval(*str);
	}
	return (nb * sign);
}

long	jdecode_int(const char *str, size_t size)
{
	long nb = 0;
	long sign;

	if (*str == '-')
	{
		sign = -1;
		str++;
		size--;
	}
	else
		sign = 1;
	
	for (; size; size--)
		nb = nb * 10 + toval(*str++);
	return (nb * sign);
}

bool	jdecode_bool(size_t size)
{
	return (size == 4);
}
