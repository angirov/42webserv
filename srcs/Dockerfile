FROM ubuntu:22.04

RUN apt update && apt install -y  make python3 lsof curl
RUN apt install g++ -y

WORKDIR /webserv
COPY . .

RUN make re

EXPOSE 7777
EXPOSE 8080

ENTRYPOINT [ "./server" ]