#ifndef ATTENTION_H
#define ATTENTION_H

#include <Windows.h>
#include <TlHelp32.h>

#define BUFFER_SIZE 1024

//Function Prototypes
int add_process_blocklist_entry(wchar_t* process_name, wchar_t*** process_blocklist_array, int* process_count);
int add_web_blocklist_entry(const wchar_t* domain);
int block_program(PROCESSENTRY32* pe32);
int check_process_list(wchar_t*** process_blocklist, int* process_count);
int check_process_list(wchar_t*** process_blocklist, int* process_count);
void create_required_files(void);
int initialise_process_blocklist_array(wchar_t*** process_blocklist_array, int* process_count);
int remove_blank_lines(const wchar_t* file_path);
int remove_process_blocklist_entry(wchar_t* process_name, wchar_t*** process_blocklist_array, int* process_count);
int remove_web_blocklist_entry(const wchar_t* domain);
int remove_whitespace(const wchar_t* src, wchar_t* dst);
int restore_hostfile(void);
int start_block(wchar_t*** process_blocklist, int* process_count);
int update_hostfile(void);


#endif