#include <stdio.h>   // Librería estándar de entrada/salida para funciones como printf, scanf, etc.
#include <stdlib.h>  // Para funciones de manejo de memoria y control de ejecución como exit()
#include <string.h>  // Para operaciones con cadenas de caracteres, como strcmp, strlen, etc.
#include <ctype.h>   // Para verificar el tipo de caracteres, como isdigit (ver si un carácter es numérico)
#include "tablaDeSimbolos.h"  // Archivo de cabecera donde se define la estructura de la tabla de símbolos

////// TABLA DE SÍMBOLOS //////

SIMBOLO TS[TAMAN_TS];  // Arreglo que representa la tabla de símbolos (TS), de tamaño definido por TAMAN_TS

// Inicializa la tabla de símbolos, asignando a cada posición un valor inválido (-1)
void init_TS(){
    // Se recorre la tabla y se asigna -1 a todos los valores de las entradas, indicando que están vacías
    for(int i=0; i<TAMAN_TS; (TS[i].val = -1, i++));  // valores inadmitidos en Micro
}

// Retorna el valor de un identificador (ID) si está en la tabla de símbolos (TS). 
// Si no lo encuentra, imprime un error y termina el programa.
int ValorSimbolo(char* s){
    int ind = IndiceTabla(s);  // Busca el índice del símbolo en la tabla de símbolos
    if (ind<0){  // Si no se encuentra el símbolo, el índice será -1
        printf("Error: No hay valor asignado para '%s'\n",s);  // Muestra un mensaje de error con el ID no encontrado
        exit(EXIT_FAILURE);  // Termina la ejecución del programa con código de error
    }
    return TS[ind].val;  // Si el ID está en la tabla, retorna su valor
}

// Retorna el índice de la tabla donde se encuentra el ID dado. Si no está en la tabla, retorna -1.
int IndiceTabla(char* s){
    int i=0;
    // Se recorre la tabla de símbolos buscando un identificador que coincida con el nombre pasado como parámetro (s)
    for (i; i<TAMAN_TS; i++)
        if (!strcmp(TS[i].id, s)) return i;  // strcmp compara cadenas, si son iguales retorna el índice actual
    return -1;  // Si no encuentra el identificador, retorna -1 indicando que no está en la tabla
}

// Escribe un valor (v) en la tabla asociado a un identificador (s).
// Si el identificador ya existe, actualiza su valor. Si no existe, crea una nueva entrada.
void EscribirATabla(char* s, int v){
    int ind = IndiceTabla(s);  // Busca si el identificador ya está en la tabla
    // Si el identificador no está en la tabla (ind = -1)
    if (ind == -1){
        int i=0;
        // Busca la primera posición vacía en la tabla de símbolos
        for (i; (i<TAMAN_TS && TS[i].val != -1); i++);  // Encuentra la primera entrada vacía (donde el valor es -1)

        if (i > TAMAN_TS-1){  // Si no encuentra espacio en la tabla
            printf("No hay mas espacio en la TS.\n");  // Mensaje de error indicando que la tabla está llena
            return;  // Sale de la función sin realizar más acciones
        }
        // Asigna el identificador (s) y su valor (v) en la primera entrada vacía encontrada
        TS[i].val = v;  // Guarda el valor (v) en la tabla
        snprintf(TS[i].id, sizeof(TS[i].id), "%s", s);  // Copia el identificador (s) en la tabla
    }
    // Si el identificador ya está en la tabla (ind != -1)
    else
        TS[ind].val = v;  // Actualiza el valor del identificador ya existente en la tabla
}

// Función auxiliar para verificar si una cadena es numérica
// Retorna el valor numérico si toda la cadena contiene dígitos, o -1 si no es numérica
static int numerico(char* s){
    // Recorre cada carácter de la cadena (s)
    for(int i=0; i<strlen(s); i++)
        if (!isdigit(s[i])) return -1;  // Si encuentra un carácter que no es dígito, retorna -1
    return atoi(s);  // Si todos los caracteres son dígitos, convierte la cadena a un número entero
}

// Función utilizada para leer valores asignados a identificadores por medio de "leer(IDs)"
// Solicita un valor desde la entrada estándar para el identificador p1, verifica que sea numérico
// y lo guarda en la tabla de símbolos. Si el valor no es numérico, se muestra un error.
void cargarEntradas(char* p1){
    int valor;
    char temp[15];  // Buffer temporal para almacenar la entrada del usuario
    printf("Ingresa el valor de %s: ", p1);  // Solicita al usuario que ingrese un valor para el identificador
    fscanf(stdin, "%s", temp);  // Lee el valor ingresado desde la entrada estándar y lo almacena en temp

    // Verifica si el valor ingresado es numérico usando la función 'numerico'
    if((valor = numerico(temp)) == -1){  // Si no es numérico
        printf("Error: El valor '%s' no es un numero\n", temp);  // Muestra un mensaje de error
        exit(EXIT_FAILURE);  // Termina la ejecución del programa con código de error
    }
    // Si el valor es numérico, lo guarda en la tabla de símbolos usando la función 'EscribirATabla'
    EscribirATabla(p1, valor);  // Asigna el valor al identificador correspondiente en la tabla de símbolos
}