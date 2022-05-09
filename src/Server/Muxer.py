from sys import argv
from serial import Serial
from struct import pack
from PIL import Image
from sys import argv
from imutils.video import FileVideoStream

if len(argv) != 2:
    print("Invalid arguments!")
    exit(1)

filename = argv[1]

# Initialize serial with 115.2K bit baudrate.
newSerial = Serial('COM7', 921600)

# Initialize test video file.
video = FileVideoStream(filename).start()

while True:
    # Go through frame by frame, and send each frame to the ESP8266.
    try:
        while video.running():
            frame = video.read()
            packed = b'ARBI'
            frame_PIL = Image.frombytes("RGB", (len(frame[0]), len(frame)), frame).convert('RGB').resize((32, 32))
            packed += pack('BB', frame_PIL.width, frame_PIL.height)
            for y in range(frame_PIL.height):
                for x in range(frame_PIL.width):
                    p = frame_PIL.getpixel((x, y))
                    packed += pack("BBB", p[0], p[1], p[2])
            newSerial.write(packed)
    except TypeError:
        video = FileVideoStream(filename).start()