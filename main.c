/*      В данной программе мы будем использовать тип uint32_t       */ 
/*      для хранения слова и работы с ним, 0 - 1 биты отданы        */
/*      для хранения бит четнотси прошлого сообщения                */
/*      2 - 31 биты занимает само слово                             */
#include <stdio.h>
#include <stdint.h>
#include <libserialport.h>
#include <stdbool.h>



/*      Создание масоек для вычисления четности        */
#define B25 ((uint32_t) 0x3B1F3480)
#define B26 ((uint32_t) 0x1D8F9A40)
#define B27 ((uint32_t) 0x2EC7CD00)
#define B28 ((uint32_t) 0x1763E680)
#define B29 ((uint32_t) 0x2BB1F340)
#define B30 ((uint32_t) 0x0B7A89C0)



/*      Создание масок для нахождения приеамболы        */
#define Pre ((uint8_t) 0x99)
#define InvPre ((uint8_t) 0x66)



uint8_t Xor(uint32_t);
uint8_t ParityCreate(uint32_t, uint8_t, uint8_t);
uint8_t PreambleChek(uint32_t);
uint8_t RollAndCut(uint8_t);
bool ParityChek(uint32_t);

int main()
{
    uint8_t buffer, cachebuffer;
    uint32_t word = 0x000000C0;
    uint8_t preambleresult;
    uint32_t message[32];
    bool parityresult;

    FILE *file = fopen("1.cor", "rb");
    if (file == NULL)
        printf("Error");
    
    while(1)
    {
        buffer = (uint8_t)fgetc(file);
        buffer = RollAndCut(buffer);
        word += buffer;
        word <<= 6;

/*      Проверяем приамбудлу, при ее наличии сдвигаем word так,     */
/*      чтобы преамбула становилась в начало слова                  */
/*      (начиная со 2ого бита), если сдвиг произошел не на целое    */
/*      число байт, то остаток байта заносим в cachebuffer          */
        preambleresult = PreambleChek(word);
        if (0 < preambleresult & preambleresult < 8)
        {
            word << preambleresult;
            buffer = (uint8_t)fgetc(file);
            buffer = RollAndCut(buffer);
            word += buffer >> preambleresult;
        }
        else if (preambleresult == 8)
            word = ~word;
        else if (8 < preambleresult & preambleresult < 16)
        {
            preambleresult -= 8;
            word << preambleresult;
            buffer = (uint8_t)fgetc(file);
            buffer = RollAndCut(buffer);
            word += buffer >> preambleresult;
            word = ~word;
        }
        else if (preambleresult == 16)
            continue;
        cachebuffer = buffer & (0xff >> (7 - preambleresult));

        if (ParityChek(word))
            message[0] = word;
        else
        {
            word <<= preambleresult;
            word += cachebuffer;
        }
                



    }

    return 0;
}

uint8_t Xor(uint32_t word)
{
    uint8_t bit = 0;
    for (int i = 0; i < 30; i++)
    {
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

bool ParityChek(uint32_t word)
{
    uint8_t parity = ParityCreate(word, word >> 31 & 0x01, word >> 30 & 0x01);
    if (word & 0x3f == parity)
        return true;
    return false;
}

uint8_t PreambleChek(uint32_t word)
{
    for (int i = 0; i < 8; i++)
    {
        word <<= 1;
        if (word & 0x3FC00000 == Pre)
            return i;
        else if (word & 0x3FC00000 == InvPre)
            return (i + 8);
    }
    return 16;
}

uint8_t RollAndCut(uint8_t buffer)
{
    uint8_t bufferroll = 0;
    for (int i = 0; i < 6; i++)
        bufferroll = ((buffer >> 5 - i & 0x01) == 1) ? (1 << i) : (0 << i);
    return bufferroll;
}

