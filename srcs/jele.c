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

inline bool	jele_setvalue(JEle *ele, const void *value, size_t size)
{
	return (ele->ptr = jencode(ele->type, value, size, &ele->size));
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
