

def create_define(input_file : str = 'content.sql',output_file : str = 'sql_content.h'):
    file = open(input_file, 'r')
    content = file.read()
    file.close()
    
    queries = []
    current_query = ""
    in_string = False
    i = 0
    
    while i < len(content):
        char = content[i]
        
        if not in_string and char == "-" and i + 1 < len(content) and content[i + 1] == "-":
            while i < len(content) and content[i] != "\n":
                i += 1
            continue
        
        if char == "'":
            if i + 1 < len(content) and content[i + 1] == "'":
                current_query += "''"
                i += 2
                continue
            else:
                in_string = not in_string
                current_query += char
        elif char == ";":
            if not in_string:
                queries.append(current_query.strip())
                current_query = ""
            else:
                current_query += ","
        else:
            current_query += char
        
        i += 1
    
    if current_query.strip():
        queries.append(current_query.strip())

    line = '#pragma once\n#define SQL_CONTENT "'
    for request in queries:
        if not request:
            continue
        request = request.replace("\n", "").replace("\t", "").replace("\"", "\\\"")

        print(request)
        line += request + ";"

    line += '"'

    out_file = open(output_file, 'w+')

    out_file.write(line)
    out_file.close()

create_define()