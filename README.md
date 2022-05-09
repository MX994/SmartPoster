# Ibraboard

My own 32x32 LED Matrix, created for my personal usage.

## Usage
Currently, it has three modes, toggleable via the onboard WiFi:
- Text Display Mode: Allows me to display scrolling text across the matrix.
- Image Display Mode: Allows me to display static image binaries, written to the microprocessor's flash, across the matrix.
- Streaming Mode: Allows me to send frame data through UART, and display it on the matrix. To send data, use `Muxer.py` in the `src/Server` directory. It can be utilized as follows (after installing Python3, OpenCV, PySerial, Pillow, and imutils):

> python Muxer.py <path to video>


## Documentation
**All data is written in little endian!**

### Static Image Binary Format
```C
struct Ibraboard_StaticImage {
    uint32_t magic; // 'IBRA'
    uint16_t width; // Width of image.
    uint16_t height; // Height of image.
    StaticImageEntry entries[width * height];
}

struct StaticImageEntry {
    byte blue;
    byte green;
    byte red;
}
```

### UART Packet Format
```C
struct Ibraboard_UARTImage {
    uint32_t magic; // 'IBRA'
    uint8_t width; // Width of image.
    uint8_t height; // Height of image.
    ImageEntry entries[width * height];
}

struct ImageEntry {
    byte blue;
    byte green;
    byte red;
}
```