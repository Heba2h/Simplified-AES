# Simple AES Implementation in C

This repository contains a simplified implementation of the Advanced Encryption Standard (AES) algorithm in pure C language. The implementation is designed to work on 2-byte data and key lengths, making it suitable for educational purposes or small-scale applications.

## Features

- **Encryption:** Encrypt 2-byte data using a 2-byte key.
- **Decryption:** Decrypt the encrypted data using the same key.

## Usage

To use this implementation, follow these steps:

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/Heba2h/Simplified-AES.git
   cd Simplified-AES
2. **Compile the Code:**

    ``` bash
    gcc saes_1900022.c -o saes_1900022
    ```
3. **Run the Program:**

    ``` bash
    ./saes_1900022 ENC|DEC key data
    ```
    Replace ENC|DEC with either "ENC" for encryption or "DEC" for decryption.
    Replace key with a 4-digit hexadecimal key (e.g., A73B).
    Replace data with a 4-digit hexadecimal data block to be encrypted or decrypted (e.g., 6F6B).

4. **View Results:**
    The program will output the result of encryption or decryption based on the provided parameters.

5. **Example**
    Encrypt data 0x6F6B with key 0xA73B:
    
      ``` bash
      ./saes_1900022 ENC A73B 6F6B
      ```
    Decrypt the encrypted data:
    
      ``` bash
      ./saes_1900022 DEC A73B <result-of-encryption>
      ```
