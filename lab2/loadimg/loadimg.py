import struct
import os
import serial
import time

def wait_for_magic(ser, magic: bytes) -> None:
    matched = 0
    while matched < len(magic):
        byte = ser.read(1)
        print(byte, byte[0])
        print(magic, magic[matched])
        if not byte:
            raise TimeoutError(f"Timeout waiting for magic {magic!r}")
        if byte[0] == magic[matched]:
            matched += 1
        else:
            matched = 1 if byte[0] == magic[0] else 0

def send_img():
    kernel_path = "kernel8.img"
    
    # ser = serial.Serial("/dev/pts/10", 115200, )
    ser = serial.Serial("/dev/ttyUSB1", 115200)

    size = os.path.getsize(kernel_path)

    header = b"KERN" + struct.pack("<I", size)

    print("magic:", header[:4])
    print("size:", size)
    print("size bytes:", header[4:8])
    ser.write(header)

    
    returned_size_raw = ser.read(4)
    returned_size = struct.unpack("<I", returned_size_raw)[0]

    print("bootloader size:", returned_size)
    print("expected size:", size)

    with open(kernel_path, "rb") as f:
        kernel_data = f.read()
    print(f"bootloader size: {returned_size_raw}")
    print(f"expected size: {len(kernel_data):x}")

    header = b"KERN" + kernel_data
    ser.write(header)
    wait_for_magic(ser, b"BOOT")
    print("done!")

# python3 loadimg/loadimg.py
if __name__=="__main__":
    send_img()