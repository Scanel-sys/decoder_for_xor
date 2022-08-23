#define unsInt unsigned int

#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#define textSize 10000

using namespace std;



void ciph(char * textFlname, char * outFlname, char * keyFlName);

void clearWCArr(wchar_t arr[][textSize], unsInt offset, unsInt realTxtSize);

void deCiph(char * ciphText, char * outFLname);

wchar_t freqSymb(wchar_t symbs[], int offset, int realTextSize);

int main()
{
    setlocale(LC_ALL, "ru_Ru");

    char inpFl[] = "kursovaya/notCiphedText.txt";
    char outFL[] = "kursovaya/ciphedText.txt";
    char keyFl[] = "kursovaya/key.txt";
    ciph(inpFl, outFL, keyFl);
    
    strcpy(inpFl, "kursovaya/ciphedText.txt");
    strcpy(outFL, "kursovaya/deCiphedText.txt");

    deCiph(inpFl, outFL);

    return 0;
}

void ciph(char * textFlname, char * outFlname, char * keyFlName)
{
    FILE * text = fopen(textFlname, "r");
    FILE * key = fopen(keyFlName, "r");
    FILE * outFl = fopen(outFlname, "w");

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

    fclose(text);
    fclose(key);
    fclose(outFl);
}


void deCiph(char * ciphText, char * outFLname)
{
    FILE * fin = fopen(ciphText, "r");
    
    wchar_t text[textSize];
    int realTextSize;
    unsInt i = 0;
    for(; (text[i] = getwc(fin)) != WEOF; i++);
    text[i] = '\0';
    text[i + 1] = '\0';
    realTextSize = i;

    unsInt maxSameSymb = 0;
    int offset = 0;
    unsInt actSymb, tempSameSymb;
    //gettin length of the KEY
    for(i = 0; i < realTextSize - 1; i++)
    {
        actSymb = i + 1;
        tempSameSymb = 0;
        for(unsInt j = 0; j < realTextSize; j++)
        {
            if(text[j] == text[(j + actSymb) % realTextSize])
                tempSameSymb++;
        }
        if(tempSameSymb > maxSameSymb)
        {
            maxSameSymb = tempSameSymb;
            offset = actSymb;
        }
    }
    
    //making lettGroup array empty
    wchar_t lettGroup[offset][realTextSize / offset];
    for(unsInt z = 0; z < offset; z++)
    {
        for(unsInt j = 0; j < realTextSize / offset; j++)
        {
            lettGroup[z][j] = L'\0';
        }
    }

    //dividing letter by OFFSET groups and XORing with ' ' space symbol
    for(unsInt j = 0; j < offset; j++)
    {
        for(unsInt l = j; l < realTextSize; l += offset)
        {
            wchar_t tempLetter = text[l];
            unsInt e = 0;
            while(lettGroup[j][e] != tempLetter && lettGroup[j][e] != '\0' && lettGroup[j][e] != tempLetter)
                e++;        
            
            if(e < realTextSize / offset && lettGroup[j][e] == L'\0')
                lettGroup[j][e] = tempLetter ^ L' ';
        }
    }

    wchar_t key[offset];
    for(unsInt j = 0; j < offset; j++)
    {
        key[j] = freqSymb(lettGroup[j], offset, realTextSize);
    }
    

    fclose(fin);
}

wchar_t freqSymb(wchar_t symbs[], int offset, int realTextSize)
{
    wchar_t output, temp;
    wchar_t uniqLett[realTextSize/offset];
    
    for(unsInt i = 0; i < realTextSize / offset; i++)
    {
        uniqLett[i] = L'\0';
    }
    unsInt i = 0;
    int tLettCount = -1;
    //getin uniq letters
    while(i < realTextSize / offset && tLettCount < realTextSize / offset)
    {
        tLettCount++;
        temp = symbs[tLettCount];
        unsInt j = 0;
        //finding TEMP letter in letter array (not adding if letter in array)
        while(j < i && uniqLett[j] != temp)
            j++;
        
        //adding letter if its not included
        if(j == i)
        {
            uniqLett[i] = temp;
            i++;
        }
    }
    uniqLett[i] = L'\0';

    unsInt tCount, countMax = 0;
    for(unsInt j = 0; j < i; j++)
    {
        temp = uniqLett[j];
        tCount = 0;
        for(int s = 0; s < realTextSize / offset; s++)
        {
            if(symbs[s] == temp)
                tCount++;
        }
        if(countMax < tCount)
        {
            countMax = tCount;
            output = temp;
        }
    }
    return output;
}

