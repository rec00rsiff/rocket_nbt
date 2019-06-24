#ifndef nbt_h
#define nbt_h

#include <stdint.h>

#define NBT_STACK_DEPTH     5
#define NBT_ALLOC_BLOCK     1024

/* Compression types */
#define NBT_UNCOMPRESSED    0
#define NBT_ZLIB            1
#define NBT_GZIP            2

/* Tag types */
#define TAG_END             0x00
#define TAG_BYTE            0x01
#define TAG_SHORT           0x02
#define TAG_INT             0x03
#define TAG_LONG            0x04
#define TAG_FLOAT           0x05
#define TAG_DOUBLE          0x06
#define TAG_BYTE_ARRAY      0x07
#define TAG_STRING          0x08
#define TAG_LIST            0x09
#define TAG_COMPOUND        0x0a
#define TAG_INT_ARRAY       0x0b
#define TAG_LONG_ARRAY      0x0c

/* Endianness types */
#define NBT_BIG_ENDIAN      0x00
#define NBT_LITTLE_ENDIAN   0x01

//Stack signal feature W.I.P
/* Signals
 #define SIG_MEM_FAIL       0x00
 #define SIG_OK             0x01
 */

#ifdef __cplusplus
extern "C"
{
#endif

    struct NBT_TAG_NODE
    {
        uint8_t nbt_tag_type;

        struct NBT_TAG_NODE* child_nodes;
        size_t child_nodes_len;

        char* name;
        size_t name_len;

        char* payload;
        size_t payload_len;
    };

    int32_t nbt_read_len(char* buffer, uint8_t endianness); //sig, 4 bytes
    uint16_t nbt_read_str_len(char* buffer, uint8_t endianness); //unsig, 2 bytes

    int8_t  nbt_get_byte(char* buffer);
    int16_t nbt_get_short(char* buffer, uint8_t endianness);
    int32_t nbt_get_int(char* buffer, uint8_t endianness);
    int64_t nbt_get_long(char* in_buffer, uint8_t endianness);
    float   nbt_get_float(char* buffer, uint8_t endianness);
    double  nbt_get_double(char* buffer, uint8_t endianness);

    struct NBT_TAG_NODE* node_get_child_by_name(struct NBT_TAG_NODE* node, char* name, size_t name_len);

    void node_close(struct NBT_TAG_NODE* node); //returns 1 on success, 0 on error

    //Create stack instance for each nbt read session
    struct NBT_STACK
    {
        struct NBT_TAG_NODE* root; //always compound: https://wiki.vg/NBT
        size_t buffer_pos;

        uint8_t endianness;
    };

    struct NBT_STACK get_new_stack();
    void close_stack(struct NBT_TAG_NODE* root);

    struct NBT_TAG_NODE stack_get_next_node(struct NBT_STACK* stack, char* buffer, size_t buffer_len, uint8_t root);
    struct NBT_TAG_NODE stack_try_get_next_node(struct NBT_STACK* stack, char* buffer, size_t buffer_len, uint8_t type, uint8_t root);

    /*
     Reads data to stack node structure, so it can be easily read

     Input:
     stack - allocated upper on auto-memory struct
     buffer,buffer_size - nbt data, decompressed internally
     decompress_buffer,decompress_buffer_size - decompression output buffer, set decompress_buffer_size if no compression

     Returns root NBT_TAG_NODE copy
     */
    struct NBT_TAG_NODE nbt_read_data(struct NBT_STACK* stack, char* buffer, size_t buffer_size, char* decompress_buffer, size_t decompress_buffer_len);

    /*
     Decompresses data (used internally)

     Input:
     type - compression type, use identifiers defined above
     buffer,buffer_size - compressed data, including headers depending on type

     Returns 1 on success, 0 on error
     */
    uint8_t nbt_decompres(char* buffer, size_t buffer_len, char* decompress_buffer, size_t decompress_buffer_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif //ifndef nbt_h
