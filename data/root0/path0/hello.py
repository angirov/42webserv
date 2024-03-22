import os
import sys
content = sys.stdin.read()

print("Content-Type: text/html\r\n\r\n", end='')


print("<html> ")
print("<head>")
print("<title> Test CGI </title>")
print("</head>")
print("<body>")

print("<center> <h1>Hello from python</h1> </center> ")

print("<h2>Env Vars:</h2>")
for param in os.environ.keys():
   print (f"{param}: {os.environ[param]} <br>")


print("<h2>Body:</h2>")
print("Content of stdin:")
print(content)

print("</body>")
print("</html>")
