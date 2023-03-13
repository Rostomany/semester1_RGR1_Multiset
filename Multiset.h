#pragma once
#include <iostream>
using namespace std;

class Multiset
{
	unsigned int* arr;
	size_t size;

public:
	Multiset();															// конструктор по умолчанию

	Multiset(size_t size_arr);											// конструктор с параметром

	~Multiset();														// деструктор

	Multiset(const Multiset& other);									// конструктор копирования

	bool Empty();														// пустое ли мультимножество?

	void Clear();														// очистить мультимножество.

	friend ostream& operator<<(ostream& os, const Multiset& obj);		// перегрузка оператора вывода для мультимножества

	friend istream& operator>>(istream& is, Multiset& obj);				// перегрузка оператора ввода для мультимножетсва

	Multiset& Union(Multiset& obj_one, Multiset& obj_two);				// объединение двух мультимножеств в третьем

	Multiset& Intersection(Multiset& obj_one, Multiset& obj_two);		// пересечение двух мультимножеств в третьем

	Multiset& Difference(Multiset& obj_one, Multiset& obj_two);			// разность двух мультимножеств в третьем

	Multiset& SDifference(Multiset& obj_one, Multiset& obj_two);		// симметрическая разность двух мультимножеств в третьем

	Multiset& Insert(unsigned int count);								// вставить число в конец мультимножества

	unsigned int Element(size_t position);								// доступ к элементу мультимножества по его позиции

	size_t Power();														// мощность мультимножества

	size_t Size();														// размер мультимножества

	bool operator ==(Multiset& obj);									// перегрузка оператора = для класса Multiset

	bool operator !=(Multiset& obj);									// перегрузка оператора != для класса Multiset

	Multiset& operator = (Multiset& other);								// оператор присваивания

	unsigned int operator [](size_t position);							// перегрузка оператора []

	Multiset& FirstAbsent();											// добавить первый отсутствующий элемент
};