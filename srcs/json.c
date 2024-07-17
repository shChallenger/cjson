#include "../cjson.h"
#include <stdlib.h>
#include <stdarg.h>
#define __USE_MISC 1
#include <string.h>

void	json_clear(Json *json)
{
	for (JPair *pair = json->start, *temp; pair; pair = temp)
	{
		temp = pair->next;
		jpair_free(pair);
	}
}

inline void	json_free(Json *json)
{
	json_clear(json);
	free(json);
}

void	json_init(Json *json)
{
	json->start = NULL;
	json->end = NULL;
	json->size = 2;
}

void	json_add(Json *json, JPair *pair)
{
	const uchar isnt_empty = (json->start != NULL);

	if (isnt_empty)
		json->end->next = pair;
	else
		json->start = pair;
	json->end = pair;
	json->size += (isnt_empty + pair->key_size + 1 + pair->value_size);
}

inline bool	json_addsafe(Json *json, JPair *pair)
{
	if (pair)
		json_add(json, pair);
	return (pair);
}

Json	*json_build(size_t argc, ...)
{
	Json *json = malloc(sizeof(Json));

	if (!json)
		return (NULL);

	json_init(json);

	va_list args;
	va_start(args, argc);

	for (; argc; argc--)
	{
		JType type = va_arg(args, int);
		const char *key = va_arg(args, char *);
		const void *value = va_arg(args, void *);
		size_t value_size = va_arg(args, size_t);

		JPair *pair = jpair_build(type, key, value, value_size);

		if (!json_addsafe(json, pair))
			return (va_end(args), json_clear(json), NULL);
	}

	va_end(args);
	return (json);
}

char	*json_encode(Json *json)
{
	char *build = malloc(json->size + 1);

	if (!build)
		return (NULL);
	
	char *ptr = build;
	*ptr++ = '{';
	for (const JPair *pair = json->start; pair; pair = pair->next)
	{
		if (pair != json->start)
			*ptr++ = ',';
		ptr = mempcpy(ptr, pair->key, pair->key_size);
		*ptr++ = ':';
		ptr = mempcpy(ptr, pair->value, pair->value_size);
	}
	*ptr++ = '}';
	*ptr++ = '\0';
	return (build);
}

Json	*json_decode(const char *str, size_t size)
{
	Json *json = malloc(sizeof(Json));

	if (!json)
		return (NULL);
	
	json_init(json);

	const char *str_end = str + size;
	const char *temp_end = str_end;
	const char *save_start = str;
	jdecode_trim(&str, &temp_end);
	size = size - 1 - (str_end - temp_end) - (str - save_start);
	str += 1;

	while (size--)
	{
		const char *middle = jdecode_next(str, &size, ':');
		const char *end = jdecode_next(middle, &size, ',');

		const char *temp_middle = middle;
		jdecode_trim(&str, &temp_middle);

		JPair *pair = malloc(sizeof(JPair));

		if (!pair || !(pair->key = jdecode_resize(str, temp_middle)))
			return (json_clear(json), free(json), free(pair), NULL);
		
		pair->key_size = temp_middle - str;
		str = end + 1;
		temp_middle = middle + 1;
		jdecode_trim(&temp_middle, &end);
		
		if (!(pair->value = jdecode_resize(temp_middle, end)))
			return (json_clear(json), free(json), free(pair->key), free(pair), NULL);
		
		pair->value_size = end - temp_middle;
		pair->type = jdecode_type(temp_middle, end);
		pair->next = NULL;
		json_add(json, pair);
	}
	return (json);
}

const JPair *json_get(const Json *json, const char *key)
{
	size_t encoded_size;
	char *encoded_key = jencode_str(key, strlen(key), &encoded_size);

	if (!encoded_key)
		return (NULL);

	for (const JPair *pair = json->start; pair; pair = pair->next)
	{
		if (pair->key_size == encoded_size &&
			!memcmp(pair->key, encoded_key, encoded_size))
			return (free(encoded_key), pair);
	}
	return (free(encoded_key), NULL);
}
