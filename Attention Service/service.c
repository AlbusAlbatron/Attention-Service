#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#define BUFFER_SIZE 1024

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wctype.h>
#include <wchar.h>
#include <io.h>


const char HOSTFILE[] = "C:\\Windows\\System32\\drivers\\etc\\hosts";
const wchar_t HOSTFILE_EDIT[] = L"G:\\C Project - Attention Service\\Attention Service\\Attention Service\\attention_service\\host_file_edit.txt";
const wchar_t TEMP_HOSTFILE_EDIT[] = L"G:\\C Project - Attention Service\\Attention Service\\Attention Service\\attention_service\\temp_blocklist.txt";

void checker(int result, const wchar_t* func_name) {
	if (result != 0) {
		wprintf(L"%s returned error code: %d\n", func_name, result);
	}
}


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
	//Creates necessary directorys & files for program
	CreateDirectory(L"G:\\C Project - Attention Service\\Attention Service\\Attention Service\\attention_service", NULL);
	CreateFile(HOSTFILE_EDIT,
				(GENERIC_READ | GENERIC_WRITE),
				(FILE_SHARE_READ | FILE_SHARE_WRITE),
				NULL, CREATE_NEW,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	CreateFileA("G:\\C Project - Attention Service\\Attention Service\\Attention Service\\attention_service\\host.txt",
		(GENERIC_READ | GENERIC_WRITE),
		(FILE_SHARE_READ | FILE_SHARE_WRITE),
		NULL, CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}


int add_web_blocklist_entry(const wchar_t* domain) {
	FILE* fp;
	wchar_t buffer[BUFFER_SIZE] = { 0 };
	wchar_t cleaned[BUFFER_SIZE];

	errno_t err = _wfopen_s(&fp, L"attention_service\\host_file_edit.txt", L"a");
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
	errno_t err = _wfopen_s(&fp, L"attention_service\\host_file_edit.txt", L"r");
	if (err != 0 || fp == NULL) {
		wprintf(L"remove_web_blocklist_entry: Failed to open host_file_edit.txt\n");
		return 1;
	}
	err = _wfopen_s(&tfp, L"attention_service\\temp_blocklist.txt", L"w");
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

	fclose(fp);
	fclose(tfp);
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
	FILE* hfp_old;
	FILE* hfp_new = NULL;
	wchar_t wbuffer[BUFFER_SIZE] = { 0 };
	char buffer[BUFFER_SIZE] = { 0 };



	//Read list of edits
	errno_t err = _wfopen_s(&fp, L"attention_service\\host_file_edit.txt", L"r");
	if (err != 0 || fp == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open host_file_edit.txt\n");
		return 1;
	}

	//Read old host file
	err = fopen_s(&hfp_old, HOSTFILE, "r");
	if (err != 0 || hfp_old == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open original host file.\n");
		return 1;
	}

	//Create new host file
	err = fopen_s(&hfp_new, "G:\\C Project - Attention Service\\Attention Service\\Attention Service\\attention_service\\host.txt", "w");
	if (err != 0 || hfp_new == NULL) {
		wprintf(L"add_web_blocklist_to_host_file: Failed to open new host file\n");
		return 1;
	}

	//Read old hostfile into new hostfile
	while (fgets(buffer, (BUFFER_SIZE - 1), hfp_old)) {
		fprintf_s(hfp_new, "%s", buffer);
	}


	while (fgetws(wbuffer, BUFFER_SIZE, fp) != NULL) {
		wchar_t cleaned[BUFFER_SIZE];
		for (int i = 0; buffer[i] != L'\0'; i++) {
			if (iswspace(buffer[i])) {
				continue;
			}

			remove_whitespace(wbuffer, cleaned);
			wprintf(L"File Contents:\n%s\n", cleaned);
			break;
		}
	}

	fclose(fp);
	fclose(hfp_new);
	fclose(hfp_old);
	return 0;
}

int main(void) {
	int result;

	create_required_files();
	//add_web_blocklist_entry(L"youtu.be");
	result = remove_web_blocklist_entry(L"idiot.com");
	checker(result, L"remove_web_blocklist_entry");
	update_hostfile();
	return 0;
}
