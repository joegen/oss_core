
// message types
#define HELLO_MESSAGE 51
#define PUT_COMMAND 52
#define DHT_PUT_ROOT 53
#define DHT_PUT_NONROOT 54
#define DHT_GET_ROOT 55
#define GET_COMMAND 56 
#define DHT_GET_NONROOT 57
#define DHT_DUMP_STATE  58


// excluding the root of the block
#define DHT_REPLICATION 3

// test specific details
#define HOSTS_FILE "./hosts"
#define BLOCKS_FILE "./blocks"
#define TEST_NETWORK_SIZE 100
#define BLOCKS_PER_FILE 10
#define FILES_PER_NODE 10 

#define HASH_TABLE_SIZE 5000
#define ENTRY_SIZE 100
#define true 1
#define false 0

// malice types
#define MALICE_NONE 0
#define MALICE_ALL_BOOTSTRAP 1

typedef int bool;

typedef struct DhtMessage
{
	char blockId[ENTRY_SIZE];
	int hops;
	bool hijacked;
}DhtMessage;

DhtMessage* get_new_dhtmessage(char *blockId)
{
	DhtMessage *msg = (DhtMessage *) malloc(sizeof(DhtMessage));
	msg->hops = 0;
	msg->hijacked = false;
	strcpy(msg->blockId, blockId);
}
