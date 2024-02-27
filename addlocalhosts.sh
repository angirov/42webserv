#!/bin/bash

# Define configurations for virtual servers
serverDomains=(
    "allmethods.local"
    "Maximilian.com"
    "maximilian.local"
    "Vladimir.LOCAL"
    "www.vladimir.org"
)

# Add server domains to /etc/hosts
for domain in "${serverDomains[@]}"; do
    echo "127.0.0.1 $domain" | sudo tee -a /etc/hosts
done
