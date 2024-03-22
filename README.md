# 42webserv

# How to use

`docker-compose build --no-cache && docker-compose up --force-recreate -d`

Try: `http://localhost:80/colorful/index.html`

Also often used: `docker compose build  && docker compose up --force-recreate -d && docker logs webserv &&  docker exec -it webserv tail logfile.txt -f`

# Python save script

For encoding of qurery strings:
https://www.url-encode-decode.com/

```
curl -X POST -d 'Hello from curl again!' http://localhost:80/save/?savepath=%2Fdata%2Fnewdir%2Ffile.txt
```

# Config googledoc
https://docs.google.com/document/d/1AY-qDocJuQR4Rgr7KgyeAjsX5wkgn2mUF8NiFqpTxOQ/edit?usp=sharing

# Uploads tests

```
curl -X POST 127.0.0.1:80/uploads/test.txt -d "@./data/files/100k.txt" -v
curl -X DELETE 127.0.0.1:80/uploads/test.txt  -v  
```

```
telnet 127.0.0.1 80

GET /cgi-bin/../../data/pycgi/hellopy/hello.py?name=Max HTTP/1.1

GET /cgi-bin/hello.py?name=Max HTTP/1.1

curl 127.0.0.1/cgi-bin/hello.py?name=Max -v

curl -X POST 127.0.0.1/cgi-bin/hello.py?name=Max -d "this is the body" -v
```

# Blog

```
curl -X POST '127.0.0.1/cgi-bin/blog.py?title=webserv&date=2024-03-20' -d "This project is here to make you write your own HTTP server. You will be able to test it with a real browser. HTTP is one of the most used protocol on internet. Knowing its arcane will be useful, even if you won't be working on a website." -v

curl -X POST '127.0.0.1/cgi-bin/blog.py?title=webserv&date=2024-03-22' -d "This is hopefully our final blogpost before we finish this project and try to transcend." -v
```

# Simple Calculator

provide 2 numnbers and a valid operand to do simple arithmetic operations
argument:
num1=
num2=
operand= 

```
http://localhost/cgi-bin/calc.py?num1=10&num2=5&operand=+
```



# Config explanation
```
default.conf

default.local
port 80
http://127.0.0.1:80

/
/colorful
/uploads
/cgi-bin
/blog


redirect.local
port 8080
http://127.0.0.1:8080

/ -> default page same as default.local maybe not necessary: could/should be deleted
/git_vladimir
/git_kolja
/git_max
/42wolfsburg
/columbia301
/columbia303


indexserver.local
port 8040
http://127.0.0.1:8040

/ NO METHODS ALLOWED -> FORBIDDEN
/colorful : index set, takes priority over autoindex on
/colorfulaion : no index page, autoindex on
/colorfulaioff : no index page, autoindex off -> FORBIDDEN




benchmark.conf

benchmark.local
port 80
http://127.0.0.1:80

/ -> empty index page


eval.conf

default.local
port 80
http://127.0.0.1:80

/
/colorful
/uploads
/cgi-bin
/blog

Same as default, to be modified by evaluator
```