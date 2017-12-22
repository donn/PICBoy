#define SET_BIT(i, j) do { i = i | (1 << j); } while (0)
#define RESET_BIT(i, j) do { i = i & ~(1 << j); } while (0)
#define TEST_BIT(i, j)  ((i >> j) & 1)
#define SWAP_NIBBLES(i) do { i = ((byte)i >> 4) | ((byte)i << 4); } while(0)
#define SIGN_EXTEND(i) (uint16_t)(int16_t)(int8_t)(uint8_t)(i)

#ifdef _DEBUG
#define PICBOY_LOG(...) do { printf(__VA_ARGS__); fflush(NULL); } while (0)
#else
#define PICBOY_LOG(...) do { } while (0)
#endif
