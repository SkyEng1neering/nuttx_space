#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "fileXferCallbacks.h"
#include "fileXferConf.h"
#include "fileXferUtils.h"

#define STORAGE_PATH           "/mnt/tmp"
#define PATH_LEN_MAX           64

char str_buf[PATH_LEN_MAX];

void files_list_gotten_cb(uint8_t files_num, uint8_t *files_names_arr) {
    (void)files_num;
    (void)files_names_arr;
}

void form_files_list_cb(uint8_t *payload_ptr, uint16_t free_space, uint16_t *payload_len) {
    DIR *d;
    uint16_t arr_offset = 1;
    uint8_t files_cnt = 0;

    struct dirent *dir;
    d = opendir(STORAGE_PATH);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            uint8_t len = strlen(dir->d_name);
            if (free_space - arr_offset < 1 + len) {
                break;
            }
            payload_ptr[arr_offset] = len;
            arr_offset++;
            memcpy(&payload_ptr[arr_offset], dir->d_name, len);
            arr_offset += len;
            files_cnt++;


            printf("%s\n", dir->d_name);
        }
        payload_ptr[0] = files_cnt;
        *payload_len = arr_offset;
        closedir(d);
    }
}

void file_hash_gotten_cb(uint32_t *file_hash) {
    (void)file_hash;
}

bool get_file_hash_cb(const char *file_name, uint32_t *file_hash) {
    snprintf(str_buf, PATH_LEN_MAX, "%s/%s", STORAGE_PATH, file_name);
    
    FILE *file = fopen(str_buf, "r");
    if (file == NULL) {
        return false;
    }
    if (crc32_compute_file(file, file_hash) != 0) {
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

bool get_file_size_cb(const char *file_name, uint32_t *file_size) {
    (void)file_name;
    (void)file_size;
    return true;
}

bool file_read_partial_cb(const char *file_name, uint32_t offset,
        uint32_t chunc_size, uint8_t *out_buf) {
    (void)file_name;
    (void)offset;
    (void)chunc_size;
    (void)out_buf;
    return true;
}

bool file_append_cb(const char *file_name, uint32_t chunc_size,
        uint8_t *in_buf, bool *eof_flag) {
    snprintf(str_buf, PATH_LEN_MAX, "%s/%s", STORAGE_PATH, file_name);

    FILE *file = fopen(file_name, "a");
    if (file == NULL) {
        return false;
    }
    uint16_t num = fwrite(in_buf, 1, chunc_size, file);
    if (num != chunc_size) {
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}
