/*      В данной программе мы будем использовать тип uint32_t       */ 
/*      для хранения слова и работы с ним, 0 - 1 биты отданы        */
/*      для хранения бит четнотси прошлого сообщения                */
/*      2 - 31 биты занимает само слово                             */
#include <stdio.h>
#include <stdint.h>
#include <libserialport.h>
#include <stdbool.h>



/*      Создание масоек для вычисления четности        */
#define B25 ((uint32_t) 0xEC7CD200)
#define B26 ((uint32_t) 0x763E6900)
#define B27 ((uint32_t) 0xBB1F3400)
#define B28 ((uint32_t) 0x5D8F9A00)
#define B29 ((uint32_t) 0xAEC7CD00)
#define B30 ((uint32_t) 0x2DEA2700)



/*      Создание масок для нахождения приеамболы        */
#define B30 ((uint8_t) 0x99)
#define B30 ((uint8_t) 0x66)



uint8_t Xor(uint32_t);
uint8_t ParityCreate(uint32_t, uint8_t, uint8_t);
bool PreambleChek(uint32_t);
bool ParityChek(uint32_t, uint8_t);

int main()
{
    return 0;
}

uint8_t Xor(uint32_t word)
{
    uint8_t bit = 0;
    word >>= 2;
    for (int i = 0; i < 30; i++) {
        bit ^= word & 0x00000001;
        word >>= 1;
    }
    return bit;
}

uint8_t ParityCreate(uint32_t word, uint8_t bit29, uint8_t bit30)
{
    uint8_t parity = 0;
    parity >> 5 |= Xor(word & B25) ^ bit29;
    parity >> 4 |= Xor(word & B26) ^ bit30;
    parity >> 3 |= Xor(word & B27) ^ bit29;
    parity >> 2 |= Xor(word & B28) ^ bit30;
    parity >> 1 |= Xor(word & B29) ^ bit30;
    parity |= Xor(word & B30) ^ bit29;
    return parity;
}

bool ParityChek(uint32_t word, uint8_t parity)
{
    word >>= 2;
    if (word & 0x3f == parity)
        return true;
    return false;
}

bool PreambleChek(uint32_t word)
{
    
} 