from unittest import main, TestCase

from subprocess import Popen, PIPE
from os import remove, path


DB_NAME = "test.db"


def run_commands(commands):
    with Popen(["./db", DB_NAME], stdin=PIPE, stdout=PIPE, text=True) as p:
        raw_output = ""

        for command in commands:
            p.stdin.write(command + "\n")
            p.stdin.flush()

        p.stdin.close()
        raw_output = p.stdout.read()
        p.wait()

        p.kill()

        return raw_output.split("\n")


def reset_db_file():
    if (path.exists(DB_NAME)):
        remove(DB_NAME)


class TestProcess(TestCase):
    def test_insert_row_and_retrieve(self):
        reset_db_file()
        commands = [
            "insert 1 user1 person1@example.com",
            "select",
            ".exit"
        ]

        expected = [
            "db > EXECUTED",
            "db > (1, user1, person1@example.com)",
            "EXECUTED",
            "db > "
        ]

        ouput = run_commands(commands)

        self.assertEqual(ouput, expected)
        reset_db_file()

    def test_table_full(self):
        reset_db_file()
        commands = []
        for i in range(1, 1400):
            commands.append(f'insert {i} user{i} mail{i}@mail.com')

        commands.append('.exit')

        output = run_commands(commands)

        self.assertEqual(output[-2], "db > Error: TABLE FULL")
        reset_db_file()

    def test_insert_max_length_string(self):
        reset_db_file()
        username = "a"*32
        mail = "b"*255
        commands = [
            f'insert 1 {username} {mail}',
            "select",
            ".exit"
        ]

        expected = [
            "db > EXECUTED",
            f'db > (1, {username}, {mail})',
            "EXECUTED",
            "db > "
        ]

        output = run_commands(commands)

        self.assertEqual(output, expected)
        reset_db_file()

    def test_insert_long_string_error(self):
        reset_db_file()
        username = "a"*33
        mail = "b"*256
        commands = [
            f'insert 1 {username} {mail}',
            "select",
            ".exit"
        ]

        expected = [
            "db > Error: INPUT TOO LONG",
            "db > EXECUTED",
            "db > "
        ]

        output = run_commands(commands)

        self.assertEqual(output, expected)
        reset_db_file()

    def test_insert_negative_id_error(self):
        reset_db_file()
        commands = [
            "insert -1 user1 person1@example.com",
            "select",
            ".exit"
        ]

        expected = [
            "db > Error: NEGATIVE ID",
            "db > EXECUTED",
            "db > "
        ]

        ouput = run_commands(commands)

        self.assertEqual(ouput, expected)
        reset_db_file()

    def test_db_saving(self):
        reset_db_file()
        commands = [
            "insert 1 user1 person1@example.com",
            ".exit"
        ]

        expected = [
            "db > EXECUTED",
            "db > "
        ]

        ouput = run_commands(commands)

        self.assertEqual(ouput, expected)

        commands = [
            "select",
            ".exit"
        ]

        expected = [
            "db > (1, user1, person1@example.com)",
            "EXECUTED",
            "db > ",
        ]

        output = run_commands(commands)

        self.assertEqual(output, expected)
        reset_db_file()


if __name__ == "__main__":
    main()
