#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define Nb 2
#define Nr 2

uint16_t RoundKey[3]={0};
uint16_t state={0};
uint16_t stateDEC={0};

static const uint8_t SBox[16] = {
     0x09,0x04,0x0A,0x0B,
     0x0D,0x01,0x08,0x05,
     0x06,0x02,0x00,0x03,
     0x0C,0x0E,0x0F,0x07
};

static const uint8_t iSBox[16] = {
    0x0A, 0x05, 0x09, 0x0B,
    0x01, 0x07, 0x08, 0x0F,
    0x06, 0x00, 0x02, 0x03,
    0x0C, 0x04, 0x0D, 0x0E
};

static const uint8_t Rcon[3] = {
    0x00, 0x80, 0x30
};

static const uint8_t mixCol[] = {
    0x01, 0x04,
    0x04, 0x01
};

void keyExpansion(const uint16_t key) {
    uint8_t wKey[6]={0};
    wKey[0] = (key & 0xFF00) >> 8;
    wKey[1] = (key & 0x00FF);

    for (int i = Nb; i < (Nb * (Nr + 1)); i++) {
        uint8_t temp = (wKey[i - 1]);

        if (i % Nb == 0) {
            temp = ((temp & 0x0F) << 4) | ((temp & 0xF0) >> 4);
            temp = ((SBox[((temp & 0xF0) >> 4)]) << 4) | (SBox[(temp & 0x0F)]);
            temp ^= (Rcon[i / Nb]);
        }

        wKey[i] = (wKey[i - Nb] ^ temp);
    }

    for (int j = 0; j < 3; j++) {
        RoundKey[j] = (wKey[j * 2] << 8) | wKey[j * 2 + 1];
    }
}

void addRoundKey(uint16_t *state, const uint16_t *roundKey, int round) {
    *state ^= roundKey[round];
}

void nibbleSub(uint16_t *state) {
    *state = ((SBox[(*state & 0x000F)]) |
              (SBox[(*state & 0x00F0) >> 4]) << 4 |
              (SBox[(*state & 0x0F00) >> 8]) << 8 |
              (SBox[(*state & 0xF000) >> 12]) << 12);
}

void inverseNibbleSubstitution(uint16_t *state) {
    *state = ((iSBox[(*state & 0x000F)]) |
              (iSBox[(*state & 0x00F0) >> 4]) << 4 |
              (iSBox[(*state & 0x0F00) >> 8]) << 8 |
              (iSBox[(*state & 0xF000) >> 12]) << 12);
}

void shiftRows(uint16_t *state) {
    *state = ((*state & 0xF000) |
              (*state & 0x0F00) >> 8 |
              (*state & 0x00F0) |
              (*state & 0x000F) << 8);
}

uint16_t polyMul(uint16_t m1, uint16_t m2) {
    uint16_t result = 0x0;
    uint16_t shift = 0;
    while (m1) {
        result = ((m1 & 0x0001) * (m2 << shift)) ^ result;
        m1 = m1 >> 1;
        shift = shift + 1;
    }
    return result;
}

uint16_t multiply(uint16_t S1, uint16_t S2) {
    uint16_t multResult = polyMul(S1, S2);

    uint16_t shift = 0;
    while (!(multResult <= 15)) {
        shift = ceil(log(multResult + 1) / log(2)) - ceil(log(0x13) / log(2));
        multResult = multResult ^ (0x13 << shift);
    }

    return multResult;
}

void mixColumns(uint16_t *state) {
    *state = ((((*state & 0xF000) >> 12) ^ (multiply(0x04, ((*state & 0x0F00) >> 8)))) << 12 |
              ((multiply(0x04, ((*state & 0xF000) >> 12))) ^ ((*state & 0x0F00) >> 8)) << 8 |
              (((*state & 0x00F0) >> 4) ^ (multiply(0x04, (*state & 0x000F)))) << 4 |
              (multiply(0x04, ((*state & 0x00F0) >> 4))) ^ ((*state & 0x000F)));
}

void inverseMixColumns(uint16_t *state) {
    *state = (((multiply(0x09, ((*state & 0xF000) >> 12))) ^ (multiply(0x02, ((*state & 0x0F00) >> 8)))) << 12) |
              (((multiply(0x02, ((*state & 0xF000) >> 12))) ^ (multiply(0x09, ((*state & 0x0F00) >> 8)))) << 8) |
              (((multiply(0x09, ((*state & 0x00F0) >> 4))) ^ (multiply(0x02, (*state & 0x000F)))) << 4) |
              ((multiply(0x02, ((*state & 0x00F0) >> 4))) ^ (multiply(0x09, (*state & 0x000F))));
}

uint16_t AES_encrypt(const uint16_t key, const uint16_t plainText) {
    state = plainText;
    keyExpansion(key);
    addRoundKey(&state, RoundKey, 0);

    for (int round = 1; round < Nr; round++) {
        nibbleSub(&state);
        shiftRows(&state);
        mixColumns(&state);
        addRoundKey(&state, RoundKey, round);
    }

    nibbleSub(&state);
    shiftRows(&state);
    addRoundKey(&state, RoundKey, Nr);

    return state;
}
uint16_t AES_Decrypt(const uint16_t key, const uint16_t cipherText){

    stateDEC = cipherText;
    keyExpansion(key);
    addRoundKey(&stateDEC, RoundKey, 2);

    shiftRows(&stateDEC);
    inverseNibbleSubstitution(&stateDEC);
    addRoundKey(&stateDEC, RoundKey, 1);
    inverseMixColumns(&stateDEC);

    shiftRows(&stateDEC);
    inverseNibbleSubstitution(&stateDEC);
    addRoundKey(&stateDEC, RoundKey, 0);

    return stateDEC;
}


int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s ENC|DEC key data\n", argv[0]);
        return 1;
    }

    char *endptr;
    const char *operation = argv[1];
    uint16_t key = strtol(argv[2], &endptr, 16);
    if (*endptr != '\0') {
        printf("Invalid key format: %s\n", argv[2]);
        return 1;
    }

    uint16_t data = strtol(argv[3], &endptr, 16);
    if (*endptr != '\0') {
        printf("Invalid data format: %s\n", argv[3]);
        return 1;
    }

    uint16_t output;

    if (strcmp(operation, "ENC") == 0) {
        output = AES_encrypt(key, data);
        printf("Encrypted Data: %04x\n", output);
    } else if (strcmp(operation, "DEC") == 0) {
        output = AES_Decrypt(key, data);
        printf("Decrypted Data: %04x\n", output);
    } else {
        printf("Invalid operation: %s\n", operation);
        return 1;
    }

    return 0;
}
