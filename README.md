# MiShell

El objetivo del proyecto es hacer un shell que simule los shells del sistema operativo linux.

Las funcionalidades son las siguientes:

1. Al ejecutar el programa este debe imprimir un prompt (ejemplo "$ ")
2. Ejecutar comandos e imprimir su salida
3. Poder ejecutar el comando `cd` (y que cambie de directorio)
4. Redirigir entrada y salida estandar de comandos hacia/desde ficheros con `>`, `<`, `>>` 
5. Redirigir la salida estandar de un comando hacia la entrada de otro con un pipe `|`
6. Una vez terminado el comando previo, se debe mostrar el prompt nuevamente.
7. Solamente habrá un espacio entre cada uno de los *tokens* de la entrada
  * comandos
  * parámetros
  * operadores de redirección (`>`, `|`, etc.)
8. Implementar mas de una tubería
9. Implementar el operador `&` y tener procesos en el background
  * jobs => listar todos los procesos que estan correiendo en el background
  * fg <pid> => envia el proceso <pid> hacia el foreground
  * NOTA: Funcionan comandos como el siguiente: `command1 < file1 | command2 > file2 &`
10. Permitir cualquier cantidad de espacios o no entre los comandos y parametros
11. Implementar un historial de comandos (un commando `history`) que permita imprimir enumeradamente los últimos 50 comandos ejecutados en la consola. Un comando no va a parar al historial si:
  * Si se repite el último comando.
  * Si el comando empieza con espacio.
12. Implementar Re-utilización de comandos.
  * El comando history muestra la secuencia de comandos, con un número a su izquierda. Con este número es posible llamar de nuevo el comando utilizando el caracter admiración "!" (comando `![número]`). El último comando se repite con doble admiración "!!"
  * Ejecutar el último comando (`command`) que se ha ejecutado escribiendo `!command`
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
