import os

test_dir = './Tests'
parse_exe = ".\parse.exe"
log_dir = './log'

if not os.path.exists(log_dir):
    os.mkdir(log_dir)

for file_name in os.listdir(test_dir):
    if file_name.endswith('.cmm'):
        file_path = os.path.join(test_dir, file_name)
        log_file_path = os.path.join(log_dir, f'{file_name}.log')
        cmd = f'{parse_exe} {file_path} > {os.path.join(log_dir, file_name)}.log'
        os.system(cmd)
        with open(file_path, 'r') as f:
            content = f.read()
            with open(log_file_path, 'a') as log_f:
                log_f.write(f'\n\nTest Input:\n{content}\n\n')


import zipfile

zip_name = 'parser.zip'
file_list = ['lex.yy.h', 'syntax.tab.c', 'syntax.tab.h', 'Node.h']

with zipfile.ZipFile(zip_name, 'w') as f:
    for file_name in file_list:
        f.write(file_name)
