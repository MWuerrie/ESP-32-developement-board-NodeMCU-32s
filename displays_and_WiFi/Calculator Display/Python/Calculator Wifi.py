import socket


# =====================================================
# ESP32 wifi address & Port
# =====================================================


ESP32_IP = "192.X.X.X"     # IP address of your ESP32
ESP32_PORT = XXXX          # Port, e.g. 5000



# =====================================================
# Connect to ESP32
# =====================================================

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# socket                 → Python-Modul
# socket.socket()        → produce Socket
# AF_INET                → IPv4
# SOCK_STREAM            → TCP
# client                 → our variable name

print("Connecting to ESP32...")

client.connect((ESP32_IP, ESP32_PORT))

print("Connected!")
print()


# =====================================================
# Send message
# =====================================================

def send_message(message):
    client.sendall((message + "\n").encode())


# =====================================================
# First number
# =====================================================

send_message("first number")

first_number = int(input("First number: "))

send_message(str(first_number))

# =====================================================
# Operator
# =====================================================

send_message("operator? (+,-,*,/)")

while True:

    operator = input("Operator (+,-,*,/): ")

    if operator in ["+", "-", "*", "/"]:
        break

    print("Please enter +, -, * or /.")

send_message(operator)

# =====================================================
# Second number
# =====================================================

send_message("second number")

second_number = int(input("Second number: "))

send_message(str(second_number))

# =====================================================
# Calculate result
# =====================================================

if operator == "+":
    result = first_number + second_number

elif operator == "-":
    result = first_number - second_number

elif operator == "*":
    result = first_number * second_number

elif operator == "/":

    if second_number == 0:

        print("Error: Division by zero.")

        send_message("error")

        client.close()

        exit()

    result = first_number / second_number


# =====================================================
# Check result
# =====================================================

if not float(result).is_integer():

    print()
    print("Result is not a whole number.")
    print("The 3641AS currently displays whole numbers only.")

    send_message("error")

    client.close()

    exit()


result = int(result)


# =====================================================
# Check 3641AS range
# =====================================================

if result < 0 or result > 9999:

    print()
    print("Result cannot be displayed.")
    print("Allowed range: 0 - 9999")

    send_message("error")

    client.close()

    exit()


# =====================================================
# Send result to ESP32
# =====================================================

print()
print("Result:", result)

send_message("result:" + str(result))

response = client.recv(1024).decode().strip()

print("ESP32:", response)

# =====================================================
# Close connection
# =====================================================
                                                                                                        
client.close()

print()
print("Connection closed.")

