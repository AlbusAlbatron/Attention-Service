#ifndef ATTENTION_H
#define ATTENTION_H

#include <Windows.h>
#include <TlHelp32.h>

#define BUFFER_SIZE 1024

#define HOSTFILE "C:\\Windows\\System32\\drivers\\etc\\hosts"
#define HOSTFILE_EDIT L"C:\\ProgramData\\AttentionService\\host_file_edit.txt"
#define TEMP_HOSTFILE_EDIT L"C:\\ProgramData\\AttentionService\\temp_blocklist.txt"

#define PROCESS_BLOCKLIST L"C:\\ProgramData\\AttentionService\\process_blocklist.txt"
#define TEMP_PROCESSBLOCKLIST L"C:\\ProgramData\\AttentionService\\temp_process_blocklist.txt"

#define ATTENTION_SERVICE_CONFIG L"C:\\ProgramData\\AttentionService\\cfg.txt"

//Function Prototypes
__declspec(dllexport) int add_process_blocklist_entry(wchar_t* process_name, wchar_t*** process_blocklist_array, int* process_count);
__declspec(dllexport)int add_web_blocklist_entry(const wchar_t* domain);
__declspec(dllexport)int block_program(PROCESSENTRY32* pe32);
__declspec(dllexport)int check_process_list(wchar_t*** process_blocklist, int* process_count);
__declspec(dllexport)int check_process_list(wchar_t*** process_blocklist, int* process_count);
__declspec(dllexport)void create_required_files(void);
__declspec(dllexport)int initialise_process_blocklist_array(wchar_t*** process_blocklist_array, int* process_count);
__declspec(dllexport)int remove_blank_lines(const wchar_t* file_path);
__declspec(dllexport)int remove_process_blocklist_entry(wchar_t* process_name, wchar_t*** process_blocklist_array, int* process_count);
__declspec(dllexport)int remove_web_blocklist_entry(const wchar_t* domain);
__declspec(dllexport)int remove_whitespace(const wchar_t* src, wchar_t* dst);
__declspec(dllexport)int restore_hostfile(void);
__declspec(dllexport)int start_block(wchar_t*** process_blocklist, int* process_count);
__declspec(dllexport)int update_hostfile(void);
__declspec(dllexport)int add_process_blocklist_entry_fileonly(wchar_t* process_name);
__declspec(dllexport)int remove_process_blocklist_entry_fileonly(wchar_t* process_name);

#endif