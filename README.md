# 42webserv

# How to use

`docker-compose build --no-cache && docker-compose up --force-recreate -d`

Try: `http://localhost:8000/colorful/index.html`

Also often used: `docker compose build  && docker compose up --force-recreate -d && docker logs webserv &&  docker exec -it webserv tail logfile.txt -f`

# Python save script

For encoding of qurery strings:
https://www.url-encode-decode.com/

```
curl -X POST -d 'Hello from curl again!' http://localhost:8000/save/?savepath=%2Fdata%2Fnewdir%2Ffile.txt
```

