//*************LIBRERIAS******************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

//***************STRUCTS*****************
typedef struct { //ESTRUCTURA DE LOS COMANDOS
    char comando [100];
} Comando;

typedef struct {
    char part_status; //indica si esta activa o no
    char part_type; //indica si es primaria o extendida [P o E]
    char part_fit; //Tipo de ajuste de la particion [B(best),F(first),W(worst)]
    int part_start; //indica en que byte del disco inicia la particion
    int part_size; //Contiene el tamaño total de la particion
    char part_name[16]; //Nombre de la particion


} struct_particion;

typedef struct {
    char mbr_fecha_creacion[16]; //Fecha y hora de creación del disco
    int mbr_tamano; //tamaño total del disco en bytes
    int mbr_disk_signature; //numero random que identifica de forma unica cada disco
    struct_particion mbr_particiones[4]; //info de la particion 1
} struct_mbr; //TAMAÑO 136 BYTES

typedef struct {
    char part_status; //Inidica si la particion esta activa o no
    char part_fit; //Tipo de ajuste de la particion [B(best),F(first),W(worst)]
    int part_start; //Indica en que byte del disco inicia la particion
    int part_size; //Contiene el tamaño total de la particion en bytes
    int part_next; //byte que esta el proximo EBR, -1 si no hay
    char part_name[16]; //Nombre de la particion
} struct_ebr; //TAMAÑO 32 BYTES
//**************VARS GLOBALES************
Comando Comandos[20];

//********************PROTOTIPOS***********************
void Mkdisk(Comando comando[]); //METODO PARA CREAR DISCOS
void VerificarPath(char Path[]); //Verificar si una path existe o sino crearla
void CrearDisco(char cadena[], char unit[],char name[], int size); //Metodo donde se crea el disco
void Rmdisk(Comando comando[]); //METODO PARA ELIMINAR DISCOS

//PROTOTIPOS AUXILIARES
int ContarSlash(char path[]);
void cambio(char aux[]);
void limpiarvar(char aux[], int n);
char** SplitComando(char* texto, const char caracter_delimitador);
int ContarSlash(char* path);

int main()
{
    printf("PROYECTO MANEJO E IMPLEMENTACION DE ARCHIVOS \n");
    printf("SECCION A+\n");
    printf("201314412\n\n");
    printf("CONSOLA DE COMANDOS\n\n");
    int salida = 0;

    while(salida != 1){
        int i;
        for (i = 0; i < 25; i++) {
            strcpy(Comandos[i].comando, "VACIO");
        }
        char comando[200]; //variable donde se ira guardando cada linea de comando
        limpiarvar(comando, 200);
        char comando2[200]; //variable donde se ira guardando cada linea de comando
        limpiarvar(comando2, 200);
        printf("Juande@juande: ");
        fgets(comando, 200, stdin);
        cambio(comando);
        //printf("El comando es: %s \n", comando);
        char** tokens;
        char * completo;
        char** variables;
        char * completo2;
        char** ssplit; //segundo split con :
        int count = 0;
        tokens = SplitComando(comando, ' '); //separando el comando por espacios en blanco
        if(tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                char * valor = *(tokens + i);
                if(valor[0] == '\\'){
                    fgets(comando2, 200, stdin);
                    cambio(comando2);
                    strcat(comando, " ");
                    strcat(comando, comando2);
                    free(tokens);
                }else{
                    if(i!=0){
                        strcat(comando, " ");
                        strcat(comando, valor);
                    }
                }
            }
        }
        //printf("El comando es: %s \n", comando);
        tokens = SplitComando(comando, ' '); //separando el comando por espacios en blanco
        if (tokens) {
            int i;
            for (i = 0; *(tokens + i); i++){
                completo = *(tokens + i);
                variables = SplitComando(completo, ':');
                if (variables) {
                    int j;
                    for (j = 0; *(variables + j); j++) {
                        completo2 = *(variables + j);
                        ssplit = SplitComando(completo2, ':');
                        if(ssplit){
                            int k;
                            for (k = 0; *(ssplit + k); k++) {
                                char * valor = *(ssplit + k);
                                strcpy(comando,valor);
                                strcpy(Comandos[count].comando,valor);
                                count++;
                            }
                            free(ssplit);
                        }
                    }
                    free(variables);
                }
            }
            free(tokens);
        }
//        int re;
//        for (re = 0; re < 25; re++) {
//            printf("Comando %i : %s \n",re,Comandos[re].comando);
//        }
        printf("\n");
        if (strcasecmp(Comandos[0].comando, "mkdisk") == 0) {
            printf("CREACION DE DISCO DURO \n");
            Mkdisk(Comandos);
        } else if (strcasecmp(Comandos[0].comando, "rmdisk") == 0) {
            printf("ELIMINACION DE DISCO DURO \n");
            Rmdisk(Comandos);
        } else if (strcasecmp(Comandos[0].comando, "fdisk") == 0) {
            printf("EDICION DE PARTICIONES \n");
            //Fdisk(Comandos);
        } else if (strcasecmp(Comandos[0].comando, "mount") == 0) {
            printf("MONTAR PARTICION \n");
            //Mount(Comandos);
        } else if (strcasecmp(Comandos[0].comando, "unmount") == 0) {
            printf("DESMONTAR PARTICION \n");
            //Unmount(Comandos);
        } else if (strcasecmp(Comandos[0].comando, "clear") == 0) {
            system("clear");
            printf("PROYECTO MANEJO E IMPLEMENTACION DE ARCHIVOS \n");
            printf("SECCION A\n");
            printf("201314412\n\n");
            printf("CONSOLA DE COMANDOS\n\n");
        } else if (strcasecmp(Comandos[0].comando, "exit") == 0) {
            salida = 1;
        } else if (Comandos[0].comando[0] == '#') {
            printf("Comentario\n\n");
        } else {
            printf("COMANDO INVALIDO \n\n");
        }
    }
    return 0;
}

void Mkdisk(Comando comando[]){ //METODO PARA CREAR DISCOS
    srand(time(NULL)); //semilla para el numero random
    //variables para almacenar datos
    int size;
    char unit[2] = "m";
    char name[50];
    limpiarvar(name,50);
    char path[200];
    limpiarvar(path, 200);
    char cadena[200] = ""; //path para usar en el fopen
    limpiarvar(cadena, 200);
    //booleanos para verificar existencia
    bool existesize = false;
    bool existepath = false;
    bool existename = false;
    bool error = false;
    int i = 1;
    while (strcasecmp(comando[i].comando, "vacio") != 0){
        if (strcasecmp(comando[i].comando, "-size") == 0){
            i++;
            size = atoi(comando[i].comando);
            if (size >= 0) {
                existesize = true;
            } else {
                printf("Tamaño para el disco incorrecto debe ser mayor a cero \n");
                error = true;
            }
        } else if (strcasecmp(comando[i].comando, "+unit") == 0) {
            i++;
            strcpy(unit, comando[i].comando);
            if (strcasecmp(unit, "m") != 0 && strcasecmp(unit, "k") != 0) {
                printf("Parametro de unidad incorrecto [m|k] \n");
                char unit[2] = "m";
                error = true;
            }
        } else if (strcasecmp(comando[i].comando, "-path") == 0) {
            i++;
            strcpy(path, comando[i].comando);
            if (path[0] == '\"') {
                int a;
                for (a = i + 1; a < 25; a++) {
                    if (strcasecmp(comando[a].comando, "-size") != 0 && strcasecmp(comando[a].comando, "+unit") != 0 && strcasecmp(comando[a].comando, "-name") != 0 && strcasecmp(comando[a].comando, "vacio") != 0 && strcasecmp(comando[a].comando, "\\") != 0) {
                        strcat(path, " ");
                        strcat(path, comando[a].comando);
                    } else {
                        i = a - 1;
                        a = 25;

                    }
                }

            }
            existepath = true;
        } else if (strcasecmp(comando[i].comando, "-name") == 0) {
            i++;
            strcpy(name, comando[i].comando);
            if (name[0] == '\"') {
                int a;
                for (a = i + 1; a < 25; a++) {
                    if (strcasecmp(comando[a].comando, "-size") != 0 && strcasecmp(comando[a].comando, "+unit") != 0 && strcasecmp(comando[a].comando, "-path") != 0 && strcasecmp(comando[a].comando, "vacio") != 0 && strcasecmp(comando[a].comando, "\\") != 0) {
                        strcat(name, " ");
                        strcat(name, comando[a].comando);
                    } else {
                        i = a - 1;
                        a = 25;
                    }
                }

            }
            existename = true;
        } else {
            printf("Atributo no valido en el comando \n");
            error = true;
        }
        i++;
    }

    if (existesize == false || existepath == false || existename == false) {
        printf("FALTAN PARAMETROS PARA COMPLETAR EL PROCESO DE CREACION DE DISCOS (SIZE|PATH|NAME) \n\n");
    }else{
        if (error == false) {
            VerificarPath(path);
            CrearDisco(path, unit,name, size);
        } else {
            printf("EL COMANDO CONTIENE ERRORES \n");
        }
    }
}

void VerificarPath(char Path[]){
    char cmd[] = "mkdir";
    char path[200];
    limpiarvar(path, 200);
    int slash = ContarSlash(Path);
    strcpy(path, Path);
    char** carpetas;
    char auxiliar[200] = "";
    char pathauxiliar[200];
    limpiarvar(pathauxiliar,200);
    if (path[0] == '\"'){ //La path trae espacios en blanco
        int esp;
        int pos = 0;
        for(esp = 1; esp<200;esp++){
            if(path[esp] == '\"'){
                esp = 200;
            }else{
                pathauxiliar[pos] = path[esp];
                pos++;
            }
        }
        carpetas = SplitComando(pathauxiliar, '/'); //separar la path por cada directorio
        if (carpetas) //si la lista no esta vacia
        {
            int i;
            //for (i = 0; *(carpetas + i) ; i++)
            for (i = 0; i < slash; i++) {
                strcat(auxiliar, "/");
                strcat(auxiliar, *(carpetas + i));
                //printf("%s \n",auxiliar);
                if (mkdir(auxiliar, S_IRWXU) == 0) {
                    printf("Directorio inexistente, se ha creado la carpeta: %s \n", auxiliar);
                }
            }
            free(*(carpetas + i));
        }
        free(carpetas);
    } else {
        carpetas = SplitComando(path, '/'); //separar el comando introducido por slash
        if (carpetas) //si la lista no esta vacia
        {
            int i;
            //for (i = 0; *(carpetas + i) ; i++)
            for (i = 0; i < slash; i++) {
                strcat(auxiliar, "/");
                strcat(auxiliar, *(carpetas + i));
                //printf("%s \n",auxiliar);
                if (mkdir(auxiliar, S_IRWXU) == 0) {
                    printf("Directorio inexistente, se ha creado la carpeta: %s \n", auxiliar);
                }

            }
            free(*(carpetas + i));
            //printf("\n");
        }
        free(carpetas);
    }
}

void CrearDisco(char cadena[], char unit[],char name[], int size){
    char auxcadena[200];
    limpiarvar(auxcadena, 200);
    char auxnombre[50];
    limpiarvar(auxnombre,50);
    if (name[0] == '\"') {
        int x;
        int y = 0;
        for (x = 1; x < 50; x++) {
            if (name[x] == '\"') {
                x = 50;
            } else {
                auxnombre[y] = name[x];
                y++;
            }
        }
    } else {
        strcpy(auxnombre, name);
    }
    if (cadena[0] == '\"') {
        int x;
        int y = 0;
        for (x = 1; x < 200; x++) {
            if (cadena[x] == '\"') {
                x = 200;
            } else {
                auxcadena[y] = cadena[x];
                y++;
            }
        }
    } else {
        strcpy(auxcadena, cadena);
    }
    strcpy(cadena, auxcadena);
    strcat(cadena,"/");
    strcat(cadena,auxnombre);
    int espaciorestante; //Variable para obtener el espacio restante en el disco despues de escribir el mbr
    int cont;
    int tamanodeldisco = size;
    FILE *disco;
    struct_mbr mbr;
    struct_particion particion;
    particion.part_status = '0';
    particion.part_type = 'N';
    particion.part_fit = 'N';
    particion.part_start = 0;
    particion.part_size = 0;
    strcpy(particion.part_name, "\0");
    for (cont = 0; cont < 4; cont++) {
        mbr.mbr_particiones[cont] = particion;
    }
    int tamanombr = sizeof (struct_mbr); //el mbr ocupa 136bytes
    mbr.mbr_disk_signature = (rand() % 10);
    time_t hora = time(0);
    struct tm *tlocal = localtime(&hora);
    strftime(mbr.mbr_fecha_creacion, 16, "%d/%m/%y %H:%S", tlocal);
    disco = fopen(cadena, "rb+");
    if (disco == NULL) {
        if (strcasecmp(unit, "m") == 0) {
            espaciorestante = (size * 1024 * 1024) - sizeof (mbr);
            tamanodeldisco = tamanodeldisco * 1024 * 1024;
            /*for(cont = 0; cont<(size*1024*1024);cont++){
                fwrite("\0",sizeof(char),1,disco);
            }*/
        } else if (strcasecmp(unit, "k") == 0) {
            espaciorestante = (size * 1024) - sizeof (mbr);
            tamanodeldisco = tamanodeldisco * 1024;
            /*for(cont = 0; cont<(size*1024);cont++){
                fwrite("\0",sizeof(char),1,disco);
            }*/
        }
        disco = fopen(cadena, "wb");
        mbr.mbr_tamano = tamanodeldisco;
        fwrite(&mbr, sizeof (mbr), 1, disco);
        fseek(disco, sizeof (mbr) + 1, SEEK_SET);
        char relleno = '\0'; //variable con la que se va a rellenar el disco duro
        for (cont = 0; cont < espaciorestante - 1; cont++) {
            fwrite(&relleno, 1, 1, disco);
        }
        rewind(disco); //Regresa el puntero al inicio del archivo
        printf("Se creo un disco \n");
        printf("Tamaño: %i bytes \n", tamanodeldisco);
        printf("Ruta: %s \n\n", cadena);
    }else{
        printf("El disco ya existe\n");
    }
    fclose(disco);
}

void Rmdisk(Comando comando[]){ //METODO PARA ELIMINAR DISCOS
    char resp[5]; //variable que almacena
    limpiarvar(resp, 5);
    char path[200];
    limpiarvar(path, 200);
    char pathaux[200];
    limpiarvar(pathaux, 200);
    int cont = 0;
    //bool para verificar si existe parametro obligatorio
    bool existepath = false;
    while (strcasecmp(comando[cont].comando, "vacio") != 0) {
        if (strcasecmp(comando[cont].comando, "-path") == 0) {
            strcpy(pathaux, comando[cont + 1].comando);
            if (pathaux[0] == '\"') {
                int a;
                for (a = cont + 2; a < 25; a++) {
                    if (strcasecmp(comando[a].comando, "-size") != 0 && strcasecmp(comando[a].comando, "-unit") != 0 && strcasecmp(comando[a].comando, "vacio") != 0) {
                        strcat(pathaux, " ");
                        strcat(pathaux, comando[a].comando);
                    } else {
                        a = 25;
                    }
                }
            }
            existepath = true;
        }
        cont++;
    }
    if (pathaux[0] == '\"') {
        int x;
        int y = 0;
        for (x = 1; x < 200; x++) {
            if (pathaux[x] == '\"') {
                x = 200;
            } else {
                path[y] = pathaux[x];
                y++;
            }
        }
    } else {
        strcpy(path, pathaux);
    }

if (existepath == true) {
        FILE *archivo;
        archivo = fopen(path, "rb");
        if (archivo == NULL) {
            printf("El disco no existe \n\n");
        } else {
            printf("Esta seguro que desea eliminar esta unidad: [Y/N] \n");
            fgets(resp, 5, stdin);
            cambio(resp);
            if (strcasecmp(resp, "y") == 0) {
                if (remove(path) == 0) {
                    printf("El disco ha sido eliminado de forma exitosa \n\n");
                } else {
                    printf("Error al tratar de eliminar disco \n\n");
                }
            } else {

                printf("Eliminacion cancelada \n\n");
            }
            fclose(archivo);
        }
    } else {
        printf("Error, parametros faltantes [path] o incorrectos \n\n ");
    }
}

int ContarSlash(char path[]) {
    char *tampath = "";
    int cantidad = 0;
    while (*path) {
        if (*path == '/') {

            cantidad++;
        }
        path++;
    }
    return cantidad;
}

void cambio(char aux[]) {//METODO PARA ELIMINAR EL SALTO DE LINEA EN LOS COMANDOS
    int i, temp = 0;
    for (i = 0; i < 200 && temp == 0; i++) {
        if (aux[i] == '\n' || aux[i] == '\r') {
            aux[i] = '\0';
            temp = 1;
        }
    }
}

void limpiarvar(char aux[], int n){ //METODO PARA LIMPIAR LOS CHAR[] PONIENDOLES '\0'
    int i;
    for (i = 0; i < n; i++) {

        aux[i] = '\0';
    }
}

char** SplitComando(char* texto, const char caracter_delimitador){ //METODO QUE REALIZA UN SPLIT EN BASE AL DELIMITADOR QUE RECIBE

int ContarSlash(char* path){
    char *tampath = "";
    int cantidad = 0;
    while (*path) {
        if (*path == '/') {

            cantidad++;
        }
        path++;
    }
    return cantidad;
}
    char** cadena = 0;
    size_t contador = 0;
    char* temp = texto;
    char* apariciones = 0;
    char delimitador[2];
    delimitador[0] = caracter_delimitador;
    delimitador[1] = 0;

    while (*temp) {
        if (caracter_delimitador == *temp) {
            contador++;
            apariciones = temp;
        }
        temp++;
    }
    contador += apariciones < (texto + strlen(texto) - 1);
    contador++;
    cadena = malloc(sizeof (char*) * contador);
    if (cadena) {
        size_t idx = 0;
        char* token = strtok(texto, delimitador);
        while (token){
            if (idx < contador)
                *(cadena + idx++) = strdup(token);
            token = strtok(0, delimitador);
        }
        if (idx == contador - 1)
            *(cadena + idx) = 0;
    }
    return cadena;
}
