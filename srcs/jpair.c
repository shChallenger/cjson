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

inline bool	jpair_setkey(JPair *pair, const char *key, size_t size)
{
	return (pair->key = jencode_str(key, size, &pair->key_size));
}

inline bool	jpair_setvalue(JPair *pair, const void *value, size_t size)
{
	return (pair->value = jencode(pair->type, value, size, &pair->value_size));
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
