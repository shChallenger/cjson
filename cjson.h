#ifndef CJSON_H

# define CJSON_H

# include <stdbool.h>
# include <stddef.h>

# ifndef uchar
#  define uchar unsigned char
# endif

# define EXTRA_CHARS_CASE case '"': case '\\': case '\n': case '\r': case '\t'

typedef enum jtype
{
	J_SELF,
	J_ARRAY,
	J_STRING,
	J_DECIMAL,
	J_INTEGER,
	J_BOOL,
	J_NULL
} JType;

// JsonArray

typedef struct jele JEle;

typedef struct jele
{
	JType	type;
	char	*ptr;
	size_t	size;
	JEle	*next;
} JEle;

typedef struct jarray
{
	JEle	*start;
	JEle	*end;
	size_t	count;
	size_t	size;
} JArray;

// Json

typedef struct jpair JPair;

typedef struct jpair
{
	JType	type;
	char	*key;
	char	*value;
	size_t	key_size;
	size_t	value_size;
	JPair	*next;
} JPair;

typedef struct json
{
	JPair	*start;
	JPair	*end;
	size_t	count;
	size_t	size;
} Json;

// jdecode

void	jdecode_trim(const char **str, const char **end);

char	*jdecode_resize(const char *str, const char *end, size_t *new_length);

const char	*jdecode_next(const char *str, size_t *size, char c);

JType	jdecode_type(const char *str, const char *end);

char	*jdecode_str(const char *str, size_t size);

double	jdecode_decimal(const char *str, size_t size);

long	jdecode_int(const char *str, size_t size);

bool	jdecode_bool(size_t size);

// jencode

char	*jencode_str(const char *str, size_t size, size_t *new_size);

char 	*jencode_decimal(const void *ptr, size_t size, size_t *new_size);

char	*jencode_int(const void *ptr, size_t size, size_t *new_size);

char	*jencode_bool(const bool *b, size_t *new_size);

char	*jencode_null(size_t *new_size);

char	*jencode(JType type, const void *value, size_t size, size_t *new_size);

// jele

void	jele_free(JEle *ele);

JEle	*jele_new(JType type);

bool	jele_setvalue(JEle *ele, const void *value, size_t size);

JEle	*jele_build(JType type, const void *value, size_t value_size);

// jpair

void	jpair_free(JPair *pair);

JPair	*jpair_new(JType type);

bool	jpair_setkey(JPair *pair, const char *key, size_t size);

bool	jpair_setvalue(JPair *pair, const void *value, size_t size);

JPair	*jpair_build(JType type, const char *key, const void *value, size_t value_size);

// jarray

void	jarray_clear(JArray *array);

void	jarray_free(JArray *array);

void	jarray_init(JArray *array);

void	jarray_add(JArray *array, JEle *ele);

bool	jarray_addsafe(JArray *array, JEle *ele);

JArray	*jarray_build(size_t argc, ...);

char	*jarray_encode(JArray *array);

JArray	*jarray_decode(const char *str, size_t size);

// json

void	json_clear(Json *json);

void	json_free(Json *json);

void	json_init(Json *json);

void	json_add(Json *json, JPair *pair);

bool	json_addsafe(Json *json, JPair *pair);

Json	*json_build(size_t argc, ...);

char	*json_encode(Json *json);

Json	*json_decode(const char *str, size_t size);

const JPair	*json_get(const Json *json, const char *key);

#endif
