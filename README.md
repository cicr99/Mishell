Orientacion del Proyecto
========================

El objetivo del proyecto es hacer un shell que simule los shells del sistema operativo linux.

Las funcionalidades son las siguiente:

1. x Al ejecutar el programa este debe imprimir un prompt (ejemplo "$ ")
2. x Ejecutar comandos e imprimir su salida
3. x Poder ejecutar el comando `cd` (y que cambie de directorio)
4. x Redirigir entrada y salida estandar de comandos hacia/desde ficheros con `>`, `<`, `>>` 
5. x Redirigir la salida estandar de un comando hacia la entrada de otro con un pipe `|`
6. x Una vez terminado el comando previo, se debe mostrar el prompt nuevamente.
7. x Solamente habrá un espacio entre cada uno de los *tokens* de la entrada
  * comandos
  * parámetros
  * operadores de redirección (`>`, `|`, etc.)

y hasta aqui tienen 3 puntos en el proyecto

8. x Implementar mas de una tubería (1pt adicional)
9. Implementar el operador `&` y tener procesos en el background (1pt adicional)
  * jobs => listar todos los procesos que estan correiendo en el background
  * fg <pid> => envia el proceso <pid> hacia el foreground
  * NOTA: aqui tiene que funcionar algo como `command1 < file1 | command2 > file2 &`
10. x Permitir cualquier cantidad de espacios o no entre los comandos y parametros (0.5pt adicional)
11. Implementar un historial de comandos (un commando `history`) que permita imprimir enumeradamente los últimos 50 comandos ejecutados en la consola (0.5pt adicional). Un comando no va a parar al historial si:
  * Si se repite el último comando.
  * Si el comando empieza con espacio.
12. Implementar Re-utilización de comandos.
  * El comando history muestra la secuencia de comandos, con un número a su izquierda. Con este número es posible llamar de nuevo el comando utilizando el caracter admiración "!" (comando `![número]`). El último comando se repite con doble admiración "!!" (0.5pt adicional)
  * Ejecutar el último comando (`command`) que se ha ejecutado escribiendo `!command` (0.5pt adicional)
  * NOTA: Por ejemplo history retorna:
    1  history 
    2  ls 
    3  cd /dev 
    4  ls 
    5  rm *.bak
    6  cd /etc 
    7  history
   `!5` ejecuta "rm *.bak"
   `!!` ejecuta "history"
   `!cd` ejecuta "cd /etc"

Sobre la Entrega
================

Fecha de entrega: Jueves 3 de octubre
-------------------------------
