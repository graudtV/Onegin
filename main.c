#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "line_t.h"


/*!
 * \brief Считывает весь файл в одну большую строку
 *
 * Рассчитывает длину файла, потом динамически выделяет необходимую память и переписывает туда содержимое файла 
 *
 * \param const char path[] Строка-путь к файлу
 * \returns Указатель на выделенную память
 * \returns NULL Если не удалось открыть файл
 *
 * В случае ошибки при выделении памяти выполнение программы прерывается
 * P.S. Файл открывается и закрывается автоматически. Но память, выделенную под содержимое файла, нужно освобождать вручную!
 */
char *freadAll(const char path[])
{
	FILE *f = fopen(path, "r");
	if (f == NULL) //не удалось открыть файл
	{
		//printf("ERROR: Cannot open file. May be, it does not exist.\n");
		return NULL;
	}
		printf("INFO: Reading from file '%s'\n", path);
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
 * \brief Компаратор для qsort для сортировки line_t по первым буквам
 */
int _compareLinesFront(const void *arg1, const void *arg2)
{
	const struct line_t *line1 = (const struct line_t*)arg1;
	const struct line_t *line2 = (const struct line_t*)arg2;
	return strcmp(line1->ptr, line2->ptr);
}

/*!
 * \brief Функция сортировка массива строк line_t по первым буквам
 */
void sortLinesFront(struct line_t lines[], size_t nLines)
{
	qsort(lines, nLines, sizeof(struct line_t), &_compareLinesFront);
}

/*!
 * \brief Компаратор для qsort для сортировки line_t по последним буквам
 */
int _compareLinesBack(const void *arg1, const void *arg2)
{
	const struct line_t *line1 = (const struct line_t*)arg1;
	const struct line_t *line2 = (const struct line_t*)arg2;
	//Записываем указатель на последние элементы строк
	char *i = (line1->ptr + line1->len - 1); 
	char *j = (line2->ptr + line2->len - 1);

	while (1)
	{	//Работает и ладно
		while (i >= line1->ptr && (isspace(*i) || ispunct(*i)))
			--i; //пропускаем все пробелы и пунктуационные знаки в первой строке
		while (j >= line1->ptr && (isspace(*j) || ispunct(*j)))
			--j; //пропускаем все пробелы и пунктуационные знаки во второй строке	
		if (*i > *j)
			return 1;
		if (*i < *j)
			return -1;
		--i, --j;
		if (i < line1->ptr) //i < j
			return -1;
		if (j < line2->ptr)
			return 1;
		//чем больше if-ов, тем лучше
	}
	return 1;

}

/*!
 * \brief Функция сортировка массива строк line_t по последним буквам
 */
void sortLinesBack(struct line_t lines[], size_t nLines)
{
	qsort(lines, nLines, sizeof(struct line_t), &_compareLinesBack);
}


/*!
 * Запускает остальные функции для сортировки текста
 * Через параметры в консоли можно передать имена файлов для чтения и записи
 * \param argv[0] Путь к файлу, из которого производится чтение
 * \param argv[1] Путь к файлу, в который записывается результат
 * По умолчанию чтение из example.txt, Запись в result.txt
 * Если возникает проблема с файлами, то программа сообщит об этом
 */
int main(int argc, char *argv[])
{
	printf("\n***********************************************");
	printf("\n************** WELCOME TO ONEGIN **************\n");
	printf("***********************************************\n");

	char path[500] = "example.txt"; //path хранит путь к файлу, из которого происходит чтение. Тут указан файл по умолчанию
	if (argc > 1) strcpy(path, argv[1]); //Если пользователь передал путь к файлу через консоль, то используем путь пользователя

	char *text = NULL; //Сюда будем считывать весь текста из файла (одной большой строчкой)
	while ((text = freadAll(path)) == NULL) //Считываем текст. Если такого файла не существует
	{
		printf("Error: Cannot open file '%s'. May be, it does not exist.\n", path);
		printf("Please write correct path.\nPath: "); //Заставляем пользователя ввести нормальный путь
		scanf("%s", path);
		rewind(stdin); //стираем все оставшееся в stdin, чтобы не мешало при следующем вводе (если пользователь опять ошибся)
	}

	size_t nLines = 0;
	struct line_t *lines = textToLines(text, &nLines);

	#if TEST_TEXTTOLINES == 1 
	test_textToLines(lines); //unit-test
	#endif

	(argc > 2) ? strcpy(path, argv[2]) : strcpy(path, "result.txt");
		//Если передан второй параметр, то используем его как имя файла, куда записывать результат
		//Если не передан - записываем в дефолтный
	FILE *fout = NULL;
	while ((fout = fopen(path, "w")) == NULL) //Пока не удастся открыть файл
	{
		printf("Error: Cannot open file '%s' to write result.\n", path);
		printf("Please write correct path.\nPath: "); //Заставляем пользователя ввести нормальный путь
		scanf("%s", path);
		rewind(stdin);
	}

	fputs("Сортировка по первым буквам:\n", fout);
	sortLinesFront(lines, nLines); //Сортируем по алфавиту	
	lineWriteAllToFile(lines, fout);
	fputs("\nСортировка по последним буквам (по рифмам)\n", fout);
	sortLinesBack(lines, nLines);
	lineWriteAllToFile(lines, fout);

	fclose(fout);
	fout = NULL;

	free(lines); //!!!
	lines = NULL;
	free(text); //!!!
	text = NULL;

	printf("***********************************************\n");
	printf("***********************************************\n");
	printf("***********************************************\n\n");
}