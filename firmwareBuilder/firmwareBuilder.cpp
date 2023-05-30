// firmwareBuilder.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define WORD 4UL
#define BLOCKSIZE (40 * WORD)

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
#include "FirmwareStruct.h"
void dump(void* ptr, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%02X ", ((uint8_t *)ptr)[i]);
    }
    printf("\n");
}
int main() {
    // Чтение входного файла с байтами прошивки
    setlocale(LC_ALL, "Russian");
    ifstream inputFile("E:\\WorkSpaces\\STM32\\Firmware\\Debug\\Firmware.bin", ios::binary);

    // Проверка, успешно ли открыт входной файл
    if (!inputFile) {
        cerr << "Ошибка открытия входного файла прошивки." << endl;
        return 1;
    }

    // Чтение байтов прошивки из файла
    vector<uint8_t> firmwareBytes(
        (istreambuf_iterator<char>(inputFile)),
        (istreambuf_iterator<char>())
    );
    while (firmwareBytes.size() % BLOCKSIZE)firmwareBytes.push_back(0x00);
    // Закрытие входного файла
    inputFile.close();

    // Создание заголовка прошивки
    FirmwareHeader header;
    header.blocksize = BLOCKSIZE;
    header.version = 1; // Установите версию прошивки по вашему выбору
    header.num_blocks = static_cast<uint16_t>((firmwareBytes.size() % BLOCKSIZE) ? (firmwareBytes.size() / BLOCKSIZE)+1 : ((firmwareBytes.size() / BLOCKSIZE)));
    calc_crc_header(&header); // Вычислите CRC по вашим требованиям
    //change_ending(&header);
    // Создание выходного файла
    ofstream outputFile("F303_new.bin", ios::binary);

    // Проверка, успешно ли открыт выходной файл
    if (!outputFile) {
        cerr << "Ошибка открытия выходного файла прошивки." << endl;
        return 1;
    }

    // Запись заголовка прошивки в выходной файл
    dump(&header, sizeof(header));
    outputFile.write(reinterpret_cast<char*>(&header), sizeof(FirmwareHeader));

    //change_ending(&header);
    // Разбивка байтов прошивки на блоки и запись каждого блока в выходной файл
    for (int i = 0; i < header.num_blocks; ++i) {
        FirmwareBlock block;
        block.data = &firmwareBytes[i * header.blocksize];
        calc_crc_block(&block, header.blocksize);
        outputFile.write(reinterpret_cast<char*>(block.data), header.blocksize);
        outputFile.write(reinterpret_cast<char*>(&block.crc), 1);
    }

    // Закрытие выходного файла
    outputFile.close();

    cout << "Преобразование файла прошивки завершено. numblocks = " << header.num_blocks  << endl;

    return 0;
}


// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
