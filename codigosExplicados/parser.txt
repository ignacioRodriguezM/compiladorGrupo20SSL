%{
int yylex();  // Declaración de la función que realizará el análisis léxico, generada por Flex

#include <stdio.h>   // Para funciones de entrada/salida estándar
#include <stdlib.h>  // Para manejo de funciones y tipos estándar (malloc, free, etc.)
#include <string.h>  // Para funciones de manipulación de strings como strcmp y strcpy
#include "tablaDeSimbolos.h"  // Inclusión del archivo de cabecera que contiene las funciones para manipular la tabla de símbolos

void yyerror(char *s);  // Prototipo de la función de manejo de errores sintácticos

extern int yynerrs;  // Variable externa que Flex/Bison usa para contar los errores sintácticos (ya definida por Bison)
int yylexerrs = 0;   // Contador de errores léxicos, inicializado en 0. Se incrementará si Flex detecta errores.
extern FILE* yyin;   // Archivo de entrada que utilizará Flex para el análisis léxico. Puede ser stdin o un archivo.
%}

/* Declaración de tokens: palabras clave, operadores, símbolos. Estos tokens representan las diferentes partes del lenguaje que el parser debe reconocer. */
%token INICIO FIN LEER ESCRIBIR PUNTOYCOMA IMPRIMIR VECES  // Palabras clave del lenguaje
%token <id> ID  // Token que representa un identificador (variable). Está asociado al valor semántico id, que es un string.
%token <cte> CONSTANTE  // Token que representa una constante numérica (un entero). Asociado a cte.

%union {  // Unión que define los tipos de valores que los tokens pueden tomar
    char* id;  // Los identificadores (ID) serán representados como cadenas de caracteres (puntero a char)
    int cte;   // Las constantes (CONSTANTE) serán representadas como enteros
}

/* Definición de la precedencia y asociatividad de los operadores */
%left '+' '-' ',' '*'  // Los operadores de suma, resta, multiplicación y coma tienen asociatividad izquierda
%right ASIGNACION  // El operador de asignación tiene asociatividad derecha

/* Definición de los tipos de valores asociados a las reglas gramaticales. Esto permite que las reglas tengan un tipo de valor semántico retornado. */
%type <cte> expresion primaria termino

%%  // Inicia la sección de reglas gramaticales de Bison (el corazón del parser)

/* Regla inicial (la regla principal de la gramática, el punto de partida) */
programa:
       INICIO listaSentencias FIN   /* El programa comienza con INICIO y termina con FIN, conteniendo una lista de sentencias */
       {if (yynerrs || yylexerrs) YYABORT; return -1;}  // Si hay errores léxicos o sintácticos, aborta el parsing y devuelve -1 (error)
; 

/* Definición de la lista de sentencias. Puede ser una sola sentencia o varias */
listaSentencias:
       sentencia                    // La lista de sentencias puede consistir en una única sentencia
    |  listaSentencias sentencia     // O una lista de sentencias seguidas de otra sentencia
;

/* Definición de las diferentes sentencias que puede manejar el lenguaje */
sentencia:
       ID ASIGNACION expresion PUNTOYCOMA  // Asignación de una expresión a una variable (ID = expresión;)
       {EscribirATabla($1, $3);}  // Guarda el resultado de la expresión (valor semántico $3) en la tabla de símbolos asociado al ID ($1)
    |  LEER '(' listaIdentificadores ')' PUNTOYCOMA  // La sentencia de lectura (leer una o más variables desde la entrada)
    |  ESCRIBIR '(' listaExpresiones ')' PUNTOYCOMA  // La sentencia de escritura (imprimir una o más expresiones)
    |  IMPRIMIR expresion expresion VECES PUNTOYCOMA  // Imprimir la primera expresión un número de veces indicado por la segunda expresión
       {for(int i=0; i<$3; i++) printf("%d\n",$2);}  // Un bucle que imprime $2 (primera expresión) $3 (segunda expresión) veces
;

/* Definición de una lista de identificadores, separados por comas */
listaIdentificadores:
       ID  {cargarEntradas($1);}   // Carga el identificador $1 en la tabla de símbolos
    |  listaIdentificadores ',' ID  {cargarEntradas($3);}  // Añade otro identificador a la lista, manejando varios IDs separados por comas
;

/* Definición de una lista de expresiones, separadas por comas */
listaExpresiones:
       expresion  {printf("%d\n", $1);}  // Imprime el valor de una expresión
    |  listaExpresiones ',' expresion   {printf("%d\n", $3);}  // Imprime el valor de varias expresiones separadas por comas
;

/* Definición de una expresión aritmética */
expresion:
       termino          {$$ = $1;}  // Una expresión puede ser simplemente un término (número o variable)
    |  expresion '+' termino   {$$ = $1 + $3;}  // Una suma de una expresión y un término, el valor es la suma de ambos
    |  expresion '-' termino   {$$ = $1 - $3;}  // Una resta de una expresión y un término, el valor es la diferencia de ambos
;

/* Definición de un término, que puede ser una primaria o una multiplicación de términos */
termino:
        primaria           // Un término puede ser una primaria (una variable o constante)
    |   termino '*' primaria  {$$ = $1 * $3;}  // Un término puede ser el producto de otro término y una primaria
;

/* Definición de una primaria, que es la unidad básica de una expresión aritmética */
primaria:
       ID    {$$ = ValorSimbolo($1);}  // Una primaria puede ser un identificador (una variable), cuyo valor se busca en la tabla de símbolos
    |  CONSTANTE  {$$ = $1;}           // O una constante numérica, cuyo valor es simplemente el número en sí
    |  '(' expresion ')'  {$$ = $2;}   // O una expresión encerrada en paréntesis, que se evalúa de forma recursiva
;

%%  // Fin de la sección de reglas de Bison

/* Función para manejar errores sintácticos. Se llama automáticamente cuando Bison detecta un error en la entrada. */
void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);  // Muestra el mensaje de error en stderr
}

////// MAIN //////

int main(int argc, char** argv) {
    
    // Argumentos
    if (argc > 2){  // Si hay más de 2 argumentos (nombre del programa + archivo de entrada), es un error
        printf("Numero incorrecto de argumentos.");
        return EXIT_FAILURE;
    }
    else if (argc == 2) {  // Si hay exactamente 2 argumentos, el segundo debe ser el nombre del archivo de entrada
        char filename[50];  // Buffer para almacenar el nombre del archivo
        sprintf(filename, "%s", argv[1]);  // Copia el nombre del archivo en el buffer
        int largo = strlen(filename);  // Calcula la longitud del nombre del archivo

        // Verifica que el archivo tenga la extensión .m (nuestro lenguaje fuente). Si no, da error.
        if (argv[1][largo-1] != 'm' || argv[1][largo-2] != '.'){
            printf("Extension incorrecta (debe ser .m)");
            return EXIT_FAILURE;
        }

        yyin = fopen(filename, "r");  // Abre el archivo para lectura y lo asigna a yyin (entrada del parser)
    }
    else
        yyin = stdin;  // Si no hay archivo de entrada, usa la entrada estándar (stdin)

    init_TS();  // Inicializa la tabla de símbolos. Generalmente, esto puede establecer los valores iniciales de las variables en -1 o nulos.

    // Parser
    switch (yyparse()){  // Llama al parser para analizar el archivo o la entrada estándar
        case 0: printf("\n\nProceso de compilacion termino exitosamente");  // Si devuelve 0, la compilación fue exitosa
        break;
        case 1: printf("\n\nErrores de compilacion");  // Si devuelve 1, hubo errores de compilación
        break;
        case 2: printf("\n\nNo hay memoria suficiente");  // Si devuelve 2, hubo un error por falta de memoria
        break;
    }
    printf("\n\nErrores sintacticos: %i\tErrores lexicos: %i\n", yynerrs, yylexerrs);  // Muestra el número de errores léxicos y sintácticos

    return 0;  // Termina el programa
}