#include <iostream>
#include "Multiset.h"
using namespace std;

class Multiset;

//Конструктор по умолчанию.
Multiset::Multiset()
{
	size = 0;
	arr = nullptr;
}

//Конструктор с параметром.
Multiset::Multiset(size_t size_arr)
{
	size = size_arr;
	arr = new unsigned int[size];
	for (size_t i = 0; i < size; ++i)
	{
		arr[i] = 0;
	}
}

//Деструткор.
Multiset::~Multiset()
{
	delete[] arr;
}

//Конструктор копирования.
Multiset::Multiset(const Multiset& other)
{
	size = other.size;
	arr = new unsigned int[size];
	for (size_t i = 0; i < size; ++i)
	{
		arr[i] = other.arr[i];
	}
}

//Пустое ли мультимножество?
bool Multiset::Empty()
{
	return (size == 0);
}

void Multiset::Clear()
{
	size = 0;
	delete[] arr;
	arr = nullptr;
}

//Оператор вывода мультимножетсва.
ostream& operator<<(ostream& os, const Multiset& obj)
{
	os << "[[";
	for (size_t i = 0; i < obj.size; ++i)
	{
		if (i != obj.size - 1)
			os << obj.arr[i] << ", ";
		else
			os << obj.arr[i] << "]]" << endl;;
	}
	if (!obj.size)
		os << "]]"<<endl;
	return os;
}

//Оператор ввода мультимножества в формате: (пробелы и переходы на новые строки)[числа через запятую, между числами, скобками и запятыми могут быть пробелы]
istream& operator>>(istream& is, Multiset& obj)
{
	try
	{
		while (is.peek() == ' ' || is.peek() == '\n')
			is.ignore();

		if (is.peek() != '[')
			throw 1;
		else
			is.ignore();

		if (is.peek() != '[')
			throw 1;
		else
			is.ignore();

		while (is.peek() == ' ')
			is.ignore();

		if (is.peek() == ']')
		{
			is.ignore();
			if (is.peek() != ']')
				throw 1;
			else
				is.ignore();
			if (!obj.Empty())
			{
				obj.size = 0;
				delete[] obj.arr;
				obj.arr = nullptr;
			}
			return is;
		}


		unsigned int number;
		is >> number;
		if (is.fail())
			throw 1;

		size_t counter = 1;
		unsigned int* arr_one = new unsigned int[counter];
		unsigned int* arr_two = new unsigned int[counter];
		arr_one[0] = number;

		while (true)
		{
			while (is.peek() == ' ')
				is.ignore();

			if (is.peek() == ']')
			{
				is.ignore();
				if (is.peek() != ']')
					throw 1;
				else
					is.ignore();
				obj.size = counter;
				delete[] obj.arr;
				obj.arr = new unsigned int[obj.size];
				for (size_t i = 0; i < obj.size; ++i)
					obj.arr[i] = arr_one[i];
				delete[] arr_one;
				delete[] arr_two;
				return is;
			}

			if (is.peek() != ',')
			{
				delete[] arr_one;
				delete[] arr_two;
				throw 1;
			}
			is.ignore();
			counter++;

			while (is.peek() == ' ')
				is.ignore();

			is >> number;
			if (is.fail())
			{
				delete[] arr_one;
				delete[] arr_two;
				throw 1;
			}
			delete[] arr_two;
			arr_two = new unsigned int[counter];
			for (size_t i = 0; i < counter - 1; ++i)
				arr_two[i] = arr_one[i];
			arr_two[counter - 1] = number;
			delete[] arr_one;
			arr_one = new unsigned int[counter];
			for (size_t i = 0; i < counter; ++i)
				arr_one[i] = arr_two[i];
		}
	}
	catch (int)
	{
		cout << "Ошибочка, введен некорректный символ." << endl;
		is.setstate(ios::failbit);
	}
}

/*
Oбъединение двух мультимножеств в третьем.
Объединением мультимножеств А и В называется мультимножество, 
состоящее из всех элементов, которые присутствуют хотя бы в одном из мультимножеств, 
и кратность каждого элемента равна максимальной кратности 
соответствующих элементов в объединяемых мультимножествах.
*/
Multiset& Multiset::Union(Multiset& obj_one, Multiset& obj_two)
{
	unsigned int element_repetition = 0;
	unsigned int max_element_repetition = 0;
	unsigned int count = 0;
	size = obj_one.size;
	for (size_t i = 0; i < obj_two.size; ++i)
	{
		element_repetition = 0;
		max_element_repetition = 0;
		for (size_t j = 0; j < obj_two.size; ++j)
		{
			if (obj_two.arr[i] == obj_two.arr[j])
				++element_repetition;
		}

		max_element_repetition = element_repetition;
		element_repetition = 0;
		for (size_t j = 0; j < obj_one.size; ++j)
		{
			if (obj_two.arr[i] == obj_one.arr[j])
				++element_repetition;
		}
		if (element_repetition > max_element_repetition)
			max_element_repetition = element_repetition;
		for (size_t j = 0; j < i; ++j)
		{
			if (obj_two.arr[j] == obj_two.arr[i])
				++element_repetition;
		}
		if (element_repetition < max_element_repetition)
			++size;
	}
	delete[] arr;
	arr = new unsigned int[size];

	for (size_t i = 0; i < obj_one.size; ++i)
		arr[i] = obj_one.arr[i];
	for (size_t i = 0; i < obj_two.size; ++i)
	{
		element_repetition = 0;
		max_element_repetition = 0;
		for (size_t j = 0; j < obj_two.size; ++j)
		{
			if (obj_two.arr[i] == obj_two.arr[j])
				++element_repetition;
		}

		max_element_repetition = element_repetition;
		element_repetition = 0;
		for (size_t j = 0; j < obj_one.size; ++j)
		{
			if (obj_two.arr[i] == obj_one.arr[j])
				++element_repetition;
		}
		if (element_repetition > max_element_repetition)
			max_element_repetition = element_repetition;
		for (size_t j = 0; j < i; ++j)
		{
			if (obj_two.arr[j] == obj_two.arr[i])
				++element_repetition;
		}
		if (element_repetition < max_element_repetition)
		{
			arr[obj_one.size + count] = obj_two.arr[i];
			++count;
		}
	}
	return *this;
}

/*
Пересечение двух мультимножеств в третьем.
Пересечением мультимножеств А и В называется мультимножество, 
состоящее из всех элементов, которые одновременно присутствуют а каждом из мультимножеств, 
и кратность каждого элемента равна минимальной кратности 
соответствующих элементов в пересекаемых мультимножествах.
*/
Multiset& Multiset::Intersection(Multiset& obj_one, Multiset& obj_two)
{
	unsigned int element_repetition = 0;
	unsigned int min_element_repetition = 0;
	size = 0;
	for (size_t i = 0; i < obj_one.size; ++i)
	{
		element_repetition = 0;
		min_element_repetition = 0;
		bool check = true;
		for (size_t j = 0; j < i; ++j)
			if (obj_one.arr[i] == obj_one.arr[j])
			{
				check = false;
				break;
			}
		if (!check)
			continue;
		check = false;
		for (size_t j = 0; j < obj_two.size; ++j)
			if (obj_one.arr[i] == obj_two.arr[j])
			{
				check = true;
				break;
			}
		if (!check)
			continue;
		for (size_t j = 0; j < obj_one.size; ++j)
			if (obj_one.arr[i] == obj_one.arr[j])
				++element_repetition;
		min_element_repetition = element_repetition;
		element_repetition = 0;
		for (size_t j = 0; j < obj_two.size; ++j)
			if (obj_one.arr[i] == obj_two.arr[j])
				++element_repetition;
		if (element_repetition < min_element_repetition)
			min_element_repetition = element_repetition;
		size += min_element_repetition;
	}
	delete[] arr;
	arr = new unsigned int[size];

	unsigned int count = 0;
	for (size_t i = 0; i < obj_one.size; ++i)
	{
		element_repetition = 0;
		min_element_repetition = 0;
		bool check = false;
		for (unsigned int j = 0; j < obj_two.size; ++j)
			if (obj_one.arr[i] == obj_two.arr[j])
			{
				check = true;
				break;
			}
		if (!check)
			continue;
		for (size_t j = 0; j < obj_two.size; ++j)
			if (obj_one.arr[i] == obj_two.arr[j])
				++element_repetition;
		min_element_repetition = element_repetition;
		element_repetition = 0;
		for (size_t j = 0; j < obj_one.size; ++j)
			if (obj_one.arr[i] == obj_one.arr[j])
				++element_repetition;
		if (element_repetition < min_element_repetition)
			min_element_repetition = element_repetition;
		element_repetition = 0;
		for (size_t j = 0; j < count; ++j)
		{
			if (arr[j] == obj_one.arr[i])
				++element_repetition;
		}
		if (element_repetition < min_element_repetition)
		{
			arr[count] = obj_one.arr[i];
			++count;
		}
	}
	return *this;
}

/*
Разность двух мультимножеств в третьем.
Арифметической разностью мультимножеств А и В называется мультимножество, 
состоящее из всех элементов мультимножества A, 
кратность которых превышает кратность соответствующих элементов в мультимножестве В, 
и кратность каждого элемента равна разности кратностей 
соответствующих элементов в вычитаемых мультимножествах.
*/
Multiset& Multiset::Difference(Multiset& obj_one, Multiset& obj_two)
{
	unsigned int element_repetition_obj_one = 0;
	unsigned int element_repetition_obj_two = 0;
	unsigned int element_repetition_obj_three = 0;
	unsigned int count = 0;
	size = 0;

	for (size_t i = 0; i < obj_one.size; ++i)
	{
		element_repetition_obj_one = 0;
		element_repetition_obj_two = 0;
		bool check = true;
		for (size_t j = 0; j < i; ++j)
			if (obj_one.arr[i] == obj_one.arr[j])
			{
				check = false;
				break;
			}
		if (!check)
			continue;
		for (size_t j = 0; j < obj_one.size; ++j)
		{
			if (obj_one.arr[i] == obj_one.arr[j])
				++element_repetition_obj_one;
		}
		for (size_t j = 0; j < obj_two.size; ++j)
		{
			if (obj_one.arr[i] == obj_two.arr[j])
				++element_repetition_obj_two;
		}
		if (element_repetition_obj_one > element_repetition_obj_two)
			size += (element_repetition_obj_one - element_repetition_obj_two);
	}
	delete[] arr;
	arr = new unsigned int[size];

	for (size_t i = 0; i < obj_one.size; ++i)
	{
		element_repetition_obj_three = 0;
		element_repetition_obj_one = 0;
		element_repetition_obj_two = 0;
		for (size_t j = 0; j < obj_one.size; ++j)
		{
			if (obj_one.arr[i] == obj_one.arr[j])
				++element_repetition_obj_one;
		}
		for (size_t j = 0; j < obj_two.size; ++j)
		{
			if (obj_one.arr[i] == obj_two.arr[j])
				++element_repetition_obj_two;
		}
		if (element_repetition_obj_one > element_repetition_obj_two)
		{
			for (size_t j = 0; j < count; ++j)
			{
				if (obj_one.arr[i] == arr[j])
					++element_repetition_obj_three;
			}
			if (element_repetition_obj_three < element_repetition_obj_one - element_repetition_obj_two)
			{
				arr[count] = obj_one.arr[i];
				++count;
			}
		}
	}
	return *this;
}

/*
Симметрическая разность двух мультимножеств в третьем.
Симметрической разностью мультимножеств А и В называется мультимножество, 
состоящее из всех элементов обоих мультимножеств, кратности которых различны, 
и кратность каждого элемента равна модулю разности кратностей 
соответствующих элементов в вычитаемых мультимножествах.
*/
Multiset& Multiset::SDifference(Multiset& obj_one, Multiset& obj_two)
{
	unsigned int element_repetition_obj_one = 0;
	unsigned int element_repetition_obj_two = 0;
	unsigned int element_repetition_obj_three = 0;
	unsigned int count = 0;
	size = 0;

	for (size_t i = 0; i < obj_one.size; ++i)
	{
		element_repetition_obj_one = 0;
		element_repetition_obj_two = 0;
		bool check = true;
		for (size_t j = 0; j < i; ++j)
			if (obj_one.arr[i] == obj_one.arr[j])
			{
				check = false;
				break;
			}
		if (!check)
			continue;
		for (size_t j = 0; j < obj_one.size; ++j)
		{
			if (obj_one.arr[i] == obj_one.arr[j])
				++element_repetition_obj_one;
		}
		for (size_t j = 0; j < obj_two.size; ++j)
		{
			if (obj_one.arr[i] == obj_two.arr[j])
				++element_repetition_obj_two;
		}
		if (element_repetition_obj_one > element_repetition_obj_two)
			size += (element_repetition_obj_one - element_repetition_obj_two);
		else
			size += (element_repetition_obj_two - element_repetition_obj_one);
	}
	for (size_t i = 0; i < obj_two.size; ++i)
	{
		element_repetition_obj_two = 0;
		bool check = true;
		for (size_t j = 0; j < obj_one.size; ++j)
			if (obj_two.arr[i] == obj_one.arr[j])
			{
				check = false;
				break;
			}
		if (!check)
			continue;
		for (size_t j = 0; j < i; ++j)
			if (obj_two.arr[i] == obj_two.arr[j])
			{
				check = false;
				break;
			}
		if (!check)
			continue;
		for (size_t j = 0; j < obj_two.size; ++j)
		{
			if (obj_two.arr[i] == obj_two.arr[j])
				++element_repetition_obj_two;
		}
		size += element_repetition_obj_two;
	}
	delete[] arr;
	arr = new unsigned int[size];

	for (size_t i = 0; i < obj_one.size; ++i)
	{
		element_repetition_obj_one = 0;
		element_repetition_obj_two = 0;
		element_repetition_obj_three = 0;
		for (size_t j = 0; j < obj_one.size; ++j)
		{
			if (obj_one.arr[i] == obj_one.arr[j])
				++element_repetition_obj_one;
		}
		for (size_t j = 0; j < obj_two.size; ++j)
		{
			if (obj_one.arr[i] == obj_two.arr[j])
				++element_repetition_obj_two;
		}
		for (size_t j = 0; j < count; ++j)
		{
			if (obj_one.arr[i] == arr[j])
				++element_repetition_obj_three;
		}
		if (element_repetition_obj_one > element_repetition_obj_two)
			if (element_repetition_obj_three < element_repetition_obj_one - element_repetition_obj_two)
			{
				arr[count] = obj_one.arr[i];
				++count;
			}
		if (element_repetition_obj_one < element_repetition_obj_two)
			if (element_repetition_obj_three < element_repetition_obj_two - element_repetition_obj_one)
			{
				arr[count] = obj_one.arr[i];
				++count;
			}
	}
	for (size_t i = 0; i < obj_two.size; ++i)
	{
		element_repetition_obj_one = 0;
		element_repetition_obj_two = 0;
		element_repetition_obj_three = 0;
		for (size_t j = 0; j < obj_one.size; ++j)
		{
			if (obj_two.arr[i] == obj_one.arr[j])
				++element_repetition_obj_one;
		}
		for (size_t j = 0; j < obj_two.size; ++j)
		{
			if (obj_two.arr[i] == obj_two.arr[j])
				++element_repetition_obj_two;
		}
		for (size_t j = 0; j < count; ++j)
		{
			if (obj_two.arr[i] == arr[j])
				++element_repetition_obj_three;
		}
		if (element_repetition_obj_one < element_repetition_obj_two)
			if (element_repetition_obj_three < element_repetition_obj_two - element_repetition_obj_one)
			{
				arr[count] = obj_two.arr[i];
				++count;
			}
	}
	return *this;
}

// Вставить элемент в конец мультимножества.
Multiset& Multiset::Insert(unsigned int count)
{
	unsigned int* temp = new unsigned int[size + 1];
	for (size_t i = 0; i < size; ++i)
	{
		temp[i] = arr[i];
	}
	temp[size] = count;
	delete[] arr;
	size += 1;
	arr = new unsigned int[size];
	for (size_t i = 0; i < size; ++i)
	{
		arr[i] = temp[i];
	}
	delete[] temp;
	return *this;
}

//Доступ к элементу мультимножества по его позиции.
unsigned int Multiset::Element(size_t position)
{
	return arr[position];
}

//Мощность мультимножества - число всех элементов с учетом повторяющихся.
size_t Multiset::Power()
{
	return size;
}

//Размер мультимножества - число элеметов без повторений.
size_t Multiset::Size()
{
	size_t unique_elements = 0;
	for (size_t i = 0; i < size; ++i)
	{
		for (size_t j = 0; j < i; ++j)
		{
			if (arr[i] == arr[j])
				break;
		}
		++unique_elements;
	}
	return unique_elements;
}

//Перегрузка оператора ==
bool Multiset::operator ==(Multiset& obj)
{
	if (size != obj.size)
		return false;
	Multiset test;
	test.SDifference(*this, obj);
	if (test.size == 0)
		return true;
	else
		return false;
}

//Перегрузка оператора !=
bool Multiset::operator !=(Multiset& obj)
{
	return !(*this == obj);
}

//Перегрузка оператора присваивания.
Multiset& Multiset::operator = (Multiset& other)
{
	if (&other == this)
		return *this;
	if (arr != nullptr)
		delete[] arr;
	size = other.size;
	arr = new unsigned int[size];
	for (size_t i = 0; i < size; ++i)
	{
		arr[i] = other.arr[i];
	}
	return *this;
}

//Перегрузка оператора присваивания доступа к элементу.
unsigned int Multiset::operator [](size_t position)
{
	return arr[position];
}

//Добавление на первую позицию наименьшего отсутствующего элемента.
Multiset& Multiset::FirstAbsent()
{
	if (size == 0)
	{
		size = 1;
		delete[] arr;
		arr = new unsigned int[size];
		arr[0] = 0;
		return *this;
	}
	else
	{
		bool check = true;
		unsigned int max_element = 0;
		for (size_t i = 0; i < size; ++i)
			if (arr[i] > max_element)
				max_element = arr[i];
		for (size_t i = 0; i < max_element; ++i)
		{
			check = true;
			for (size_t j = 0; j < size; ++j)
				if (arr[j] == i)
				{
					check = false;
					break;
				}
			if (check == false)
				continue;
			Multiset copy = *this;
			size += 1;
			delete[] arr;
			arr = new unsigned int[size];
			arr[0] = i;
			for (int k = 1; k < size; ++k)
				arr[k] = copy.arr[k-1];
			return *this;
		}
	}

}