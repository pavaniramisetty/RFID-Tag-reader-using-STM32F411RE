import serial
import time
from Adafruit_IO import Client, Feed, RequestError

# Adafruit IO setup
ADAFRUIT_IO_KEY = '/*Replace with your Adafruit IO key*/'  
ADAFRUIT_IO_USERNAME = '/* Replace with your Adafruit IO username*/'  
aio = Client(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

# Feeds setup (Ensure these feeds exist in your Adafruit IO dashboard)
try:
    status_feed = aio.feeds('status')  # Replace 'status' with your feed name for status
    number_feed = aio.feeds('number')  # Replace 'number' with your feed name for number
except RequestError:  # Create the feeds if they don't exist
    status_feed = aio.create_feed(Feed(name='status'))
    number_feed = aio.create_feed(Feed(name='number'))

# Set up the serial port connection
ser = serial.Serial(
    port='/dev/ttyACM0',    # Replace with your port (e.g., 'COM3' for Windows, '/dev/ttyS0' for Linux)
    baudrate=9600,        # Set baudrate (ensure this matches your device)
    timeout=1             # Timeout in seconds
)

# Function to read data from the serial port and extract values between slashes
def read_serial_data():
    try:
        if ser.is_open:
            data = ser.readline().decode('utf-8').strip()
            if data:
                print(f"Received: {data}")
                
                # Split the data based on the slash '/'
                parts = data.split('/')
                
                if len(parts) >= 3:
                    status = parts[1].strip()  # Data between 1st and 2nd slash (status)
                    number = parts[2].strip()  # Data between 2nd and 3rd slash (number)

                    print(f"Status: {status}")
                    print(f"Number: {number}")

                    # Send data to Adafruit IO
                    send_to_adafruit_io(status, number)
                    
                    return status, number
                else:
                    print("Data format is incorrect. Expected two slashes.")
    except serial.SerialException as e:
        print(f"Error reading from serial port: {e}")
    return None

# Function to send data to Adafruit IO
def send_to_adafruit_io(status, number):
    try:
        # Send the status and number to their respective feeds
        aio.send_data(status_feed.key, status)
        aio.send_data(number_feed.key, number)
        print(f"Status and Number sent to Adafruit IO.")
    except Exception as e:
        print(f"Error sending data to Adafruit IO: {e}")

if __name__ == "__main__":
    print("Starting serial port reading and Adafruit IO update...")
    try:
        while True:
            serial_data = read_serial_data()
            if serial_data:
                # Further processing of the separated variables can be done here
                pass
            time.sleep(1)
    except KeyboardInterrupt:
        print("Stopping serial port reading.")
    finally:
        ser.close()
