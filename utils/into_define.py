

def create_define(input_file : str = 'content.sql',output_file : str = 'sql_content.h'):
    file = open(input_file, 'r')
    content = file.read()
    file.close()
    content = content.split(";")

    line = '#pragma once\n#define SQL_CONTENT "'
    for request in content:
        request = request.replace("\n", "").replace("\t", "").replace("\"", "\\\"".replace("''", "\\\'"))

        print(request)
        line += request + ";"

    line += '"'

    out_file = open(output_file, 'w+')

    out_file.write(line)
    out_file.close()

create_define()