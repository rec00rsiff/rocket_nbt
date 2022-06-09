#ifndef nbt_h
#define nbt_h

#include <stdint.h>
#include <stddef.h>

#define NBT_STACK_DEPTH     5
#define NBT_ALLOC_BLOCK     1024

/* Compression types */
#define NBT_UNCOMPRESSED    0
#define NBT_GZIP            1
#define NBT_ZLIB            2

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
        uint8_t write_end;
        
        struct NBT_TAG_NODE* child_nodes;
        size_t child_nodes_len;
        
        char* name;
        size_t name_len;
        
        char* payload;
        size_t payload_len;
    };
    
    uint32_t nbt_read_uint24(char* buffer, uint8_t endianness); //unsig, 3 bytes
    int32_t nbt_read_len(char* buffer, uint8_t endianness, uint8_t protobuf, uint8_t* size); //sig, 4 bytes
    uint16_t nbt_read_str_len(char* buffer, uint8_t endianness, uint8_t protobuf, uint8_t* size); //unsig, 2 bytes
    
    int8_t  nbt_get_byte(char* buffer);
    int16_t nbt_get_short(char* buffer, uint8_t endianness, uint8_t protobuf);
    int32_t nbt_get_int(unsigned char* buffer, uint8_t endianness, uint8_t protobuf);
    int32_t nbt_get_uvarint32(char* buffer, uint8_t* size);
    int32_t nbt_get_sigvarint32(char* buffer, uint8_t* size);
    int64_t nbt_get_long(char* in_buffer, uint8_t endianness, uint8_t protobuf);
    int64_t nbt_get_uvarint64(char* buffer, uint8_t* size);
    int64_t nbt_get_sigvarint64(char* buffer, uint8_t* size);
    float   nbt_get_float(char* buffer, uint8_t endianness);
    double  nbt_get_double(char* buffer, uint8_t endianness);
    
    int64_t nbt_encode_zigzag_32(int64_t val);
    int64_t nbt_encode_zigzag_64(int64_t val);
    
    int32_t nbt_decode_zigzag_32(int32_t val);
    int64_t nbt_decode_zigzag_64(int64_t val);
    
    void nbt_write_uint24(uint32_t val, char* buffer, uint8_t endianness); //unsig, 3 bytes
    uint8_t nbt_write_len(int32_t val, char* buffer, uint8_t endianness, uint8_t protobuf); //sig, 4 bytes
    uint8_t nbt_write_str_len(uint16_t val, char* buffer, uint8_t endianness, uint8_t protobuf); //unsig, 2 bytes
    uint8_t nbt_write_str(char* str, uint16_t str_len, char* buffer, uint8_t endianness, uint8_t protobuf);
    
    void nbt_write_byte(int8_t val, char* buffer);
    
    uint8_t nbt_write_short(int16_t val, char* buffer, uint8_t endianness, uint8_t protobuf);
    
    void nbt_write_int(int32_t val, char* buffer, uint8_t endianness);
    uint8_t nbt_write_uvarint32(uint32_t val, char* buffer);
    uint8_t nbt_write_sigvarint32(int32_t val, char* buffer);
    
    void nbt_write_long(int64_t val, char* in_buffer, uint8_t endianness);
    uint8_t nbt_write_uvarint64(uint64_t val, char* buffer);
    uint8_t nbt_write_sigvarint64(int64_t val, char* buffer);
    
    void nbt_write_float(float val, char* buffer, uint8_t endianness);
    void nbt_write_double(double val, char* buffer, uint8_t endianness);
    
    void nbt_create_node(struct NBT_TAG_NODE* node, char* name, char* payload, size_t payload_len, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root);
    
    void nbt_create_list_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, uint8_t list_type, size_t child_capacity, struct NBT_TAG_NODE* child_nodes, uint8_t endianness, uint8_t protobuf);
    
    void nbt_create_byte_array_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, size_t child_capacity, struct NBT_TAG_NODE* child_nodes, uint8_t endianness, uint8_t protobuf);
    
    void nbt_create_int_array_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, size_t child_capacity, struct NBT_TAG_NODE* child_nodes, uint8_t endianness, uint8_t protobuf);
    
    void nbt_create_long_array_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, size_t child_capacity, struct NBT_TAG_NODE* child_nodes, uint8_t endianness, uint8_t protobuf);
    
    void nbt_create_compound_node(struct NBT_TAG_NODE* node, char* name, size_t child_capacity, struct NBT_TAG_NODE* child_nodes);
    
    void nbt_create_str_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, char* str, uint16_t str_len, uint8_t endianness, uint8_t protobuf);
    
    void nbt_create_byte_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, int8_t val, uint8_t endianness, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root);
    
    void nbt_create_short_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, int16_t val, uint8_t endianness, uint8_t protobuf, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root);
    
    void nbt_create_int_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, int32_t val, uint8_t endianness, uint8_t protobuf, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root);
    
    void nbt_create_long_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, int64_t val, uint8_t endianness, uint8_t protobuf, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root);
    
    void nbt_create_float_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, float val, uint8_t endianness, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root);
    
    void nbt_create_double_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, double val, uint8_t endianness, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root);
    
    struct NBT_TAG_NODE* node_get_child_by_name(struct NBT_TAG_NODE* node, char* name, size_t name_len);
    
    void node_close(struct NBT_TAG_NODE* node); //returns 1 on success, 0 on error
    
    //Create stack instance for each nbt read session
    struct NBT_STACK
    {
        struct NBT_TAG_NODE* root; //always compound: https://wiki.vg/NBT
        size_t buffer_pos;
        
        uint8_t endianness;
        uint8_t protobuf;
    };
    
    struct NBT_STACK get_new_stack();
    void close_stack(struct NBT_TAG_NODE* root);
    
    struct NBT_TAG_NODE stack_get_next_node(struct NBT_STACK* stack, char* buffer, size_t buffer_len, uint8_t root);
    struct NBT_TAG_NODE stack_try_get_next_node(struct NBT_STACK* stack, char* buffer, size_t buffer_len, uint8_t type, uint8_t root);
    
    void stack_write_next_node(struct NBT_TAG_NODE* node, struct NBT_STACK* stack, char* buffer);
    
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
