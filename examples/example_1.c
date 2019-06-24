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
    size_t len = 0;
    char *string = read_file("bigtest.nbt", &len);
    
    const int CAP = 64 * 1024;
    char decompress_buf[CAP];
    size_t decompress_buf_len = CAP;
    if (string)
    {
        struct NBT_STACK stack = get_new_stack(); //open read session
        stack.endianness = NBT_BIG_ENDIAN;
        
        struct NBT_TAG_NODE root_node = nbt_read_data(&stack, string, len, decompress_buf, decompress_buf_len);
        
        printf("root_childs: %d\n", (int)root_node.child_nodes_len);
        for(int i = 0; i < root_node.child_nodes_len; ++i)
        {
            struct NBT_TAG_NODE current_node = root_node.child_nodes[i];
            printf("- %d (%.*s) ", current_node.nbt_tag_type, (int)current_node.name_len, current_node.name);
            print_value(current_node.nbt_tag_type, current_node.payload, current_node.payload_len, stack.endianness);
            printf("\n");
        }
        
        close_stack(&root_node); //end read session, nodes can no longer be accessed
    }
    return 0;
}

