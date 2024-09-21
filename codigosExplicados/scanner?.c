%{
#include <stdio.h>   // Para operaciones de entrada y salida estándar (como printf)
#include <string.h>  // Para manipulación de cadenas, necesario para funciones como strdup
#include "moduloParser.tab.h"  // Incluye las definiciones generadas por Bison (tokens y tipos)

void yyerror(char *s);  // Prototipo de la función de manejo de errores
int yylex();  // Prototipo de la función de análisis léxico generada por Flex

extern int yylexerrs;  // Contador de errores léxicos, declarado como extern para compartirlo con el parser
char buffer[128];  // Buffer para almacenar mensajes de error. Incrementado a 128 caracteres para manejar errores más largos.
%}

/* Definición de patrones que serán utilizados en las reglas del scanner. Estos patrones están definidos usando expresiones regulares. */
digito                  [0-9]  // Un solo dígito (cualquier número entre 0 y 9)
letra                   [a-zA-Z]  // Una sola letra (mayúscula o minúscula)
identificador           {letra}({letra}|{digito})*  // Un identificador: empieza con una letra y puede contener letras y/o dígitos
constante               ({digito})+  // Una constante numérica: uno o más dígitos consecutivos

%%  // Inicia la sección de reglas de Flex

/* Reglas para palabras clave: si se encuentra alguna de estas cadenas exactas en el código fuente, se devuelve el token correspondiente */
"inicio"                return INICIO;  // Palabra clave "inicio" -> retorna el token INICIO
"fin"                   return FIN;  // Palabra clave "fin" -> retorna el token FIN
"leer"                  return LEER;  // Palabra clave "leer" -> retorna el token LEER
"escribir"              return ESCRIBIR;  // Palabra clave "escribir" -> retorna el token ESCRIBIR
"imprimir"              return IMPRIMIR;  // Palabra clave "imprimir" -> retorna el token IMPRIMIR
"veces"                 return VECES;  // Palabra clave "veces" -> retorna el token VECES

/* Reglas para identificadores y constantes: */
/* Si el patrón coincide con un identificador (letra seguida de letras o dígitos), lo almacena en yylval.id */
{identificador}         {yylval.id = strdup(yytext); return ID;}  // Duplica el identificador (yytext) y lo guarda en yylval.id para uso en el parser
/* Si el patrón coincide con una constante numérica (uno o más dígitos), lo convierte a entero y lo guarda en yylval.cte */
{constante}             {yylval.cte = atoi(yytext); return CONSTANTE;}  // Convierte la constante numérica a entero y lo guarda en yylval.cte

/* Reglas para símbolos individuales (paréntesis, punto y coma, operadores, etc.): */
"("                     return '(';  // Paréntesis izquierdo
")"                     return ')';  // Paréntesis derecho
";"                     return  PUNTOYCOMA;  // Punto y coma
"+"                     return '+';  // Operador de suma
"-"                     return '-';  // Operador de resta
"*"                     return '*';  // Operador de multiplicación
","                     return ',';  // Coma
":="                    return ASIGNACION;  // Operador de asignación

/* Ignorar espacios en blanco, tabulaciones y saltos de línea: */
/* Estos caracteres no son relevantes para el análisis léxico, por lo que se descartan silenciosamente */
[ \t\n]                 ;  // Espacios en blanco, tabulaciones o saltos de línea no generan tokens

/* Manejo de cualquier otro carácter no reconocido: */
/* Si se encuentra un carácter no reconocido, se incrementa el contador de errores léxicos y se genera un mensaje de error */
.                       {yylexerrs++; sprintf(buffer,"Error Lexico: %s es un caracter invalido", yytext); yyerror(buffer);}

%%  // Fin de la sección de reglas de Flex

/* Función que Flex llama cuando llega al final del archivo (EOF). En este caso, simplemente retorna 1 para indicar que el archivo terminó. */
int yywrap(void) {
    return 1;  // Retorna 1 para indicar que no hay más datos por procesar
}