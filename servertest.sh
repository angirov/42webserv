#!/bin/bash

# Define configurations for virtual servers
serverConfigs=(
    "allmethods.local:7777/methods"
    "Maximilian.com:8080/blog/pizza/"
    "Maximilian.com:8080/path2/"
    "Vladimir.LOCAL:8080/travel/destination/"
)

# Loop through each server configuration and try to access the location path
for serverConfig in "${serverConfigs[@]}"; do
    echo "Trying to access: $serverConfig"
    curl "$serverConfig"
    echo -e "\n"
done
