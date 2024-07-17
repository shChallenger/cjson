#include "../cjson.h"
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <float.h>
#include <stdio.h>

char 	*jencode_str(const char *str, size_t size, size_t *new_size)
{
	size_t extra_chars = 0;

	for (const char *end = str + size, *ptr = str; ptr != end;)
	{
		switch (*ptr++)
		{
			EXTRA_CHARS_CASE:
				extra_chars++;
				break;
			default:
				break;
		}
	}

	*new_size = size + 2 + extra_chars;
	char *encoded = malloc(*new_size);

	if (!encoded)
		return (NULL);

	char *ptr = encoded;
	*ptr++ = '"';

	for (;size;size--)
	{
		switch (*str)
		{
			EXTRA_CHARS_CASE:
				*ptr++ = '\\';
				break;
			default:
				break;
		}
		*ptr++ = *str++;
	}
	*ptr++ = '"';
	return (encoded);
}

inline static size_t nbr_length(size_t nb)
{
    if (nb < 100000000LL) // < 9
    {
        if (nb < 1000000LL) // < 7
        {
            if (nb < 10000LL) // < 5
            {
                if (nb < 100LL) // < 3
                    return (1 + (nb >= 10LL));
                return (3 + (nb >= 1000LL));
            }
            return (5 + (nb >= 100000LL));
        }
        return (7 + (nb >= 10000000LL));
    }
    if (nb < 1000000000000LL) // < 13
    {
        if (nb < 10000000000LL) // < 11
            return (9 + (nb >= 1000000000LL));
        return (11 + (nb >= 100000000000LL));
    }
    if (nb < 1000000000000000LL) // < 16
    {
        if (nb < 100000000000000LL) // < 15
            return (13 + (nb >= 10000000000000LL));
        return (15);
    }
    if (nb < 100000000000000000LL) // < 18
        return (17 + (nb >= 10000000000000000LL));
    return (19);
}

// Function to encode integers and floating-point numbers with maximum precision
char *jencode_decimal(const void *ptr, size_t size, size_t *new_size)
{
    double temp_nb = 0.0;
    
    // Determine the value of temp_nb based on the input size
    switch (size)
    {
        case sizeof(float):
            temp_nb = *(float *) ptr;
            break;
        case sizeof(double):
            temp_nb = *(double *) ptr;
            break;
        default:
            return (NULL); // Handle unsupported sizes
    }

    // Calculate the size needed for the formatted string
    *new_size = snprintf(NULL, 0, "%f", temp_nb);

    // Allocate memory for the encoded string
    char *encoded = malloc(*new_size + 1);
    if (!encoded)
        return (NULL); // Handle allocation failure

    // Format double into the buffer
    snprintf(encoded, *new_size + 1, "%f", temp_nb);

    return (encoded);
}

char	*jencode_int(const void *ptr, size_t size, size_t *new_size)
{
	long temp_nb;

	switch (size)
	{
		case sizeof(char):
			temp_nb = *(char *) ptr;
			break;
		case sizeof(short):
			temp_nb = *(short *) ptr;
			break;
		case sizeof(int):
			temp_nb = *(int *) ptr;
			break;
		case sizeof(long):
			temp_nb = *(long *) ptr;
			break;
		default:
			return (NULL);
	}

	bool is_negative = temp_nb < 0;
	size_t nb = is_negative ? -temp_nb : temp_nb;

	*new_size = nbr_length(nb) + is_negative;
	char *encoded = malloc(*new_size);

	if (!encoded)
		return (NULL);
	
	for (char *temp = encoded + *new_size - 1, *end = encoded + is_negative; temp >= end; temp--)
	{
		*temp = nb % 10 + '0';
		nb /= 10;
	}

	if (is_negative)
		*encoded = '-';
	
	return (encoded);
}

char	*jencode_bool(const bool *b, size_t *new_size)
{
	char		*encoded;
	const char	*repr;

	if (*b)
	{
		encoded = malloc(*new_size = 4);
		repr = "true";
	}
	else
	{
		encoded = malloc(*new_size = 5);
		repr = "false";
	}

	if (encoded)
		memcpy(encoded, repr, *new_size);
	return (encoded);
}

char	*jencode_null(size_t *new_size)
{
	char *encoded = malloc(*new_size = 4);

	if (encoded)
		memcpy(encoded, "null", 4);
	return (encoded);
}

char	*jencode(JType type, const void *value, size_t size, size_t *new_size)
{
	char	*encoded;

	switch (type)
	{
		case J_SELF:
		{
			Json *json = (Json *) value;

			*new_size = json->size;
			encoded = json_encode(json);
			break;
		}
		case J_ARRAY:
		{
			JArray *array = (JArray *) value;

			*new_size = array->size;
			encoded = jarray_encode(array);
			break;
		}
		case J_STRING:
			encoded = jencode_str((char *) value, size, new_size);
			break;
		case J_DECIMAL:
			encoded = jencode_decimal(value, size, new_size);
			break;
		case J_INTEGER:
			encoded = jencode_int(value, size, new_size);
			break;
		case J_BOOL:
			encoded = jencode_bool((bool *) value, new_size);
			break;
		case J_NULL:
			encoded = jencode_null(new_size);
			break;
		default:
			return (NULL);
	}

	return (encoded);
}
