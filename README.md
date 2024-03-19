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
