import serial
import numpy as np
import cv2

# Set up the serial connection
serial_port = '/dev/cu.usbmodem1101'
baud_rate = 115200
ser = serial.Serial(serial_port, baud_rate, timeout=1)

# Buffer to store incoming data
data_buffer = bytearray()

while True:
    line = ser.readline()
    if line.startswith(b'IMG_START'):
        data_buffer = bytearray()  # Clear buffer for new image
        while True:
            part = ser.read(ser.inWaiting())
            if b'IMG_END' in part:
                data_buffer.extend(part.split(b'IMG_END')[0])
                break
            else:
                data_buffer.extend(part)

        if len(data_buffer) >= 314928:  # 324x324 pixels, 3 bytes per pixel (RGB)
            img = np.frombuffer(data_buffer, dtype=np.uint8).reshape((324, 324, 3))
            cv2.imshow('Frame', img)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

ser.close()
cv2.destroyAllWindows()
