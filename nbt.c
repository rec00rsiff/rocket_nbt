#include <zlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "nbt.h"

struct NBT_TAG_NODE stack_try_get_next_node(struct NBT_STACK* stack, char* buffer, size_t buffer_len, uint8_t type, uint8_t root)
{
    struct NBT_TAG_NODE node;
    node.name_len = 0;
    node.child_nodes_len = 0;
    
    uint8_t tag = 0;
    if (type == 0)
    {
        if (root)
        {
            stack->root = &node;
        }
        
        tag = buffer[stack->buffer_pos];
        stack->buffer_pos++;
        
        node.nbt_tag_type = tag;
        
        if(tag == TAG_END)
        {
            return node;
        }
        
        node.payload_len = nbt_read_str_len(buffer + stack->buffer_pos, stack->endianness);
        stack->buffer_pos += 2;
        
        node.payload = buffer + stack->buffer_pos;
        stack->buffer_pos += node.payload_len;
        
        node.name = node.payload;
        node.name_len = node.payload_len;
    }
    else
    {
        tag = type;
        node.nbt_tag_type = tag;
    }
    
    
    if (tag == TAG_COMPOUND)
    {
        size_t roll_back_buffer_pos = stack->buffer_pos;
        size_t alloc_size = 0;
        while(1)
        {
            struct NBT_TAG_NODE node1 = stack_try_get_next_node(stack, buffer, buffer_len, 0, 0);
            
            alloc_size++;
            if (node1.nbt_tag_type == TAG_END)
            {
                break;
            }
        }
        stack->buffer_pos = roll_back_buffer_pos;
        
        node.child_nodes = (struct NBT_TAG_NODE*) malloc(alloc_size*sizeof(struct NBT_TAG_NODE));
        
        while(1)
        {
            struct NBT_TAG_NODE node1 = stack_try_get_next_node(stack, buffer, buffer_len, 0, 0);
            if (node1.nbt_tag_type == TAG_END)
            {
                break;
            }
            node.child_nodes[node.child_nodes_len] = node1;
            node.child_nodes_len++;
            
        }
    }
    else
    {
        if(tag == TAG_BYTE)
        {
            node.payload_len = 1;
            node.payload = buffer + stack->buffer_pos;
            stack->buffer_pos += node.payload_len;
        }
        else if(tag == TAG_SHORT)
        {
            node.payload_len = 2;
            node.payload = buffer + stack->buffer_pos;
            stack->buffer_pos += node.payload_len;
        }
        else if(tag == TAG_INT)
        {
            node.payload_len = 4;
            node.payload = buffer + stack->buffer_pos;
            stack->buffer_pos += node.payload_len;
        }
        else if(tag == TAG_LONG)
        {
            node.payload_len = 8;
            node.payload = buffer + stack->buffer_pos;
            stack->buffer_pos += node.payload_len;
        }
        else if(tag == TAG_FLOAT)
        {
            node.payload_len = 4;
            node.payload = buffer + stack->buffer_pos;
            stack->buffer_pos += node.payload_len;
        }
        else if(tag == TAG_DOUBLE)
        {
            node.payload_len = 8;
            node.payload = buffer + stack->buffer_pos;
            stack->buffer_pos += node.payload_len;
        }
        else if(tag == TAG_BYTE_ARRAY)
        {
            node.payload_len = nbt_read_len(buffer + stack->buffer_pos, stack->endianness);
            stack->buffer_pos += 4;
            
            node.payload = buffer + stack->buffer_pos;
            stack->buffer_pos += node.payload_len;
        }
        else if(tag == TAG_STRING)
        {
            node.payload_len = nbt_read_str_len(buffer + stack->buffer_pos, stack->endianness);
            stack->buffer_pos += 2;
            
            node.payload = buffer + stack->buffer_pos;
            stack->buffer_pos += node.payload_len;
        }
        else if(tag == TAG_LIST)
        {
            uint8_t list_type = buffer[stack->buffer_pos];
            stack->buffer_pos += 1;
            
            size_t alloc_size = nbt_read_len(buffer + stack->buffer_pos, stack->endianness);
            stack->buffer_pos += 4;
            
            node.child_nodes = (struct NBT_TAG_NODE*) malloc(alloc_size*sizeof(struct NBT_TAG_NODE));
            
            for(node.child_nodes_len = 0; node.child_nodes_len < alloc_size; ++node.child_nodes_len)
            {
                struct NBT_TAG_NODE node1 = stack_try_get_next_node(stack, buffer, buffer_len, list_type, 0);
                
                node.child_nodes[node.child_nodes_len] = node1;
            }
        }
    }
    
    return node;
}

int32_t nbt_read_len(char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        return (int32_t)((uint8_t)(buffer[0]) << 24 |
                         (uint8_t)(buffer[1]) << 16 |
                         (uint8_t)(buffer[2]) << 8  |
                         (uint8_t)(buffer[3]));
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        return (int32_t)((uint8_t)(buffer[3]) << 24 |
                         (uint8_t)(buffer[2]) << 16 |
                         (uint8_t)(buffer[1]) << 8  |
                         (uint8_t)(buffer[0]));
    }
}

uint16_t nbt_read_str_len(char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        return (uint16_t)((uint8_t)(buffer[0]) << 8 |
                          (uint8_t)(buffer[1]));
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        return (uint16_t)((uint8_t)(buffer[1]) << 8 |
                          (uint8_t)(buffer[0]));
    }
}

int8_t nbt_get_byte(char* buffer)
{
    return (int8_t)(buffer[0]);
}

int16_t nbt_get_short(char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        return (int16_t)((uint8_t)(buffer[0]) << 8 |
                         (uint8_t)(buffer[1]));
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        return (int16_t)((uint8_t)(buffer[1]) << 8 |
                         (uint8_t)(buffer[0]));
    }
}

int32_t nbt_get_int(char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        return (int32_t)((uint8_t)(buffer[0]) << 24 |
                         (uint8_t)(buffer[1]) << 16 |
                         (uint8_t)(buffer[2]) << 8  |
                         (uint8_t)(buffer[3]));
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        return (int32_t)((uint8_t)(buffer[3]) << 24 |
                         (uint8_t)(buffer[2]) << 16 |
                         (uint8_t)(buffer[1]) << 8  |
                         (uint8_t)(buffer[0]));
    }
}

int64_t nbt_get_long(char* in_buffer, uint8_t endianness)
{
    unsigned char* buffer = (unsigned char*)in_buffer;
    if (endianness == NBT_BIG_ENDIAN)
    {
        return (int64_t)((uint64_t)(buffer[0]) << 56 |
                         (uint64_t)(buffer[1]) << 48 |
                         (uint64_t)(buffer[2]) << 40 |
                         (uint64_t)(buffer[3]) << 32 |
                         (uint64_t)(buffer[4]) << 24 |
                         (uint64_t)(buffer[5]) << 16 |
                         (uint64_t)(buffer[6]) << 8  |
                         (uint64_t)(buffer[7])
                         );
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        return (int64_t)((uint64_t)(buffer[7]) << 56 |
                         (uint64_t)(buffer[6]) << 48 |
                         (uint64_t)(buffer[5]) << 40 |
                         (uint64_t)(buffer[4]) << 32 |
                         (uint64_t)(buffer[3]) << 24 |
                         (uint64_t)(buffer[2]) << 16 |
                         (uint64_t)(buffer[1]) << 8  |
                         (uint64_t)(buffer[0]));
    }
}

float nbt_get_float(char* buffer, uint8_t endianness)
{
    float out;
    
    if(endianness == NBT_BIG_ENDIAN)
    {
        uint8_t bytes[4] = {
            (uint8_t)(buffer[3]),
            (uint8_t)(buffer[2]),
            (uint8_t)(buffer[1]),
            (uint8_t)(buffer[0])
        };
        memcpy(&out, &bytes, 4);
    }
    else if (endianness == NBT_LITTLE_ENDIAN)
    {
        uint8_t bytes[4] = {
            (uint8_t)(buffer[0]),
            (uint8_t)(buffer[1]),
            (uint8_t)(buffer[2]),
            (uint8_t)(buffer[3])
        };
        memcpy(&out, &bytes, 4);
    }
    return out;
}

double nbt_get_double(char* buffer, uint8_t endianness)
{
    double out;
    
    
    if(endianness == NBT_BIG_ENDIAN)
    {
        uint8_t bytes[8] = {
            (uint8_t)(buffer[7]),
            (uint8_t)(buffer[6]),
            (uint8_t)(buffer[5]),
            (uint8_t)(buffer[4]),
            (uint8_t)(buffer[3]),
            (uint8_t)(buffer[2]),
            (uint8_t)(buffer[1]),
            (uint8_t)(buffer[0])
        };
        memcpy(&out, &bytes, 8);
    }
    else if (endianness == NBT_LITTLE_ENDIAN)
    {
        uint8_t bytes[8] = {
            (uint8_t)(buffer[0]),
            (uint8_t)(buffer[1]),
            (uint8_t)(buffer[2]),
            (uint8_t)(buffer[3]),
            (uint8_t)(buffer[4]),
            (uint8_t)(buffer[5]),
            (uint8_t)(buffer[6]),
            (uint8_t)(buffer[7])
        };
        memcpy(&out, &bytes, 8);
    }
    return out;
}

struct NBT_TAG_NODE* node_get_child_by_name(struct NBT_TAG_NODE* node, char* name, size_t name_len)
{
    for(int i = 0; i < node->child_nodes_len; ++i)
    {
        struct NBT_TAG_NODE* child = (node->child_nodes + i);
        if (strncmp(name, child->name, name_len) == 0)
        {
            return child;
        }
    }
    return 0;
}

void node_close(struct NBT_TAG_NODE* node)
{
    if (node->child_nodes_len == 0)
    {
        return;
    }
    
    for(int i = 0; i < node->child_nodes_len; ++i)
    {
        node_close(&(node->child_nodes[i]));
    }
    
    free(node->child_nodes);
    return;
}

struct NBT_STACK get_new_stack()
{
    struct NBT_STACK stack;
    stack.buffer_pos = 0;
    return stack;
}

void close_stack(struct NBT_TAG_NODE* root)
{
    node_close(root);
}

struct NBT_TAG_NODE nbt_read_data(struct NBT_STACK* stack, char* buffer, size_t buffer_size, char* decompress_buffer, size_t decompress_buffer_len)
{
    struct NBT_TAG_NODE root_node;
    stack->root = &root_node;
    
    if (decompress_buffer_len == 0)
    {
        root_node = stack_try_get_next_node(stack, buffer, buffer_size, 0, 1);
    }
    else
    {
        nbt_decompres(buffer, buffer_size, decompress_buffer, decompress_buffer_len);
        
        root_node = stack_try_get_next_node(stack, decompress_buffer, decompress_buffer_len, 0, 1);
    }
    
    return root_node;
}

uint8_t nbt_decompres(char* buffer, size_t buffer_len, char* decompress_buffer, size_t decompress_buffer_len)
{
    if ( buffer_len == 0 ) {
        decompress_buffer = buffer;
        decompress_buffer_len = buffer_len;
        
        return 1;
    }
    
    z_stream strm;
    strm.next_in = (Bytef *) buffer;
    strm.avail_in = buffer_len;
    strm.total_out = 0;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    
    
    if (inflateInit2(&strm, (32+MAX_WBITS)) != Z_OK)
    {
        return 0;
    }
    
    strm.next_out = (Bytef *) decompress_buffer;
    strm.avail_out = decompress_buffer_len;
    
    inflate (&strm, Z_FINISH);
    
    if (inflateEnd (&strm) != Z_OK)
    {
        return 0;
    }
    return 1;
}
