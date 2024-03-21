import os
import sys
import time
from urllib import parse

sys.stderr.write("Python: starting the script.\n")

content = sys.stdin.read()
if not content:
   content="<EMPTY>"
sys.stderr.write("Python: stdin: " + content + "\n")

print("Content-Type: text/html\r\n\r\n", end='')


print("<html> ")
print("<head>")
print("<title> Test CGI </title>")
print("</head>")
print("<body>")

print("<center> <h1>Hello from python!</h1> </center> ")

print("<h2>Env Vars:</h2>")
for param in os.environ.keys():
   print (f"{param}: {os.environ[param]} <br>")

print("<h2>Body:</h2>")
print("Content of stdin:")
print(content)


print("<h2>Saving:</h2>")

method = os.environ.get("REQUEST_METHOD")

if (method != "POST"):
    print("Only POST method allowed for this script!", "<br>")
else:
    qs = os.environ.get("QUERY_STRING")
    if (qs):
        print("Query srting: ", qs, "<br>")
        savepath = parse.parse_qs(qs)["savepath"][0]
        print("Savepath decoded: ", savepath, "<br>")
        directory = os.path.dirname(savepath)
        print("Savepath dir: ", directory, "<br>")

        try: 
            if not os.path.exists(directory):
                os.makedirs(directory)

            with open(savepath, 'w+') as file:
                file.write(content)
        except Exception as exc:
            print("An error occurred:", exc, "<br>")
        else:
            print("Saved successfully!")




print("</body>")
print("</html>")
