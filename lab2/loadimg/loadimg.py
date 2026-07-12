import struct
import os
import serial

def send_img():
    kernel_path = "../kernel8.img"
    ser = serial.Serial("/dev/pts/7", 115200)
    size = os.path.getsize(kernel_path)

    print(size)
    ser.write(struct.pack("<I", size))
    # ser.write(0xe6)
    print(struct.pack("<I", size))

    while(1):
        if ser.in_waiting > 0:
            print(ser.read(4))

    # with open(kernel_path, "rb") as f:
    #     ser.write(f.read())

    # while(1):
    #     if serial.in_waiting:
    #         response = ser.readline().decode("utf-8", errors="ignore")
    #         if "Jumping" in response:
    #             break
    # ser.close()
    # if ser.in_waiting > 0:
    #     raw_data = ser.readline()
    #     text = raw_data.decode("utf-8", errors="ignore")
    #     print(text.strip())


if __name__=="__main__":
    send_img()