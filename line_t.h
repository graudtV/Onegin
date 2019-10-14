/*!
 * \brief Заголовочный файл с прототипами функций из line_t.c
 */
#ifndef LINE_T
#define LINE_T

#include "stdlib.h" //для size_t
#include "stdio.h" //для FILE

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

int lineIsNull(struct line_t *line);
int lineIsCorrect(struct line_t *line);
void lineAssertCorrectness(struct line_t *line);
struct line_t *textToLines(char *text, size_t *NumberOfLines);
void printLines(struct line_t lines[]);
void printnLines(struct line_t lines[], size_t n);
void swapLines(struct line_t *line1, struct line_t *line2);
void lineWriteAllToFile(struct line_t *lines, FILE *f);

#define TEST_TEXTTOLINES 1 //!< Включает/выключает unit test функции test_textToLines()

#if TEST_TEXTTOLINES == 1
void test_textToLines(struct line_t lines[]);
#endif //TEST_TEXTTOLINES

#endif //LINE_T