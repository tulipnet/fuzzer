#include <stddef.h>

#include "afl-fuzz-static-buffers.h"
#include "config.h"

char* wrapper_input_buffer;
char* wrapper_input_buffer2;
size_t wrapper_input_buffers_max_size = MAX_FILE + 100;
size_t wrapper_input_buffers_size = WRAPPER_MAX_INPUT_SIZE_BY_DEFAULT;
struct wildcard* global_current_wildcard = NULL;
char filename_buffer[STATIC_FILENAME_BUFFER_SIZE];
char filename_buffer2[STATIC_FILENAME_BUFFER_SIZE];
char get_full_path_of_the_first_file_that_matches_the_path_buffer[AFL_MAX_STATIC_BUFFER_SIZE];
char get_path_without_the_last_part_buffer[STATIC_FILENAME_BUFFER_SIZE];
char get_associated_wildcard_name_from_queue_entry_path_buffer[STATIC_FILENAME_BUFFER_SIZE];
char save_if_interesting_sourceFileDir_buffer[AFL_MAX_STATIC_BUFFER_SIZE];
char save_if_interesting_sourceFilePath_buffer[AFL_MAX_STATIC_BUFFER_SIZE];
char fuzz_one_original_in_buf_cpy[AFL_MAX_STATIC_BUFFER_SIZE];