#include <stdio.h>
#include <stdlib.h>
#include "nbt.h"

char* read_file(char *filename, size_t* len)
{
    char *buffer = NULL;
    int string_size, read_size;
    FILE *handler = fopen(filename, "r");
    
    if (handler)
    {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);
        
        buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
        *len = string_size + 1;
        read_size = fread(buffer, sizeof(char), string_size, handler);
        
        buffer[string_size] = '\0';
        
        if (string_size != read_size)
        {
            free(buffer);
            buffer = NULL;
        }
        
        fclose(handler);
    }
    
    return buffer;
}

void print_value(uint8_t type, char* buf, size_t buf_len, uint8_t endianness)
{
    if(type == TAG_BYTE)
    {
        printf(" [%d]", (int)(nbt_get_byte(buf)));
    }
    else if(type == TAG_SHORT)
    {
        printf(" [%d]", nbt_get_short(buf, endianness));
    }
    else if(type == TAG_INT)
    {
        printf(" [%d]", nbt_get_int(buf, endianness));
    }
    else if(type == TAG_LONG)
    {
        printf(" [%lld]", nbt_get_long(buf, endianness));
    }
    else if(type == TAG_FLOAT)
    {
        printf(" [%f]", nbt_get_float(buf, endianness));
    }
    else if(type == TAG_DOUBLE)
    {
        printf(" [%f]", nbt_get_double(buf, endianness));
    }
    else if(type == TAG_STRING)
    {
        printf(" [%.*s]", (int)buf_len, buf);
    }
}

int main()
{
    int x = 0;
    int z = 0;
    
    char path[300];
    sprintf(path, "%s/r.%d.%d.mca", ".", x >> 5, z >> 5);
    
    size_t len = 0;
    char *string = read_file(path, &len);
    if (!string)
    {
        printf("region file not found!");
        return 0;
    }
    
    const int CAP = 5 * 64 * 1024;
    char decompress_buf[CAP];
    size_t decompress_buf_len = CAP;
    if (string)
    {
        struct NBT_STACK stack = get_new_stack(); //open read session
        stack.endianness = NBT_BIG_ENDIAN;
        
        uint32_t content_table_offset = 4 * ((x & 31) + (z & 31) * 32); //xz-indexed
        uint32_t chunk_offset = nbt_read_uint24(string + content_table_offset, stack.endianness) * 4096;
        //skipped padded size byte here
        
        uint32_t chunk_size = nbt_read_len(string + chunk_offset, stack.endianness); //len without 4096 align
        //skipped compression byte here
        
        struct NBT_TAG_NODE root_node = nbt_read_data(&stack, string + chunk_offset + 5, chunk_size, decompress_buf, decompress_buf_len);
        
        struct NBT_TAG_NODE* chunk_node = node_get_child_by_name(&root_node, "Level", 5);
        
        for(int i = 0; i < chunk_node->child_nodes_len; ++i)
        {
            struct NBT_TAG_NODE current_node = chunk_node->child_nodes[i];
            
            printf("- %d (%.*s) ", current_node.nbt_tag_type, (int)current_node.name_len, current_node.name);
            print_value(current_node.nbt_tag_type, current_node.payload, current_node.payload_len, stack.endianness);
            printf("\n");
        }
        
        printf("-----Sections-----\n");
        
        struct NBT_TAG_NODE* sections_node = node_get_child_by_name(chunk_node, "Sections", 8);
        
        for(int i = 0; i < sections_node->child_nodes_len; ++i)
        {
            struct NBT_TAG_NODE current_node = sections_node->child_nodes[i];
            
            
            
            printf("- %d (%.*s) ", current_node.nbt_tag_type, (int)current_node.name_len, current_node.name);
            print_value(current_node.nbt_tag_type, current_node.payload, current_node.payload_len, stack.endianness);
            printf("\n");
            
            for(int j = 0; j < current_node.child_nodes_len; ++j)
            {
                struct NBT_TAG_NODE section_node = current_node.child_nodes[j];
                
                printf("-- %d (%.*s) ", section_node.nbt_tag_type, (int)section_node.name_len, section_node.name);
                print_value(section_node.nbt_tag_type, section_node.payload, section_node.payload_len, stack.endianness);
                printf("\n");
            }
        }
        
        close_stack(&root_node); //end read session, nodes can no longer be accessed
    }
    return 0;
}

