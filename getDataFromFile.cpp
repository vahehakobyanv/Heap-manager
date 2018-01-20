#include "myheader.h"


_NODE* _createNewNode(char data)
{
    _NODE *newNode = NULL;
    newNode=(_NODE*)malloc(sizeof(_NODE));
    if(newNode == NULL) {
        printf("Memory allocation error\n");
        return NULL;
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}



_INTNODE* _intcreateNewNode(int data)
{
    _INTNODE *newNode = NULL;
    newNode=(_INTNODE*)malloc(sizeof(_INTNODE));
    if(newNode == NULL) {
        printf("Memory allocation error\n");
        return NULL;
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}



_NODE* _insertAtEnd(char data, _NODE **head)
{
    _NODE *current = *head;
       if (current == NULL) {
        *head = _createNewNode(data);
    }else{
        while(current->next != NULL){
            current = current->next;
        }
        current->next = _createNewNode(data);
    }
    return *head;
}


_INTNODE* _intinsertAtEnd(int data, _INTNODE **head)
{
    _INTNODE *current = *head;
       if (current == NULL) {
        *head = _intcreateNewNode(data);
    }else{
        while(current->next != NULL){
            current = current->next;
        }
        current->next = _intcreateNewNode(data);
    }
    return *head;
}


_NODE* _removeExtraSpaces(_NODE** head)
{
    _NODE*current = *head;
    while(current->next != NULL){
        if(current->data == ' '){
            if((current->next)->data == ' '){
                _NODE*temp = (current->next)->next;
                free(current->next);
                current->next = temp;
            }
        }
        current = current->next;
    }
    return *head;
}



int _countINTNodes(_INTNODE *head)
{
    int count=0;
    while(head != NULL){
        count++;
        head=head->next;
  }
    return count;
}



void _deleteCharList(_NODE **head)
{
    _NODE *current = *head;
    while(current->next != NULL){
        *head = current->next;
        free(current);
        current=*head;
    }
}


void _deleteINTList(_INTNODE **inthead)
{
    _INTNODE *current = *inthead;
    while(current != NULL){
        *inthead = current->next;
        free(current);
        current=*inthead;
    }
}


_INTNODE *_deleteINTFront(_INTNODE **inthead)
{
    _INTNODE *current = *inthead;
    if(current == NULL) {
        printf("\nList is empty\n");
        return NULL;
    }else{
        *inthead = current->next;
        free(current);
    }
    current = current->next;
    return *inthead;
}

_INTNODE *_deleteINTEnd(_INTNODE **inthead)
{
    _INTNODE *current = *inthead;

    if(_countINTNodes(current) >= 2){
        while((current->next)->next != NULL){
            current=current->next;
        }
        free(current->next);
        current->next=NULL;
    }else{
        return _deleteINTFront(&(*inthead));
    }

    return *inthead;
}

_INTNODE* _deleteINT_FromSecondNode(_INTNODE **inthead)
{
    _INTNODE *headTemp = *inthead;
    if(_countINTNodes(*inthead) > 1){
        _INTNODE*current = (*inthead)->next;

        while(current != NULL){
            if(current->data == 0){
                headTemp->next = current->next;
                free(current);
                current = headTemp->next;
            }else{
                break;
            }
        }
    }
    return headTemp;
}



void _separateNumbDatas(_NODE *head, _INTNODE **inthead)
{
    head = _insertAtEnd(' ',&head);
    _NODE*current = head;
    char holdStr[50];
    int index = 0;

    while(current != NULL){
        if(current->data == ' '){
                holdStr[index] = '\0';
                _intinsertAtEnd(atoi(holdStr),&(*inthead));
                index = 0;
               holdStr[index] = '\0';
               current = current->next;
        }else{
                holdStr[index] = current->data;
                index++;
                current = current->next;
            }
        }
}


_INTNODE *getProcessData(const char *filename)
{
    _NODE* head = NULL;
    _INTNODE *inthead = NULL;   
    FILE *fp;
    char readData;
    bool flag = false;

    fp=fopen(filename,"r");
    if(fp == NULL) {
        printf("File openning error\n");
        exit(0);
    }

    do{
        readData = getc(fp);
        if(readData == ' '&& !flag){
            flag = false;
        }
        else{
            flag=true;
        }

        if(readData=='\t'){
            readData=' ';
        }
       
        if(readData == '#'){
            readData = ' ';
            char ch;
            while((ch=getc(fp)) != '\n' && ch != EOF){
            }
            readData = '\n';
        }
                    
     
        if(flag){
            head = _insertAtEnd(readData,&head);
        }

    } while(readData != EOF);

    fclose(fp);

    for(int i=0;i<5;i++){
       head = _removeExtraSpaces(&head);
    }

    _separateNumbDatas(head,&inthead);

    _deleteINTFront(&inthead); 

    _deleteCharList(&head);
    free(head);


    return inthead;
}


