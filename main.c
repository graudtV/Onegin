#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*!
 * \brief Считывает весь файл в одну большую строку
 *
 * Рассчитывает длину файла, потом динамически выделяет необходимую память и переписывает туда содержимое файла 
 *
 * \param const char path[] Строка-путь к файлу
 * \returns Указатель на выделенную память
 *
 * В случае ошибок (при открытии файла, выделении памяти) выполнение программы прерывается
 * P.S. Файл открывается и закрывается автоматически. Но память, выделенную под содержимое файла, нужно освобождать вручную!
 */
char *freadAll(const char path[])
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

/*!
 * \brief Хранит указатель на си-строку и длину этой строки
 * 
 * В этой программе массивы структур line_t заканчивается нулевой структурой line_t.
 * Нулевая структура - такая, что ptr = NULL, len = -1;
 * Нулевая структура != структура с пустой строкой. Пустая строка содержит единственный символ \0, ptr != NULL и len = 0, а не -1.
 * Такое оформление массива позволяет не передавать длину массива.
 * Функция lineIsNull() проверяет, является ли объект структуры нулевым.
 */
struct line_t
{
	char *ptr; //< Указатель на строку
	size_t len; //< Длина строки
};

/*!
 * \brief Проверяет, является ли структура нулевой
 *
 * \param line Указатель на исследуемую структуру
 * \returns 1 Является нулевой 
 * \returns 0 Не является
 */
int lineIsNull(struct line_t *line)
{
	if(line->ptr == NULL)
	{
		if(line->len != -1) 
			printf("WARNING: line->ptr == NULL, but line->len != -1.\tlen = %zu;\n", line->len);
		return 1;
	}
	return 0;
}

/*!
 * \brief Разбивает одну большую строку-текст на отдельные строки
 *
 * Функция определяет адреса начала отдельных строки и их длины и записывает эти данные
 * в массив структур типа line_t
 * \warning Функция НЕ копирует строки. Это значит, что нельзя удалять массив text, 
 * пока строки еще используются
 *
 * \param text Исходный текст
 * \param NumberOfLines Указатель на переменную, в которую нужно записать количество строк.
 * Если в этом нет необходимости, то можно передать NULL
 * \returns указатель на массив lines типа line_t
 *
 * Последняя структура в массиве является нулевой (см описание line_t)
 */
struct line_t *textToLines(char *text, int *NumberOfLines)
{
	//TODO: не вписывать пустые строки
	int nLines = 0;
	for(int i = 0; text[i] != '\0'; ++i) //считаем кол-во строк
		if(text[i] == '\n')
			++nLines;

	printf("INFO: Number of lines in file: %d; \n", nLines);

	struct line_t *lines = calloc(nLines + 1, sizeof(struct line_t));
	assert(lines != NULL); //память успешно выделена
	lines[0].ptr = text;
	nLines = 0;
	int lineLenCounter = 0;
	for(int i = 0; text[i] != '\0'; ++i, ++lineLenCounter) //проходим по всему text[], инициализируем структуру lines[]
	if(text[i] == '\n') //если i-ая строка закончиалась
	{
		text[i] = '\0'; //заменяем \n на нуль-терминант (\0)
		lines[i].len = lineLenCounter; //записываем ее длину.
		lineLenCounter = -1; //Сбрасываем счетчик длины строки. Символ после итерации цикла должен быть нулевым, а не первым, поэтому -1
		lines[++nLines].ptr = &text[i] + 1; //записываем адрес начала следующей строки
	}
	lines[nLines].ptr = NULL; //завершающая строка - нулевая. (Работает налогично си-строкам)
	lines[nLines].len = -1;
	
	printf("INFO: Number of lines after deleting empty ones: %d; \n", nLines);	
	if(NumberOfLines != NULL)
		*NumberOfLines = nLines;
	return lines;
}

/*!
 * \brief Печатает все структуры из массива lines[]
 *
 * Нумерует каждую строку, добавляет кавычки для удобства чтения
 * Останавливается, когда встречает нулевую структуру (см описание line_t)
 */
void printLines(struct line_t lines[])
{
	printf("***** beginning of the text *****\n");
	int i = 0; //счетчик строк
	for(i = 0; !lineIsNull(lines); ++i) //no more lines
	{
		printf("line №%d: '%s'\n", i + 1, lines->ptr);
		++lines; //переходим к следующей строке
	}
	printf("******** end of the text ********\n");
	printf("***** INFO: Lines printed: %d ****\n\n", i);
}

/*!
 * \brief Печатает n строк из массива lines[]
 *
 * Нумерует каждую строку, добавляет кавычки для удобства чтения
 * \param lines[] - Массив структур
 * \param n Длина массива lines[] или количество строк, которые нужно напечатать из него
 */
void printnLines(struct line_t lines[], int n)
{
	printf("***** beginning of the text *****\n");
	int i = 0; //счетчик строк
	for(i = 0; i < n; ++i) //no more lines
	{
		assert(!lineIsNull(lines));
		printf("line [%d/%d]: '%s'\n", i + 1, n, lines->ptr);
		++lines; //переходим к следующей строке
	}
	printf("******** end of the text ********\n");
	printf("***** INFO: Lines printed: %d ****\n\n", i);
}

/*!
 * \brief Меняет содержимое двух структур типа line_t местами
 *
 * \paran line1 Указатель на первый объект структуры 
 * \param line2 Указатель на второй объект структуры
 */
void swapLines(struct line_t *line1, struct line_t *line2)
{
	char *tPtr = line1->ptr;
	size_t tLen = line1->len;
	line1->ptr = line2->ptr;
	line1->len = line2->len;
	line2->ptr = tPtr;
	line2->len = tLen;
}

int main(int argc, char *argv[])
{
	char path[100] = "textfile2.txt";
	if(argc > 1) strcpy(path, argv[1]);

	char *text = freadAll(path); //считываем весь файл сразу в одну строку
	//printf("@\n%s@\n", text);
	int nLines = 0;
	struct line_t *lines = textToLines(text, &nLines);

	printf("\nOriginal file:\n");
	//printLines(lines);

	swapLines(&lines[0], &lines[1]);

	printf("Lines 1 and 2 swaped:\n");
	//printLines(lines);
	////printnLines(lines, nLines);

	free(lines); //!!!
	lines = NULL;
	free(text); //!!!
	text = NULL;
}