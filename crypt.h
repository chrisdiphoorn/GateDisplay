
#include "mbedtls/aes.h"
#include "Base64.h"
#include "esp_random.h"
#include <vector>

const uint8_t key[16]  = {0x0F, 0x0E, 0x0E, 0x0A, 0x04, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x01};

// Helper: Encrypt binary data and return a Base64 char array
void encryptAndBase64(const char* input, char* b64Output) {
    mbedtls_aes_context aes;
    size_t inputLen = strlen(input);
    
    // Calculate PKCS7 padding
    size_t padLen = 16 - (inputLen % 16);
    size_t totalDataLen = inputLen + padLen;
    
    // Buffers for binary work
    uint8_t iv[16];
    uint8_t paddedInput[totalDataLen];
    uint8_t encrypted[16 + totalDataLen]; // [IV (16)] + [Data]

    // Generate Hardware Random IV
    esp_fill_random(iv, 16);
    memcpy(encrypted, iv, 16); // Put IV at the start

    // Apply Padding
    memcpy(paddedInput, input, inputLen);
    for (size_t i = inputLen; i < totalDataLen; i++) paddedInput[i] = (uint8_t)padLen;

    // Hardware AES Encryption
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 128);
    
    uint8_t iv_temp[16]; // mbedtls modifies the IV buffer, so use a copy
    memcpy(iv_temp, iv, 16);
    
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, totalDataLen, iv_temp, paddedInput, encrypted + 16);
    mbedtls_aes_free(&aes);

    // Convert binary [IV + Encrypted] to Base64 string
    // Xander library: base64_encode(target, source, sourceLen)
    Base64.encode(b64Output, (char*)encrypted, 16 + totalDataLen);
}

// Helper: Take a Base64 char array and decrypt to a plain char array
int base64AndDecrypt(char* b64Input, char* plainOutput) {
    mbedtls_aes_context aes;
    
    // Decode Base64 to binary
    int b64Len = strlen(b64Input);
    uint8_t decoded[b64Len]; // Buffer large enough for decoded data
    int decodedLen = Base64.decode((char*)decoded, b64Input, b64Len);

    // Extract IV (first 16 bytes)
    uint8_t iv[16];
    memcpy(iv, decoded, 16);
    uint8_t* encryptedData = decoded + 16;
    int encryptedLen = decodedLen - 16;

    // Hardware AES Decryption
    uint8_t decrypted[encryptedLen];
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, key, 128);
    mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, encryptedLen, iv, encryptedData, decrypted);
    mbedtls_aes_free(&aes);

    // 4. Remove PKCS7 Padding
    uint8_t padVal = decrypted[encryptedLen - 1];
    if (padVal > 16) return -1; // Error
    
    int finalLen = encryptedLen - padVal;
    memcpy(plainOutput, decrypted, finalLen);
    plainOutput[finalLen] = '\0'; // Null terminate
    
    return finalLen;
}


/*
    const uint8_t key[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
    const char* secretMsg = "ESP32-Hardware-Security";
    
    // Buffers (Ensure they are large enough!)
    char base64Buffer[128]; 
    char resultBuffer[128];

    // --- ENCRYPT ---
    encryptAndBase64(secretMsg, base64Buffer);
    Serial.printf("Base64 Result: %s\n", base64Buffer);

    // --- DECRYPT ---
    int len = base64AndDecrypt(base64Buffer, resultBuffer);
    if (len > 0) {
        Serial.printf("Decrypted: %s\n", resultBuffer);
    } else {
        Serial.println("Decryption Failed!");
    }
    */
    