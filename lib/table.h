#ifndef TABLE_H
#define TABLE_H

#include <stdint.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
typedef struct{
	int id;
	char username[COLUMN_USERNAME_SIZE+1];
	char email[COLUMN_EMAIL_SIZE+1];
} Row;

#define TABLE_MAX_PAGES 100

typedef struct {
	int file_descriptor;
	uint32_t file_length;
	void* pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
	uint32_t current_row_num;
	Pager *pager;
} Table;

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

extern const uint32_t TABLE_MAX_ROWS;

void serialize_row(Row *source, void *destination);

void deserialize_row(void *source, Row *destination);

Pager *pager_open(const char *filename);

void *get_page(Pager *pager, uint32_t page_num);

void pager_flush(Pager *pager, uint32_t page_num, uint32_t size);

void db_close(Table *table);

Table *db_open(const char* filename);

void *row_slot(Table *table, uint32_t row_num);

#endif
