#include "../cjson.h"
#include <stdlib.h>
#include <stdarg.h>
#define __USE_MISC 1
#include <string.h>

void	jarray_clear(JArray *array)
{
	for (JEle *ele = array->start, *temp; ele; ele = temp)
	{
		temp = ele->next;
		jele_free(ele);
	}
}

inline void	jarray_free(JArray *array)
{
	jarray_clear(array);
	free(array);
}

void	jarray_init(JArray *array)
{
	array->start = NULL;
	array->end = NULL;
	array->size = 2;
	array->count = 0;
}

void	jarray_add(JArray *array, JEle *ele)
{
	const uchar isnt_empty = (array->start != NULL);

	if (isnt_empty)
		array->end->next = ele;
	else
		array->start = ele;
	array->end = ele;
	array->size += (isnt_empty + ele->size);
	array->count++;
}

inline bool	jarray_addsafe(JArray *array, JEle *ele)
{
	if (ele)
		jarray_add(array, ele);
	return (ele);
}

JArray	*jarray_build(size_t argc, ...)
{
	JArray *array = malloc(sizeof(JArray));

	if (!array)
		return (NULL);

	jarray_init(array);

	va_list args;
	va_start(args, argc);

	for (; argc; argc--)
	{
		JType type = va_arg(args, JType);
		const char *value = va_arg(args, char *);
		size_t value_size = va_arg(args, size_t);

		JEle *ele = jele_build(type, value, value_size);

		if (!jarray_addsafe(array, ele))
			return (va_end(args), jarray_clear(array), NULL);
	}

	va_end(args);
	return (array);
}

char	*jarray_encode(JArray *array)
{
	char *build = malloc(array->size + 1);

	if (!build)
		return (NULL);
	
	char *ptr = build;
	*ptr++ = '[';
	for (const JEle *ele = array->start; ele; ele = ele->next)
	{
		if (ele != array->start)
			*ptr++ = ',';
		ptr = mempcpy(ptr, ele->ptr, ele->size);
	}
	*ptr++ = ']';
	*ptr++ = '\0';
	return (build);
}

JArray	*jarray_decode(const char *str, size_t size)
{
	JArray *array = malloc(sizeof(JArray));

	if (!array)
		return (NULL);
	
	jarray_init(array);
	
	// trim input
	const char *str_end = str + size;
	jdecode_trim(&str, &str_end);
	size = str_end - str++ - 1;

	while (size--)
	{
		const char *end = jdecode_next(str, &size, ',');
		JEle *ele = malloc(sizeof(JEle));

		if (!ele || !(ele->ptr = jdecode_resize(str, end, &ele->size)))
			return (jarray_clear(array), free(array), free(ele), NULL);
		
		ele->type = jdecode_type(str, str + ele->size);
		ele->next = NULL;
		jarray_add(array, ele);
		str = end + 1;
	}
	return (array);
}
