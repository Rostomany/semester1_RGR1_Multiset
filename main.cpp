#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <list>
#include <tuple>
#include <map>
#include <stack>
#include "Multiset.h"
using namespace std;

// Перечисление содержит константы для обозначения классов символьных лексем: БУКВА, ЦИФРА, АР_ОП, ОТНОШ, 
//ПРОБЕЛ, LF, ОШИБКА, ",", ";" "[", "]", КОНЕЦ.
enum SymbolicTokenClass { Letter, Digit, Aur_OP, Relation, Space, LF, Error, Comma, Semicon, Open_Bracket, Close_Bracket, EndOfFile };

// Перечисление содержит константы для обозначения состояний автомата и одну дополнительную константу s_Stop.
enum States { s_A1, s_A2, s_B1, s_C1, s_D1, s_E1, s_E2, s_E3, s_F1, s_F2, s_F3, s_G1, s_H1, s_I1, s_I2, s_J1, s_K1, s_K2, s_K3, s_K4, s_K5, s_STOP };

enum Lexem { push, pop, ar_op, relat, jmp, ji, read, write, End, comment, err, mark_end, at, dif, firab, inter, neq, eq, power, symdif, uni };

//Структура хранит информацию о типе константы.
struct variable
{
    string name;
    int type;            //0 - int; 1 - Multiset
    int integer_value;
    Multiset multiset;
};

// Структура для представления символьной лексемы.
struct SymbolicToken
{
    SymbolicTokenClass token_class;   // Класс символьной лексемы.
    int value;                        // Значение символьной лексемы.
};

// Глобальные переменные, используемые процедурами автомата.
SymbolicToken s;    // Символьная лексема.

long long int reg_nT_pointer;   // Регистр указателя. Содержит указатель для лексем PUSH и POP
int reg_number;                 // Регистр числа. Используется для вычисления констант
int reg_relation;               // Регистр отношения. Хранит информацию о первом символе отношения
string reg_var_name;            // Регистр переменной. Накапливает имя переменной
int reg_detection = 0;          // Регистр обнаружения. Хранит номер позиции в таблице обнаружения для поиска ключевых слов.
int reg_line_num = 1;           // Номер строки. Хранит номер текущей строки в программе.
Lexem reg_class;                // Регистр класса. Служит для хранения класса лексемы
int reg_value;                  // Регистр значения. Хранит значения лексем
Multiset constMultiset;         // Регистр константного мультимножества. Используется для создания константы мультимножества в стеке.
int amount_multiset = 0;        // Регист количества мультимножеств.

map<string, variable> nameTable;
list<tuple<Lexem, long long int, int> > lexem_list;

stack<variable> main_stack;

vector<int>beg_vector(26);

void init_beg_vect()
{
    beg_vector[0] = 18;
    beg_vector[3] = 19;
    beg_vector[4] = 1;
    beg_vector[5] = 21;
    beg_vector[8] = 25;
    beg_vector[9] = 3;
    beg_vector[13] = 29;
    beg_vector[15] = 6;
    beg_vector[17] = 11;
    beg_vector[18] = 35;
    beg_vector[20] = 40;
    beg_vector[22] = 14;
}


const int state_number = 21; // Число состояний (без s_Stop).
const int class_number = 12; // Число символов входного алфавита.

typedef int (*function_pointer)();
function_pointer table[state_number][class_number];

vector<tuple<char, int, function_pointer>> detect_table(42);

/* Функция транслитерации.
 * ch - символ
 * Возвращаемое значение - результат транслитерации символа ch
 */
SymbolicToken transliterator(int ch)
{
    SymbolicToken s;
    s.value = 0;
    if (isdigit(ch))
    {
        s.token_class = Digit;
        s.value = ch - '0';
    }
    else if ((ch >= 65 && ch <= 90) || (ch >= 97 && ch <= 122))
    {
        s.token_class = Letter;
        s.value = ch;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%')
    {
        s.token_class = Aur_OP;
        s.value = ch;
    }
    else if (ch == '=' || ch == '!' || ch == '<' || ch == '>')
    {
        s.token_class = Relation;
        s.value = ch;

    }
    else if (ch == ' ' || ch == '\t')
    {
        s.token_class = Space;
    }
    else if (ch == '\n' || ch == '\r\n')
    {
        s.token_class = LF;
    }
    else if (ch == EOF)
    {
        s.token_class = EndOfFile;
    }
    else if (ch == ',')
    {
        s.token_class = Comma;
        s.value = ch;
    }
    else if (ch == ';')
    {
        s.token_class = Semicon;
        s.value = ch;
    }
    else if (ch == '[')
    {
        s.token_class = Open_Bracket;
        s.value = ch;
    }
    else if (ch == ']')
    {
        s.token_class = Close_Bracket;
        s.value = ch;
    }
    else
    {
        s.token_class = Error;
    }
    return s;
}



bool const_flag = false;
bool multiset_flag = false;
bool err_flag = false;


//Важные процедуры, не связанные с состояниями
void create_lexem()
{
    if (reg_class == push || reg_class == pop)
    {
        tuple<Lexem, long long int, int> tup(reg_class, reg_nT_pointer, reg_line_num);
        lexem_list.push_back(tup);
    }
    else if (reg_class == ar_op)
    {
        tuple<Lexem, long long int, int> tup(reg_class, reg_value, reg_line_num);
        lexem_list.push_back(tup);
    }
    else if (reg_class == relat)
    {
        tuple<Lexem, long long int, int> tup(reg_class, reg_relation, reg_line_num);
        lexem_list.push_back(tup);
    }
    else if (reg_class == jmp || reg_class == ji)
    {
        tuple<Lexem, long long int, int> tup(reg_class, reg_number, reg_line_num);
        lexem_list.push_back(tup);
    }
    else if (reg_class == read || reg_class == write || reg_class == End || reg_class == comment || reg_class == mark_end)
    {
        tuple<Lexem, long long int, int> tup(reg_class, -1, reg_line_num);
        lexem_list.push_back(tup);
    }
    else if (reg_class == err)
    {
        tuple<Lexem, long long int, int> tup(reg_class, reg_line_num, reg_line_num);
        lexem_list.push_back(tup);
    }
    else if (reg_class == at || reg_class == dif || reg_class == firab || reg_class == inter || reg_class == neq || reg_class == eq || reg_class == power || reg_class == symdif || reg_class == uni)
    {
        tuple<Lexem, long long int, int> tup(reg_class, -1, reg_line_num);
        lexem_list.push_back(tup);
    }
}

int Error1()
{
    reg_class = err;
    create_lexem();
    err_flag = true;
    return s_J1;
}

string ITOA(int num)
{
    string str;
    for (int i = floor(log10(num)); i >= 0; --i)
    {
        str.push_back((num / (int)pow(10, i) % 10) + '0');
    }
    return str;
}

void add_constant()
{
    if (const_flag == false && multiset_flag == false)
        return;

    variable var;
    if (const_flag == true)
    {
        var.type = 0;
        var.name = ITOA(reg_number);
        var.integer_value = reg_number;
    }
    else
    {
        var.type = 1;
        amount_multiset++;
        var.name = "#multiset" + ITOA(amount_multiset);
        var.multiset = constMultiset;
    }
    bool flag = false;
    for (auto it : nameTable)
    {
        if ((get<0>(it) == var.name) && ((get<1>(it).integer_value == var.integer_value) || get<1>(it).multiset == var.multiset))
        {
            flag = true;
            reg_nT_pointer = reinterpret_cast<long long int>(&nameTable[var.name]);
            return;
        }
    }
    if (!flag)
        nameTable.insert(pair<string, variable>(var.name, var));
    reg_nT_pointer = reinterpret_cast<long long int>(&nameTable[var.name]);
}

void add_variable()
{
    if (reg_var_name == "push" || reg_var_name == "pop" || reg_var_name == "jmp" || reg_var_name == "ji")
    {
        Error1();
        return;
    }
    else if (reg_var_name == "end" || reg_var_name == "write" || reg_var_name == "read")
    {
        Error1();
        return;
    }
    else if (reg_var_name == "at" || reg_var_name == "dif" || reg_var_name == "firab" || reg_var_name == "inter" || reg_var_name == "neq" || reg_var_name == "eq" || reg_var_name == "power" || reg_var_name == "symdif" || reg_var_name == "uni")
    {
        Error1();
        return;
    }

    variable var;
    var.name = reg_var_name;
    var.type = 0;
    bool flag = false;
    for (auto it : nameTable)
    {
        if ((get<0>(it) == var.name) && (get<1>(it).integer_value == var.integer_value) && (get<1>(it).type == var.type))
        {
            flag = true;
            reg_nT_pointer = reinterpret_cast<long long int>(&nameTable[var.name]);
            return;
        }
    }
    if (!flag)
        nameTable.insert(pair<string, variable>(var.name, var));
    reg_nT_pointer = reinterpret_cast<long long int>(&nameTable[var.name]);
}

//Процедуры автомата
int A1a()
{
    create_lexem();
    reg_line_num++;
    return s_A1;
}
int A1b()
{
    reg_line_num++;
    return s_A1;
}
int A2a()
{
    reg_line_num++;
    return s_A2;
}
int A2b()
{
    create_lexem();
    reg_line_num++;
    return s_A2;
}
int A2c()
{
    add_constant();
    create_lexem();
    reg_line_num++;
    return s_A2;
}
int A2d()
{
    add_variable();
    if (reg_class != err)
        create_lexem();
    reg_line_num++;
    return s_A2;
}
int A2e()
{
    if (reg_relation == '!') Error1();
    if (reg_relation == '=') reg_relation = 1;
    else if (reg_relation == '<') reg_relation = 3;
    else if (reg_relation == '>') reg_relation = 4;
    create_lexem();
    reg_line_num++;
    return s_A2;
}
int A2f()
{
    reg_class = err;
    err_flag = 1;
    create_lexem();
    reg_line_num++;
    return s_A2;
}
int B1a()
{
    reg_detection = beg_vector[s.value - (size_t)97];
    if (reg_detection == 0)
        Error1();
    return s_B1;

}
int B1b()
{
    ++reg_detection;
    return s_B1;
}
int C1a()
{
    reg_class = ar_op;
    reg_value = s.value;
    create_lexem();
    return s_C1;
}
int C1b()
{
    reg_class = End;
    create_lexem();
    return s_C1;
}
int C1c()
{
    reg_class = read;
    create_lexem();
    return s_C1;
}
int C1d()
{
    reg_class = write;
    create_lexem();
    return s_C1;
}
int C1e()
{
    add_constant();
    create_lexem();
    return s_C1;
}
int C1f()
{
    add_variable();
    create_lexem();
    return s_C1;
}
int C1g()
{
    if (reg_relation == '!')
        Error1();
    if (reg_relation == '=')
        reg_relation = 1;
    else if (reg_relation == '<')
        reg_relation = 3;
    else if (reg_relation == '>')
        reg_relation = 4;

    create_lexem();
    return s_C1;
}
int C1h()
{
    if (s.value == '=')
    {
        if (reg_relation == '!')
        {
            reg_relation = 2;
        }
        else if (reg_relation == '<')
        {
            reg_relation = 5;
        }
        else if (reg_relation == '>')
        {
            reg_relation = 6;
        }
        else
            return Error1();

        create_lexem();
        return s_C1;
    }
    return Error1();
}
int C1i()
{
    reg_class = at;
    create_lexem();
    return s_C1;
}
int C1j()
{
    reg_class = dif;
    create_lexem();
    return s_C1;
}
int C1k()
{
    reg_class = firab;
    create_lexem();
    return s_C1;
}
int C1l()
{
    reg_class = inter;
    create_lexem();
    return s_C1;
}
int C1m()
{
    reg_class = neq;
    create_lexem();
    return s_C1;
}
int C1n()
{
    reg_class = eq;
    create_lexem();
    return s_C1;
}
int C1o()
{
    reg_class = power;
    create_lexem();
    return s_C1;
}
int C1p()
{
    reg_class = symdif;
    create_lexem();
    return s_C1;
}
int C1q()
{
    reg_class = uni;
    create_lexem();
    return s_C1;
}
bool check_empty_mult = true;
int C1r()
{
    if (check_empty_mult == true )
    {
        Multiset temp;
        constMultiset = temp;
    }
    else
    {
        if (constMultiset.Power() == 0)
        {
            Multiset temp;
            temp.Insert(reg_number);
            constMultiset = temp;
        }
        else
            constMultiset.Insert(reg_number);
    }
    multiset_flag = true;
    check_empty_mult = true;
    const_flag = false;
    add_constant();
    constMultiset.Clear();
    create_lexem();
    return s_C1;
}
int D1a()
{
    reg_class = relat;
    reg_relation = s.value;
    return s_D1;
}
int E1a()
{
    reg_class = push;
    const_flag = 1;
    return s_E1;
}
int E2a()
{
    reg_class = ji;
    const_flag = 0;
    return s_E2;
}
int E2b()
{
    reg_class = jmp;
    const_flag = 0;
    return s_E2;
}
int E3a()
{
    reg_class = pop;
    return s_E3;
}
int G1a()
{
    reg_number = s.value;
    return s_G1;
}
int G1b()
{
    reg_number = 10 * reg_number + s.value;
    return s_G1;
}
int H1a()
{
    reg_var_name = (s.value);
    return s_H1;
}
int H1b()
{
    reg_var_name += (s.value);
    return s_H1;
}
int I1a()
{
    reg_class = comment;
    return s_I1;
}
int I2a()
{
    reg_class = comment;
    return s_I2;
}
int I2b()
{
    add_constant();
    create_lexem();
    reg_class = comment;
    return s_I2;
}
int I2c()
{
    add_variable();
    create_lexem();
    reg_class = comment;
    return s_I2;
}
int I2d()
{
    if (reg_relation == '!')
        Error1();
    create_lexem();
    reg_class = comment;
    return s_I2;

}
int K1a()
{
    if (constMultiset.Power() == 0)
    {
        Multiset temp;
        temp.Insert(reg_number);
        constMultiset = temp;
    }
    else
        constMultiset.Insert(reg_number);
    return s_K1;
}
int K2a()
{
    check_empty_mult = false;
    reg_number = s.value;
    return s_K2;
}
int K2b()
{
    reg_number = 10 * reg_number + s.value;
    return s_K2;
}
int EXIT1()
{
    reg_class = mark_end;
    create_lexem();
    return s_STOP;
}
int EXIT2()
{
    if (reg_relation == '!')
    {
        reg_class = err;
        err_flag = 1;
    }
    create_lexem();
    reg_class = mark_end;
    create_lexem();
    return s_STOP;
}
int EXIT3()
{
    add_constant();
    create_lexem();
    reg_class = mark_end;
    create_lexem();
    return s_STOP;
}
int EXIT4()
{
    add_variable();
    create_lexem();
    reg_class = mark_end;
    create_lexem();
    return s_STOP;
}
int p_A1()
{
    return s_A1;
}
int p_A2()
{
    reg_line_num++;
    return s_A2;
}
int p_C1()
{
    return s_C1;
}
int p_F1()
{
    return s_F1;
}
int p_F2()
{
    return s_F2;
}
int p_F3()
{
    return s_F3;
}
int p_I1()
{
    return s_I1;
}
int p_I2()
{
    return s_I2;
}
int p_J1()
{
    return s_J1;
}
int p_K3()
{
    return s_K3;
}
int p_K4()
{
    return s_K4;
}
int p_K5()
{
    return s_K5;
}
int M1()
{
    if (reg_detection == 0)
        return s_J1;
    if (s.value == get<0>(detect_table[(reg_detection - (size_t)1)]))
        return (*get<2>(detect_table[(reg_detection - (size_t)1)]))();
    else
    {
        reg_detection = get<1>(detect_table[(reg_detection - (size_t)1)]);
        if (reg_detection == 0)
            Error1();
        M1();
    }
}


void init_detect_table()
{
    detect_table[0] = tuple<char, int, function_pointer>('n', 31, B1b);
    detect_table[1] = tuple<char, int, function_pointer>('d', 0, C1b);
    detect_table[2] = tuple<char, int, function_pointer>('i', 4, E2a);
    detect_table[3] = tuple<char, int, function_pointer>('m', 0, B1b);
    detect_table[4] = tuple<char, int, function_pointer>('p', 0, E2b);
    detect_table[5] = tuple<char, int, function_pointer>('o', 8, B1b);
    detect_table[6] = tuple<char, int, function_pointer>('p', 32, E3a);
    detect_table[7] = tuple<char, int, function_pointer>('u', 0, B1b);
    detect_table[8] = tuple<char, int, function_pointer>('s', 0, B1b);
    detect_table[9] = tuple<char, int, function_pointer>('h', 0, E1a);
    detect_table[10] = tuple<char, int, function_pointer>('e', 0, B1b);
    detect_table[11] = tuple<char, int, function_pointer>('a', 0, B1b);
    detect_table[12] = tuple<char, int, function_pointer>('d', 0, C1c);
    detect_table[13] = tuple<char, int, function_pointer>('r', 0, B1b);
    detect_table[14] = tuple<char, int, function_pointer>('i', 0, B1b);
    detect_table[15] = tuple<char, int, function_pointer>('t', 0, B1b);
    detect_table[16] = tuple<char, int, function_pointer>('e', 0, C1d);
    detect_table[17] = tuple<char, int, function_pointer>('t', 0, C1i);
    detect_table[18] = tuple<char, int, function_pointer>('i', 0, B1b);
    detect_table[19] = tuple<char, int, function_pointer>('f', 0, C1j);
    detect_table[20] = tuple<char, int, function_pointer>('i', 0, B1b);
    detect_table[21] = tuple<char, int, function_pointer>('r', 0, B1b);
    detect_table[22] = tuple<char, int, function_pointer>('a', 0, B1b);
    detect_table[23] = tuple<char, int, function_pointer>('b', 0, C1k);
    detect_table[24] = tuple<char, int, function_pointer>('n', 0, B1b);
    detect_table[25] = tuple<char, int, function_pointer>('t', 0, B1b);
    detect_table[26] = tuple<char, int, function_pointer>('e', 0, B1b);
    detect_table[27] = tuple<char, int, function_pointer>('r', 0, C1l);
    detect_table[28] = tuple<char, int, function_pointer>('e', 0, B1b);
    detect_table[29] = tuple<char, int, function_pointer>('q', 0, C1m);
    detect_table[30] = tuple<char, int, function_pointer>('q', 0, C1n);
    detect_table[31] = tuple<char, int, function_pointer>('w', 0, B1b);
    detect_table[32] = tuple<char, int, function_pointer>('e', 0, B1b);
    detect_table[33] = tuple<char, int, function_pointer>('r', 0, C1o);
    detect_table[34] = tuple<char, int, function_pointer>('y', 0, B1b);
    detect_table[35] = tuple<char, int, function_pointer>('m', 0, B1b);
    detect_table[36] = tuple<char, int, function_pointer>('d', 0, B1b);
    detect_table[37] = tuple<char, int, function_pointer>('i', 0, B1b);
    detect_table[38] = tuple<char, int, function_pointer>('f', 0, C1p);
    detect_table[39] = tuple<char, int, function_pointer>('n', 0, B1b);
    detect_table[40] = tuple<char, int, function_pointer>('i', 0, C1q);
}

void initialize_table()
{
    for (int i = 0; i < state_number; i++)
    {
        for (int j = 0; j < class_number; j++)
        {
            table[i][j] = Error1;
        }
    }
    // Далее заполняются ячейки для остальных процедур.
    table[s_A1][Letter] = B1a; table[s_A1][Aur_OP] = C1a; table[s_A1][Relation] = D1a;
    table[s_A1][Space] = p_A1; table[s_A1][LF] = A1b; table[s_A1][Semicon] = I1a;

    table[s_A2][Letter] = B1a; table[s_A2][Aur_OP] = C1a; table[s_A2][Relation] = D1a;
    table[s_A2][Space] = p_A2; table[s_A2][LF] = A2a; table[s_A2][Semicon] = I2a; table[s_A2][EndOfFile] = EXIT1;

    table[s_B1][Letter] = M1; table[s_B1][LF] = A2f;

    table[s_C1][Space] = p_C1; table[s_C1][LF] = A2a; table[s_C1][Semicon] = I2a; table[s_C1][EndOfFile] = EXIT1;

    table[s_D1][Relation] = C1h; table[s_D1][Space] = C1g; table[s_D1][LF] = A2e; table[s_D1][Semicon] = I2d; table[s_D1][EndOfFile] = EXIT2;

    table[s_E1][Space] = p_F1; table[s_E1][LF] = A2f;

    table[s_E2][Space] = p_F2; table[s_E2][LF] = A2f;
    table[s_E3][Space] = p_F3; table[s_E3][LF] = A2f;

    table[s_F1][Letter] = H1a; table[s_F1][Digit] = G1a; table[s_F1][Space] = p_F1; table[s_F1][LF] = A2f; table[s_F1][Open_Bracket] = p_K4;

    table[s_F2][Digit] = G1a; table[s_F2][Space] = p_F2; table[s_F2][LF] = A2f;

    table[s_F3][Letter] = H1a; table[s_F3][Space] = p_F3; table[s_F3][LF] = A2f;

    table[s_G1][Digit] = G1b; table[s_G1][Space] = C1e; table[s_G1][LF] = A2c; table[s_G1][Semicon] = I2b; table[s_G1][EndOfFile] = EXIT3;

    table[s_H1][Letter] = H1b; table[s_H1][Digit] = H1b; table[s_H1][Space] = C1f; table[s_H1][LF] = A2d; table[s_H1][Semicon] = I2c; table[s_H1][EndOfFile] = EXIT4;

    table[s_K1][Digit] = K2a;

    table[s_K2][Digit] = K2b; table[s_K2][Comma] = K1a; table[s_K2][Close_Bracket] = p_K5;

    table[s_K3][Digit] = K2a; table[s_K3][Close_Bracket] = p_K5;

    table[s_K4][Open_Bracket] = p_K3;

    table[s_K5][Close_Bracket] = C1r;

    for (int i = 0; i < 11; ++i)
    {
        if (i == LF)
        {
            table[s_I1][i] = A1a;
            table[s_I2][i] = A2b;
            table[s_J1][i] = A2a;
        }
        else
        {
            table[s_I1][i] = p_I1;
            table[s_I2][i] = p_I2;
            table[s_J1][i] = p_J1;
        }
    }

    table[s_I2][EndOfFile] = EXIT1;
    table[s_J1][EndOfFile] = EXIT1;
}

void parse(const char* filename)
{
    ifstream in(filename);
    if (!in)
    {
        cout << "Couldn't open file " << filename << endl;
        return;
    }
    int ch;             // Входной символ.
    int state = s_A1;   // Текущее состояние автомата.
    while (state != s_STOP)
    {
        ch = in.get();
        s = transliterator(ch);
        state = (*table[state][s.token_class])();
    }

    in.close();
}


const char* relation_table(long long int ch)
{
    switch (ch)
    {
    case(1): {return "="; }
    case(2): {return "!="; }
    case(3): {return "<"; }
    case(4): {return ">"; }
    case(5): {return "<="; }
    case(6): {return ">="; }
    default:
        break;
    }
    return 0;
}

void printNameTable()
{
    cout << "VARIABLES" << endl;
    for (auto it : nameTable)
    {
        if (get<0>(it)[0] > '9' && get<0>(it)[0] != '#')
        {
            cout << get<0>(it);
            if (get<1>(it).type == 0)
            {
                if (get<1>(it).integer_value > -858993460)
                    cout << " = " << get<1>(it).integer_value << endl;
                else
                    cout << endl;
            }
            else
                cout << " - " << get<1>(it).multiset << endl;
        }
    }
    cout << endl;
}

void printConstTable()
{
    cout << "CONSTANTS" << endl;
    for (auto it : nameTable)
    {
        if (get<0>(it)[0] >= '0' && get<0>(it)[0] <= '9')
            cout << get<0>(it) << endl;

        if (get<0>(it)[0] == '#')
            cout << get<1>(it).multiset << endl;
    }
    cout << endl;
}

void printLexemList()
{
    for (auto it = lexem_list.begin(); it != lexem_list.end(); ++it)
    {
        switch (get<0>(*it))
        {
        case 0:
        {
            cout << get<2>(*it) << ": ";
            cout << "push" << ' ';
            if (((variable*)get<1>(*it))->name[0] > '9' && ((variable*)get<1>(*it))->name[0] != '#')
            {
                cout << ((variable*)get<1>(*it))->name << ' ';

                if (((variable*)get<1>(*it))->integer_value > -858993460)
                    cout << " = " << ((variable*)get<1>(*it))->integer_value << endl;
                else
                    cout << endl;
            }
            if (((variable*)get<1>(*it))->name[0] >= '0' && ((variable*)get<1>(*it))->name[0] <= '9')
            {
                cout << ((variable*)get<1>(*it))->name << ' ' << endl;
            }
            if (((variable*)get<1>(*it))->name[0] == '#')
                cout << ((variable*)get<1>(*it))->multiset;
            break;
        }
        case 1:
        {
            cout << get<2>(*it) << ": ";
            cout << "pop" << ' ';
            cout << ((variable*)get<1>(*it))->name << endl;
            break;
        }
        case 2:
        {
            cout << get<2>(*it) << ": ";
            cout << (char)get<1>(*it) << endl;
            break;
        }
        case 3:
        {
            cout << get<2>(*it) << ": ";
            cout << relation_table(get<1>(*it)) << endl;
            break;
        }
        case 4:
        {
            cout << get<2>(*it) << ": ";
            cout << "jmp" << ' ';
            cout << get<1>(*it) << endl;
            break;
        }
        case 5:
        {
            cout << get<2>(*it) << ": ";
            cout << "ji" << ' ';
            cout << get<1>(*it) << endl;
            break;
        }
        case 6:
        {
            cout << get<2>(*it) << ": ";
            cout << "read" << endl;
            break;
        }
        case 7:
        {
            cout << get<2>(*it) << ": ";
            cout << "write" << endl;
            break;
        }
        case 8:
        {
            cout << get<2>(*it) << ": ";
            cout << "end" << endl;
            break;
        }
        case 9:
        {
            cout << get<2>(*it) << ": ";
            cout << "comment" << endl;
            break;
        }
        case 10:
        {
            cout << "error in line" << ' ';
            cout << get<1>(*it) << endl;
            break;
        }
        case 11:
        {
            cout << get<2>(*it) << ": ";
            cout << "endoffile" << endl;
            break;

        }
        case 12:
        {
            cout << get<2>(*it) << ": ";
            cout << "at" << endl;
            break;
        }
        case 13:
        {
            cout << get<2>(*it) << ": ";
            cout << "dif" << endl;
            break;
        }
        case 14:
        {
            cout << get<2>(*it) << ": ";
            cout << "firab" << endl;
            break;
        }
        case 15:
        {
            cout << get<2>(*it) << ": ";
            cout << "inter" << endl;
            break;
        }
        case 16:
        {
            cout << get<2>(*it) << ": ";
            cout << "neq" << endl;
            break;
        }
        case 17:
        {
            cout << get<2>(*it) << ": ";
            cout << "eq" << endl;
            break;
        }
        case 18:
        {
            cout << get<2>(*it) << ": ";
            cout << "power" << endl;
            break;
        }
        case 19:
        {
            cout << get<2>(*it) << ": ";
            cout << "symdif" << endl;
            break;
        }
        case 20:
        {
            cout << get<2>(*it) << ": ";
            cout << "uni" << endl;
            break;
        }
        default:
            break;
        };
    }
}

void interpretator()
{
    if (err_flag)
    {
        cout << "There are errors in programm." << endl;
        return;
    }
    try
    {
        for (auto lex = lexem_list.begin(); lex != lexem_list.end(); ++lex)
        {
            switch (get<0>(*lex))
            {
            case 0:// push
            {
                main_stack.push(*(variable*)get<1>(*lex));
                break;
            }
            case 1:// pop
            {
                if (main_stack.top().type == 0)
                {
                    (*(variable*)get<1>(*lex)).integer_value = main_stack.top().integer_value;
                    (*(variable*)get<1>(*lex)).type = 0;
                }
                else
                {
                    (*(variable*)get<1>(*lex)).multiset = main_stack.top().multiset;
                    (*(variable*)get<1>(*lex)).type = 1;
                }
                main_stack.pop();
                break;
            }
            case 2:// aurethmetic operation
            {
                auto secondVar = main_stack.top();
                main_stack.pop();
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 0 && secondVar.type == 0)
                {
                    variable res;
                    res.name = "";
                    res.type = 0;
                    switch (std::get<1>(*lex))
                    {
                    case 37: // %
                    {
                        res.integer_value = (firstVar.integer_value) % (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    case 42: // *
                    {
                        res.integer_value = (firstVar.integer_value) * (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    case 43: // +
                    {
                        res.integer_value = (firstVar.integer_value) + (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    case 45: // -
                    {
                        res.integer_value = (firstVar.integer_value) - (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    case 47: // /
                    {
                        res.integer_value = (firstVar.integer_value) / (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }

                    default:
                        break;
                    };
                }
                else
                    throw 1;
                break;
            }
            case 3:// relations
            {
                auto secondVar = main_stack.top();
                main_stack.pop();
                auto firstVar = main_stack.top();
                main_stack.pop();
                if (firstVar.type == 0 && secondVar.type == 0)
                {
                    variable res;
                    res.name = "";
                    res.type = 0;
                    switch (get<1>(*lex))
                    {
                    case 1: // ==
                    {
                        res.integer_value = (firstVar.integer_value) == (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    case 2: // !=
                    {
                        res.integer_value = (firstVar.integer_value) != (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    case 3: // <
                    {
                        res.integer_value = (firstVar.integer_value) < (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    case 4: // >
                    {
                        res.integer_value = (firstVar.integer_value) > (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    case 5: // <=
                    {
                        res.integer_value = (firstVar.integer_value) <= (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    case 6: // >=
                    {
                        res.integer_value = (firstVar.integer_value) >= (secondVar.integer_value);
                        main_stack.push(res);

                        break;
                    }
                    default:
                        break;
                    };
                }
                else
                    throw 1;
                break;
            }
            case 4:// jmp
            {
                long long int line_num = get<1>(*lex);
                for (auto lexSearch = lexem_list.begin(); lexSearch != lexem_list.end(); ++lexSearch)
                {
                    if (get<2>(*lexSearch) >= line_num)
                    {
                        lex = lexSearch;
                        --lex;
                        break;
                    }
                }
                break;
            }
            case 5:// ji
            {
                if (main_stack.top().integer_value == 0)
                {
                    main_stack.pop();
                    break;
                }
                long long int line_num = get<1>(*lex);
                main_stack.pop();
                for (auto lexSearch = lexem_list.begin(); lexSearch != lexem_list.end(); ++lexSearch)
                {
                    if (get<2>(*lexSearch) >= line_num)
                    {
                        lex = lexSearch;
                        --lex;
                        break;
                    }
                }
                break;
            }
            case 6:// read
            {
                if (cin.peek() == '\n')
                    cin.ignore();

                if (cin.peek() == '[')
                {
                    Multiset input;
                    cin >> input;
                    cout << endl;

                    variable var;
                    amount_multiset++;
                    var.name = "#multiset" + ITOA(amount_multiset);
                    var.multiset = input;
                    var.type = 1;
                    main_stack.push(var);
                }
                else
                {
                    int input = 0;
                    cin >> input;
                    cout << endl;

                    if (input < 0)
                    {
                        cout << "Non-natural number has been inputed" << endl;
                        return;
                    }

                    variable var;
                    var.name = "";
                    var.integer_value = input;
                    var.type = 0;
                    main_stack.push(var);
                }
                break;
            }
            case 7:// write
            {
                if (main_stack.top().type == 0)
                    cout << main_stack.top().integer_value << endl;
                else
                    cout << main_stack.top().multiset << endl;
                main_stack.pop();

                break;
            }
            case 8:// end
            {
                return;
                break;
            }
            case 11:// EndOfFile 
            {
                return;
                break;
            }
            case 12:// at
            {
                auto secondVar = main_stack.top();
                main_stack.pop();
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 0 && secondVar.type == 1)
                {
                    variable res;
                    res.name = "";
                    res.integer_value = secondVar.multiset[firstVar.integer_value];
                    res.type = 0;
                    main_stack.push(res);
                }
                else
                    throw 1;

                break;
            }
            case 13:// dif
            {
                auto secondVar = main_stack.top();
                main_stack.pop();
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 1 && secondVar.type == 1)
                {
                    variable res;
                    amount_multiset++;
                    res.name = "#multiset" + ITOA(amount_multiset);
                    res.multiset.Difference(firstVar.multiset, secondVar.multiset);
                    res.type = 1;
                    main_stack.push(res);
                }
                else
                    throw 1;

                break;
            }
            case 14:// firab
            {
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 1)
                {
                    firstVar.multiset.FirstAbsent();
                    main_stack.push(firstVar);
                }
                else
                    throw 1;

                break;
            }
            case 15:// inter
            {
                auto secondVar = main_stack.top();
                main_stack.pop();
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 1 && secondVar.type == 1)
                {
                    variable res;
                    amount_multiset++;
                    res.name = "#multiset" + ITOA(amount_multiset);
                    res.multiset.Intersection(firstVar.multiset, secondVar.multiset);
                    res.type = 1;
                    main_stack.push(res);
                }
                else
                    throw 1;

                break;
            }
            case 16:// neq
            {
                auto secondVar = main_stack.top();
                main_stack.pop();
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 1 && secondVar.type == 1)
                {
                    variable res;
                    res.name = "";
                    res.integer_value = firstVar.multiset != secondVar.multiset;
                    res.type = 0;
                    main_stack.push(res);
                }
                else
                    throw 1;

                break;
            }
            case 17:// eq
            {
                auto secondVar = main_stack.top();
                main_stack.pop();
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 1 && secondVar.type == 1)
                {
                    variable res;
                    res.name = "";
                    res.integer_value = firstVar.multiset == secondVar.multiset;
                    res.type = 0;
                    main_stack.push(res);
                }
                else
                    throw 1;

                break;
            }
            case 18:// power
            {
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 1)
                {
                    variable res;
                    res.name = "";
                    res.integer_value = firstVar.multiset.Power();
                    res.type = 0;
                    main_stack.push(res);
                }
                else
                    throw 1;

                break;
            }
            case 19:// symdif
            {
                auto secondVar = main_stack.top();
                main_stack.pop();
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 1 && secondVar.type == 1)
                {
                    variable res;
                    amount_multiset++;
                    res.name = "#multiset" + ITOA(amount_multiset);
                    res.multiset.SDifference(secondVar.multiset, firstVar.multiset);
                    res.type = 1;
                    main_stack.push(res);
                }
                else
                    throw 1;

                break;
            }
            case 20:// uni
            {
                auto secondVar = main_stack.top();
                main_stack.pop();
                auto firstVar = main_stack.top();
                main_stack.pop();

                if (firstVar.type == 1 && secondVar.type == 1)
                {
                    variable res;
                    amount_multiset++;
                    res.name = "#multiset" + ITOA(amount_multiset);
                    res.multiset.Union(firstVar.multiset, secondVar.multiset);
                    res.type = 1;
                    main_stack.push(res);
                }
                else
                    throw 1;

                break;
            }
            default:
                break;
            };
        }
    }
    catch (int)
    {
        cout << "Error. Commands do not match values." << endl;
        return;
    }
}


int main()
{
    init_beg_vect();
    init_detect_table();
    initialize_table();

    parse("input3");


    printNameTable();
    printConstTable();
    printLexemList();
    interpretator();

    return 0;
}