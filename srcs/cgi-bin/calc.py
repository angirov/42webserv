#!/usr/bin/env python3

import os
import sys
from urllib import parse

sys.stderr.write("Python: starting the script.\n")

print("Content-Type: text/html\r\n\r\n")

print("<html>")
print("<head>")
print("<title>Simple Calculator</title>")
print("</head>")
print("<body>")

print("<h1>Simple Calculator</h1>")

qs = os.environ.get("QUERY_STRING", "")
qs = qs.replace("+", "%2B")  # Replace '+' with its URL-encoded equivalent '%2B'
parsed_qs = parse.parse_qs(qs)

num1 = parsed_qs.get("num1", [""])[0]
num2 = parsed_qs.get("num2", [""])[0]
operand = parsed_qs.get("operand", [""])[0]

 # print("<p>Debugging: num1 =", num1, ", num2 =", num2, ", operand =", operand, "</p>")

if num1.isdigit() and num2.isdigit() and operand in ['+', '-', '*', '/']:
    num1 = float(num1)
    num2 = float(num2)
    result = None
    if operand == '+':
        result = num1 + num2
    elif operand == '-':
        result = num1 - num2
    elif operand == '*':
        result = num1 * num2
    elif operand == '/':
        if num2 != 0:
            result = num1 / num2
        else:
            print("<p>Error: Division by zero!</p>")
            result = None

    if result is not None:
        print(f"<p>{num1} {operand} {num2} = {result}</p>")
else:
    print("<p>Error: Invalid input. Please provide two numbers and a valid operand (+, -, *, /).</p>")

print("</body>")
print("</html>")
