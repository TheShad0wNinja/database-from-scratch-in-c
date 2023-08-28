#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "meta_command.h"
#include "input_buffer.h"
#include "table.h"

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table){
		if(strcmp(input_buffer->buffer, ".exit") == 0){
			close_input_buffer(input_buffer);
			db_close(table);	
			exit(EXIT_SUCCESS);
		}else{
			return META_COMMAND_UNRECOGNIZED_COMMAND;
		}
}
