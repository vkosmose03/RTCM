#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <gtk/gtk.h>

GMutex lock1;
uint8_t *wordbit29, *wordbit30;
uint32_t *seq;
uint32_t ***messagefiled;
uint32_t *byteoffirst, *byteofsecond;
GtkTextBuffer *global_buffer = NULL;

typedef struct 
{
  GtkWidget *entry1;
  GtkWidget *entry2;
} EntryData;


typedef struct 
{
  GtkWidget *entry1;
  GtkWidget *entry2;
  GtkWidget *entry3;
} EntryDataWrite;


void initialize() {
      g_mutex_init(&lock1);

    wordbit29 = (uint8_t *)malloc(sizeof(uint8_t));
    if (!wordbit29) {
        perror("Failed to allocate memory for wordbit29");
        exit(1);
    }
    
    wordbit30 = (uint8_t *)malloc(sizeof(uint8_t));
    if (!wordbit30) {
        perror("Failed to allocate memory for wordbit30");
        exit(1);
    }
    
    seq = (uint32_t *)malloc(sizeof(uint32_t));
    if (!seq) {
        perror("Failed to allocate memory for seq");
        exit(1);
    }

    messagefiled = (uint32_t ***)malloc(64 * sizeof(uint32_t **));
    if (messagefiled == NULL) {
        perror("Failed to allocate memory for messagefild");
        getchar();
        exit(1);
    }
    for(int i = 0; i < 64; i++) {
        messagefiled[i] = (uint32_t **)malloc(32 * sizeof(uint32_t *));
        if (messagefiled == NULL) {
            perror("Failed to allocate memory for messagefild");
            getchar();
            exit(1);
        }
        for(int j = 0; j < 32; j++)
            messagefiled[i][j] = NULL;
    }

    
    *seq = 0;
    *wordbit29 = 1;
    *wordbit30 = 1;
}


uint8_t wordXor(uint32_t word){
    char bit = 0;
    uint32_t buf = word;
    for(int i = 0; i < 30; i++){
        bit = bit ^ (buf & 0x01);
        buf >>= 1;
    }
    return bit;
}

uint8_t ParityCreate(uint32_t word, uint8_t Bit30, uint8_t Bit29){

    uint8_t cod = 0;
    uint32_t bufWord = word;
    cod += wordXor(bufWord & 0x3b1f3480) ^ Bit29;
    cod <<= 1;
    cod += wordXor(bufWord & 0x1d8f9a40) ^ Bit30;
    cod <<= 1;
    cod += wordXor(bufWord & 0x2ec7cd00) ^ Bit29;
    cod <<= 1;
    cod += wordXor(bufWord & 0x1763e680) ^ Bit30;
    cod <<= 1;
    cod += wordXor(bufWord & 0x2bb1f340) ^ Bit30;
    cod <<= 1;
    cod += wordXor(bufWord & 0x0b7a89c0) ^ Bit29;

    return cod;
}

uint32_t SequencesChanger(uint32_t word) {

    uint32_t seqe = *seq;
    word = (word & 0xfff8fffc) | ((seqe << 16) & 0x00070000);
    
    if(*seq == 7) {
        *seq = 0;
    } else {
        *seq = *seq + 1;
    }
    
    return word;

}

typedef struct {
    bool preamble_result;
    uint8_t preamble_place;
    uint8_t Preamble_priznak;
} Preamble;

Preamble PreambleSearch(uint64_t potok, uint8_t bit30, uint8_t bit29) {
    Preamble pre;
    uint32_t word;
    uint8_t parity;
    bool parity_result = false;
    pre.preamble_result = false;
    for(int f = 0; f < 32; f++) {
        if(bit30 == 0) {
            if(((potok << f) & 0xff00000000000000) == 0x6600000000000000) {
                pre.preamble_place = f;
                pre.Preamble_priznak = 0;
                word = (potok >> (32 - pre.preamble_place)) & 0xffffffff;

                if(bit30 == 1) {
                    word = (~word & 0xffffff00) | word & 0x000000ff;
                }
                parity = ParityCreate(((word >> 2) & 0x3fffffff), bit30, bit29);
                if(((word >> 2) & 0x0000003f) == parity) {
                    parity_result = true;
                }
                if(!parity_result) {
                    continue;
                }

                pre.preamble_result = true;
                break;
            }
        } else if(bit30 == 1) {
            if(((potok << f) & 0xff00000000000000) == 0x9900000000000000) {
                pre.preamble_place = f;
                pre.Preamble_priznak = 1;
                word = (potok >> (32 - pre.preamble_place)) & 0xffffffff;

                if(bit30 == 1) {
                    word = (~word & 0xffffff00) | word & 0x000000ff;
                }
                parity = ParityCreate(((word >> 2) & 0x3fffffff), bit30, bit29);
                if(((word >> 2) & 0x0000003f) == parity) {
                    parity_result = true;
                }
                if(!parity_result) {
                    continue;
                }

                pre.preamble_result = true;
                break;
            }
        }
    }
    return pre;
}

void log_to_text_view(const char *message) {
    if (global_buffer) {
        gtk_text_buffer_set_text(global_buffer, message, -1);
    }
}

gpointer COMWriter(gpointer user_data) {
    EntryDataWrite *dataw = (EntryDataWrite *)user_data;
    const gchar *com_name = gtk_entry_get_text(GTK_ENTRY(dataw->entry1));
    const gchar *com_speed = gtk_entry_get_text(GTK_ENTRY(dataw->entry2));
    const gchar *zader = gtk_entry_get_text(GTK_ENTRY(dataw->entry3));
    char *COM3 = g_strdup(com_name);
    char *endptr1;
    uint32_t speed3 = (uint32_t)strtoul(com_speed, &endptr1, 10);
    char *endptr2;
    uint16_t zaderzka = (uint16_t)strtoul(zader, &endptr2, 10);
    
    g_free(dataw);

    uint16_t zaderzka_count = 0;
    bool zaderzka_priznak;
    uint8_t buffer, buffer_roll, bit29, bit30, parity;
    DWORD bytesWritten, bytesRead;
    uint32_t *mes = NULL;
    uint8_t kadry, mes_no, mes_id;
    uint8_t NNpast, NNsimple;
    char line[512] = {0};
    char *message = NULL;
    char otladka[256];
    uint16_t index = 0;

    HANDLE hSerial3 = CreateFile(COM3, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if(hSerial3 == INVALID_HANDLE_VALUE) {
        printf("Can't open %s\n", COM3);
    }
    PurgeComm(hSerial3, PURGE_RXCLEAR | PURGE_TXCLEAR);

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial3, &dcbSerialParams)) {
        printf("Error getting %s state\n", COM3);
        CloseHandle(hSerial3);
    }

    dcbSerialParams.BaudRate = speed3;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.StopBits = ONESTOPBIT;

    if (!SetCommState(hSerial3, &dcbSerialParams)) {
        printf("Error setting COM port state\n");
    } else {
        printf("COM port configured successfully\n");
    }

   COMMTIMEOUTS timeouts = {0};
   timeouts.ReadTotalTimeoutConstant = 50;

   if (!SetCommTimeouts(hSerial3, &timeouts)) {
       printf("TtmeOutsError\n");
       CloseHandle(hSerial3);
   }

   DWORD readBufferSize = 512;
   DWORD writeBufferSize = 0;
   if (!SetupComm(hSerial3, readBufferSize, writeBufferSize)) {
       printf("BufferError\n");
       CloseHandle(hSerial3);
    }

    if (zaderzka == 0) {
        zaderzka_priznak = false;
        zaderzka = 300;
    } else {
        zaderzka_priznak = true;
    }

    while (1) {
//        Sleep(50);
        for(int l = 0; l < 64; l++) {
            for(int s = 0; s < 32; s++) {
                g_mutex_lock(&lock1);
                if(messagefiled[l][s] != NULL) {
                    bit29 = *wordbit29;
                    bit30 = *wordbit30;
                    kadry = messagefiled[l][s][1] >> 11 & 0x1f;
                    mes = (uint32_t *)malloc((kadry + 2) * sizeof(uint32_t));
                    memcpy(mes, messagefiled[l][s], (kadry + 2) * sizeof(uint32_t));
                    free(messagefiled[l][s]);
                    messagefiled[l][s] = NULL;
                    g_mutex_unlock(&lock1);
                    message = (char *)malloc((kadry + 2) * 5 * sizeof(char));
                    mes_id = mes[0] >> 18 & 0x3f;
                    mes_no = mes[1] >> 11 & 0x1f;
                    mes[1] = SequencesChanger(mes[1]);
                    for(int i = 0; i < (kadry + 2); i++) {

                        parity = ParityCreate(((mes[i] >> 2) & 0x3fffffff), bit30, bit29);

                        mes[i] = (mes[i] & 0xffffff00) | (parity << 2);

                        if(bit30 == 1) {
                            mes[i] = (~mes[i] & 0xffffff00) | mes[i] & 0x000000ff;
                        }

                        bit29 = (parity & 0x02) >> 1;
                        bit30 = parity & 0x01;

                        mes[i] >>= 2;
                        for(int j = 0; j < 5; j++) {
                            buffer_roll = 0;
                            buffer = (((mes[i] >> 6 * (4 - j)) & 0x3f) << 2);
                            for(int g = 0; g < 8; g++) {
                                uint8_t bit = (buffer >> g) & 0x01;
                                if(bit == 1) {
                                    buffer_roll |= 0x01 << (7 - g);
                                }
                                bit = 0;
                            }
                            buffer_roll |= 0x40;
    //                        uint8_t ghh = 0x22;
                            // printf("Data to send: %02X\n", buffer_roll);
                            message[index] = buffer_roll;
                            index++;
                        }
                    }
//                    PurgeComm(hSerial3, PURGE_RXCLEAR);
                    WriteFile(hSerial3, message, (kadry + 2) * 5 * sizeof(char), &bytesWritten, NULL);
                        ReadFile(hSerial3, line, sizeof(line) - 1, &bytesRead, NULL);
                        line[bytesRead] = '\0';
                        char *back = strtok(line, "$");
                        while (back != NULL) {
                            if (sscanf(back, "MRSSR,1,%d*hh", &NNsimple) == 1)
                                NNpast += NNsimple;
                            back = strtok(NULL, "$");
                        }
                    if (zaderzka_priznak) {
                        if (NNpast > 10) {
                            zaderzka -= 5;
                        } else if (NNpast > 5) {
                            zaderzka -= 3;
                        } else if (NNpast > 2) {
                            zaderzka -= 2;
                        } else if (NNpast > 0) {
                            zaderzka -= 1;
                        } else if (NNpast == 0) {
                            zaderzka_count++;
                            if (zaderzka_count == 10)
                                zaderzka += 2;
                        }
                    }
                    snprintf(otladka, sizeof(otladka), "Start writing frame %u - %u mess; Null message now: %d, waittime now: %u\n\n", (l + 1), mes_no, NNpast, zaderzka);
                    g_idle_add((GSourceFunc) log_to_text_view, g_strdup(otladka));
                    Sleep(zaderzka * (kadry + 2));
                    *wordbit29 = bit29;
                    *wordbit30 = bit30;
                    free(mes);
                    mes = NULL;
                    free(message);
                    message = NULL;
                    index = 0;
                    NNpast = 0;
                    NNsimple = 0;
                }
                g_mutex_unlock(&lock1);
            }
        }
        g_mutex_unlock(&lock1);
    }
    CloseHandle(hSerial3);
}

gpointer firstCOM(gpointer user_data) {
    EntryData *data = (EntryData *)user_data;
    const gchar *com_name = gtk_entry_get_text(GTK_ENTRY(data->entry1));
    const gchar *com_speed = gtk_entry_get_text(GTK_ENTRY(data->entry2));
    char *COM1 = g_strdup(com_name);
    char *endptr;
    uint32_t speed1 = (uint32_t)strtoul(com_speed, &endptr, 10);

    g_free(data);


    printf("Start first thread\n");
    HANDLE hSerial1 = CreateFile(COM1, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if(hSerial1 == INVALID_HANDLE_VALUE) {
        printf("Can't open %s\n", COM1);
    }
    PurgeComm(hSerial1, PURGE_RXCLEAR | PURGE_TXCLEAR);

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial1, &dcbSerialParams)) {
        printf("Error getting %s state\n", COM1);
        CloseHandle(hSerial1);
    }

    dcbSerialParams.BaudRate = speed1; // Set baud rate to 38400
    dcbSerialParams.ByteSize = 8;       // Set data bits to 8
    dcbSerialParams.Parity = NOPARITY;  // Set parity to none
    dcbSerialParams.StopBits = ONESTOPBIT; // Set stop bits to 1

    if (!SetCommState(hSerial1, &dcbSerialParams)) {
        printf("Error setting %s state\n", COM1);
        CloseHandle(hSerial1);
    }

    printf("As first opened: %s\n", COM1);

    uint64_t potok;
    uint32_t word;
    uint32_t message[31];
    uint16_t Z1count;
    uint8_t buffer, buffer_roll;
    uint8_t bit29, bit30, startbit29, startbit30, parity;
    uint8_t cut1, cut2, chislo_kadrov;
    bool parity_result;
    DWORD bytesRead;
    bit29 = 1;
    bit30 = 1;
    startbit29 = bit29;
    startbit30 = bit30;
    uint8_t frame_id, sattNo;

    while(1) {
        parity_result = false;
        // printf("Первая контрольная точка первого потока\n");
        ReadFile(hSerial1, &buffer, sizeof(buffer), &bytesRead, NULL); // Чтение начало
        for(int i = 0; i < 8; i++) {
            uint8_t bit = (buffer >> i) & 0x01;
            if(bit == 1) {
                buffer_roll |= 0x01 << (7 - i);
            }
            bit = 0;
        }
        potok <<= 6;
        potok += (buffer_roll >> 2) & 0x3f;
        if (bytesRead > 0)
        buffer_roll = 0;
        buffer = 0; // Чтение конец

        Preamble preamble = PreambleSearch(potok, bit30, bit29);

        if(!preamble.preamble_result) {
            continue;
        }

        word = (potok >> (32 - preamble.preamble_place)) & 0xffffffff;

        if(preamble.Preamble_priznak == 1) {
            word = (~word & 0xffffff00) | word & 0x000000ff;
        }

        parity = ParityCreate(((word >> 2) & 0x3fffffff), bit30, bit29);

        if(((word >> 2) & 0x0000003f) == parity) {
            parity_result = true;
        }

        if(!parity_result) {
            continue;
        }

        parity_result = false;

        bit29 = (parity & 0x02) >> 1;
        bit30 = parity & 0x01;

        message[0] = word;

        cut1 = preamble.preamble_place / 6;
        cut2 = 6 - (preamble.preamble_place - cut1 * 6);
        
        for(int i = 0; i < (cut1 + 1); i++) {
        ReadFile(hSerial1, &buffer, sizeof(buffer), &bytesRead, NULL); // Чтение начало
        for(int i = 0; i < 8; i++) {
            uint8_t bit = (buffer >> i) & 0x01;
            if(bit == 1) {
                buffer_roll |= 0x01 << (7 - i);
            }
            bit = 0;
        }
        potok <<= 6;
        potok += (buffer_roll >> 2) & 0x3f;
//        printf("%c", buffer);
        if (bytesRead > 0)
        buffer_roll = 0;
        buffer = 0; // Чтение конец
        }

        word = (potok >> (cut2 + 2)) & 0xffffffff;

        if(bit30 == 1) {
            word = (~word & 0xffffff00) | word & 0x000000ff;
        }

        parity = ParityCreate(((word >> 2) & 0x3fffffff), bit30, bit29);

        if(((word >> 2) & 0x0000003f) == parity) {
            parity_result = true;
        }

        if(!parity_result)
            continue;
        
        parity_result = false;

        message[1] = word;

        bit29 = (parity & 0x02) >> 1;
        bit30 = parity & 0x01;

        chislo_kadrov = (message[1] >> 11) & 0x1f;
        
        for(int i = 0; i < chislo_kadrov; i++) {
            for(int j = 0; j < 5; j++) {
                ReadFile(hSerial1, &buffer, sizeof(buffer), &bytesRead, NULL); // Чтение начало
                for(int i = 0; i < 8; i++) {
                    uint8_t bit = (buffer >> i) & 0x01;
                    if(bit == 1) {
                        buffer_roll |= 0x01 << (7 - i);
                    }
                    bit = 0;
                }
                potok <<= 6;
                potok += (buffer_roll >> 2) & 0x3f;
                if (bytesRead > 0)
                buffer_roll = 0;
                buffer = 0; // Чтение конец
            }
            
            
            word = (potok >> (cut2 + 2)) & 0xffffffff;
            if(bit30 == 1) {
                word = (~word & 0xffffff00) | word & 0x000000ff;
            }
            parity = ParityCreate(((word >> 2) & 0x3fffffff), bit30, bit29);

            if(((word >> 2) & 0x0000003f) == parity) {
                parity_result = true;
            }

            if(!parity_result)
                continue;
            
            parity_result = false;

            bit29 = (parity & 0x02) >> 1;
            bit30 = parity & 0x01;
            message[i+2] = word;
        }
        
        frame_id = message[0] >> 18 & 0x3f;
        if(frame_id == 9 || frame_id == 34) {
            sattNo = message[2] >> 24 & 0x1f;
        } else {
            sattNo = 1;
        }
        g_mutex_lock(&lock1);
//        printf("First thread start writing\n");
        messagefiled[(frame_id - 1)][sattNo - 1] = (uint32_t *)malloc((chislo_kadrov + 2) * sizeof(uint32_t));
        // for(int i = 0; i < (chislo_kadrov + 2); i++)
        //     messagefiled[(frame_id - 1)][i] = message[i];
        memcpy(messagefiled[(frame_id - 1)][sattNo - 1], message, (chislo_kadrov + 2) * sizeof(uint32_t));
        g_mutex_unlock(&lock1);
        
    }
    CloseHandle(hSerial1);
}

gpointer secondCOM(gpointer user_data) {
    EntryData *data = (EntryData *)user_data;
    const gchar *com_name = gtk_entry_get_text(GTK_ENTRY(data->entry1));
    const gchar *com_speed = gtk_entry_get_text(GTK_ENTRY(data->entry2));
    char *COM2 = g_strdup(com_name);
    char *endptr;
    uint32_t speed2 = (uint32_t)strtoul(com_speed, &endptr, 10);

    g_free(data);

    printf("Start second thread\n");
    HANDLE hSerial2 = CreateFile(COM2, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if(hSerial2 == INVALID_HANDLE_VALUE) {
        printf("Can't open %s\n", COM2);
    }
    PurgeComm(hSerial2, PURGE_RXCLEAR | PURGE_TXCLEAR);

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial2, &dcbSerialParams)) {
        printf("Error getting %s state\n", COM2);
        CloseHandle(hSerial2);
    }

    dcbSerialParams.BaudRate = speed2; // Set baud rate to 38400
    dcbSerialParams.ByteSize = 8;       // Set data bits to 8
    dcbSerialParams.Parity = NOPARITY;  // Set parity to none
    dcbSerialParams.StopBits = ONESTOPBIT; // Set stop bits to 1

    if (!SetCommState(hSerial2, &dcbSerialParams)) {
        printf("Error setting %s state\n", COM2);
        CloseHandle(hSerial2);
    }

    printf("As second opened: %s\n", COM2);

    uint64_t potok;
    uint32_t word;
    uint32_t message[31];
    uint16_t Z2count;
    uint8_t buffer, buffer_roll;
    uint8_t bit29, bit30, startbit29, startbit30, parity;
    uint8_t cut1, cut2, chislo_kadrov;
    bool parity_result;
    DWORD bytesRead;
    bit29 = 1;
    bit30 = 1;
    startbit29 = bit29;
    startbit30 = bit30;
    uint8_t frame_id, sattNo;

    while(1) {
                parity_result = false;
        // printf("Первая контрольная точка первого потока\n");
        ReadFile(hSerial2, &buffer, sizeof(buffer), &bytesRead, NULL); // Чтение начало
        for(int i = 0; i < 8; i++) {
            uint8_t bit = (buffer >> i) & 0x01;
            if(bit == 1) {
                buffer_roll |= 0x01 << (7 - i);
            }
            bit = 0;
        }
        potok <<= 6;
        potok += (buffer_roll >> 2) & 0x3f;
        if (bytesRead > 0)
        buffer_roll = 0;
        buffer = 0; // Чтение конец

        Preamble preamble = PreambleSearch(potok, bit30, bit29);

        if(!preamble.preamble_result) {
            continue;
        }

        word = (potok >> (32 - preamble.preamble_place)) & 0xffffffff;

        if(preamble.Preamble_priznak == 1) {
            word = (~word & 0xffffff00) | word & 0x000000ff;
        }

        parity = ParityCreate(((word >> 2) & 0x3fffffff), bit30, bit29);

        if(((word >> 2) & 0x0000003f) == parity) {
            parity_result = true;
        }

        if(!parity_result) {
            continue;
        }

        parity_result = false;

        bit29 = (parity & 0x02) >> 1;
        bit30 = parity & 0x01;

        message[0] = word;

        cut1 = preamble.preamble_place / 6;
        cut2 = 6 - (preamble.preamble_place - cut1 * 6);
        
        for(int i = 0; i < (cut1 + 1); i++) {
        ReadFile(hSerial2, &buffer, sizeof(buffer), &bytesRead, NULL); // Чтение начало
        for(int i = 0; i < 8; i++) {
            uint8_t bit = (buffer >> i) & 0x01;
            if(bit == 1) {
                buffer_roll |= 0x01 << (7 - i);
            }
            bit = 0;
        }
        potok <<= 6;
        potok += (buffer_roll >> 2) & 0x3f;
//        printf("%c", buffer);
        if (bytesRead > 0)
        buffer_roll = 0;
        buffer = 0; // Чтение конец
        }

        word = (potok >> (cut2 + 2)) & 0xffffffff;

        if(bit30 == 1) {
            word = (~word & 0xffffff00) | word & 0x000000ff;
        }

        parity = ParityCreate(((word >> 2) & 0x3fffffff), bit30, bit29);

        if(((word >> 2) & 0x0000003f) == parity) {
            parity_result = true;
        }

        if(!parity_result)
            continue;
        
        parity_result = false;

        message[1] = word;

        bit29 = (parity & 0x02) >> 1;
        bit30 = parity & 0x01;

        chislo_kadrov = (message[1] >> 11) & 0x1f;
        
        for(int i = 0; i < chislo_kadrov; i++) {
            for(int j = 0; j < 5; j++) {
                ReadFile(hSerial2, &buffer, sizeof(buffer), &bytesRead, NULL); // Чтение начало
                for(int i = 0; i < 8; i++) {
                    uint8_t bit = (buffer >> i) & 0x01;
                    if(bit == 1) {
                        buffer_roll |= 0x01 << (7 - i);
                    }
                    bit = 0;
                }
                potok <<= 6;
                potok += (buffer_roll >> 2) & 0x3f;
                if (bytesRead > 0)
                buffer_roll = 0;
                buffer = 0; // Чтение конец
            }
            
            
            word = (potok >> (cut2 + 2)) & 0xffffffff;
            if(bit30 == 1) {
                word = (~word & 0xffffff00) | word & 0x000000ff;
            }
            parity = ParityCreate(((word >> 2) & 0x3fffffff), bit30, bit29);

            if(((word >> 2) & 0x0000003f) == parity) {
                parity_result = true;
            }

            if(!parity_result)
                continue;
            
            parity_result = false;

            bit29 = (parity & 0x02) >> 1;
            bit30 = parity & 0x01;
            message[i+2] = word;
        }
        
        frame_id = message[0] >> 18 & 0x3f;
        if(frame_id == 9 || frame_id == 34) {
            sattNo = message[2] >> 24 & 0x1f;
        } else {
            sattNo = 1;
        }
        g_mutex_lock(&lock1);
//        printf("First thread start writing\n");
        messagefiled[(frame_id - 1)][sattNo - 1] = (uint32_t *)malloc((chislo_kadrov + 2) * sizeof(uint32_t));
        // for(int i = 0; i < (chislo_kadrov + 2); i++)
        //     messagefiled[(frame_id - 1)][i] = message[i];
        memcpy(messagefiled[(frame_id - 1)][sattNo - 1], message, (chislo_kadrov + 2) * sizeof(uint32_t));
        g_mutex_unlock(&lock1);
        
    }
    CloseHandle(hSerial2);
}