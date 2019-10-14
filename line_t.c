/*!
 * \brief Содержит функции для работы со строками особого типа line_t
 *
 * Такие строки удобно использовать, когда имеется большой массив char-ов,
 * и необходимо уметь находить в нем отдельные строчки (последовательности, заканчивающиеся \n).
 * line_t позволяет записывать адреса строк и сохранять их длину
 * line_t НЕ копирует строки в собственную память - он работает с чужой памятью, лишь храня указатели на нее
 */

#include "line_t.h"
#include "stdio.h"
#include "assert.h"
#include "string.h"
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
 * \brief Проверяет строчку на соответсвие длины строки, записанной в структуре, реальной длине
 *
 * \returns 1 Если длина внутри line_t правильная или структура нулевая (см описание line_t)
 * \returns 0 Если структура заполнена неправильно
 *
 * Если передан нулевой указатель, то программа прерывается.
 * (Т.к. если Вы знаете, что указатель нулевой, то его незачем проверять. Если же не знаете, то это, вероятно, баг, который лучше поймать сразу)
 */
int lineIsCorrect(struct line_t *line)
{
	assert(line != NULL);
	if(line->ptr == NULL || strlen(line->ptr) == line->len)
		return 1;
	return 0;
}
/*!
 * \brief Останавливает программу, если объект структуры line_t некорректен
 *
 * Выводит подробные пояснения о причине останова.
 * Под некорректностью подразумевается неравенство реальной длины строки
 * и длины, записанной внутри структуры (в len)
 * Нулевая структура считается корректной
 *
 * \param line Указатель на проверяемый объект структуры line_t
 *
 * Если нужно проверить line_t на корректность, не завершая программы,
 * то нужно воспользоваться функцией lineIsCorrect()
 */
void lineAssertCorrectness(struct line_t *line)
{
	if (lineIsCorrect(line))
		return;

	printf("ERROR: Assertation of line_t object correctness failed.\n");
	printf("Line pointer: [%p];\n", line);
	printf("Line->ptr pointer: [%p];\n", line->ptr);
	printf("Line->ptr contains: '%s'\n", line->ptr);
	printf("Line->len is: %zu;\n", line->len);
	printf("Strlen(line->ptr) returns (real length): %lu;\n", strlen(line->ptr));
	exit(1);
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

struct line_t *textToLines(char *text, size_t *NumberOfLines)
{
	#define bool short
	#define true 1
	#define false 0

	//TODO: не вписывать пустые строки
	const bool IGNORE_LEFT_SPACES = true;
	const bool IGNORE_EMPTY_LINES = true;

	size_t nLines = 1; //Цикл ниже не учтет последнюю строчку, т.к. она заканчивается на \0, а не \n, поэтому счет с 1
	for(int i = 0; text[i] != '\0'; ++i) //считаем кол-во строк в text
		if(text[i] == '\n')
			++nLines;

	printf("INFO: Number of lines in file: %zu; \n", nLines);

	struct line_t *lines = calloc(nLines + 1, sizeof(struct line_t));
	assert(lines != NULL); //память успешно выделена
	lines[0].ptr = text;
	nLines = 0;
	size_t lineLenCounter = 0; //счетчик длины текущей строки
	bool lineIsEmpty = true; //true, если строка состоит только из пробелов и \t
	for(size_t i = 0; ; ++i, ++lineLenCounter) //проходим по всему text[], инициализируем структуру lines[]
	{
		if(text[i] == '\n' || text[i] == '\0') //если i-ая строка закончиалась
		{
			if(!(lineIsEmpty && (IGNORE_EMPTY_LINES || text[i] == '\0')))
			{   //Если строчка пустая и ее нужно проигнорировать, 
				//	то это условие НЕ выполнится
				//Индекс nLines останется тем же, и следующая строка запишется в тот же элемент lines вместо текущей, пустой
				//Последняя пустая строчка в text (если прямо перед \0 стоит \n) игнорируется автоматически.
				lines[nLines].len = lineLenCounter; //записываем ее длину.
				++nLines; //увеличиваем индекс в массиве строк
			}
			
			lines[nLines].ptr = &text[i] + 1; //Записываем указатель на следующую строку
			lineLenCounter = -1; //Сбрасываем счетчик длины строки. Символ после итерации цикла должен быть нулевым, а не первым, поэтому -1
			lineIsEmpty = true; //Сбрасываем флажок

			if(text[i] == '\0') //Если это была последняя строка в text, то выходим из цикла
				break; 
			text[i] = '\0'; //заменяем \n на нуль-терминант (\0)
		}	
		else if(text[i] != ' ' && text[i] != '\t') //Если считан не пробельный символ
			lineIsEmpty = false;
		else if(IGNORE_LEFT_SPACES && lineIsEmpty) //игнорирование левых пробелов
		{
			++(lines[nLines].ptr); // "сдвигаем" указатель вправо. Теперь пробел не входит в строку (стоит перед ней)
			--lineLenCounter;
		}		
	}


	lines[nLines].ptr = NULL; //завершающая строка - нулевая. (Работает налогично си-строкам)
	lines[nLines].len = -1;
	
	printf("INFO: Number of lines after deleting empty ones: %zu; \n", nLines);	
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
	size_t i = 0; //счетчик строк
	for(i = 0; !lineIsNull(lines); ++i) //no more lines
	{
		printf("line №%zu: '%s'\n", i + 1, lines->ptr);
		++lines; //переходим к следующей строке
	}
	printf("******** end of the text ********\n");
	printf("***** INFO: Lines printed: %zu ****\n\n", i);
}

/*!
 * \brief Печатает n строк из массива lines[]
 *
 * Нумерует каждую строку, добавляет кавычки для удобства чтения
 * \param lines[] - Массив структур
 * \param n Длина массива lines[] или количество строк, которые нужно напечатать из него
 */
void printnLines(struct line_t lines[], size_t n)
{
	printf("***** beginning of the text *****\n");
	size_t i = 0; //счетчик строк
	for(i = 0; i < n; ++i) //no more lines
	{
		assert(!lineIsNull(lines));
		printf("line [%zu/%zu]: '%s'\n", i + 1, n, lines->ptr);
		++lines; //переходим к следующей строке
	}
	printf("******** end of the text ********\n");
	printf("***** INFO: Lines printed: %zu ****\n\n", i);
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

void lineWriteAllToFile(struct line_t lines[], FILE *f)
{
	assert(f != NULL);

	size_t i = 0;
	for(i = 0; lines->ptr != NULL; ++i)
	{
		fputs(lines->ptr, f);
		fputc('\n', f);
		++lines;
	}
	printf("INFO: %zu lines are successfully written to a file\n", i);
}


#if TEST_TEXTTOLINES == 1
void test_textToLines(struct line_t lines[])
{
	printf("\nTest of textToLines() started.\n");
	size_t i = 0;
	for(i = 0; lines[i].ptr != NULL; ++i)
		lineAssertCorrectness(&lines[i]);
	printf("Tested successfully.\n");
	printf("Lines checked: %zu\n", i);
	printf("All of the line_t lines are correct.\n\n");
}
#endif