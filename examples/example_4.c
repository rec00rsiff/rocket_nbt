#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
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
    
    struct NBT_STACK write_stack = get_new_stack(); //open write session
    write_stack.endianness = NBT_BIG_ENDIAN;
    
    struct NBT_TAG_NODE wr_root_node;
    const char* root_name = "root_compound";
    
    struct NBT_TAG_NODE child_nodes[9];
    nbt_create_compound_node(&wr_root_node, (char*)root_name, 9, child_nodes);
    
    char write_int_buf[4];
    const char* name = "x";
    
    struct NBT_TAG_NODE wr_child_node;
    nbt_create_int_node(
                        &wr_child_node,
                        (char*)name,
                        write_int_buf,
                        217,
                        write_stack.endianness,
                        0,
                        0,
                        0);
    
    char write_float_buf[4];
    const char* name1 = "y";
    
    struct NBT_TAG_NODE wr_child_node1;
    nbt_create_float_node(
                          &wr_child_node1,
                          (char*)name1,
                          write_float_buf,
                          -74.3888888,
                          write_stack.endianness,
                          0,
                          0,
                          0);
    
    char write_long_buf[8];
    const char* name2 = "z";
    
    struct NBT_TAG_NODE wr_child_node2;
    nbt_create_long_node(
                         &wr_child_node2,
                         (char*)name2,
                         write_long_buf,
                         9223372036854775807,
                         write_stack.endianness,
                         0,
                         0,
                         0);
    
    char write_short_buf[4];
    const char* name3 = "short";
    
    struct NBT_TAG_NODE wr_child_node3;
    nbt_create_short_node(
                          &wr_child_node3,
                          (char*)name3,
                          write_short_buf,
                          32767,
                          write_stack.endianness,
                          0,
                          0,
                          0);
    
    const char* str = "hello_world!";
    uint16_t str_len = strlen(str);
    char write_str_buf[str_len + 2];
    const char* name4 = "str";
    
    struct NBT_TAG_NODE wr_child_node4;
    nbt_create_str_node(
                        &wr_child_node4,
                        (char*)name4,
                        write_str_buf,
                        (char*)str,
                        str_len,
                        write_stack.endianness);
    
    char write_double_buf[8];
    const char* name5 = "double_test";
    
    struct NBT_TAG_NODE wr_child_node5;
    nbt_create_double_node(
                           &wr_child_node5,
                           (char*)name5,
                           write_double_buf,
                           0.49312871321823148,
                           write_stack.endianness,
                           0,
                           0,
                           0);
    
    char write_list_buf[5]; //type - 1 byte, len - 4 bytes
    const char* name6 = "list_test";
    struct NBT_TAG_NODE list_child_nodes[2];
    
    struct NBT_TAG_NODE wr_child_node6;
    nbt_create_list_node(&wr_child_node6,
                         (char*)name6,
                         write_list_buf,
                         TAG_INT,
                         2,
                         list_child_nodes,
                         write_stack.endianness);
    
    char write_int_buf1[4];
    const char* lc_name = "";
    
    struct NBT_TAG_NODE wr_list_child_node;
    nbt_create_int_node(
                        &wr_list_child_node,
                        (char*)lc_name,
                        write_int_buf1,
                        777,
                        write_stack.endianness,
                        0,
                        0,
                        0);
    wr_list_child_node.nbt_tag_type = 255; //no tag type writing
    wr_child_node6.child_nodes[0] = wr_list_child_node;
    wr_child_node6.child_nodes[1] = wr_list_child_node;
    
    struct NBT_TAG_NODE wr_comp_node;
    const char* comp_name = "nested_compound";
    
    struct NBT_TAG_NODE comp_child_nodes[2];
    nbt_create_compound_node(&wr_comp_node, (char*)comp_name, 2, comp_child_nodes);
    
    const char* cc_str = "nested compound test";
    uint16_t cc_str_len = strlen(cc_str);
    char write_cc_str_buf[cc_str_len + 2];
    const char* cc_name = "name";
    
    struct NBT_TAG_NODE wr_comp_child_node;
    nbt_create_str_node(
                        &wr_comp_child_node,
                        (char*)cc_name,
                        write_cc_str_buf,
                        (char*)cc_str,
                        cc_str_len,
                        write_stack.endianness);
    
    char write_cc_short_buf[4];
    const char* cc_name1 = "val";
    
    struct NBT_TAG_NODE wr_comp_child_node1;
    nbt_create_short_node(
                          &wr_comp_child_node1,
                          (char*)cc_name1,
                          write_cc_short_buf,
                          1464,
                          write_stack.endianness,
                          0,
                          0,
                          0);
    
    comp_child_nodes[0] = wr_comp_child_node;
    comp_child_nodes[1] = wr_comp_child_node1;
    
    wr_root_node.child_nodes[0] = wr_child_node;
    wr_root_node.child_nodes[1] = wr_child_node1;
    wr_root_node.child_nodes[2] = wr_child_node2;
    wr_root_node.child_nodes[3] = wr_child_node3;
    wr_root_node.child_nodes[4] = wr_child_node4;
    wr_root_node.child_nodes[5] = wr_child_node5;
    wr_root_node.child_nodes[6] = wr_child_node6;
    wr_root_node.child_nodes[7] = wr_comp_node;
    wr_root_node.child_nodes[8] = wr_child_node1;
    
    char write_buf[1024];
    bzero(write_buf, 1024);
    stack_write_next_node(&wr_root_node, &write_stack, write_buf);
    
    for(int i = 0; i < 1024; ++i)
    {
        printf("%d ", (int)(write_buf[i]));
    }
    printf("\n");
    
    const int CAP = 64 * 1024;
    char decompress_buf[CAP];
    size_t decompress_buf_len = 0;
    if (write_buf)
    {
        struct NBT_STACK stack = get_new_stack(); //open read session
        stack.endianness = NBT_BIG_ENDIAN;
        
        struct NBT_TAG_NODE root_node = nbt_read_data(&stack, write_buf, len, decompress_buf, decompress_buf_len);
        
        printf("root_name: %.*s , name_len: %d\n", (int)root_node.name_len, root_node.name, (int)root_node.name_len);
        printf("root_childs: %d\n", (int)root_node.child_nodes_len);
        for(int i = 0; i < root_node.child_nodes_len; ++i)
        {
            struct NBT_TAG_NODE current_node = root_node.child_nodes[i];
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

