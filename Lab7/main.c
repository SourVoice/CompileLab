#include "inter.h"

extern unsigned lexError;
extern unsigned synError;
extern pNode root;
int main() {


    if (!lexError && !synError) {
        table = initTable();
        //printTreeInfo(root, 0);
        traverseTree(root);
        // printTable(table);
        interCodeList = newInterCodeList();
        genInterCodes(root);
        if (!interError) {
            //printInterCode(NULL, interCodeList);
            printInterCode(NULL, interCodeList);
        }
        // deleteInterCodeList(interCodeList);
        deleteTable(table);
    }

}