#include <stdio.h>
#include <stdlib.h>
#include "input_buffer.h"

InputBuffer* new_input_buffer(){
	InputBuffer* input_buffer = malloc(sizeof(InputBuffer));

	input_buffer->buffer = NULL;
	input_buffer->buffer_length = 0;
	input_buffer->input_length = 0;

	return input_buffer;
}

void close_input_buffer(InputBuffer* input_buffer){
	free(input_buffer->buffer);
	free(input_buffer);
}


void read_input(InputBuffer* input_buffer){
	ssize_t bytes_read = getline(&(input_buffer->buffer), &( input_buffer->buffer_length ), stdin);

	if(bytes_read == 0){
		printf("FAILED TO READ INPUT");
		exit(EXIT_FAILURE);
	}

	// Removing trail end char
	input_buffer->input_length = bytes_read - 1;	
	input_buffer->buffer[bytes_read - 1] = 0;
}
