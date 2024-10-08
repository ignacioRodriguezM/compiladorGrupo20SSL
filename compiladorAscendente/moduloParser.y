%{
int yylex();

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tablaDeSimbolos.h" // Todas las funciones de la TS

void yyerror(char *s);

extern int yynerrs;
int yylexerrs = 0;  // Definir aquí
extern FILE* yyin;

%}

%token INICIO FIN LEER ESCRIBIR PUNTOYCOMA IMPRIMIR VECES
%token <id> ID
%token <cte> CONSTANTE
%union {
    char* id;
    int cte;
}
%left '+' '-' ',' '*'
%right ASIGNACION

%type <cte> expresion primaria termino

%%

programa:
       INICIO listaSentencias FIN                       {if (yynerrs || yylexerrs) YYABORT; return -1;}
; 

listaSentencias:
       sentencia
    |  listaSentencias sentencia
;

sentencia:
       ID ASIGNACION expresion PUNTOYCOMA               {EscribirATabla($1, $3);}         
    |  LEER '(' listaIdentificadores ')' PUNTOYCOMA     
    |  ESCRIBIR '(' listaExpresiones ')' PUNTOYCOMA
    |  IMPRIMIR expresion expresion VECES PUNTOYCOMA    {for(int i=0; i<$3; i++) printf("%d\n",$2);}
;

listaIdentificadores:
       ID                               {cargarEntradas($1);}                                                   
    |  listaIdentificadores ',' ID      {cargarEntradas($3);}
;

listaExpresiones:
       expresion                        {printf("%d\n", $1);}
    |  listaExpresiones ',' expresion   {printf("%d\n", $3);}
;

expresion:
       termino                          {$$ = $1;}
    |  expresion '+' termino            {$$ = $1 + $3;}
    |  expresion '-' termino            {$$ = $1 - $3;}                    
;

termino:
        primaria
    |   termino '*' primaria            {$$ = $1 * $3;}
;

primaria:
       ID                               {$$ = ValorSimbolo($1);}
    |  CONSTANTE                        {$$ = $1;}
    |  '(' expresion ')'                {$$ = $2;}
;

%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

////// MAIN //////

int main(int argc, char** argv) {
    
    // Argumentos
    if (argc > 2){
        printf("Numero incorrecto de argumentos.");
        return EXIT_FAILURE;
    }
    else if (argc == 2) {
        char filename[50];                  // Nombre del archivo
        sprintf(filename, "%s", argv[1]);   // El 2do argumento
        int largo = strlen(filename);       // Largo del nombre del archivo

        // Si no termina en .m dar error
        if (argv[1][largo-1] != 'm' || argv[1][largo-2] != '.'){
            printf("Extension incorrecta (debe ser .m)");
            return EXIT_FAILURE;
        }

        yyin = fopen(filename, "r");
    }
    else
        yyin = stdin;

    init_TS(); // Inicializa la tabla con todo en -1

    // Parser
    switch (yyparse()){
        case 0: printf("\n\nProceso de compilacion termino exitosamente");
        break;
        case 1: printf("\n\nErrores de compilacion");
        break;
        case 2: printf("\n\nNo hay memoria suficiente");
        break;
    }
    printf("\n\nErrores sintacticos: %i\tErrores lexicos: %i\n", yynerrs, yylexerrs);

    return 0;
}
