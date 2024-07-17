#include "../cjson.h"
#include <stdlib.h>

inline void	jele_free(JEle *ele)
{
	free(ele->ptr);
	free(ele);
}

JEle	*jele_new(JType type)
{
	JEle *ele = malloc(sizeof(JEle));

	if (ele)
	{
		ele->type = type;
		ele->ptr = NULL;
		ele->size = 0;
		ele->next = NULL;
	}
	return (ele);
}

bool	jele_setvalue(JEle *ele, const void *value, size_t size)
{
	size_t	new_size;
	char *encoded = jencode(ele->type, value, size, &new_size);

	if (encoded)
	{
		ele->ptr = encoded;
		ele->size = new_size;
	}
	return (encoded);
}

JEle	*jele_build(JType type, const void *value, size_t value_size)
{
	JEle *ele = jele_new(type);

	if (!ele)
		return (NULL);
	
	if (!jele_setvalue(ele, value, value_size))
		return (free(ele), NULL);
	
	return (ele);
}
