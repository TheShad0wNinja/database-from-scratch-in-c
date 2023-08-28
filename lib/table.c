#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "table.h"

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE/ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

void serialize_row(Row *source, void *destination){
	memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
	memcpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
	memcpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destination){
	memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
	memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
	memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

Pager *pager_open(const char *filename){
	int file_descriptor = open(
		filename,
		O_RDWR | O_CREAT,
		S_IWUSR | S_IRUSR
	);

	if (file_descriptor  == -1){
		printf("Error: UNABLE TO OPEN FILE\n");
		exit(EXIT_FAILURE);
	}

	off_t file_length = lseek(file_descriptor, 0, SEEK_END);

	Pager *pager = malloc(sizeof(Pager));

	pager->file_descriptor = file_descriptor;
	pager->file_length = file_length;

	for(uint32_t i=0; i< TABLE_MAX_PAGES; i++){
		pager->pages[i] = NULL;
	}

	return pager;
}

void *get_page(Pager *pager, uint32_t page_num){
	if(page_num > TABLE_MAX_PAGES){
		printf("Error: FETCHED OUT OF BOUND PAGE\n");
		exit(EXIT_FAILURE);
	}

	if(pager->pages[page_num] == NULL){
		void *page = malloc(PAGE_SIZE);
		
		uint32_t num_of_pages = pager->file_length / PAGE_SIZE;

		if (pager->file_length % PAGE_SIZE){
			num_of_pages += 1;
		}

		if(page_num <= num_of_pages){
			lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
			ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
			if(bytes_read == -1) {
				printf("Error: UNABLE TO READ FILE\n");
				exit(EXIT_FAILURE);
			}
		}
		pager->pages[page_num] = page;
	}

	return pager->pages[page_num];
}

Table *db_open(const char *filename){
	Pager *pager = pager_open(filename);
	
	uint32_t current_row_num = pager->file_length / ROW_SIZE;

	Table *table = malloc(sizeof(Table));
	table->pager = pager;
	table->current_row_num = current_row_num;

	return table;
}

void pager_flush(Pager *pager, uint32_t page_num, uint32_t size){
	if(pager->pages[page_num] == NULL){
		printf("Error: FLUSHED EMPTY PAGE");
		exit(EXIT_FAILURE);
	}

	off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

	if (offset == -1){
		printf("Error: UNABLE TO SEEK\n");
		exit(EXIT_FAILURE);
	}

	ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);

	if(bytes_written == -1){
		printf("Error: UNABLE TO WRITE TO FILE\n");
		exit(EXIT_FAILURE);
	}
}

void db_close(Table *table){
	Pager *pager = table->pager;
	uint32_t num_of_full_pages = table->current_row_num / ROWS_PER_PAGE; 

	for(uint32_t i=0; i<num_of_full_pages; i++){
		if(pager->pages[i] == NULL){
			continue;;
		}

		pager_flush(pager, i, PAGE_SIZE);
		free(pager->pages[i]);
		pager->pages[i] = NULL;
	}
	
	uint32_t num_of_additional_rows = table->current_row_num % ROWS_PER_PAGE;
	if(num_of_additional_rows > 0){
		uint32_t page_num = num_of_full_pages;
		if (pager->pages[page_num] != NULL){
			pager_flush(pager, page_num, num_of_additional_rows * ROW_SIZE);
			free(pager->pages[page_num]);
			pager->pages[page_num] = NULL;
		}
	}

	int result = close(pager->file_descriptor);
	if(result == -1){
		printf("Error: UNABLE TO CLOSE DB FILE\n");
		exit(EXIT_FAILURE);
	}

	for(uint32_t i = 0; i < TABLE_MAX_PAGES; i++){
		void *page = pager->pages[i];
		if(page){
			free(page);
			pager->pages[i] = NULL;
		}
	}
	free(pager);
	free(table);
}


void *row_slot(Table *table, uint32_t row_num){
	uint32_t page_num = row_num/ROWS_PER_PAGE;

	void *page = get_page(table->pager, page_num);

	uint32_t row_offset = row_num % ROWS_PER_PAGE;
	uint32_t byte_offset = row_offset  * ROW_SIZE;

	return page + byte_offset;
}
