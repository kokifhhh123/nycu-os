import struct
import os
import serial
import time

def test():
    kernel_path = "../kernel8.img"
    # ser = serial.Serial("/dev/pts/7", 115200)
    ser = serial.Serial("/dev/ttyUSB1", 115200)
    while(1):
        if ser.in_waiting > 0:
            print(ser.read(4))

    while(1):
        if serial.in_waiting:
            response = ser.readline().decode("utf-8", errors="ignore")
            if "Jumping" in response:
                break
    ser.close()
    if ser.in_waiting > 0:
        raw_data = ser.readline()
        text = raw_data.decode("utf-8", errors="ignore")
        print(text.strip())

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
    kernel_path = "../kernel8.img"
    
    # ser = serial.Serial("/dev/pts/10", 115200, )
    ser = serial.Serial("/dev/ttyUSB1", 115200)

    size = os.path.getsize(kernel_path)
    # ser.write(b"KERN")
    # while True:
    #     line = ser.readline()
    #     print(line)

    #     if b"READY" in line:
    #         break
    
    # ser.write(struct.pack("<I", size))
    header = b"KERN" + struct.pack("<I", size)

    print("magic:", header[:4])
    print("size:", size)
    print("size bytes:", header[4:8])
    ser.write(header)

    # ser.flush()

    # while True:
    #     line = ser.readline()
    #     print(line)

    #     if b"READY" in line:
    #         break
    
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
    # chunk_size = 64
    # for offset in range(0, len(kernel_data), chunk_size):
    #     chunk = kernel_data[offset:offset + chunk_size]
    # # chunk = kernel_data[0:chunk_size]
    #     ser.write(chunk)
    #     # ser.flush()
    #     time.sleep(0.002)
    wait_for_magic(ser, b"BOOT")
    # data = ser.readline()
    # text = data.decode("utf-8", errors="ignore")
    # print(text.strip())

    # while(1):
    #     print("waiting...")
    #     data = ser.readline()
    #     if data:
    #         print("waiting... 1")
    #         text = data.decode("utf-8", errors="ignore")
    #         # print(data.decode(errors="replace", end=""))
    #         print(text.strip())
    #     if b"kernel loaded" in data:
    #         print("waiting... 2")
    #         print("Transfer successful")
    #         break
    print("done!")
    # while(1):
    #     if ser.in_waiting > 0:
    #         print(ser.read(1))

# python3 loadimg.py
if __name__=="__main__":
    send_img()