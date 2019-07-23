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
        ++stack->buffer_pos;
        
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
        else if(tag == TAG_INT_ARRAY)
        {
            uint8_t list_type = 3;
            
            size_t alloc_size = nbt_read_len(buffer + stack->buffer_pos, stack->endianness);
            stack->buffer_pos += 4;
            
            node.child_nodes = (struct NBT_TAG_NODE*) malloc(alloc_size*sizeof(struct NBT_TAG_NODE));
            
            for(node.child_nodes_len = 0; node.child_nodes_len < alloc_size; ++node.child_nodes_len)
            {
                struct NBT_TAG_NODE node1 = stack_try_get_next_node(stack, buffer, buffer_len, list_type, 0);
                
                node.child_nodes[node.child_nodes_len] = node1;
            }
        }
        else if(tag == TAG_LONG_ARRAY)
        {
            uint8_t list_type = 4;
            
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

void stack_write_next_node(struct NBT_TAG_NODE* node, struct NBT_STACK* stack, char* buffer)
{
    //write node
    
    if (node->nbt_tag_type != 255)
    {
        nbt_write_byte(node->nbt_tag_type, buffer + stack->buffer_pos);
        ++stack->buffer_pos;
    }
    
    if(node->name_len > 0)
    {
        nbt_write_str(node->name, node->name_len, buffer + stack->buffer_pos, stack->endianness);
        stack->buffer_pos += 2 + node->name_len;
    }
    
    if(node->payload_len > 0)
    {
        for(int i = 0; i < node->payload_len; ++i)
        {
            buffer[i + stack->buffer_pos] = node->payload[i];
        }
        stack->buffer_pos += node->payload_len;
    }
    
    //write childs if child_nodes_len > 0
    if(node->child_nodes_len > 0)
    {
        for(int i = 0; i < node->child_nodes_len; ++i)
        {
            stack_write_next_node(&(node->child_nodes[i]), stack, buffer);
        }
        
        //write TAG_END if node type is TAG_COMPOUND
        if(node->nbt_tag_type == TAG_COMPOUND)
        {
            buffer[stack->buffer_pos] = TAG_END;
            ++stack->buffer_pos;
        }
    }
}

uint32_t nbt_read_uint24(char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        return (uint32_t)((uint8_t)0                |
                          (uint8_t)(buffer[0]) << 16 |
                          (uint8_t)(buffer[1]) << 8  |
                          (uint8_t)(buffer[2]));
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        return (uint32_t)((uint8_t)0                |
                          (uint8_t)(buffer[2]) << 16 |
                          (uint8_t)(buffer[1]) << 8  |
                          (uint8_t)(buffer[0]));
    }
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


void nbt_write_uint24(uint32_t val, char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        buffer[0] = val >> 16;
        buffer[1] = val >> 8;
        buffer[2] = val;
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        buffer[0] = val;
        buffer[1] = val >> 8;
        buffer[2] = val >> 16;
    }
}

void nbt_write_len(int32_t val, char* buffer, uint8_t endianness)
{
    nbt_write_int(val, buffer, endianness);
}

void nbt_write_str_len(uint16_t val, char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        buffer[0] = val >> 8;
        buffer[1] = val;
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        buffer[0] = val;
        buffer[1] = val >> 8;
    }
}

void nbt_write_str(char* str, uint16_t str_len, char* buffer, uint8_t endianness)
{
    nbt_write_str_len(str_len, buffer, endianness);
    buffer += 2;
    for(int i = 0; i < str_len; ++i)
    {
        buffer[i] = str[i];
    }
    //buffer[0] = (char)222;
}

void nbt_write_byte(int8_t val, char* buffer)
{
    buffer[0] = (uint8_t)val;
}

void nbt_write_short(int16_t val, char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        buffer[0] = val >> 8;
        buffer[1] = val;
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        buffer[0] = val;
        buffer[1] = val >> 8;
    }
}

void nbt_write_int(int32_t val, char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        buffer[0] = val >> 24;
        buffer[1] = val >> 16;
        buffer[2] = val >> 8;
        buffer[3] = val;
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        buffer[0] = val;
        buffer[1] = val >> 8;
        buffer[2] = val >> 16;
        buffer[3] = val >> 24;
    }
}

void nbt_write_long(int64_t val, char* in_buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        in_buffer[0] = val >> 56;
        in_buffer[1] = val >> 48;
        in_buffer[2] = val >> 40;
        in_buffer[3] = val >> 32;
        in_buffer[4] = val >> 24;
        in_buffer[5] = val >> 16;
        in_buffer[6] = val >> 8;
        in_buffer[7] = val;
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        in_buffer[0] = val;
        in_buffer[1] = val >> 8;
        in_buffer[2] = val >> 16;
        in_buffer[3] = val >> 24;
        in_buffer[4] = val >> 32;
        in_buffer[5] = val >> 40;
        in_buffer[6] = val >> 48;
        in_buffer[7] = val >> 56;
    }
}

void nbt_write_float(float val, char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        int32_t f;
        memcpy(&f, &val, 4);
        
        nbt_write_int(f, buffer, endianness);
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        int32_t f;
        memcpy(&f, &val, 4);
        
        int32_t f2 = 0;
        f2 = ( f << 24 )                             |
        ( ((f >> 8) << 24) >> 8 )                    |
        ( ((f >> 16) << 24) >> 16 )                  |
        (   f >> 24 );
        
        nbt_write_int(f, buffer, endianness);
    }
}

void nbt_write_double(double val, char* buffer, uint8_t endianness)
{
    if (endianness == NBT_BIG_ENDIAN)
    {
        int64_t df;
        memcpy(&df, &val, 8);
        
        nbt_write_long(df, buffer, endianness);
    }
    else if(endianness == NBT_LITTLE_ENDIAN)
    {
        int64_t df;
        memcpy(&df, &val, 8);
        
        int64_t df2 = 0;
        df2 = ( df << 56 )                           |
        (     ((df >> 8) << 56) >> 8 )               |
        (     ((df >> 16) << 56) >> 16 )             |
        (     ((df >> 24) << 56) >> 24 )             |
        (     ((df >> 32) << 56) >> 32 )             |
        (     ((df >> 40) << 56) >> 40 )             |
        (     ((df >> 48) << 56) >> 48 )             |
        (       df >> 56 );
        
        nbt_write_long(df, buffer, endianness);
    }
}

void nbt_create_node(struct NBT_TAG_NODE* node, char* name, char* payload, size_t payload_len, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root)
{
    node->name = name;
    node->name_len = strlen(name);
    
    node->payload = payload;
    node->payload_len = payload_len;
    
    node->child_nodes_len = child_nodes_len;
    node->child_nodes = child_nodes;
}

void nbt_create_list_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, uint8_t list_type, size_t child_capacity, struct NBT_TAG_NODE* child_nodes, uint8_t endianness)
{
    nbt_create_node(node, name, payload_buf, 5,
                    child_nodes,
                    child_capacity,
                    0);
    node->nbt_tag_type = TAG_LIST;
    
    nbt_write_byte(list_type, payload_buf);
    payload_buf++;
    
    nbt_write_len((int32_t)child_capacity, payload_buf, endianness);
}

void nbt_create_byte_array_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, size_t child_capacity, struct NBT_TAG_NODE* child_nodes, uint8_t endianness)
{
    nbt_create_node(node, name, payload_buf, 4,
                    child_nodes,
                    child_capacity,
                    0);
    node->nbt_tag_type = TAG_BYTE_ARRAY;
    
    nbt_write_len((int32_t)child_capacity, payload_buf, endianness);
}

void nbt_create_int_array_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, size_t child_capacity, struct NBT_TAG_NODE* child_nodes, uint8_t endianness)
{
    nbt_create_node(node, name, payload_buf, 4,
                    child_nodes,
                    child_capacity,
                    0);
    node->nbt_tag_type = TAG_INT_ARRAY;
    
    nbt_write_len((int32_t)child_capacity, payload_buf, endianness);
}

void nbt_create_long_array_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, size_t child_capacity, struct NBT_TAG_NODE* child_nodes, uint8_t endianness)
{
    nbt_create_node(node, name, payload_buf, 4,
                    child_nodes,
                    child_capacity,
                    0);
    node->nbt_tag_type = TAG_LONG_ARRAY;
    
    nbt_write_len((int32_t)child_capacity, payload_buf, endianness);
}

void nbt_create_compound_node(struct NBT_TAG_NODE* node, char* name, size_t child_capacity, struct NBT_TAG_NODE* child_nodes)
{
    nbt_create_node(node, name, 0, 0,
                    child_nodes,
                    child_capacity,
                    0);
    node->nbt_tag_type = TAG_COMPOUND;
}

void nbt_create_str_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, char* str, uint16_t str_len, uint8_t endianness)
{
    nbt_create_node(node, name, payload_buf, str_len + 2, 0, 0, 0);
    node->nbt_tag_type = TAG_STRING;
    
    nbt_write_str(str, str_len, payload_buf, endianness);
}

void nbt_create_byte_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, int8_t val, uint8_t endianness, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root)
{
    nbt_create_node(node, name, payload_buf, 1, child_nodes, child_nodes_len, root);
    node->nbt_tag_type = TAG_BYTE;
    
    nbt_write_byte(val, payload_buf);
}

void nbt_create_short_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, int16_t val, uint8_t endianness, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root)
{
    nbt_create_node(node, name, payload_buf, 2, child_nodes, child_nodes_len, root);
    node->nbt_tag_type = TAG_SHORT;
    
    nbt_write_short(val, payload_buf, endianness);
}

void nbt_create_int_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, int32_t val, uint8_t endianness, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root)
{
    nbt_create_node(node, name, payload_buf, 4, child_nodes, child_nodes_len, root);
    node->nbt_tag_type = TAG_INT;
    
    nbt_write_int(val, payload_buf, endianness);
}

void nbt_create_long_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, int64_t val, uint8_t endianness, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root)
{
    nbt_create_node(node, name, payload_buf, 8, child_nodes, child_nodes_len, root);
    node->nbt_tag_type = TAG_LONG;
    
    nbt_write_long(val, payload_buf, endianness);
}

void nbt_create_float_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, float val, uint8_t endianness, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root)
{
    nbt_create_node(node, name, payload_buf, 4, child_nodes, child_nodes_len, root);
    node->nbt_tag_type = TAG_FLOAT;
    
    nbt_write_float(val, payload_buf, endianness);
}

void nbt_create_double_node(struct NBT_TAG_NODE* node, char* name, char* payload_buf, double val, uint8_t endianness, struct NBT_TAG_NODE* child_nodes, size_t child_nodes_len, uint8_t root)
{
    nbt_create_node(node, name, payload_buf, 8, child_nodes, child_nodes_len, root);
    node->nbt_tag_type = TAG_DOUBLE;
    
    nbt_write_double(val, payload_buf, endianness);
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
