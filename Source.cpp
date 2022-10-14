#define unsInt unsigned int

#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

#define textSize 10000

using namespace std;

struct match
{
    int offset;
    double prc;
};

void ciph(FILE * text, FILE * key, FILE * outFl);
void deCiphKey(FILE * text, FILE * outFl);
wchar_t freqSymb(wchar_t symbs[], unsInt offset, unsInt realTextSize);
match * findKLen(wchar_t *text, unsInt realTexSize);
void matchSort(match *matches, unsInt realTextSize, int flag);
void usrMenu(char basic_textFlName[], char basic_ciphedFlName[], char basic_keyFlName[], char basic_outKeyFlName[], char basic_deciphedFlName[]);

int main()
{
    char textFlName[] = "decoder_for_xor/notCiphedText.txt";
    char ciphedFlName[] = "decoder_for_xor/ciphedText.txt";
    char keyFlName[] = "decoder_for_xor/key.txt";
    char outKeyFlName[] = "decoder_for_xor/deciphedKey.txt";
    char deciphedFlName[] = "decoder_for_xor/deciphedKey.txt";
    
    usrMenu(textFlName, ciphedFlName, keyFlName, outKeyFlName, deciphedFlName);

    return 0;
}

void ciph(FILE * text, FILE * key, FILE * fout)
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
            fwprintf(fout, L"%lc", outCh);
        }
    }
}

void deCiphKey(FILE * fin, FILE * fout)
{   
    wchar_t text[textSize];
    unsInt i = 0;
    for(; (text[i] = getwc(fin)) != WEOF; i++);
    text[i] = '\0';
    text[i + 1] = '\0';
    unsInt realTextSize = i;

    //getting potential keylens
    match * offsets = findKLen(text, realTextSize);
    int offset = -1;
    int usrCh = 0;
    int exitVal = 3;
    bool flag;
    wchar_t key[realTextSize];
    while(usrCh != exitVal)
    {
        wprintf(L"_________________________________________________________________");        
        wprintf(L"\n1 | choose offset");
        if(offset != -1)
        {
            wprintf(L"\n2 | try to deciph the key and print it (chosen offset : %d, text size : %d)", offset, realTextSize);
        }
        wprintf(L"\n3 | save deciphed key and exit\n");
        wscanf(L"%d", &usrCh);
        if(usrCh == 1)
        {
            flag = true;
            while(flag == true)
            {
                wprintf(L"_________________________________________________________________\n");        
                wprintf(L"text size : %d\n", realTextSize);
                flag = false;
                wprintf(L"the offset should be chosen\n");
                i = 0;
                while(offsets[i].offset > 0)
                {
                    wprintf(L"%d | offset : %d  | prc : %f\n", i, offsets[i].offset, offsets[i].prc);
                    i++;
                }
                wprintf(L"choose offsets : ");
                wscanf(L"%d", &usrCh);
                if(usrCh > -1 && usrCh < i)
                    offset = offsets[usrCh].offset;
                else
                    flag = true;
            }
            usrCh = 0;
        }
        else if(offset != -1 && usrCh == 2)
        {
            //making lettGroup array empty
            int lett_in_group = realTextSize / offset;
            wchar_t ** lettGroup = (wchar_t **)malloc(sizeof(wchar_t *) * offset);
            for(int j = 0; j < offset; j++)
                lettGroup[j] = (wchar_t *)malloc(sizeof(wchar_t)*lett_in_group);

            for(unsInt z = 0; z < offset; z++)
                for(unsInt j = 0; j < lett_in_group; j++)   
                    lettGroup[z][j] = L'\0';

            //dividing letter by OFFSET lett_in_group and XORing with ' ' space symbol
            for(unsInt j = 0; j < offset; j++)
            {
                for(unsInt l = j; l < realTextSize; l += offset)
                {
                    wchar_t tempLetter = text[l];
                    unsInt e = 0;
                    while(lettGroup[j][e] != tempLetter && lettGroup[j][e] != L'\0')
                        e++;        
                    
                    if(e < lett_in_group && lettGroup[j][e] == L'\0')
                        lettGroup[j][e] = tempLetter ^ L' ';
                }
            }
            for(unsInt j = 0; j < offset; j++)
            {
                key[j] = freqSymb(lettGroup[j], offset, realTextSize);
                wprintf(L"%lc", key[j]);
            }   
            wprintf(L"\n");
            // for(int i = 0; i < offset; i++)
            //     for(int j = 0; j < lett_in_group; j++)
            //         free(lettGroup[i]);
            free(lettGroup);
        }
        else if(usrCh == 3)
        {
            for(int j = 0; j < offset; j++)
                fwprintf(fout,  L"%lc", key[j]);      
            usrCh = 3;
        }
        else
        {
            wprintf(L"\n##################################\n");
            wprintf(L"## wrong parameter has passed ##\n");
            wprintf(L"##################################\n");
        }
    }
    free(offsets);
}

wchar_t freqSymb(wchar_t symbs[], unsInt offset, unsInt realTextSize)
{
    int lett_in_group = realTextSize/offset;
    wchar_t output, temp;
    wchar_t uniqLett[lett_in_group];
    for(unsInt i = 0; i < lett_in_group; i++)
    {
        uniqLett[i] = L'\0';
    }
    unsInt i = 0;
    unsInt j;
    int tLettCount = 0;
    //getin uniq letters
    while(i < lett_in_group && tLettCount < lett_in_group && symbs[tLettCount] != L'\0')
    {
        temp = symbs[tLettCount++];
        //finding TEMP letter in letter array (not adding if letter in array)
        j = 0;
        while(j < i && uniqLett[j] != temp)
            j++;
        
        //adding letter if its not included
        if(j == i)
        {
            uniqLett[i] = temp;
            i++;
        }
    }

    unsInt tCount, countMax = 0;
    for(unsInt j = 0; j < i; j++)
    {
        temp = uniqLett[j];
        tCount = 0;
        for(int s = 0; s < lett_in_group && symbs[s] != L'\0'; s++)
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

match * findKLen(wchar_t *text, unsInt realTextSize)
{
    unsInt offset = 0;
    unsInt cnt;
    unsInt i;
    match matches[realTextSize];
    //gettin potential matches
    for(i = 0; i < realTextSize - 1; i++)
    {
        offset = i + 1;
        cnt = 0;
        for(unsInt j = 0; j < realTextSize; j++)
        {
            if(text[j] == text[(j + offset) % realTextSize])
                cnt++;
        }
        matches[i].offset = offset;
        matches[i].prc = ((double)cnt / realTextSize) * 100;
    }

    //selection of more prob vars
    matchSort(matches, realTextSize, 1);
    bool flag = true;
    i = 0;
    match pot_offs_rep[realTextSize];
    double prev_prc = 0;
    for(int j = 0; j < realTextSize && flag; j++)
    {
        if(matches[j].prc < prev_prc * 0.8 || matches[j].prc < 6.)
            flag = false;
        else
        {
            pot_offs_rep[i].prc = matches[j].prc;
            pot_offs_rep[i].offset = matches[j].offset;
            prev_prc = pot_offs_rep[i].prc;
            i++;
        }
    }
    //del repeats of offsets
    matchSort(pot_offs_rep, i, 0);
    struct match *pot_offs = (struct match *)malloc(sizeof(struct match) * i + 1);
    for(int z = 0; z < i + 1; z++)
    {
        pot_offs[z].offset = -1;
        pot_offs[z].prc = 0;
    }
    cnt = 0;
    for(int z = 0; z < i; z++)
    {
        flag = true;
        for(int j = z + 1; flag && j < i; j++)
            if(pot_offs_rep[z].offset % pot_offs_rep[j].offset == 0)
                flag = false;
        
        if(flag)
        {
            pot_offs[cnt].offset = pot_offs_rep[z].offset;
            pot_offs[cnt].prc = pot_offs_rep[z].prc;
            cnt++;
        }
    }
    matchSort(pot_offs, cnt, 1);
    return pot_offs;
}

void matchSort(match *matches, unsInt realTextSize, int flag)
{
    //sorting by : 0 - offset | 1 - prc
    unsInt t;
    match tMatch;
    if(flag == 1)
    {
        for(int i = 0; i < realTextSize - 1; i++)
        {
            t = i;
            for(int z = i + 1; z < realTextSize; z++)
            {
                if(matches[t].prc < matches[z].prc)
                    t = z;
            }
            tMatch.offset = matches[i].offset;
            tMatch.prc = matches[i].prc;
            matches[i].offset = matches[t].offset;
            matches[i].prc = matches[t].prc;
            matches[t].offset = tMatch.offset;
            matches[t].prc = tMatch.prc;
        }
    }
    else if(flag == 0)
    {
        for(int i = 0; i < realTextSize - 1; i++)
        {
            t = i;
            for(int z = i + 1; z < realTextSize; z++)
            {
                if(matches[t].offset < matches[z].offset)
                    t = z;
            }
            tMatch.offset = matches[i].offset;
            tMatch.prc = matches[i].prc;
            matches[i].offset = matches[t].offset;
            matches[i].prc = matches[t].prc;
            matches[t].offset = tMatch.offset;
            matches[t].prc = tMatch.prc;
        }
    }
}

void usrMenu(char basic_textFlName[], char basic_ciphedFlName[], char basic_keyFlName[], char basic_outKeyFlName[], char basic_deciphedFlName[])
{
    char textFl[256], keyFl[256], outFl[256];
    FILE *text, *key, *fout;
    int usrCh = 0;
    int exitVal = 4;
    while(usrCh != exitVal)
    {
        usrCh = 0;
        while(usrCh < 1 || usrCh > exitVal)
        {
            printf("1 | ciph text with key\n");
            printf("2 | deciph text with key\n");
            printf("3 | deciph key\n");
            printf("4 | exit\n");
            scanf("%d", &usrCh);
        }
        switch (usrCh)
        {
        case 1:            
            printf("there are basic file names\n");
            printf("%s - basic text file name\n", basic_textFlName);
            printf("%s - basic key file name\n", basic_keyFlName);
            printf("%s - basic out ciphed file name\n", basic_ciphedFlName);
            printf("wanna use them?\n");
            
            printf("1 | yes\n2 | no\n");
            scanf("%d", &usrCh);
            if(usrCh == 1)
            {
                text = fopen(basic_textFlName, "r");
                key = fopen(basic_keyFlName, "r");
                fout = fopen(basic_ciphedFlName, "w");
            }
            else
            {
                printf("\nciphin text with a key\n");
                printf("text file path : ");
                scanf("%s", textFl);
                text = fopen(textFl, "r");
                while(text == NULL)
                {
                    printf("\nwrong text file path given\ntext file path : ");
                    scanf("%s", textFl);
                    text = fopen(textFl, "r");
                }
                
                printf("key file path : ");
                scanf("%s", keyFl);
                key = fopen(keyFl, "r");
                while(key == NULL)
                {
                    printf("\nwrong key file path given\nkey file path : ");
                    scanf("%s", textFl);
                    key = fopen(keyFl, "r");    
                }

                printf("output text file path : ");
                scanf("%s", outFl);
                fout = fopen(outFl, "w");
            }
            ciph(text, key, fout);
            fclose(text);
            fclose(key);
            fclose(fout);
            printf("\n----------------------------------------------------------------\n");
            printf("                        ciphed successfully");
            printf("\n----------------------------------------------------------------\n");
            break;

        case 2:
            printf("there are basic file names\n");
            printf("%s - basic ciphed text file name\n", basic_ciphedFlName);
            printf("%s - basic key file name\n", basic_keyFlName);
            printf("%s - basic deciphed out file name\n", basic_deciphedFlName);
            printf("wanna use them?\n");
            
            printf("1 | yes\n2 | no\n");
            scanf("%d", &usrCh);
            if(usrCh == 1)
            {
                text = fopen(basic_ciphedFlName, "r");
                key = fopen(basic_keyFlName, "r");
                fout = fopen(basic_deciphedFlName, "w");
            }
            else
            {
                printf("\ndeciphin text with a key\n");
                printf("ciphed text file path : ");
                scanf("%s", textFl);
                text = fopen(textFl, "r");
                while(text == NULL)
                {
                    printf("\nwrong text file path given\ntext file path : ");
                    scanf("%s", textFl);
                    text = fopen(textFl, "r");
                }
                
                printf("key file path : ");
                scanf("%s", keyFl);
                key = fopen(keyFl, "r");
                while(key == NULL)
                {
                    printf("\nwrong key file path given\nkey file path : ");
                    scanf("%s", textFl);
                    key = fopen(keyFl, "r");    
                }

                printf("deciphed output text file path : ");
                scanf("%s", outFl);
                fout = fopen(outFl, "w");
            }
            ciph(text, key, fout);
            fclose(text);
            fclose(key);
            fclose(fout);
            printf("\n----------------------------------------------------------------\n");
            printf("                        deciphed successfully");
            printf("\n----------------------------------------------------------------\n");
            break;

        case 3:
            printf("there are basic file names\n");
            printf("%s - basic ciphed text file name\n", basic_ciphedFlName);
            printf("%s - basic key file name\n", basic_outKeyFlName);
            printf("wanna use them?\n");
            
            printf("1 | yes\n2 | no\n");
            scanf("%d", &usrCh);
            if(usrCh == 1)
            {
                text = fopen(basic_ciphedFlName, "r");
                key = fopen(basic_outKeyFlName, "w");
            }
            else
            {
                printf("deciphin key\n");
                printf("text file path : ");
                scanf("%s", textFl);
                text = fopen(textFl, "r");
                while(text == NULL)
                {
                    printf("\nwrong text file path given\ntext file path : ");
                    scanf("%s", textFl);
                    text = fopen(textFl, "r");
                }

                printf("key output file path : ");
                scanf("%s", outFl);
                key = fopen(keyFl, "w");
            }
            deCiphKey(text, key);
            fclose(text);
            fclose(key);
            printf("\n----------------------------------------------------------------\n");
            printf("                        wrote the key successfully");
            printf("\n----------------------------------------------------------------\n");
            break;

        default:
            break;
        }
    }
}