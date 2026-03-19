#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include "attention.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wctype.h>
#include <wchar.h>
#include <io.h>
#include <tlHelp32.h>
#include <time.h>


#define HOSTFILE "C:\\Windows\\System32\\drivers\\etc\\hosts"
#define HOSTFILE_EDIT L"C:\\ProgramData\\AttentionService\\host_file_edit.txt"
#define TEMP_HOSTFILE_EDIT L"C:\\ProgramData\\AttentionService\\temp_blocklist.txt"

#define PROCESS_BLOCKLIST L"C:\\ProgramData\\AttentionService\\process_blocklist.txt"
#define TEMP_PROCESSBLOCKLIST L"C:\\ProgramData\\AttentionService\\temp_process_blocklist.txt"

#define ATTENTION_SERVICE_CONFIG L"C:\\ProgramData\\AttentionService\\cfg.txt"




int remove_whitespace(const wchar_t* src, wchar_t* dst) {
	//Make sure you create an output buffer as dst inside parent function!!

	int i = 0;

	if (src == NULL || *src == L'\0') {
		return 1; //Empty string or NULL pointer error
	}
	else {
		for (int j = 0; src[j] != L'\0'; j++) {
			if (!iswspace(src[j])) {
				dst[i] = src[j];
				i++;
			}
		}
		dst[i] = L'\0';
		return 0;
	}
}

void create_required_files(void) {
	HANDLE hFile;

	//Creates necessary directorys & files for program
	CreateDirectory(L"C:\\ProgramData\\AttentionService", NULL);

	//Create hostfile edit
	hFile = CreateFileW(HOSTFILE_EDIT,
		(GENERIC_READ | GENERIC_WRITE),
		(FILE_SHARE_READ | FILE_SHARE_WRITE),
		NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}

	//Create new host file
	hFile = CreateFileA("C:\\ProgramData\\AttentionService\\host.txt",
		(GENERIC_READ | GENERIC_WRITE),
		(FILE_SHARE_READ | FILE_SHARE_WRITE),
		NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}

	//Create process_blocklist file
	hFile = CreateFileW(PROCESS_BLOCKLIST,
		(GENERIC_READ | GENERIC_WRITE),
		(FILE_SHARE_READ | FILE_SHARE_WRITE),
		NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}

	//Create config file and adds contents if first time
	hFile = CreateFileW(ATTENTION_SERVICE_CONFIG,
		(GENERIC_READ | GENERIC_WRITE),
		(FILE_SHARE_READ | FILE_SHARE_WRITE),
		NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
		FILE* cfg = _wfopen(ATTENTION_SERVICE_CONFIG, L"a");
		if (cfg) {
			fputws(L"timer_minutes=2\n", cfg);
			fclose(cfg);
		}
	}
}

int remove_blank_lines(const wchar_t* file_path) {
	FILE* fp;
	FILE* tfp;
	wchar_t buffer[BUFFER_SIZE] = { 0 };
	wchar_t temp_file[] = L"C:\\ProgramData\\AttentionService\\temp_file";

	errno_t err = _wfopen_s(&tfp, temp_file, L"w");
	if (err != 0 || tfp == NULL) {
		wprintf(L"remove_web_blocklist_entry: Failed to open temp file.txt\n");
		return 1;
	}

	err = _wfopen_s(&fp, file_path, L"r");
	if (err != 0 || fp == NULL) {
		wprintf(L"remove_web_blocklist_entry: Failed to open temp file.txt\n");
		return 1;
	}

	while (fgetws(buffer, BUFFER_SIZE - 1, fp)) {
		if (wcslen(buffer) == 0 || (wcslen(buffer) == 1 && buffer[0] == L'\n')) {
			continue;
		}
		fputws(buffer, tfp);
	}

	if (ferror(tfp) != 0) {
		wprintf(L"remove_web_blocklist_entry: File error with temp file.");
		fclose(fp);
		fclose(tfp);
		return 2;
	}

	if (_wremove(file_path) == 0) {
		if (_wrename(temp_file, file_path) != 0) {
			wprintf(L"remove_web_blocklist_entry: Error trying to rename temp_file");
			return 3;
		}
	}

	fclose(fp);
	fclose(tfp);
	return 0;
}

/*
WEBSITE BLOCKER SECTION
*/

int add_web_blocklist_entry(const wchar_t* domain) {
	FILE* fp;
	wchar_t buffer[BUFFER_SIZE] = { 0 };
	wchar_t cleaned[BUFFER_SIZE];

	errno_t err = _wfopen_s(&fp, HOSTFILE_EDIT, L"a");
	if (err != 0 || fp == NULL) {
		wprintf(L"add_web_blocklist_entry: Failed to open host_file_edit.txt\n");
		return 1;
	}
	fputwc(L'\n', fp);
	remove_whitespace(domain, cleaned);
	fputws(cleaned, fp);
	fclose(fp);

	return 0;
}


int remove_web_blocklist_entry(const wchar_t* domain) {
	FILE* fp;
	wchar_t buffer[BUFFER_SIZE] = { 0 };
	FILE* tfp;

	//Open both file and temp file
	errno_t err = _wfopen_s(&fp, HOSTFILE_EDIT, L"r");
	if (err != 0 || fp == NULL) {
		wprintf(L"remove_web_blocklist_entry: Failed to open host_file_edit.txt\n");
		return 1;
	}
	err = _wfopen_s(&tfp, TEMP_HOSTFILE_EDIT, L"w");
	if (err != 0 || tfp == NULL) {
		wprintf(L"remove_web_blocklist_entry: Failed to open temp file.txt\n");
		fclose(fp);
		return 2;
	}


	while (fgetws(buffer, (BUFFER_SIZE - 1), fp)) {
		size_t buffer_len = wcslen(buffer);
		wprintf(L"Str len: %s: %zu \n", buffer, buffer_len);
		//Add endline and remove newline symbol 
		if (buffer_len > 0 && buffer[buffer_len - 1] == L'\n') {
			buffer[buffer_len - 1] = L'\0';
			wprintf(L"%s\n", buffer);
		}
		if (wcscmp(buffer, domain) != 0) {
			if (fputws(buffer, tfp) == WEOF) {
				wprintf(L"remove_web_blocklist_entry: Failed to copy to temp file.");
				fclose(fp);
				fclose(tfp);
				return 3;
			}
			if (fputwc(L'\n', tfp) == WEOF) {
				wprintf(L"remove_web_blocklist_entry: Failed to write newline.");
				fclose(fp);
				fclose(tfp);
				return 3;
			}

		}


	}


	/*
	//Checking to see if last line is blank so trailing newline can be removed
	err = _wfopen_s(&tfp, L"attention_service\\temp_blocklist.txt", L"r+b");
	if (err != 0 || tfp == NULL) {
		wprintf(L"remove_web_blocklist_entry: Failed to open temp file.txt\n");
		return 2;
	}
	fseek(tfp, -1, SEEK_END);
	if (fgetwc(tfp) == L'\n') {
		_chsize_s(_fileno(tfp), ftell(tfp) - 10);
	}
	fclose(tfp);
	I CANT GET IT TO FUCKING STOP PRINTING THE LAST NEW LINENNENENENENOIUNIURGNIEGNBUIREHBGUIERHBGNODFHBGOIFDUBNGIODSFHBGIUODFBNGIDFGIUFDBGNIPUB*/
	

	if (ferror(tfp) != 0) {
		wprintf(L"remove_web_blocklist_entry: File error with temp file.");
		fclose(fp);
		fclose(tfp);
		return 4;
	}


	if (_wremove(HOSTFILE_EDIT) == 0) {
		if (_wrename(TEMP_HOSTFILE_EDIT, HOSTFILE_EDIT) != 0) {
			wprintf(L"remove_web_blocklist_entry: Error trying to rename temp_blocklist");
			return 5;
		}
	}

	fclose(fp);
	fclose(tfp);

	return 0;
}


int update_hostfile(void) {
	FILE* fp;
	FILE* fp_converted;
	FILE* hfp_real;
	FILE* hfp_new;
	FILE* hfp_old;
	wchar_t wbuffer[BUFFER_SIZE] = { 0 };
	char buffer[BUFFER_SIZE] = { 0 };


	//Open host file edit
	errno_t err = _wfopen_s(&fp, HOSTFILE_EDIT, L"r");
	if (err != 0 || fp == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open host_file_edit.txt\n");
		return 1;
	}

	//Open converted edit file
	err = fopen_s(&fp_converted, "C:\\ProgramData\\AttentionService\\host_file_edit_converted.txt", "w");
	if (err != 0 || fp_converted == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open converted blocklist file.\n");
		return 1;
	}

	//Read old host file
	err = fopen_s(&hfp_real, HOSTFILE, "r");
	if (err != 0 || hfp_real == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open original host file.\n");
		return 1;
	}

	//Create new host file
	err = fopen_s(&hfp_new, "C:\\ProgramData\\AttentionService\\hosts_new.txt", "w");
	if (err != 0 || hfp_new == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open new host file\n");
		return 1;
	}

	//Create old hostfile store
	err = fopen_s(&hfp_old, "C:\\ProgramData\\AttentionService\\hosts_old.txt", "w");
	if (err != 0 || hfp_old == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open old host file\n");
		return 1;
	}

	//Read old hostfile into new hostfile and save old hostfile
	while (fgets(buffer, (BUFFER_SIZE - 1), hfp_real)) {
		fprintf_s(hfp_new, "%s", buffer);
		fprintf_s(hfp_old, "%s", buffer);
	}
	fclose(hfp_real);

//Start entry for block list
fprintf_s(hfp_new, "\n#Start of entries inserted by Attention Service\n");
//Convert host file edit to utf8 and format for hostfile
remove_blank_lines(HOSTFILE_EDIT);
while (fgetws(wbuffer, (BUFFER_SIZE - 1), fp)) {
	if (WideCharToMultiByte(CP_UTF8, 0, wbuffer, -1, buffer, BUFFER_SIZE, NULL, NULL) == 0) {
		wprintf(L"Update_hostfile: Couldnt convert utf16 to utf8");
		return 2;
	}
	//Check if newline is already in string and append to new host file accordingly
	if (buffer[strlen(buffer) - 1] == '\n') {
		if (fprintf_s(hfp_new, "0.0.0.0 %s", buffer) < 0) {
			wprintf(L"Update_hostfile: Couldnt print converted string");
		}
	}
	else {
		if (fprintf_s(hfp_new, "0.0.0.0 %s\n", buffer) < 0) {
			wprintf(L"Update_hostfile: Couldnt print converted string");
		}
	}
}
//End entry for blocklist
fprintf_s(hfp_new, "\n#End of entries inserted by Attention Service\n");

fclose(hfp_new);
err = fopen_s(&hfp_new, "C:\\ProgramData\\AttentionService\\hosts_new.txt", "r");
if (err != 0 || hfp_new == NULL) {
	wprintf(L"add_web_blocklist_to_host_file: Failed to open new host file\n");
	return 1;
}

//Open real hostfile in write mode
err = fopen_s(&hfp_real, HOSTFILE, "w");
if (err != 0 || hfp_real == NULL) {
	wprintf(L"add_web_blocklist_to_host_file: Failed to open original host file.\n");
	return 1;
}

//Copy new hostfile into actual hostfile
while (fgets(buffer, (BUFFER_SIZE - 1), hfp_new)) {
	fprintf_s(hfp_real, "%s", buffer);
}


fclose(fp);
fclose(fp_converted);
fclose(hfp_real);
fclose(hfp_new);
fclose(hfp_old);
return 0;
}

int restore_hostfile(void) {
	FILE* hfp_real;
	FILE* hfp_old;
	char buffer[BUFFER_SIZE] = { 0 };

	//Read old host file
	errno_t err = fopen_s(&hfp_old, "C:\\ProgramData\\AttentionService\\hosts_old.txt", "r");
	if (err != 0 || hfp_old == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open old host file\n");
		return 1;
	}

	//Open actual hostfile
	err = fopen_s(&hfp_real, HOSTFILE, "w");
	if (err != 0 || hfp_real == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open original host file.\n");
		return 1;
	}

	//Copy old host file to the updated hostfile, restoring it to its original state
	while (fgets(buffer, (BUFFER_SIZE - 1), hfp_old)) {
		printf("%s", buffer);
		fprintf_s(hfp_real, "%s", buffer);
	}

	fclose(hfp_old);
	fclose(hfp_real);
	return 0;
}

/*
PROGRAM BLOCKER SECTION
*/
int initialise_process_blocklist_array(wchar_t*** process_blocklist_array, int* process_count) {
	//If changing the preset blocked processes make sure to change the preset count aswell
	const wchar_t* security_preset[] = { L"cmd.exe", L"powershell.exe", L"mmc.exe" , L"taskmgr.exe"};
	const int preset_count = 4;

	FILE* fp;
	wchar_t buffer[BUFFER_SIZE] = { 0 };


	errno_t err = _wfopen_s(&fp, PROCESS_BLOCKLIST, L"r");
	if (err != 0 || fp == NULL) {
		wprintf(L"initialise_process_blocklist_array: Failed to open process_blocklist\n");
		return 1;
	}


	while (fgetws(buffer, (BUFFER_SIZE - 1), fp)) {
		size_t buffer_len = wcslen(buffer);

		//Remove newline from buffer
		if (buffer_len > 0 && buffer[buffer_len - 1] == L'\n') {
			buffer[buffer_len - 1] = L'\0';
		}

		//Create array
		wchar_t** tempptr = realloc(*process_blocklist_array, (++(*process_count) * sizeof(wchar_t*)));
		if (tempptr == NULL) {
			wprintf(L"intialise_process_blocklist_array: Reallocation failed\n");
			return 1;
		}
		else {
			*process_blocklist_array = tempptr;
		}

		(*process_blocklist_array)[*process_count - 1] = malloc((sizeof(wchar_t) * BUFFER_SIZE));
		if ((*process_blocklist_array)[*process_count - 1] != 0) {
			wcscpy_s((*process_blocklist_array)[*process_count - 1], BUFFER_SIZE, buffer);
		}
	}

	//Add possible processes that could end the service to the end of the array
	wchar_t** tempptr = realloc(*process_blocklist_array, ((*process_count + preset_count) * sizeof(wchar_t*)));
	if (tempptr == NULL) {
		wprintf(L"intialise_process_blocklist_array: Reallocation failed\n");
		return 1;
	}
	else {
		*process_blocklist_array = tempptr;
	}

	for (int i = 0; i < preset_count; i++) {
		(*process_blocklist_array)[(*process_count)++] = _wcsdup(security_preset[i]);
	}

	fclose(fp);

	return 0;
}


int add_process_blocklist_entry(wchar_t* process_name, wchar_t*** process_blocklist_array, int* process_count) {
	FILE* fp;
	size_t process_name_len = wcslen(process_name);

	errno_t err = _wfopen_s(&fp, PROCESS_BLOCKLIST, L"a");
	if (err != 0 || fp == NULL) {
		wprintf(L"add_process_blocklist_entry: Failed to open process_blocklist.txt\n");
		return 1;
	}
	fputwc(L'\n', fp);
	fputws(process_name, fp);
	fclose(fp);

	wchar_t** tempptr = realloc(*process_blocklist_array, (++(*process_count) * sizeof(wchar_t*)));
	if (tempptr == NULL) {
		wprintf(L"add_process_blocklist_entry: Reallocation failed\n");
		return 1;
	}
	else {
		*process_blocklist_array = tempptr;
	}

	(*process_blocklist_array)[*process_count - 1] = malloc((process_name_len * sizeof(wchar_t)));
	if ((*process_blocklist_array)[*process_count - 1] != 0) {
		wcscpy_s((*process_blocklist_array)[*process_count - 1], process_name_len + 1, process_name);
	}

	for (int i = 0; i < *process_count; i++) {
		wprintf(L"%s\n", (*process_blocklist_array)[i]);
	}

	return 0;
}

int add_process_blocklist_entry_fileonly(wchar_t* process_name) {
	//Doesn't work right now!!
	FILE* fp;
	size_t process_name_len = wcslen(process_name);

	errno_t err = _wfopen_s(&fp, PROCESS_BLOCKLIST, L"a");
	if (err != 0 || fp == NULL) {
		wprintf(L"add_process_blocklist_entry: Failed to open process_blocklist.txt\n");
		return 1;
	}
	fputwc(L'\n', fp);
	fputws(process_name, fp);
	fclose(fp);
	return 0;
}

int remove_process_blocklist_entry(wchar_t* process_name, wchar_t*** process_blocklist_array, int* process_count) {
	int found = 0;

	//Look for and remove process from array
	for (int i = 0; i < *process_count; i++) {
		if (lstrcmpiW((*process_blocklist_array)[i], process_name) == 0) {
			for (int j = i; j < *process_count - 1; j++) {
				(*process_blocklist_array)[j] = (*process_blocklist_array)[j + 1];
			}
			(*process_count)--;
			found = 1;
			break;
		}
	}

	if (!found) {
		wprintf(L"Could not find entry\n");
		return 1;
	}

	//Remove found process from process_blocklist file

	FILE* fp;
	errno_t err = _wfopen_s(&fp, PROCESS_BLOCKLIST, L"w");
	if (err != 0 || fp == NULL) {
		wprintf(L"remove_process_blocklist_entry: Failed to open process blocklist\n");
		return 1;
	}

	for (int i = 0; i < *process_count; i++) {
		fwprintf_s(fp, L"%s\n", (*process_blocklist_array)[i]);
	}

	fclose(fp);
	
	return 0;
	
}

int remove_process_blocklist_entry_fileonly(wchar_t* process_name, wchar_t*** process_blocklist_array, int* process_count) {
	//Remove found process from process_blocklist file
	FILE* fp;
	errno_t err = _wfopen_s(&fp, PROCESS_BLOCKLIST, L"w");
	if (err != 0 || fp == NULL) {
		wprintf(L"remove_process_blocklist_entry: Failed to open process blocklist\n");
		return 1;
	}

	for (int i = 0; i < *process_count; i++) {
		fwprintf_s(fp, L"%s\n", (*process_blocklist_array)[i]);
	}

	fclose(fp);

	return 0;

}

int check_process_list(wchar_t*** process_blocklist, int* process_count) {
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;
	
	DWORD blocked_pids[50];
	int blocked_pids_count = 0;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	//Take snapshot of all processes in system
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		wprintf(L"check_process_list: Couldnt get ProcessSnap(INVALID_HANDLE_VALUE)");
		return 1;
	}

	//Check first process
	if (!Process32First(hProcessSnap, &pe32)) {
		wprintf(L"check_process_list: Problem with first process.");
		CloseHandle(hProcessSnap);
		return 1;
	}
	
	//Compare process name to list of blocked processes and terminates them if found
	do {
		//wprintf(L"Process: %s\n", pe32.szExeFile);
		int already_blocked = 0;
		for (int j = 0; j < blocked_pids_count; j++) {
			if (blocked_pids[j] == pe32.th32ParentProcessID) {
				already_blocked = 1;
				break;
			}
		}

		if (already_blocked != 1) {
			for (int i = 0; i < *process_count; i++) {
				if (_wcsicmp((*process_blocklist)[i], pe32.szExeFile) == 0) {
					int block_result = block_program(&pe32); // If returns -1 that means process is closed already
					if (block_result == 0) {
						blocked_pids[blocked_pids_count] = pe32.th32ParentProcessID;
						blocked_pids_count++;
					}
					else if (block_result == -1) {
						wprintf(L"Process already closed\n");
					}
				}
			}
		}

	} while (Process32Next(hProcessSnap, &pe32));
	
	return 0;
}

int block_program(PROCESSENTRY32* pe32) {
	HANDLE hProcess_terminate;
	DWORD dwExitCode;

	//Open process with terminate permission
	hProcess_terminate = OpenProcess(PROCESS_TERMINATE, TRUE, (*pe32).th32ProcessID);
	
	//Check if process has been closed already
	if (hProcess_terminate == NULL) {
		return -1;
	}
	else if (hProcess_terminate == INVALID_HANDLE_VALUE) {
		wprintf(L"block_program: Process_terminates is an invalid handle\n");
		return 1;
	}

	//Terminate process and ignore the bloody fact that half the time it wont be able to terminate anything (:
	GetExitCodeProcess(hProcess_terminate, &dwExitCode);
	TerminateProcess(hProcess_terminate, dwExitCode);


	CloseHandle(hProcess_terminate);
	return 0;
}


int start_block(wchar_t*** process_blocklist, int* process_count) {
	int time_buffer = 2;
	int time_in_seconds;
	time_t start_time;
	int end_time;

	FILE* fp = _wfopen(ATTENTION_SERVICE_CONFIG, L"r");
	wchar_t buffer[BUFFER_SIZE];
	
	/*For terminal input (not useful in service)
	wprintf_s(L"Enter how long you want to focus for in hours: ");
	wscanf_s(L"%f", &time_buffer);
	wprintf_s(L"\n");
	wprintf_s(L"Time: %f\n", time_buffer);
	*/

	//Get time in minutes from config file
	while (fgetws(buffer, BUFFER_SIZE, fp)) {
		if (wcsstr(buffer, L"timer_minutes=")) {
			swscanf_s(buffer, L"timer_minutes=%d", &time_buffer);
		}
	}

	time_in_seconds = time_buffer * 60;
	
	start_time = time(NULL);
	end_time = start_time + time_in_seconds;
	
	//Add hostfile blocklist
	update_hostfile();

	//Run process blocker
	while (time(NULL) != end_time) {
		check_process_list(process_blocklist, process_count);
	}

	//Remove hostfile blocklist once time is up.
	restore_hostfile();

	return 0;
} 

int main(void) {
	//Initialise process blocklist
	wchar_t** process_blocklist_array = NULL;
	int process_count = 0;

	create_required_files();


	initialise_process_blocklist_array(&process_blocklist_array, &process_count);

	wprintf(L"Original array\n");
	for (int i = 0; i < process_count; i++) {
		wprintf(L"%s, ", process_blocklist_array[i]);

	}
	wprintf(L"\n");

	/*
	add_process_blocklist_entry(L"chrome.exe", &process_blocklist_array, &process_count);

	wprintf(L"After addition\n");
	for (int i = 0; i < process_count; i++) {
		wprintf(L"%s, ", process_blocklist_array[i]);

	}
	wprintf(L"\n");
	*/

	remove_process_blocklist_entry(L"6th", &process_blocklist_array, &process_count);

	wprintf(L"After removal\n");
	for (int i = 0; i < process_count; i++) {
		wprintf(L"%s, ", process_blocklist_array[i]);

	}
	wprintf(L"\n");

	return 0;
}

//Sample main() for reference later
/*
int main(void) {
	//Initialise process blocklist
	wchar_t** process_blocklist_array = NULL;
	int process_count = 0;

	create_required_files();


	initialise_process_blocklist_array(&process_blocklist_array, &process_count);

	//add_process_blocklist_entry(L"chrome.exe", &process_blocklist_array, &process_count);

	start_block(&process_blocklist_array, &process_count);

	//add_web_blocklist_entry(L"youtu.be");
	//remove_web_blocklist_entry(L"idiot.com");
	//update_hostfile();
	//restore_hostfile();
	//add_process_blocklist_entry(wscanf_s("%s", &count), )
		
	wprintf(L"Program succesfully ended\n");
	return 0;
}
*/