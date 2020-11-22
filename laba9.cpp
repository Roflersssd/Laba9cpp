#include <stdio.h>
#include <locale.h>
#include<string.h>
const int BUF_SIZE = 256;
const char* fname = "test3.txt";
const char* result = "result.bin";

void PrintFile(const char* file_name);

int main()
{
	setlocale(LC_ALL, "rus");
	FILE* in;
	FILE* out;
	
	if ((in = fopen(fname, "r")) == NULL)
	{
		printf("Ошибка открытия файла %s\n",fname); 
		return 1;
	}//if
	
	if ((out = fopen(result, "wb")) == NULL)
	{
		printf("Ошибка открытия файла %s\n", result);
		return 1;
	}//if
	
	char buf[BUF_SIZE];
	const char delim[] = " .,;:?!\t\n";//Знаки разделители
	int bytes_count = 0;
	while (fgets(buf, BUF_SIZE, in) != NULL) {
		float len = 0;//Длина строки
		int word_count = 0;//Счетчик количества слов в строке
		char* ptr;
		if((ptr=strchr(buf,'\n'))!=NULL){//Находим указатель на символ конца строки
			*ptr = '\0';//Удаляем знак окончания строки
		}
		len = strlen(buf);
		char* word;
		long int cur = ftell(out);//Запоминаем положение курсора в начале строки
		fseek(out, sizeof(int) + sizeof(float), SEEK_CUR);//Оставляем место для записи длины строки и кол. слов
		bytes_count+=fwrite(buf, sizeof(char), len, out);//Записываем строку 
		fseek(out, cur, SEEK_SET);//Возвращаем курсор в начало строки
		
		for (word = strtok(buf, delim); word != NULL; word = strtok(NULL, delim))//Считаем количество слов
			word_count++;
		bytes_count += fwrite(&len, sizeof(float), 1, out)*sizeof(float);//Записываем длину строки
		bytes_count += fwrite(&word_count, sizeof(int), 1, out)*sizeof(int);//Записываем кол. слов
		fseek(out, 0, SEEK_END);//Возвращаем курсор в конец файла
	}//while

	if (!feof(in)) {
		fclose(in);
		fclose(out);
		printf("Ошибка чтения\n");
		return 1;
	}//if
	fclose(in);//Закрываем входной поток из файла
	fclose(out);//Закрываем поток записи в файл	
	printf("Было записанно %i byet в двоичный файл\n", bytes_count);
	PrintFile(result);//Распечатываем полученный двоичный файл
}//int main()



void PrintFile(const char* file_name)
{
	//			       [float]              [int]            [char*]
	//Структура файла: [количество символов][количество слов][строка]
	FILE* in;
	if ((in = fopen(file_name, "rb")) == NULL) {//Открытие файла и проверка ошибок
		printf("Ошибка чтения бинарного файла \"%s\"\n", fname);
		return;
	}//if
	fseek(in, 0,SEEK_END);//Перемещаем курсор в конец файла
	long int file_size = ftell(in);//Узнаем размер файла в байтах
	rewind(in);//Возвращаем курсор в начало файла
	printf("Размер файла: %li byte\n", file_size);
	float len;//Длина строки
	int word_count;//Количество слов
	char str[BUF_SIZE];//Строка
	while (fread(&len, sizeof(float), 1, in)==1)//Цикл пока можно считать строку
	{
		if ((float)((int)len) != len) {//Проверка является ли размер строки целым числом 
			printf("Неправильная структура файла\n");
			fclose(in);
			return;
		}//if
		if (fread(&word_count, sizeof(int), 1, in) !=1)//Проверка структуры файла
		{
			printf("Неправильная структура файла\n");
			fclose(in);
			return;
		}//if
		if (fread(str, sizeof(char), (int)len, in) != (int)len) {
			if (feof(in)) {
				printf("Неправильная структура файла\n");
			}//if
			else {
				printf("Ошибка чтения файла\n");
			}//else
			fclose(in);
			return;
		}//if
		str[(int)len] = '\0';//Добавляем в конец строки знак конца строки
		printf("Количество слов в строке: %i| %s\n", word_count, str);
	}//while
	fclose(in);
}//PrintFile
