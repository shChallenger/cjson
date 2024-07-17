#include "cjson.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int	main(void)
{
	// Encoding

	JArray *friends = jarray_build(4,
		J_STRING, "Diamond", 7,
		J_STRING, "Pearl", 5,
		J_STRING, "Sapphire", 8,
		J_STRING, "Alpha", 5);
	
	if (!friends)
		return (1);

	short coins = -475;
	double channel = -49.3;
	bool connected = true;

	Json *json = json_build(6,
		J_STRING, "username", "Challe\t\"nger", 12,
		J_INTEGER, "coins", &coins, sizeof(coins),
		J_DECIMAL, "channel", &channel, sizeof(channel),
		J_ARRAY, "friends", friends, friends->size,
		J_NULL, "null_value", NULL, 0,
		J_BOOL, "connected", &connected, sizeof(bool));
	
	if (!json)
		return (jarray_free(friends), 1);

	// Add json content, for testing purposes
	if (!json_addsafe(json, jpair_build(J_SELF, "itself", json, json->size)))
		return (jarray_free(friends), 1);
	
	char *encoded = json_encode(json);

	// Priting encoded string

	if (!encoded)
		return (json_free(json), jarray_free(friends), 1);
	
	printf("%s\n", encoded);

	json_free(json);
	jarray_free(friends);

	// Decoding

	Json *decoded_all = json_decode(encoded, strlen(encoded));
	free(encoded);

	if (!decoded_all)
		return (1);
	
	// For testing purposes, we will work on the Json embedded
	const JPair *embedded_pair = json_get(decoded_all, "itself");

	Json *decoded = json_decode(embedded_pair->value, embedded_pair->value_size);
	json_free(decoded_all); // We do not need this anymore

	if (!decoded)
		return (1);
	
	// Checking username value (with specials chars)
	const JPair *username_pair = json_get(decoded, "username");

	char *username = jdecode_str(username_pair->value, username_pair->value_size);

	if (!username)
		return (json_free(decoded), 1);
	
	printf("Username : '%s'\n", username);

	free(username);

	// Checking coins value
	const JPair *coins_pair = json_get(decoded, "coins");

	coins = jdecode_int(coins_pair->value, coins_pair->value_size);

	printf("Coins: %d\n", coins);

	// Checking channel value
	const JPair *channel_pair = json_get(decoded, "channel");

	channel = jdecode_decimal(channel_pair->value, channel_pair->value_size);

	printf("Channel : %f\n", channel);

	// Checking connected value
	const JPair *connected_pair = json_get(decoded, "connected");

	connected = jdecode_bool(connected_pair->value_size);

	printf("Connected : %s\n", connected ? "true" : "false");

	// Must be present if decoded succeed
	const JPair *friends_pair = json_get(decoded, "friends");

	friends = jarray_decode(friends_pair->value, friends_pair->value_size);
	json_free(decoded); // We do not need decoded more

	if (!friends)
		return (1);
	
	// Print all friends
	for (const JEle *friend = friends->start; friend; friend = friend->next)
	{
		char *friend_decoded = jdecode_str(friend->ptr, friend->size);
		
		printf("Friend name : %s\n", friend_decoded);

		free(friend_decoded);
	}

	jarray_free(friends);

	// Trying to decode trimmed json string

	char json_string[] = "          \n\t\t\n           			{		\"val\\\"ue\"    		\n  :    147   }\n ";

	decoded = json_decode(json_string, sizeof(json_string) - 1);

	if (!decoded)
		return (1);
	
	// Trying to access the value

	const JPair *json_pair = json_get(decoded, "val\"ue");

	if (!json_pair || json_pair->type != J_INTEGER)
		return (json_free(decoded), 1);

	int value = jdecode_int(json_pair->value, json_pair->value_size);

	printf("Decoded value: %d\n", value);

	json_free(decoded);

	return (0);
}
