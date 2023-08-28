#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lib/input_buffer.h"
#include "lib/table.h"
#include "lib/meta_command.h"
#include "lib/statement.h"


int main(int argc, char *argv[]){
	if(argc < 2){
		printf("Must add filename for db\n");
		exit(EXIT_FAILURE);
	}


	InputBuffer *input_buffer = new_input_buffer();
	Statement *statement = malloc(sizeof(Statement));

	char *filename = argv[1];
	Table *table = db_open(filename);

	while(true){
		printf("db > ");
		read_input(input_buffer);

		if(input_buffer->buffer[0] == '.'){
			switch (do_meta_command(input_buffer, table)) {
				case META_COMMAND_SUCCESS: 
					continue;
				case META_COMMAND_UNRECOGNIZED_COMMAND:
					printf("Error: INVALID META COMMAND\n");
					continue;
			};
		}

		
		switch(prepare_command(input_buffer, statement)){
			case PREPARE_SUCCESS: 
				break;
			case PREPARE_SYNTAX_ERROR:
				printf("Error: INVALID SYNTAX\n");
				continue;
			case PREPARE_UNRECOGNIZED_STATEMENT:
				printf("Error: UNRECOGNIZED KEYWORD %s\n", input_buffer->buffer);
				continue;
			case PREPARE_STRING_TOO_LONG:
				printf("Error: INPUT TOO LONG\n");
				continue;
			case PREPARE_NEGATIVE_ID:
				printf("Error: NEGATIVE ID\n");
				continue;
		}


		switch (execute_statement(statement, table)) {
			case EXECUTE_SUCCESS: 
				printf("EXECUTED\n");
				break;
			case EXECUTE_TABLE_FULL:
				printf("Error: TABLE FULL\n");
				break;
		};
	}
}
