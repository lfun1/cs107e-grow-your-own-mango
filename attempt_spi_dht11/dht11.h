#define DHT_TOTAL_BITS 40
#define DHT_DATA GPIO_PB6
#define HIGH 1
#define LOW 0
#define TIME_OUT 50000

void dht_init(void);

void dht_send_starter_sig(void);

void read_bits(void);