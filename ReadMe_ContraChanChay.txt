Proyecto Final de Sistemas Operativos
Contra Chanchay

Emily Valarezo Plaza
25 de Enero del 2025

Descripción del Proyecto
Este proyecto simula un sistema de administración en el que se manejan barcos que llegan al puerto con diferentes tipos de carga y destinos. El sistema está diseñado para gestionar el acceso de los barcos al puerto y su procesamiento por parte de varias entidades de control, como el SRI, SENAE, y SUPERCIA. Implementa sincronización de hilos y estructuras de datos como listas enlazadas para manejar los barcos de forma eficiente.

Archivos del Programa
1. Codigo_ContraChanChay.c: Código fuente del programa.
2. Readme.txt: Archivo con instrucciones de compilación y ejecución.
3. Makefile: Archivo para automatizar la compilación, ejecución y limpieza del proyecto.

Compilación del Programa
1. Limpiar archivos generados: make clean
2. Compilar: make
3. Ejecutar el programa: make run

Salida del Programa
El programa generará una salida en consola mostrando:
1. El procesamiento individual de cada barco, incluyendo sus datos.
2. El estado de cada barco (atracando, listo, pendiente).
3. Un resumen final con el estado de todos los barcos procesados.

Notas de Ejecución
1. El programa genera datos aleatorios para simular los barcos que llegan al puerto.
2. Cada barco tiene un identificador único, tipo de carga, peso promedio y destino final.
3. Las entidades de control operan continuamente y toman decisiones sobre el aforo de los barcos.
4. El administrador del puerto gestiona el atraco de los barcos en base a las prioridades establecidas.
