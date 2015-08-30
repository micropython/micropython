Soporte para EDU-CIAA
========================
<p align="center">
  <img src="https://avatars0.githubusercontent.com/u/6998305?v=3&s=400" alt="CIAA Logo"/>
</p>

Modo de uso:
- Editar py/Main.py y escribir el programa en Python
- Ejecutar python py2c.py para generar el archivo ProgramScript.c
- Ejecutar make download para compilar y bajar el programa en la EDU-CIAA

Archivos incluidos:
- py/ -- Aqui se encuentra el archivo Main.py donde escribiremos nuestro programa Python.

- py2c.py -- Scripy en Python que convierte el script de Python Main.py en el archivo ProgramScript.c

- ProgramScript.c -- Archivo .C que se incluye en Main.c y contiene como array de bytes el script Python.


