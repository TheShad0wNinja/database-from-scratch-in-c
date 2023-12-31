#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "input_buffer.h"
#include "table.h"
#include "meta_command.h"
#include "statement.h"

PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement){
	statement->type = STATEMENT_INSERT;

	char *keyword = strtok(input_buffer->buffer, " "); 		
	char *id_string = strtok(NULL, " ");
	char *username = strtok(NULL, " ");
	char *email = strtok(NULL, " ");

	if(id_string == NULL || username == NULL || email == NULL){
		return PREPARE_SYNTAX_ERROR;
	}

	int id = atoi(id_string);

	if(id < 0){
		return PREPARE_NEGATIVE_ID;
	}

	if(strlen(username) > COLUMN_USERNAME_SIZE){
		return PREPARE_STRING_TOO_LONG;
	}

	if(strlen(email) > COLUMN_EMAIL_SIZE){
		return PREPARE_STRING_TOO_LONG;
	}

	statement->row_to_insert.id = id;

	strcpy(statement->row_to_insert.username, username);
	strcpy(statement->row_to_insert.email, email);

	return PREPARE_SUCCESS;
}

PrepareResult prepare_command(InputBuffer *input_buffer, Statement *statement){
	if(strncmp(input_buffer->buffer, "insert", 6) == 0)	{
		return prepare_insert(input_buffer, statement);
	}

	if(strcmp(input_buffer->buffer, "select") == 0){
		statement->type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}

	return PREPARE_UNRECOGNIZED_STATEMENT;
}


ExecuteResult execute_insert(Statement *statement, Table *table){
	if(table->current_row_num >= TABLE_MAX_ROWS){
		return EXECUTE_TABLE_FULL;
	}

	Row *row_to_insert = &(statement->row_to_insert);

	void *destination = row_slot(table, table->current_row_num);

	serialize_row(row_to_insert, destination);

	table->current_row_num += 1;

	return EXECUTE_SUCCESS;
}

void print_row(Row *row){
	printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

ExecuteResult execute_select(Statement *statement, Table *table){
	Row *row = malloc(sizeof(Row));
	for(int i=0; i<table->current_row_num; i++){
		void *source = row_slot(table, i);
		deserialize_row(source, row);
		print_row( row );
	}
	free(row);
	return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement *statement, Table *table){
	switch (statement->type) {
		case STATEMENT_SELECT:
			return execute_select(statement, table);
		case STATEMENT_INSERT:
			return execute_insert(statement, table);
	}
}
