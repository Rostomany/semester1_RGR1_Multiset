#pragma once
#include <iostream>
using namespace std;

class Multiset
{
	unsigned int* arr;
	size_t size;

public:
	Multiset();															// ����������� �� ���������

	Multiset(size_t size_arr);											// ����������� � ����������

	~Multiset();														// ����������

	Multiset(const Multiset& other);									// ����������� �����������

	bool Empty();														// ������ �� ���������������?

	void Clear();														// �������� ���������������.

	friend ostream& operator<<(ostream& os, const Multiset& obj);		// ���������� ��������� ������ ��� ���������������

	friend istream& operator>>(istream& is, Multiset& obj);				// ���������� ��������� ����� ��� ���������������

	Multiset& Union(Multiset& obj_one, Multiset& obj_two);				// ����������� ���� �������������� � �������

	Multiset& Intersection(Multiset& obj_one, Multiset& obj_two);		// ����������� ���� �������������� � �������

	Multiset& Difference(Multiset& obj_one, Multiset& obj_two);			// �������� ���� �������������� � �������

	Multiset& SDifference(Multiset& obj_one, Multiset& obj_two);		// �������������� �������� ���� �������������� � �������

	Multiset& Insert(unsigned int count);								// �������� ����� � ����� ���������������

	unsigned int Element(size_t position);								// ������ � �������� ��������������� �� ��� �������

	size_t Power();														// �������� ���������������

	size_t Size();														// ������ ���������������

	bool operator ==(Multiset& obj);									// ���������� ��������� = ��� ������ Multiset

	bool operator !=(Multiset& obj);									// ���������� ��������� != ��� ������ Multiset

	Multiset& operator = (Multiset& other);								// �������� ������������

	unsigned int operator [](size_t position);							// ���������� ��������� []

	Multiset& FirstAbsent();											// �������� ������ ������������� �������
};