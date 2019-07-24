#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

//#define IS_GGD
#define IS_AWSKEY

/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
#ifdef IS_GGD
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"   \
"MIIDWjCCAkKgAwIBAgIVAKLh4ENCpNu+2GJaJHzBFashA6uQMA0GCSqGSIb3DQEB\n"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xODA4MjExNDU2\n"\
"MDJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDAo4a6WZgdzSIrkOej\n"\
"/AYThWO6iB81qImTQDLv+QML9geb2eoxwqdCbe37gUk4O3MJvWo8FzOn65RbHDnJ\n"\
"h1NmZiXRZwAAHo34xrtQUWbW6MPKNI9qyseLnYMquQHyJf4OhUwM+NZ9PiglrULm\n"\
"+7Fq7PxHqNQlP1BVadvCAT82upIP+5kHmCrgDC4clr0X2+R7KbLeNEB3Es8yqFoc\n"\
"FQuSyJ6haJ/qwlxbQVljuqU0B4fJUrC2qwWyt3BBqiX0MSBLHxYyp8Ht696t43aK\n"\
"mNjJKOulKo+u0ifPMblJZS+riYfAzSwaD89WUKrMNyoRreiCaP+ibUlyfQKixbcU\n"\
"YEF9AgMBAAGjYDBeMB8GA1UdIwQYMBaAFO8qmcNAdBq5ybYCIai6rrSEmWkeMB0G\n"\
"A1UdDgQWBBRTov+J75ZT/H7JP/7doumjlXQSODAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAJp3ph2lio/iQPemU/+JIbRZ8\n"\
"vOmGgJZWruA/5rTq5t1Wwu5vYHZAurykmqfSz4q6kLgCINxA2A3VX6WahzQ8DwRm\n"\
"so4NS34bIWMSDv7/sQtQ/L4tdWwml1yvUnEpo7Em63BJJrsEuCdwYv3LnPF+ciaQ\n"\
"ze8crk6HvvncV/QL+3MPb8LP0TMBh/DTllV/irhCG5qDU3JISkAGW1exF55I4qqE\n"\
"iAgOuqMVnifH/bVgbij4jZ1g0fELXBMHjD7bhFzb+zdbAsniZUX3V3ha3A2yu2hB\n"\
"6boCwdh2567C7UEPExDnRzMUbm61QdEEE9f3GGhOQ6etLft0vPI9tgt0MibLMg==\n"\
"-----END CERTIFICATE-----";
#elif defined(IS_AWSKEY) //aws keys
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"   \
"MIIDWTCCAkGgAwIBAgIUH0k3Sy0XkJ+59mxMyjnJ9EUyhyAwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDEzMDA4MDEw\n"\
"MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK+ODkJSGPhaVIoAlnjb\n"\
"lWIBA801gFdG2XFx2yOr79wbEf8Q71UF0z+EMWgFIRdTMJ1tveVDGDkk4MXhcPfC\n"\
"aC4Rr13TsjtRJhmp2uAmZPd8dX56quKo/hKwvkGMlS7XflwYYSrYKL72s3Q6ItYm\n"\
"UKrE8dI2oJkfNnr/Pf1Uy7zuemUN8MFvBrUVBA7umTKIP7OYZAwwZ7FiQKNnj7RP\n"\
"TCxFzvkcofU2pKiqFI5VnzzOI7WvEPUhOAYPoTLCE7sWdMD/4McxZi8txzP0zx4Z\n"\
"rQpMUphMssfDISXD5RHMGxtVKxgSlCk2XF/UUw4G5Zd2Ph0ymJUVZGztwNHirx1v\n"\
"INsCAwEAAaNgMF4wHwYDVR0jBBgwFoAUQVZNyvuX1CZgRVEWcL618ikLfVgwHQYD\n"\
"VR0OBBYEFJgjUuru5fAPbT4feWHudbAU/da2MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAFEoJL4P8Q0UqZ2snCLvkwUw+o\n"\
"slbzCZmbnzIQagP3nttSV2Aw9vdZKlOZBxSSTXguEaysfZMEP1K96YSSBLaPHR59\n"\
"xdcvbDfBn/jamI1Xt/IUwcWXRIOLgG5gXHeINAdpGJicJj+T8t4LnOMF4i2IR/b6\n"\
"DG1CH8hsWAfWWy6oGJjdoETsoMtTzkq265oaJWXj+82wAB90slC5hUUP7Q82lgMl\n"\
"ggKO43DU5q2axcXmr/N1ym5E6OMnRSG9Xr6OQSCC4lhaaKTrLzHMP2WUFxb7rQaP\n"\
"ycmCVoFvnOuQNkRG1gmA4uryEkEYlitl0/hbzChWq2RCANh+S7QjZys0pKtG\n"    \
"-----END CERTIFICATE-----";
#else // local key
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"   \
"MIIDlDCCAnwCCQCCYKMKbQdxrjANBgkqhkiG9w0BAQsFADCBizELMAkGA1UEBhMC\n"\
"VVMxCzAJBgNVBAgMAldBMQ4wDAYDVQQHDAVQbGFjZTEUMBIGA1UECgwLWW91ckNv\n"\
"bXBhbnkxCzAJBgNVBAsMAklUMRYwFAYDVQQDDA13d3cueW91cnMuY29tMSQwIgYJ\n"\
"KoZIhvcNAQkBFhVtYXJpYW4uY2luZ2VsQG54cC5jb20wHhcNMTgwNDA1MTEzMTIy\n"\
"WhcNMTkwNDA1MTEzMTIyWjCBizELMAkGA1UEBhMCVVMxCzAJBgNVBAgMAldBMQ4w\n"\
"DAYDVQQHDAVQbGFjZTEUMBIGA1UECgwLWW91ckNvbXBhbnkxCzAJBgNVBAsMAklU\n"\
"MRYwFAYDVQQDDA13d3cueW91cnMuY29tMSQwIgYJKoZIhvcNAQkBFhVtYXJpYW4u\n"\
"Y2luZ2VsQG54cC5jb20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCh\n"\
"ATRUliNxGlVSKe0jem5snNu619jSpcM5jk3O0Xmzy0G0sHe13fy1aKiqXiJ14FCL\n"\
"/YWNYIXgzFQWDhRLnOBPl9rPuHymYnVkeFVBdvUwzV5M92WhNMOg9zwXab2e4cSR\n"\
"9kG823/uxgJN+5bFhWm5nkQVC0GIDG9OUDaxKMYzCZxGIg1Jwb2d2VaUfF1JtWaJ\n"\
"gqgZ68a4BXulfkhxrzAlSf2ooFzGyqwaQGufj7ZkkNAGiQWDAlwENugSO8qFSjw0\n"\
"JB8R7brv5XQ/D+kItlzoZv/Z3iX084Rs2XBrZPRsGZ8E15E4O8vW6+Dh0h9Nc4ZW\n"\
"2Tt3qCXXv18nuILzNeGZAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAGgl8xu2orS7\n"\
"6GNWEl2KmdIru8y9PVXnrRQjX82a9TISn5H+MUv3iBs4PZcdYaMN1b+1HfIgHS3P\n"\
"gHfkLat2TVbWLspT6gTTcf3QdpC3jPYRIYj/Wzw+maaZ2dwbpt6o4/PNzaHAQLOv\n"\
"MgGKM4yMlRj4EDgWD7Wn3Q/fwDCZczpZ6WcoMojtC8d+gm2W96Ov2IvQ/yfUIZj8\n"\
"BBQxBhJaL7vu4bW5wKrU8cXdauI1vXQZd03btd3xzN/ZC+KAzWbd811Yxc39uDfV\n"\
"/77jW+XE0naInD6uBBnEoOVxC3icZ68ix3+Q53nG++aGnWe8NbL7b8Ublr/tO/Km\n"\
"meRvPDOi8vc=\n"                                                    \
"-----END CERTIFICATE-----\n";
#endif

/*
 * PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 * This is required if you're using JITR, since the issuer (Certificate
 * Authority) of the client certificate is used by the server for routing the
 * device's initial request. (The device client certificate must always be
 * sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html,
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set the below pointer to NULL.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----"
 * "...base64 data..."
 * "-----END CERTIFICATE-----";
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----"
 * "...base64 data..."
 * "-----END RSA PRIVATE KEY-----";
 */
#ifdef IS_GGD
#define keyCLIENT_PRIVATE_KEY_PEM  \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpgIBAAKCAQEAwKOGulmYHc0iK5Dno/wGE4VjuogfNaiJk0Ay7/kDC/YHm9nq\n"\
"McKnQm3t+4FJODtzCb1qPBczp+uUWxw5yYdTZmYl0WcAAB6N+Ma7UFFm1ujDyjSP\n"\
"asrHi52DKrkB8iX+DoVMDPjWfT4oJa1C5vuxauz8R6jUJT9QVWnbwgE/NrqSD/uZ\n"\
"B5gq4AwuHJa9F9vkeymy3jRAdxLPMqhaHBULksieoWif6sJcW0FZY7qlNAeHyVKw\n"\
"tqsFsrdwQaol9DEgSx8WMqfB7evereN2ipjYySjrpSqPrtInzzG5SWUvq4mHwM0s\n"\
"Gg/PVlCqzDcqEa3ogmj/om1Jcn0CosW3FGBBfQIDAQABAoIBAQCV/b1lU2VRrusw\n"\
"KL9zu0Ov52hjUKGcUgHsmUwP9T8eDQ6XlvD2la1N1/I4pc2w7vj/WHBjIV2jHZaB\n"\
"6lRESg3x6Q8BIMWrUh8Q2Qv32anNI6duxcX1TLZaSuZaWXreZB6Qh9FTOF0pX0A7\n"\
"NdClsMvPiX5U/WtRJR8TojztrbLelMHpVeMR9+gHROt4OdvzQ+ZW3G2wlnNg+vFJ\n"\
"S5WHkZAnHthC5EGDwpKwVFuHpe5NtCpi5iPwXOOVHOruqbsB0TmJUsI6z3WAVOyJ\n"\
"q0U105vqibXuBvC8z4B+IsRxX6pHZkp293mP7zLBl686k7fpaCVQBPYFEb6vPNc4\n"\
"zA/58yWBAoGBAPoaY4p33r2Y2ziIGOeK6NH52yxL3lmj2iLVVx1eOsGpslzU5/uR\n"\
"wU7wO+uplewcWZ4Sc+Pkw3r6voJeGZnc5txs2MjuR34wX1gzGsLigWiD6cxqpWYC\n"\
"RcO45KezMHER6UiMDmnTDk57aBj1Fnzyg2teyxhQxUVIh0/Ogt6FKbS5AoGBAMUu\n"\
"SRHLAjOMW96oRXbEPGkfTb2C18IQfuC4HjY0Md1DGCV+vRpiVcPGsaXbC/M1pc4l\n"\
"erCISwQtOUkEZEAEhqGR/zSJQk1p+d5IGDwMoAx8CuJYao9Bqwwku7UdikYKLmUl\n"\
"r1ML8ZON0ToniwVkhMdOtdBPwEdsyWCaz8uqgljlAoGBAORrW4QojH8e7wDHk+9M\n"\
"t+ymRN1FAXNfKafsrt58pC8h3Gvaa3EgoquEz0UhonJ9RgAPnFaEJWDZR0trw/f5\n"\
"f8PLiYah0RvX9AxqyEM7ebNJ3ys6L4tmAe+KmxLqnhB2qlf+yl8uSn+1R9KbbQl6\n"\
"uDpE3jp78l+fqS1jT7UJa5jJAoGBAJV09ueos7t02THU5AoYxh18H8XUhkvPiww0\n"\
"Ipo9R8qJj6TkAtwbyxxA3+hyNwWbFVlC6VMji5DX4oLo9D/d7lpLOmCM5ZeeC0oj\n"\
"5VKm0y2DXBCiqep6APeMZEbs5CEgmHgMb28IL40DQqLM1EA7J7WWgJMrz/eTirRR\n"\
"w82BKZnNAoGBAIDF9hcK40ytkJkmLWRjK3DiGuOIKM9NvIKpfi5LIrP7PzLXRb4n\n"\
"vkjzisw4ZBfTp0di0fM0g1ujxhb53ecTncrkCi7tfuIubIVw4GryyeSi/6yifMK6\n"\
"PVl+paLTqwrfhSUF5prBPgIN/NWy1vqBdxDWIXJrLVhPedZWVpe/vgwA\n"\
"-----END RSA PRIVATE KEY-----";
#elif defined(IS_AWSKEY) //aws keys
#define keyCLIENT_PRIVATE_KEY_PEM  \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpQIBAAKCAQEAr44OQlIY+FpUigCWeNuVYgEDzTWAV0bZcXHbI6vv3BsR/xDv\n"\
"VQXTP4QxaAUhF1MwnW295UMYOSTgxeFw98JoLhGvXdOyO1EmGana4CZk93x1fnqq\n"\
"4qj+ErC+QYyVLtd+XBhhKtgovvazdDoi1iZQqsTx0jagmR82ev89/VTLvO56ZQ3w\n"\
"wW8GtRUEDu6ZMog/s5hkDDBnsWJAo2ePtE9MLEXO+Ryh9TakqKoUjlWfPM4jta8Q\n"\
"9SE4Bg+hMsITuxZ0wP/gxzFmLy3HM/TPHhmtCkxSmEyyx8MhJcPlEcwbG1UrGBKU\n"\
"KTZcX9RTDgbll3Y+HTKYlRVkbO3A0eKvHW8g2wIDAQABAoIBAQCjAq8u2fihs47I\n"\
"Td9q/cpqJDMZe1JeuET0WBp5tw+UO/ar91ekhTIwyljuE8U0WWTM6bfoiDKX7Sxh\n"\
"0gIJrRT7oiRKZWYcOi+8qMG6XSxnucOH2/ONYAn75O3lA9sKkcKdVW30UJ8VIxJj\n"\
"HBB49T72MsWwwlTBmgAGmbc/4lsEZIp4a9dlLU4fuc+lLWqzn8MkqfThwbweDEJi\n"\
"zpbedatHL0yXDGKUZBxfGpGhRRV47jNqLC2X+WtIRkjZC95vo0cH/lU9efkskRQy\n"\
"VZBJa7B8cWP8d28Gwbdi/Ktt8vkC5tqzhH5oIGmxUHb284BpI28OOvG6L8sYdCit\n"\
"yEUTW7YxAoGBANxL00pveHcVCrZMm4O65NeAszdLz0nroMaTzErQYs/OmpR0IF51\n"\
"EhMzmq9QaOrL/Ed6GexkPSq8+Q3kRrLB5AWMwwQbl+F7uQ4uixo/ETMWR+SMW9F0\n"\
"VtMUyIgiNKZa88Ar2DGsPlKjDcQjOcQqSdGrJiPC+8KePuR4VlHhc2LHAoGBAMwB\n"\
"5Z1Jwcb0+cHC6LYjGMGkTBd1Ogb/cjPE+VWUFcW4ZgLR8uAsy4YpP/SgdiM2vgC2\n"\
"Y5AGCYmbGOBCjpBeg7k8+2yk8H7Gi5iW2d+vDPHDE3aP0V1zW57GXJJgWUaR4z2q\n"\
"ID55tZrsXLbZumm9DX6WKwxcgWqlmkjm7RVGRz1NAoGBAL7oTxDgjQrVh1T4CoUb\n"\
"/5jqBWVUIy0yL0AncREciaUArGEeSbbRyXoubEylaGaho9zlaXW8YG2bGlH5k3Ly\n"\
"KALPO2lh6DsVlwc9OnriUKtvEZp0MFXRdjSyfB3NVscbZsUQuehCFjhS6WkiBbuD\n"\
"0jkLEqBHJxo/PM2No3CN6gqdAoGAdZnGVmRVOy6u5v5Hje8vbzkZ0A/4kl4g5Czx\n"\
"Gl8SKfdBfvCuFIJeFImk34y8bVI6N0jsnfKM/vEUZvqXGiwRvO+ldBykskAb6qtT\n"\
"DznmuVsd1iPTqg48y4tHg+3cqOyiVULD8Z/Ri2wjihh6+EdOByHj0O1mgSbTiKDu\n"\
"HGx11qUCgYEAyBX5DQIaedp/V+6elsWbrw09qxjbpUoHgOgrhKhz61gW+AHxlRHG\n"\
"3Mzoav3Q+emcWzkz94uWI1ET5XByqauR7hH6dKPQofqmGIp78GTut1B5bF1nWYe1\n"\
"Jc5k1qS5L4BNDLpQbOfpBH4d639tjDWPdVh7jz0fB3k3g9qPcMsHNGc=\n"\
"-----END RSA PRIVATE KEY-----";
#else // local
#define keyCLIENT_PRIVATE_KEY_PEM  \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAoQE0VJYjcRpVUintI3pubJzbutfY0qXDOY5NztF5s8tBtLB3\n"\
"td38tWioql4ideBQi/2FjWCF4MxUFg4US5zgT5faz7h8pmJ1ZHhVQXb1MM1eTPdl\n"\
"oTTDoPc8F2m9nuHEkfZBvNt/7sYCTfuWxYVpuZ5EFQtBiAxvTlA2sSjGMwmcRiIN\n"\
"ScG9ndlWlHxdSbVmiYKoGevGuAV7pX5Ica8wJUn9qKBcxsqsGkBrn4+2ZJDQBokF\n"\
"gwJcBDboEjvKhUo8NCQfEe267+V0Pw/pCLZc6Gb/2d4l9POEbNlwa2T0bBmfBNeR\n"\
"ODvL1uvg4dIfTXOGVtk7d6gl179fJ7iC8zXhmQIDAQABAoIBAF7JbbyKs4znuuln\n"\
"Y4PqlU3+PuJOJwlEq52b6LTk4Ch4dlNhPOjmKtvGJN5AEzugdusjs1ebRtYklTBy\n"\
"04H8dpaFST3XWBNAjeo7ab3jwdAcZ/MaB0wT20RNqaQDDa6XLfncp7D+7N9u1XaB\n"\
"sq7Qr4Cm62AUQ11MLM9HLBu6bI0ooSid/JvK/wjTnNPtmutbpkGcsOb6msdS8ixg\n"\
"rg5QIuuH71nmWTCDVvvn8I/wfYmLnytoO/CvkIVs6kkKgJLgOzsTIkRYNUEHRFrX\n"\
"alangJ8h051fZcOF7UexIK1TpU9khmR0DVFqsNu4HgxwvRnwpn7+bZF1lB8bZaF9\n"\
"CDO5eX0CgYEA0Am8oYCSeDiV5WzYaD4qjsIh09dwSnoxwukSMBekMk2/zArrFPfQ\n"\
"5Zb1IY9lLc7gSMnoiLAxtBaeF4yo9l3CBCGlF5Sg3r8FUzDnEBDUAdRKkwd6yE41\n"\
"a70BhSWBdMQkmu7vnd2OsDPk/q1wp7BBgej0ac8+KK1+mVxgBHifqgMCgYEAxh+X\n"\
"rGLa3ifJEvrkDYI9VASxa/Bll12109MkQS1eLiQCeznyEEYqBa760PzUzjnW7VtM\n"\
"TMDMLQpdFMUomqwpTdHlHSLtgP9NNqAIV1vctSANF9ojRunfEuxuldE1kDXUKR9f\n"\
"Ds6SDGS18AWPAbEFxZxrVYzRYQZLVKrjpyc/ATMCgYEAtUGGhAANnYlPsJDhDBH9\n"\
"6lviNTxjrcBBUtzW5+fxO5LaJODuaVpxO1gHQ7I4KW0xpLmmTFGXPJzoKQR4wg52\n"\
"WDsoKB9q+ifcb9sPtsOys9VPFENUCul5SeFnTaab4Cx06Dh9tUGwbH0Ka4qZuMCs\n"\
"ko9xRC84rSBcSSpezgZ6thkCgYBW8lxkyclVf05N5w8YIpT5f0nBnfFnmTeHq3yY\n"\
"oelYBHOWUpq5q6Ebd2wZNSvLCTUaW1vDv0Sba5Xn3JJyNqW1wixzOfXTC7d+BoNC\n"\
"m0zjrBoy4odiwpYkPuBVHUgNbqIaHetgmyUbDUOO/1GAI4jt7TRpifAFTXbXxFs9\n"\
"6wpKYwKBgBsBKBY0496qxMZnU2XXoT7uP21UMIc7jjINhmJzZ/6zYoH/GjiT/jsM\n"\
"VmpyguaRQpJQGDiC58snzc3ibtCzva2D9bpJLYisyLYJGitTTU5Jz0IQWoi0l4C1\n"\
"cFbCv2wWZoU2ql8CJ1dGRHrYmNb5kupzsWmAxMT/XTsCYjg92Z3f\n"\
"-----END RSA PRIVATE KEY-----\n";
#endif

/* The constants above are set to const char * pointers defined in aws_dev_mode_key_provisioning.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char *clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
