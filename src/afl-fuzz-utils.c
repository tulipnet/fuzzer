#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "afl-fuzz-static-buffers.h"
#include "afl-fuzz.h"
#include "debug.h"

char* get_filename(char* path)
{
  size_t path_length = strlen(path);
  char* path_cursor = path + path_length - 1;

  while ((path_cursor != path) && (*path_cursor != '/'))
  {
    path_cursor--;
  }

  if (*path_cursor == '/')
  {
    path_cursor++;
  }

  char* res = filename_buffer2;

  strcpy(res, path_cursor);

  return res;
}

char* get_path_without_the_last_part(char* __path) // foo/bar/toto => foo/bar
{
  char* result = get_path_without_the_last_part_buffer;
  char* last_slash_ptr;
  size_t len;

  last_slash_ptr = strrchr(__path, '/'); // We take a pointer on the last '/' of the path
  len = strlen(__path) - strlen(last_slash_ptr);

  strncpy(result, __path, len);

  result[len] = '\0';

  return result;
}

bool str_begins_with(char* __prefix, char* __str)
{
    size_t prefix_length = strlen(__prefix);

    return strncmp(__prefix, __str, prefix_length) == 0;
}

char* get_full_path_of_the_first_file_that_matches_the_path(char* __path)
{
  char* directory = get_path_without_the_last_part(__path);
  char* filename = get_filename(__path);
  struct dirent* directory_entry;
  DIR* directory_fd;
  bool cont = true;
  char* result = NULL;

  directory_fd = opendir(directory);
  directory_entry = readdir(directory_fd);

  while ((cont == true) && (directory_entry != NULL))
  {
    if (str_begins_with(filename, directory_entry->d_name) == true)
    {
      result = get_full_path_of_the_first_file_that_matches_the_path_buffer;

      sprintf(result, "%s/%s", directory, directory_entry->d_name);

      cont = false;
    }
    else
    {
      directory_entry = readdir(directory_fd);
    }
  }

  return result;
}

char* get_associated_wildcard_name_from_queue_entry_path(char* __path) // ./queue/id:000056,w:wid=51;depth=1;content=4,src:000037,time:107,execs:706,op:havoc,rep:2 => wid=51;depth=1;content=4
{
  char* filename = get_filename(__path);
  char* wildcard_part_ptr;
  char* wildcard_filename_ptr;
  char* result;
  char* parameter;
  char* strtok_saveptr;
  char* wildcard_filename_ptr_cursor;
  int semicolons_count_in_wildcard_filename_ptr_cursor = -1;

  strtok_r(filename, ",", &strtok_saveptr); // id:000056
  wildcard_part_ptr = strtok_r(NULL, ",", &strtok_saveptr); // w:wid=51;depth=1;content=4 | time:0 => In this case, we have to catch orig:wid=51;depth=1;content=4

  // wildcard_part_ptr = "w:wid=51;depth=1;content=4"
  parameter = strtok(wildcard_part_ptr, ":"); // w

  if (strcmp(parameter, "w") == 0)
  {
    wildcard_filename_ptr = strtok(NULL, ":"); // wid=51;depth=1;content=4
  }
  else // If the fuzzer takes queue item that comes from initial seeds, there is no "w" parameter, and the wildcard tag is put in the "orig" parameter
  {
    strtok_r(NULL, ",", &strtok_saveptr);
    wildcard_part_ptr = strtok_r(NULL, ",", &strtok_saveptr); // orig:wid=51;depth=1;content=4

    if (str_begins_with("orig:id", wildcard_part_ptr) == true) // orig:id:000076,w:wid=11;depth=3;content=2-3-1;merged2id=29,...
    {
      wildcard_filename_ptr = wildcard_part_ptr;

      while ((wildcard_filename_ptr[0] != 'w') || (wildcard_filename_ptr[1] != 'i') || (wildcard_filename_ptr[2] != 'd')) // CRADO
      {
        wildcard_filename_ptr++;
      }

      wildcard_filename_ptr = strtok(wildcard_filename_ptr, ",");
    }
    else
    {
      strtok(wildcard_part_ptr, ":"); // orig
      wildcard_filename_ptr = strtok(NULL, ":"); // wid=51;depth=1;content=4;seed_id=0

      if (str_contains(wildcard_filename_ptr, "merged") == false) // If the wildcard has the for "wid=3;depth=3;content=3-3-8;merged1id=624", do nothing...
      {
        semicolons_count_in_wildcard_filename_ptr_cursor = 0;
        wildcard_filename_ptr_cursor = wildcard_filename_ptr;

        while ((semicolons_count_in_wildcard_filename_ptr_cursor < 3) && (wildcard_filename_ptr != NULL)) // To remove the "seed_id=0" part in wid=51;depth=1;content=4;seed_id=0 by counting the number of semicolons (';')
        {
          if (*wildcard_filename_ptr_cursor == ';')
          {
            ++semicolons_count_in_wildcard_filename_ptr_cursor;
          }

          ++wildcard_filename_ptr_cursor;
        }
      }
      else
      {
        semicolons_count_in_wildcard_filename_ptr_cursor = 0;
        wildcard_filename_ptr_cursor = wildcard_filename_ptr;

        while ((semicolons_count_in_wildcard_filename_ptr_cursor < 4) && (wildcard_filename_ptr != NULL)) // To remove the "seed_id=0" part in wid=51;depth=1;content=4;seed_id=0 by counting the number of semicolons (';')
        {
          if (*wildcard_filename_ptr_cursor == ';')
          {
            ++semicolons_count_in_wildcard_filename_ptr_cursor;
          }

          ++wildcard_filename_ptr_cursor;
        }
      }
    }

    if ((semicolons_count_in_wildcard_filename_ptr_cursor == 3) || (semicolons_count_in_wildcard_filename_ptr_cursor == 4))
    {
      *(wildcard_filename_ptr_cursor - 1) = '\0';
    }
  }

  if (wildcard_filename_ptr != NULL)
  {
    DEBUGF("wildcard_filename_ptr = %s\n", wildcard_filename_ptr);
  }
  else
  {
    DEBUGF("wildcard_filename_ptr is null (%s:%d)\n", __FILE__, __LINE__);
  }

  result = get_associated_wildcard_name_from_queue_entry_path_buffer;

  strcpy(result, wildcard_filename_ptr);

  return result;
}

bool str_contains(char* __str, char* __content)
{
  bool found = false;
  size_t i = 0;
  size_t content_len = strlen(__content);
  size_t str_len = strlen(__str);
  char* str_ptr = __str;

  while ((found == false) && ((i + content_len) < str_len))
  {
    if (strncmp(str_ptr, __content, content_len) == 0)
    {
      found = true;
    }
    else
    {
      ++str_ptr;
      ++i;
    }
  }

  return found;
}

// Example : wid=0;depth=2;content=2-3 or id:...,wid=0;depth=2;content=2-3,...
struct wildcard* find_matching_wildcard(afl_state_t* __afl, char* __fileName)
{
  struct wildcard* result = NULL;
  struct wildcardListNode* currentWildcardListNode = __afl->wildcards;
  struct wildcard* currentWildcard;
  char* currentWildcardFileName;
  size_t fileNameLength = strlen(__fileName);
  char* fileNameCursor = __fileName;
  size_t i;
  size_t j;

  if (strncmp(__fileName, "wid=", 4) == 0)
  {
    while ((wildcardList_hasNext(currentWildcardListNode) == true) && (result == NULL))
    {
      currentWildcard = wildcardList_next(&currentWildcardListNode);
      currentWildcardFileName = currentWildcard->tag;

      if (str_begins_with(currentWildcardFileName, __fileName) == true)
      {
        result = currentWildcard;
      }
    }
  }
  else
  {
    i = 0;

    while ((strncmp(fileNameCursor, "wid=", 4) != 0) && (i < fileNameLength))
    {
      ++fileNameCursor;
      ++i;
    }

    j = 0;

    if (i < fileNameLength) // If we found "wid="
    {
      while ((fileNameCursor[j] != ',') && (i < fileNameLength))
      {
        ++i;
        ++j;
      }

      while ((wildcardList_hasNext(currentWildcardListNode) == true) && (result == NULL))
      {
        currentWildcard = wildcardList_next(&currentWildcardListNode);
        currentWildcardFileName = currentWildcard->tag;

        if (str_begins_with(currentWildcardFileName, fileNameCursor) == true)
        {
          result = currentWildcard;
        }
      }
    }
  }

  if (result == NULL)
  {
    DEBUGF("NULL in %s:%d : \"%s\"\n", __FILE__, __LINE__, __fileName);
  }

  return result;
}
