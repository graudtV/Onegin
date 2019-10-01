#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

char *freadAll(const char path[]) //считывает весь файл в одну большую строку
{
	FILE *f = fopen(path, "r");
	assert(f != NULL); //получилось открыть файл

	//определяем длину файла
	fseek(f, 0, SEEK_END);
	long fileLen = ftell(f);
	printf("INFO: File length, characters (bytes): %lu; \n", fileLen);
	fseek(f, 0, SEEK_SET);

	char *text = calloc(fileLen + 1, sizeof(char)); //выделяем память на нужное число char-ов. Еще один символ нужен для \0
	assert(text != NULL); //память успешно выделена

	int checkSum = fread(text, sizeof(char), fileLen, f);
	assert(checkSum == fileLen); //чтение прошло успешно
	text[fileLen] = '\0'; //делаем строку text нуль-терменированной

	fclose(f); //!!!
	return text;
}

char **textToLines(char *text, int *NumberOfLines)
{
	//TODO: не вписывать пустые строки
	int nLines = 0;
	for(int i = 0; text[i] != '\0'; ++i) //считаем кол-во строк
		if(text[i] == '\n')
			++nLines;

	printf("INFO: Number of lines in file: %d; \n", nLines);	

	char **lines = calloc(nLines + 1, sizeof(char *)); //выделяем память под массив указателей на char (т.е. массив хранит адреса строк)
	lines[0] = text; //нулевая строка начинается с нулевого символа text. (<=> &text[0])
	nLines = 0;
	for(int i = 0; text[i] != '\0'; ++i) //теперь записываем адреса начала строк
		if(text[i] == '\n')
		{
			text[i] = '\0';
			lines[++nLines] = &text[i] + 1;
		}
	lines[nLines] = NULL;	
	printf("INFO: Number of lines after deleting empty ones: %d; \n", nLines);	
	*NumberOfLines = nLines;
	return lines;
}

void printLines(char *lines[])
{
	printf("***** beginning of the text *****\n");
	int i = 0; //счетчик строк
	for(i = 0; *lines != NULL; ++i) //no more lines
	{
		printf("line №%d: '%s'\n", i + 1, *lines);
		++lines;
	}
	printf("******** end of the text ********\n");
	printf("***** INFO: Lines printed: %d ****\n\n", i);
}

//меняет местами строки i, j. (Точнее, указатели на эти строки в массиве указателей
//сами символы строк остаются на месте.
void swapLines(char **lines, int i, int j) 
{
	char *t = lines[i];
	lines[i] = lines[j];
	lines[j] = t;
}

int main()
{
	const char path[] = "textfile.txt";

	char *text = freadAll(path); //считываем весь файл сразу в одну строку
	//printf("@\n%s@\n", text);
	int nLines = 0;
	char **lines = textToLines(text, &nLines);

	printf("\nOriginal file:\n");
	printLines(lines);

	swapLines(lines, 0, 1);

	printf("Lines 1 and 2 swaped:\n");
	printLines(lines);

	free(text); //!!!
	text = NULL;
	free(lines); //!!!
	lines = NULL;
}