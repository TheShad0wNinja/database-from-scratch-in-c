#ifndef STATEMENT_H
#define STATEMENT_H

#include "table.h"
#include "input_buffer.h"

typedef enum{
	STATEMENT_SELECT,
	STATEMENT_INSERT
} STATEMENT_TYPE;

typedef struct{
	STATEMENT_TYPE type;
	Row row_to_insert;
} Statement;


typedef enum {
	PREPARE_SUCCESS,
	PREPARE_NEGATIVE_ID,
	PREPARE_UNRECOGNIZED_STATEMENT,
	PREPARE_SYNTAX_ERROR,
	PREPARE_STRING_TOO_LONG,
} PrepareResult;


typedef enum{
	EXECUTE_SUCCESS,
	EXECUTE_TABLE_FULL,
} ExecuteResult;


PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement);

PrepareResult prepare_command(InputBuffer *input_buffer, Statement *statement);

ExecuteResult execute_insert(Statement *statement, Table *table);

ExecuteResult execute_select(Statement *statement, Table *table);

ExecuteResult execute_statement(Statement *statement, Table *table);

#endif
