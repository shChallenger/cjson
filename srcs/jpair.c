#include "../cjson.h"
#include <stdlib.h>
#include <string.h>

inline void	jpair_free(JPair *pair)
{
	free(pair->key);
	free(pair->value);
	free(pair);
}

JPair	*jpair_new(JType type)
{
	JPair *pair = malloc(sizeof(JPair));

	if (pair)
	{
		pair->type = type;
		pair->key = NULL;
		pair->value = NULL;
		pair->key_size = 0;
		pair->value_size = 0;
		pair->next = NULL;
	}
	return (pair);
}

bool	jpair_setkey(JPair *pair, const char *key, size_t size)
{
	size_t new_size;
	char *encoded = jencode_str(key, size, &new_size);

	if (encoded)
	{
		pair->key = encoded;
		pair->key_size = new_size;
	}
	return (encoded);
}

bool	jpair_setvalue(JPair *pair, const void *value, size_t size)
{
	size_t new_size;
	char *encoded = jencode(pair->type, value, size, &new_size);

	if (encoded)
	{
		pair->value = encoded;
		pair->value_size = new_size;
	}
	return (encoded);
}

JPair	*jpair_build(JType type, const char *key, const void *value, size_t value_size)
{
	JPair *pair = jpair_new(type);

	if (!pair)
		return (NULL);
	
	if (!jpair_setkey(pair, key, strlen(key))
		|| !jpair_setvalue(pair, value, value_size))
		return (free(pair), NULL);
	
	return (pair);
}
