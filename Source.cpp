#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

void enciph(FILE * text, FILE * outFl, FILE * key);

FILE * getCiphedText();

int main()
{
    setlocale(LC_ALL, "ru_Ru");

    FILE * outFl = getCiphedText();

    


    fclose(outFl);
    return 0;
}

void enciph(FILE * text, FILE * outFl, FILE * key)
{
    wchar_t textCh, keyCh, outCh;
    while(!feof(text))
    {      
        textCh = getwc(text);
        keyCh = getwc(key);
        if(!feof(text))
        {
            if(feof(key))
            {
                fseek(key, 0, SEEK_SET);
                keyCh = fgetc(key);
            }
            outCh = textCh ^ keyCh;
            fwprintf(outFl, L"%lc", outCh);
        }
    }
}

FILE * getCiphedText()
{
    char textFile[] = "kursovaya/input.txt";
    char keyFile[] =  "kursovaya/key.txt";

    FILE *text;
    FILE *key;
    FILE *outFl;
    text = fopen(textFile, "r");
    key = fopen(keyFile, "r");
    outFl = fopen("kursovaya/output.txt", "w");

    enciph(text, outFl, key);

    fclose(text);
    fclose(key);

    return outFl;
}