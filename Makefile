dev: 
	make build
	./db

build:
	clang main.c lib/input_buffer.c lib/table.c lib/meta_command.c lib/statement.c -o db

test: 
	make build
	python3 test.py
	
clean:
	rm db
