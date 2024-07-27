import usb.core
import usb.backend.libusb1
import usb.util
import binascii
import time


data = bytearray([0] * 8)
data[0] = 0x40
data[1] = 0x41
data[2] = 0x42
data[3] = 0x43
data[4] = 0x44
data[5] = 0x45
data[6] = 0x46
data[7] = 0x47

def usb_led(b, r, g, y):
    b = 0x1 if b != 0 else 0x0
    r = 0x1 if r != 0 else 0x0
    g = 0x1 if g != 0 else 0x0
    y = 0x1 if y != 0 else 0x0    

    print(f"Blue : {b}, Red : {r}, Green : {g}, Yellow : {y}")
    
    # Specify the path to the libusb-1.0.dll
    usb.core.find(backend=usb.backend.libusb1.get_backend(find_library=lambda x: "./usblib/libusb-1.0.dll"))
    
    # Find the USB device based on its vendor and product IDs
    vendor_id = 0x04D8  # Replace with your device's vendor ID
    product_id = 0x0053  # Replace with your device's product ID

    # Find the device
    device = usb.core.find(idVendor=vendor_id, idProduct=product_id)
    if device is None:
        raise ValueError('Device not found')
    else:
    #     print(device)
        print('found device')

    # set the active configuration. With no arguments, the first
    # configuration will be the active one
    device.set_configuration()

    # Find the bulk endpoints for data in and data out
    endpoint_out = 0x01  # Replace with the correct endpoint address for data out
    endpoint_in = 0x81   # Replace with the correct endpoint address for data in

    data_to_send = bytes([0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, b, r, g, y])
    device.write(endpoint_out, data_to_send)
    
    # Clean up
    device.reset()

def usb_status():
    # Create an 8-byte placeholder data
    global data
    
    # Modify the data bytes as needed
    # Example: Set some values in the byte array
    data[0] = data[0] + 1
    data[1] = data[1] + 1
    data[2] = data[2] + 1
    data[3] = data[3] + 1
    data[4] = data[4] + 1
    data[5] = data[5] + 1
    data[6] = data[6] + 1
    data[7] = data[7] + 1
    # You can modify other bytes similarly
    
    return data


    
if __name__ == "__main__":
    for _ in range(3):
        usb_led(1, 0, 0, 0)
        time.sleep(0.5)
        usb_led(0, 1, 0, 0)
        time.sleep(0.5)
        usb_led(0, 0, 1, 0)
        time.sleep(0.5)
        usb_led(0, 0, 0, 1)
        time.sleep(0.5)
    usb_led(0, 0, 0, 0)
    
    # time.sleep(1)
    # neopixels_face()
    # time.sleep(1)
    # neopixels_hearing()
    # time.sleep(1)
    # neopixels_off()