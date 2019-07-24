The following openssl command can be used to generate self-signed server certificate:

openssl req -newkey rsa:2048 -nodes -x509 -sha256 -out certs/server.pem -keyout certs/server.key -days 365 -subj "/C=US/ST=WA/L=Place/O=YourCompany/OU=IT/CN=www.yours.com/emailAddress=yourEmail@your.com"


The following openssl commands can be used to generate client certificate:

openssl genrsa -out certs/client.key 2048

openssl req -new -key certs/client.key -out certs/client.csr -subj "/C=US/ST=WA/L=Place/O=YourCompany/OU=IT/CN=www.yours.com/emailAddress=yourEmail@your.com"

openssl x509 -req -in certs/client.csr -CA certs/server.pem -CAkey certs/server.key -CAcreateserial -out certs/client.pem -days 365 -sha256
