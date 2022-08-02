#include <stdio.h>

using namespace std;

void enciph(char *textFile, char *keyFile);

int main()
{
    char textFile[] = "kursovaya/input.txt";
    char keyFile[] =  "kursovaya/key.txt";
    enciph(textFile, keyFile);

    return 0;
}

void enciph(char *textFile, char *keyFile)
{
    FILE *text;
    FILE *key;
    FILE *output;
    text = fopen(textFile, "r");
    key = fopen(keyFile, "r");
    output = fopen("kursovaya/output.txt", "w");

    char textCh, keyCh, outCh;
    while(!feof(text))
    {   
        textCh = fgetc(text);
        keyCh = fgetc(key);
        if(!feof(text))
        {
            if(feof(key))
            {
                fseek(key, 0, SEEK_SET);
                keyCh = fgetc(key);
            }
            outCh = textCh ^ keyCh;
            fputc(outCh, output);
        }
    }

    fclose(text);
    fclose(key);
    fclose(output);
}