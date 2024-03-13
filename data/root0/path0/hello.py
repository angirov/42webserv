import os

print("Content-Type: text/html\r\n\r\n")


print("<html> ")
print("<head>")
print("<title> Test CGI </title>")
print("</head>")
print("<body>")
print("<center> <h1>Hello from python</h1> </center> ")

for param in os.environ.keys():
   print (f"{param}: {os.environ[param]} <br>")


print("</body>")
print("</html>")
