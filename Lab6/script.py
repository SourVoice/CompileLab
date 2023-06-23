# zip file name: Lab5.zip
# include: Node.h, lex.yy.h, syntax.tab.c, syntax.tab.h

import zipfile
import os

def create_zip():
    files = ['Node.h', 'lex.yy.h', 'syntax.tab.c', 'syntax.tab.h']
    zip_name = 'files.zip'
    
    with zipfile.ZipFile(zip_name, 'w') as zipf:
        for file in files:
            if os.path.exists(file):
                zipf.write(file)
                print(f'{file} 添加到 {zip_name} 中')
            else:
                print(f'{file} 不存在')
    
    print('打包完成！')

create_zip()