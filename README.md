🖼️ LSB Image Steganography
==========================

📌 Overview
-----------
LSB Image Steganography is a project that hides secret messages or data inside image files using the Least Significant Bit (LSB) technique. It leverages the fact that changing the least significant bits of pixel values doesn’t produce a visually noticeable difference—making it perfect for covert communication.

This tool enables users to embed and extract messages or files from bitmap (.bmp) images with minimal perceptual distortion.

🧠 What is LSB Steganography?
-----------------------------
Steganography is the practice of concealing a message within another medium.
LSB (Least Significant Bit) steganography hides data in the least significant bit of each pixel component (Red, Green, Blue) in an image.

Example:
Original RGB: (10101100, 11001010, 11110000)
Modified RGB: (10101101, 11001011, 11110001) <- Only last bits changed

This modification is visually imperceptible but stores 3 bits of secret data.

✨ Features
----------
- Supports 24-bit BMP images
- Hide ASCII text or binary data
- Embed & extract operations
- Minimal distortion in the output image
- CLI-based tool for simple integration
- Input validation and error detection
- Easy to extend for encryption or password-protection

🧱 Project Structure
--------------------
lsb_steganography/
├── include/
│   └── stego.h               # Function declarations
├── src/
│   ├── encode.c              # Embedding logic
│   ├── decode.c              # Extraction logic
│   ├── utils.c               # Common helper functions
│   └── main.c                # CLI interface
├── assets/
│   └── sample.bmp            # Sample cover image
├── README.md                 # Project documentation
├── Makefile                  # For compiling the project
└── output/
    ├── stego.bmp             # Image with embedded data
    └── extracted.txt         # Output of decoded message

⚙️ How It Works
----------------
🔐 Encoding
- Read the BMP image header to locate pixel data
- Convert the message/data into binary
- Modify the LSB of each pixel byte to store bits of the message
- Store message length in the first few pixels (as metadata)
- Save the modified image

🔍 Decoding
- Read the LSBs from image pixel data
- Extract the message length
- Collect and reassemble the message bits
- Output the hidden message or file

🚀 Getting Started
------------------
✅ Prerequisites
- GCC (for C implementation)
- Make (optional)
- BMP file (24-bit)

🔧 Build Instructions
$ git clone https://github.com/ganeshsimbala/LSB-Image-Steganography.git
$ cd lsb-steganography
$ make

Or compile manually:
$ gcc -o stego main.c src/*.c

🛠️ Usage
---------
To embed a message into an image:
$ ./stego encode input.bmp "Secret Message" output.bmp

To extract a message from an image:
$ ./stego decode output.bmp

📸 Supported Image Format
-------------------------
- 24-bit uncompressed BMP images only

🔐 Future Enhancements
----------------------
- AES encryption support
- Password-based access
- JPEG/PNG support using DCT or alpha channel
- GUI with drag-and-drop interface
- Compression for large messages
- Support for audio/video steganography

🧪 Testing
----------
Test with:
1. Small text message
2. Large file (base64)
3. Edge cases (empty message, corrupt image)

🧠 Concepts Involved
---------------------
- Bitwise operations
- File I/O (binary mode)
- BMP image format structure
- Data encoding and decoding
- Memory management (if using C)
- Command-line argument parsing

🧑‍💻 Authors
------------
- Ganesh Simbala — https://github.com/ganeshsimbala

📄 License
----------
This project is licensed under the MIT License — see the LICENSE file for details.

🙌 Acknowledgements
-------------------
- Wikipedia - LSB Steganography
- BMP File Format Specification
- GeeksforGeeks - Steganography

🧵 Reddit & Communities
-----------------------
- https://www.reddit.com/r/steganography/
- https://www.reddit.com/r/cryptography/
- https://www.reddit.com/r/cprogramming/
