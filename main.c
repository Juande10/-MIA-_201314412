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
void Fdisk(Comando comando[]); //METODO PARA MODIFICAR PARTICIONES
void CrearParticion(char path[], int size, char unit[], char type[], char fit[], char Name[]); //METODO DONDE SE CREAN LAS PARTICIONES

//PROTOTIPOS AUXILIARES
int ContarSlash(char path[]);
void cambio(char aux[]);
void limpiarvar(char aux[], int n);
char** SplitComando(char* texto, const char caracter_delimitador);
int ContarSlash(char *path);
int CantidadParticionesPrimarias(struct_mbr mbr);
int CantidadParticionesExtendidas(struct_mbr mbr);
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
        char** tokens, tokens2;
        char * completo;
        char** variables;
        char * completo2;
        char** ssplit; //segundo split con :
        int count = 0;
        bool multi = false;
        tokens = SplitComando(comando, ' '); //separando el comando por espacios en blanco
        if(tokens){
            int i;
            for (i = 0; *(tokens + i); i++){
                char * valor = *(tokens + i);
                if(valor[0] == '\\'){
                    multi = true;
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
    char cmd[200] = "mkdir -m 777 ";
    char path[200];
    limpiarvar(path, 200);
    int slash = ContarSlash(Path);
    strcpy(path, Path);
    char** carpetas;
    char auxiliar[200] = "";
    char pathauxiliar[200];
    limpiarvar(pathauxiliar,200);
    DIR *dirp;

    if (path[0] == '\"'){ //La path trae espacios en blanco
        char com[200];
        limpiarvar(com,200);

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
                limpiarvar(com,200);
                limpiarvar(cmd,200);
                com[0] = '\"';
                strcpy(cmd,"mkdir -m 777 ");
                strcat(auxiliar, "/");
                strcat(auxiliar, *(carpetas + i));
                strcat(com,auxiliar);
                strcat(com, "\"");

                strcat(cmd,com);
                if((dirp = opendir(auxiliar))== NULL){
                    system(cmd);
                    //printf("No existe carpeta \n");
                }
                //printf("%s \n",auxiliar);
                //if (mkdir(auxiliar, S_IRWXU) == 0) {
                  //  printf("Directorio inexistente, se ha creado la carpeta: %s \n", auxiliar);
                //}
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
            for (i = 0; i < slash ; i++) {
                strcat(auxiliar, "/");
                strcat(auxiliar, *(carpetas + i));
                //printf("%s \n",auxiliar);
                //if (mkdir(auxiliar, S_IRWXU) == 0) {
                  //  printf("Directorio inexistente, se ha creado la carpeta: %s \n", auxiliar);
                //}

                strcat(cmd, "/");
                strcat(cmd, *(carpetas + i));
                if((dirp = opendir(auxiliar))== NULL){
                    system(cmd);
                    //printf("No existe carpeta \n");
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

void Fdisk(Comando comando[]) {
    //variables para almacenar los valores
    int size; //tamaño de la particion ingresada por usuario
    char unit[2] = "k"; //unidad en la que se tomara el tamaño [b|k|m]
    char pathaux[200]; // path auxiliar que se usara por si la path trae comillas
    limpiarvar(pathaux, 200);
    char path[200]; // path del disco donde se va a crear la particion
    limpiarvar(path, 200);
    char type[2] = "p"; //tipo de particion [primaria o extendida]
    char fit[3] = "W"; //tipo de ajustes
    char delete[6]; // Se eliminara una particion ya sea Fast (solo se deja inactiva) o FULL (se borra todo y se deja con \0)
    limpiarvar(delete, 6);
    char name[16]; //Nombre de la particion, no se debe repetir
    limpiarvar(name, 16);
    int add; //Para modificar el tamaño de la particion, si es negativo se le quita y si es positivo se le agrega

    //booleanos para saber que existen los parametros
    bool existesize = false;
    bool existeunit = false;
    bool existepath = false;
    bool existetype = false;
    bool existefit = false;
    bool existedelete = false;
    bool existename = false;
    bool existeadd = false;

    bool errors = false; //Variable para saber si existe algun error en el comando
    int cont = 0; //contador para recorrer el vector de comandos
    while (strcasecmp(comando[cont].comando, "vacio") != 0) {
        if (strcasecmp(comando[cont].comando, "-size") == 0) {
            size = atoi(comando[cont + 1].comando);
            if (size > 0) {
                existesize = true;
            } else {
                printf("El parametro \"Size\" debe ser mayor a 0\n");
                errors = true; //existe un error
            }
        } else if (strcasecmp(comando[cont].comando, "+unit") == 0) {
            strcpy(unit, comando[cont + 1].comando);
            if (strcasecmp(unit, "b") != 0 && strcasecmp(unit, "k") != 0 && strcasecmp(unit, "m") != 0) {
                printf("Parametro incorrecto unit debe ser [b|k|m] \n");
                errors = true; //existe un error
            } else {
                existeunit = true;
            }
        } else if (strcasecmp(comando[cont].comando, "-path") == 0) {
            strcpy(pathaux, comando[cont + 1].comando);
            if (pathaux[0] == '\"') {
                int a;
                for (a = cont + 2; a < 25; a++) {
                    if (strcasecmp(comando[a].comando, "-size") != 0 && strcasecmp(comando[a].comando, "+unit") != 0 && strcasecmp(comando[a].comando, "-path") != 0 && strcasecmp(comando[a].comando, "+type") != 0 && strcasecmp(comando[a].comando, "+fit") != 0 && strcasecmp(comando[a].comando, "+delete") != 0 && strcasecmp(comando[a].comando, "-name") != 0 && strcasecmp(comando[a].comando, "+add") != 0 && strcasecmp(comando[a].comando, "vacio") != 0) {
                        strcat(pathaux, " ");
                        strcat(pathaux, comando[a].comando);
                    } else {
                        a = 25;
                    }
                }
            }
            if (pathaux[0] == '\"') { //Quitando las comillas de la path
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

            FILE *disco;
            disco = fopen(path, "rb");
            if (disco == NULL) { //Se comprueba que el archivo exista
                printf("Este disco no existe \n");
                errors = true; //hay un error
            } else {
                existepath = true; //la path si existe
            }
        } else if (strcasecmp(comando[cont].comando, "+type") == 0) {
            strcpy(type, comando[cont + 1].comando);
            if (strcasecmp(type, "P") != 0 && strcasecmp(type, "E") != 0 && strcasecmp(type, "L") != 0) {
                printf("Parametro incorrecto type debe ser [P|E|L] \n");
                errors = true; //existe error
            } else {
                existetype = true;
            }
        } else if (strcasecmp(comando[cont].comando, "+fit") == 0) {
            strcpy(fit, comando[cont + 1].comando);
            if (strcasecmp(fit, "BF") != 0 && strcasecmp(fit, "FF") != 0 && strcasecmp(fit, "WF") != 0) {
                printf("Parametro incorrecto type debe ser [B|F|W] \n");
                errors = true; //existe error
            } else {
                existefit = true;
            }
        } else if (strcasecmp(comando[cont].comando, "+delete") == 0) {
            strcpy(delete, comando[cont + 1].comando);
            if (strcasecmp(delete, "fast") != 0 && strcasecmp(delete, "full") != 0) {
                printf("Parametro incorrecto delete debe ser [FAST|FULL] \n");
                errors = true;
            } else {
                existedelete = true;
            }
        } else if (strcasecmp(comando[cont].comando, "-name") == 0) {
            strcpy(name, comando[cont + 1].comando);
            existename = true;
        } else if (strcasecmp(comando[cont].comando, "-add") == 0) {
            add = atoi(comando[cont + 1].comando);
            if (add > 0) {
                printf("Se agregara espacio \n");
            } else {
                printf("Se quitara espacio \n");
            }
            existeadd = true;
        }
        cont++;
    }

    if (errors == true){
        printf("Imposible de ejecutar, existen errores en el script \n\n");
    }else{
        if (existepath == false || existename == false) { //SI la path o el name no vienen
            printf("Faltan parametros obligatorios [Path o Name] \n\n");
            errors = true;
        } else {
            /***************************************/
            /*MODULO DE CREACION DE UNA PARTICION*/
            /***************************************/
            if (existesize == true) { //Si va a crear una particion
                if (errors == true) {
                    //printf("Existen errores \n");
                } else {
                    printf("Modulo de creacion de particiones \n");
                    CrearParticion(path, size, unit, type, fit, name); //Creara la particion en este disco
                }
            }
            /***************************************/
            /*MODULO DE ELIMINACION DE UNA PARTICION*/
            /***************************************/
            else if (existedelete == true) {
                if (errors == true) {
                    //printf("Existen errores \n");
                } else {
                    printf("Modulo de eliminacion de particiones \n");
                    FILE *disco;
                    struct_mbr mbr;
                    disco = fopen(path, "rb");
                    if (disco != NULL) {
                        fread(&mbr, sizeof (mbr), 1, disco);
                        printf("Fue creado: %s \n", mbr.mbr_fecha_creacion);
                        //EliminarParticion(path, delete, name);
                    } else {
                        printf("No existe el disco \n");
                    }
                }
            }
            /***************************************/
            /*MODULO DE AMPLIACION DE UNA PARTICION*/
            /***************************************/
            else if (existeadd == true) { //Si va a agregar espacio a una particion
                if (errors == true) {
                    //printf("Existen errores \n");
                } else {
                    printf("Modulo de modificacion de particiones \n");
                    FILE *disco;
                    struct_mbr mbr;
                    disco = fopen(path, "rb");
                    if (disco != NULL) {
                        fread(&mbr, sizeof (mbr), 1, disco);
                        printf("Fue creado: %s \n", mbr.mbr_fecha_creacion);
                        //ModificarParticion(path, unit, add, name);
                    } else {
                        printf("No existe el disco \n");
                    }
                }
            }

        }
    }
}

void CrearParticion(char path[], int size, char unit[], char type[], char fit[], char Name[]) {
    char name[100];
    limpiarvar(name, 100);
    strcpy(name, Name);
    char auxfit, auxtype; //variables para almacenar el fit y el type n forma de char
    int tamanoreal; //Variable para tamaño real de la particion
    int cantPrimarias; //numero de particiones primarias
    int cantExtendidas; //numero de particiones extendidas
    bool error = false; //booleano para verificar si existe algun error en el script
    /**************************/
    /*CONVERSION DE VARIABLES*/
    /*************************/
    if (strcasecmp(fit, "BF") == 0 || strcasecmp(fit, "B") == 0) {
        auxfit = 'B'; //Mejor ajuste
    } else if (strcasecmp(fit, "FF") == 0 || strcasecmp(fit, "F") == 0) {
        auxfit = 'F'; //Primer ajuste
    } else if (strcasecmp(fit, "WF") == 0 || strcasecmp(fit, "W") == 0) {
        auxfit = 'W'; //Peor ajuste
    }
    if (strcasecmp(unit, "b") == 0) {
        tamanoreal = size;
    } else if (strcasecmp(unit, "k") == 0) {
        tamanoreal = size * 1024;
    } else if (strcasecmp(unit, "m") == 0) {
        tamanoreal = size * 1024 * 1024;
    }

    /*********************************************/
    /*COMIENZAN VALIDACIONES PARA CREAR PARTICION*/
    /*********************************************/
    FILE *disco;
    FILE *nuevodisco; //el que se va a usar para sobreescribir a este [disco]
    struct_mbr mbr;
    disco = fopen(path, "rb");
    if (disco != NULL) {
        fread(&mbr, sizeof (mbr), 1, disco); //recuperando el mbr de este disco
        cantPrimarias = CantidadParticionesPrimarias(mbr);
        cantExtendidas = CantidadParticionesExtendidas(mbr);
        int i; //variable para recorrer el vector de particiones en el mbr

        /****************************************/
        /*CREANDO PARTICION QUE SE VA A INSERTAR*/
        /****************************************/
        struct_particion particion; //Particion que va a ser creada dentro del mbr
        particion.part_fit = auxfit; //Asignandole el fit
        strcpy(particion.part_name, name); //Asignandole el nombre
        particion.part_size = tamanoreal; //Asignandole el tamaño
        particion.part_status = '1'; //Estado de ocupado

        int totalparticiones = cantExtendidas + cantPrimarias;
        struct_particion vecparticiones[totalparticiones]; //vector para almacenar las particiones que existen actualmente en el disco
        int j, p, cont = 0;
        int espaciolibre; //variable que se utiliza para ir verificando si la partcion cabe en ese espacio
        if (totalparticiones > 0) {
            for (j = 0; j < 4; j++) { //Verificando que particiones existen y almacenandolas en un vector
                if (mbr.mbr_particiones[j].part_start > 0 && mbr.mbr_particiones[j].part_status != '0') {
                    vecparticiones[cont] = mbr.mbr_particiones[j];
                    cont++;
                }
            }
        }
        /**************************************/
        /*ORDENANDO EL VECTOR POR BURBUJA******/
        /*************************************/
        if (totalparticiones > 0) {
            for (j = 0; j < totalparticiones; j++) {
                for (p = 0; p < totalparticiones - 1; p++) {
                    if (vecparticiones[p].part_start > vecparticiones[p + 1].part_start) {
                        struct_particion aux = vecparticiones[p];
                        vecparticiones[p] = vecparticiones[p + 1];
                        vecparticiones[p + 1] = aux;
                    }
                }
            }
        }

        /******************************************/
        /**********PARA CREAR UNA PRIMARIA*********/
        /******************************************/
        if (strcasecmp(type, "p") == 0) {
            bool errores = false; //booleano para verificar que no existe un error antes de meterla en el mbr
            int tamanodisponible;
            auxtype = 'p'; //Particion primaria
            particion.part_type = auxtype;
            if (cantPrimarias < 3) { //Hay menos de 3 particiones primarias
                if (cantPrimarias == 0 && cantExtendidas == 0) { //El disco esta vacio
                    tamanodisponible = mbr.mbr_tamano - (sizeof (mbr) + 1);
                    bool EntraParticion = false;
                    for (i = 0; i < 3 && EntraParticion == false; i++) {
                        if (mbr.mbr_particiones[i].part_status == '0') {
                            particion.part_start = sizeof (mbr) + 1;
                            particion.part_size = tamanoreal;
                            if (tamanodisponible > tamanoreal) {
                                //printf("Si se insertara la particion \n");
                                EntraParticion = true;
                                mbr.mbr_particiones[i] = particion;
                                /*SOBREESCRIBIENDO EL DISCO*/
                                int cont;
                                nuevodisco = fopen(path, "rb+");
                                fseek(nuevodisco, 0L, SEEK_SET);
                                fwrite(&mbr, sizeof (struct_mbr), 1, nuevodisco);
                                char relleno = '\0'; //variable con la que se va a rellenar el disco duro
                                /*for(cont = 0; cont < mbr.mbr_tamano - 1;cont++ ){
                                    fwrite(&relleno, 1, 1 , nuevodisco);
                                }*/
                                fclose(nuevodisco);
                                printf("Particion creada con exito \n");
                            } else {
                                printf("No hay espacio suficiente \n");
                                errores = true;
                                break;
                            }
                        }
                    }
                } else { //Existe al menos 1 particion
                    /********************************************************************************/
                    /*COMPARANDO PARA VER SI EXISTE UN ESPACIO DEL TAMAÑO DE LA PARTICION A INSERTAR*/
                    /********************************************************************************/
                    bool SiEntro = false; //Variable para saber que si encontro un espacio con ese tamaño
                    for (j = 0; j < totalparticiones + 1 && SiEntro == false; j++) {
                        if (j == 0) {//Comparando el espacio libre entre la mas cercana al mbr y el mbr
                            espaciolibre = (vecparticiones[j].part_start) - (sizeof (mbr) + 1);
                            if (espaciolibre >= tamanoreal) {
                                //printf("Espacio suficiente para esta particion \n");
                                particion.part_start = sizeof (mbr) + 1;
                                SiEntro = true;
                            }
                        } else if (j == totalparticiones) { //Si es la ultima particion del vector
                            espaciolibre = mbr.mbr_tamano - (vecparticiones[j - 1].part_start + vecparticiones[j - 1].part_size);
                            if (espaciolibre >= tamanoreal) {
                                //printf("Espacio suficiente para esta particion \n");
                                particion.part_start = (vecparticiones[j - 1].part_start + vecparticiones[j - 1].part_size + 1);
                                SiEntro = true;
                            }
                        } else {
                            espaciolibre = vecparticiones[j].part_start - (vecparticiones[j - 1].part_start + vecparticiones[j - 1].part_size + 1);
                            if (espaciolibre >= tamanoreal) {
                                //printf("Espacio suficiente para esta particion \n");
                                particion.part_start = vecparticiones[j - 1].part_start + vecparticiones[j - 1].part_size + 1;
                                SiEntro = true;
                            }
                        }
                    }
                    /********************************************/
                    /**COMPARANDO NOMBRES PARA EVITAR REPETIDOS*/
                    /*******************************************/
                    for (j = 0; j < totalparticiones && errores == false; j++) {
                        if (strcasecmp(vecparticiones[j].part_name, particion.part_name) == 0) {
                            printf("Ya existe una particion con ese nombre \n");
                            errores = true;
                        }
                        if (vecparticiones[j].part_type == 'e' || vecparticiones[j].part_type == 'E' && errores == false) {
                            struct_ebr auxnombres;
                            int fin = vecparticiones[j].part_size + vecparticiones[j].part_start;
                            bool ultima = false; //booleano para saber si llego a la ultima logica
                            for (i = vecparticiones[j].part_start; i < fin + 1 && errores == false && ultima == false; i++) {
                                fseek(disco, i, SEEK_SET);
                                fread(&auxnombres, sizeof (struct_ebr), 1, disco);
                                if (strcasecmp(auxnombres.part_name, name) == 0) {
                                    printf("Ya existe una particion con ese nombre \n");
                                    errores = true;
                                }
                                if (auxnombres.part_next == -1) {
                                    ultima = true;
                                }
                                i = auxnombres.part_start + auxnombres.part_size;


                            }
                        }
                    }
                    /*************************************************************/
                    /*INGRESANDO LA PARTICION EN EL VECTOR DE PARTICIONES DEL MBR*/
                    /*************************************************************/
                    if (SiEntro == true && errores == false) { //logro encontrar un espacio para esa particion y no hay errores
                        bool ingresada = false;
                        for (j = 0; j < 4 && ingresada == false; j++) {
                            if (mbr.mbr_particiones[j].part_status == '0') {
                                mbr.mbr_particiones[j] = particion;
                                ingresada = true;
                            }
                        }
                        /***************************************/
                        /******ESCRIBIENDO DE NUEVO EL MBR******/
                        /***************************************/
                        if (ingresada == true) { //si logro ingresarla
                            nuevodisco = fopen(path, "rb+");
                            fseek(nuevodisco, 0L, SEEK_SET);
                            fwrite(&mbr, sizeof (struct_mbr), 1, nuevodisco);
                            fclose(nuevodisco);
                            printf("Se creo la particion exitosamente \n");
                        }
                    }
                }
            } else { //Si ya existen 3 primarias
                printf("Este disco alcanzó el limite de particiones primarias [3] \n");
                error = true;
            }
        }
        /**************************************************************************/
        /***************PARA CREAR UNA EXTENDIDA***********************************/
        /**************************************************************************/
        else if (strcasecmp(type, "e") == 0){
            bool errores = false; //booleano para verificar que no exista ningun error antes de insertarla en el mbr
            int tamanodisponible;
            auxtype = 'e'; //Particion extendida
            particion.part_type = auxtype;
            if (cantExtendidas == 0) { //Si no hay ninguna extendida
                if (cantPrimarias == 0 && cantExtendidas == 0) { //El disco esta vacio
                    tamanodisponible = mbr.mbr_tamano - (sizeof (mbr) + 1);
                    bool EntraParticion = false;
                    for (i = 0; i < 4 && EntraParticion == false; i++) {
                        if (mbr.mbr_particiones[i].part_status == '0') {
                            particion.part_start = sizeof (mbr) + 1;
                            particion.part_size = tamanoreal;
                            if (tamanodisponible > tamanoreal) {
                                //printf("Si se insertara la particion \n");
                                EntraParticion = true;
                                mbr.mbr_particiones[i] = particion;
                                struct_ebr ebrprimero; //ebr que se pondra en la particion extendida
                                ebrprimero.part_fit = 'N';
                                strcpy(ebrprimero.part_name, "N");
                                ebrprimero.part_next = -1; //apuntador al siguiente ebr
                                ebrprimero.part_size = sizeof (struct_ebr);
                                ebrprimero.part_start = particion.part_start;
                                ebrprimero.part_status = '0';
                                /*SOBREESCRIBIENDO EL DISCO*/
                                int cont;
                                nuevodisco = fopen(path, "rb+");
                                fseek(nuevodisco, 0L, SEEK_SET);
                                fwrite(&mbr, sizeof (struct_mbr), 1, nuevodisco);
                                fseek(nuevodisco, (particion.part_start), SEEK_SET);
                                fwrite(&ebrprimero, sizeof (struct_ebr), 1, nuevodisco);
                                rewind(nuevodisco);
                                char relleno = '\0'; //variable con la que se va a rellenar el disco duro
                                /*for(cont = 0; cont < mbr.mbr_tamano - 1;cont++ ){
                                    fwrite(&relleno, 1, 1 , nuevodisco);
                                }*/
                                fclose(nuevodisco);
                            } else {
                                printf("No hay espacio suficiente \n");
                                errores = true;
                            }
                        }
                    }
                } else { //Existe al menos 1 particion
                    /********************************************************************************/
                    /*COMPARANDO PARA VER SI EXISTE UN ESPACIO DEL TAMAÑO DE LA PARTICION A INSERTAR*/
                    /********************************************************************************/
                    bool SiEntro = false; //Variable para saber que si encontro un espacio con ese tamaño
                    for (j = 0; j < totalparticiones + 1 && SiEntro == false; j++) {
                        if (j == 0) {//Comparando el espacio libre entre la mas cercana al mbr y el mbr
                            espaciolibre = (vecparticiones[j].part_start) - (sizeof (mbr) + 1);
                            if (espaciolibre >= tamanoreal) {
                                //printf("Espacio suficiente para esta particion");
                                particion.part_start = sizeof (mbr) + 1;
                                SiEntro = true;
                            }
                        } else if (j == totalparticiones) { //Si es la ultima particion del vector
                            espaciolibre = mbr.mbr_tamano - (vecparticiones[j - 1].part_start + vecparticiones[j - 1].part_size);
                            if (espaciolibre >= tamanoreal) {
                                //printf("Espacio suficiente para esta particion");
                                particion.part_start = (vecparticiones[j - 1].part_start + vecparticiones[j - 1].part_size + 1);
                                SiEntro = true;
                            }
                        } else {
                            espaciolibre = vecparticiones[j].part_start - (vecparticiones[j - 1].part_start + vecparticiones[j - 1].part_size + 1);
                            if (espaciolibre >= tamanoreal) {
                                //printf("Espacio suficiente para esta particion");
                                particion.part_start = vecparticiones[j - 1].part_start + vecparticiones[j - 1].part_size + 1;
                                SiEntro = true;
                            }
                        }
                    }
                    /********************************************/
                    /**COMPARANDO NOMBRES PARA EVITAR REPETIDOS*/
                    /*******************************************/
                    for (j = 0; j < totalparticiones; j++) {
                        if (strcasecmp(vecparticiones[j].part_name, particion.part_name) == 0) {
                            printf("Ya existe una particion con ese nombre \n");
                            errores = true;
                        }
                    }
                    /*************************************************************/
                    /*INGRESANDO LA PARTICION EN EL VECTOR DE PARTICIONES DEL MBR*/
                    /*************************************************************/
                    if (SiEntro == true && errores == false) { //logro encontrar un espacio para esa particion y no hay errores
                        bool ingresada = false;
                        for (j = 0; j < 4 && ingresada == false; j++) {
                            if (mbr.mbr_particiones[j].part_status == '0') {
                                mbr.mbr_particiones[j] = particion;
                                ingresada = true;
                            }
                        }
                        /***************************************/
                        /******ESCRIBIENDO DE NUEVO EL MBR******/
                        /***************************************/
                        if (ingresada == true) { //si logro ingresarla
                            struct_ebr ebrprimero; //ebr que se pondra en la particion extendida
                            ebrprimero.part_fit = 'N';
                            strcpy(ebrprimero.part_name, "N");
                            ebrprimero.part_next = -1;
                            ebrprimero.part_size = sizeof (struct_ebr);
                            ebrprimero.part_start = particion.part_start;
                            ebrprimero.part_status = '0';
                            nuevodisco = fopen(path, "rb+");
                            fseek(nuevodisco, 0L, SEEK_SET);
                            fwrite(&mbr, sizeof (struct_mbr), 1, nuevodisco);
                            fseek(nuevodisco, (particion.part_start), SEEK_SET);
                            fwrite(&ebrprimero, sizeof (struct_ebr), 1, nuevodisco);
                            rewind(nuevodisco);
                            fclose(nuevodisco);
                            printf("Se creo la particion exitosamente \n");
                        }
                    }
                }
            } else {
                printf("Este disco alcanzó el limite de particiones extendidas [1] \n");
                error = true;
            }
        }
        /**************************************************************************/
        /************************PARA CREAR UNA LOGICA*****************************/
        /**************************************************************************/
        else if (strcasecmp(type, "l") == 0) {
            bool errornombre = false;
            bool errores = false; //booleano para verificar que no exista ningun error antes de insertarla
            int tamanodisponible;
            auxtype = 'l'; //Particion logica
            int j;
            particion.part_type = auxtype;
            struct_particion auxExtendida;
            struct_ebr ebr; //ebr que se usara para insertar

            int contLogicas = 0; //contador para saber cuantos ebr hay
            int i;
            if (cantExtendidas > 0) { //validando que ya exista una extendida donde almacemarla
                for (i = 0; i < 4; i++) {
                    if (mbr.mbr_particiones[i].part_type == 'e') {
                        auxExtendida = mbr.mbr_particiones[i]; //almacenando la extendida en una auxiliar para trabajar
                        i = 4;
                    }
                }
                if (tamanoreal <= auxExtendida.part_size) { //validando que el tamaño de la logica no sobrepase el tamaño de la extendida
                    /********************************************/
                    /**COMPARANDO NOMBRES PARA EVITAR REPETIDOS*/
                    /*******************************************/
                    for (j = 0; j < totalparticiones && errornombre == false; j++) {
                        if (strcasecmp(vecparticiones[j].part_name, name) == 0) {
                            printf("Ya existe una particion con ese nombre \n");
                            errornombre = true;
                        }
                        if (vecparticiones[j].part_type == 'e' || vecparticiones[j].part_type == 'E' && errornombre == false) {
                            struct_ebr auxnombres;
                            int fin = vecparticiones[j].part_size + vecparticiones[j].part_start;
                            bool ultima = false;
                            for (i = vecparticiones[j].part_start; i < fin + 1 && errornombre == false && ultima == false; i++) {
                                nuevodisco = fopen(path, "rb");
                                fseek(nuevodisco, i, SEEK_SET);
                                fread(&auxnombres, sizeof (struct_ebr), 1, nuevodisco);
                                if (strcasecmp(auxnombres.part_name, name) == 0) {
                                    //printf("Ya existe una particion con ese nombre \n");
                                    errornombre = true;
                                }
                                if (auxnombres.part_next == -1) {
                                    ultima = true;
                                }
                                i = auxnombres.part_next - 1; //Se mueve i hasta donde termina la particion para leer otro bloque
                                fclose(nuevodisco);
                            }
                        }
                    }

                    if (errornombre == false) { //Si no encontro ninguna particion con el mismo nombre
                        bool errorespacio = false; //booleano para saber si existe espacio disponible
                        for (j = 0; j < totalparticiones && errorespacio == false; j++) {
                            if (vecparticiones[j].part_type == 'e' || vecparticiones[j].part_type == 'E' && errorespacio == false) {
                                struct_ebr auxnombres;
                                int fin = vecparticiones[j].part_size + vecparticiones[j].part_start;
                                bool ultima = false; //para saber si entro en la ultima particion
                                for (i = vecparticiones[j].part_start; i < fin + 1 && errorespacio == false && ultima == false; i++) {
                                    fseek(disco, i, SEEK_SET);
                                    fread(&auxnombres, sizeof (struct_ebr), 1, disco);
                                    if (auxnombres.part_next == -1) { //encontro la ultima
                                        ultima = true;
                                    }
                                    i = auxnombres.part_next - 1;
                                    contLogicas++;
                                }
                            }
                        }
                        int contposlogicas = 0;
                        struct_ebr vecLogicas[contLogicas]; //vector donde se van a almacenar las logicas
                        for (j = 0; j < totalparticiones; j++) {
                            struct_ebr auxlogicas;
                            if (vecparticiones[j].part_type == 'e' || vecparticiones[j].part_type == 'E') {
                                int fin = vecparticiones[j].part_size + vecparticiones[j].part_start;
                                bool ultima = false;
                                for (i = vecparticiones[j].part_start; i < fin + 1 && ultima == false; i++) {
                                    fseek(disco, i, SEEK_SET);
                                    fread(&auxlogicas, sizeof (struct_ebr), 1, disco);
                                    vecLogicas[contposlogicas] = auxlogicas;
                                    if (auxlogicas.part_next == -1) {
                                        ultima = true;
                                    }
                                    i = auxlogicas.part_next - 1;
                                    contposlogicas++;
                                }
                            }
                        }
                        bool sientro = false;
                        /*************************************************************/
                        /*Recorriendo el vector de logicas para realizar la insercion*/
                        /*************************************************************/
                        if (contLogicas > 1) {
                            for (j = 0; j < contLogicas && sientro == false; j++) {
                                if (j == (contLogicas - 1)) {//Comparando el espacio libre entre la ultima y el final de la extendida
                                    espaciolibre = (auxExtendida.part_start + auxExtendida.part_size) - (vecLogicas[j].part_start + vecLogicas[j].part_size);
                                    if (espaciolibre > tamanoreal) {
                                        vecLogicas[j].part_next = vecLogicas[j].part_start + vecLogicas[j].part_size + 1;
                                        ebr.part_fit = auxfit;
                                        strcpy(ebr.part_name, name);
                                        ebr.part_next = -1;
                                        ebr.part_size = tamanoreal;
                                        ebr.part_start = vecLogicas[j].part_start + vecLogicas[j].part_size + 1;
                                        ebr.part_status = '1';
                                        sientro = true;

                                    }
                                } else {
                                    espaciolibre = (vecLogicas[j + 1].part_start) - (vecLogicas[j].part_start + vecLogicas[j].part_size + 1);
                                    if (espaciolibre > tamanoreal) {
                                        vecLogicas[j].part_next = vecLogicas[j].part_start + vecLogicas[j].part_size + 1;
                                        ebr.part_fit = auxfit;
                                        strcpy(ebr.part_name, name);
                                        ebr.part_next = vecLogicas[j + 1].part_next;
                                        ebr.part_size = tamanoreal;
                                        ebr.part_start = vecLogicas[j].part_start + vecLogicas[j].part_size + 1;
                                        ebr.part_status = '1';
                                        sientro = true;
                                    }
                                }
                            }
                        } else {
                            espaciolibre = (auxExtendida.part_start + auxExtendida.part_size) - (vecLogicas[j].part_start + vecLogicas[j].part_size);
                            if (espaciolibre > tamanoreal) {
                                vecLogicas[j].part_next = vecLogicas[j].part_start + vecLogicas[j].part_size + 1;
                                ebr.part_fit = auxfit;
                                strcpy(ebr.part_name, name);
                                ebr.part_next = -1;
                                ebr.part_size = tamanoreal;
                                ebr.part_start = vecLogicas[j].part_start + vecLogicas[j].part_size + 1;
                                ebr.part_status = '1';
                                sientro = true;
                            }
                        }
                        /********************************************************/
                        /*Sobreescribiendo el disco despues de las validaciones*/
                        /*******************************************************/
                        if (sientro == true) { //Si encontro un espacio para la nueva particion
                            nuevodisco = fopen(path, "rb+");
                            fseek(nuevodisco, ebr.part_start, SEEK_SET);
                            fwrite(&ebr, sizeof (struct_ebr), 1, nuevodisco);
                            int x;
                            for (x = 0; x < contLogicas; x++) {
                                fseek(nuevodisco, vecLogicas[x].part_start, SEEK_SET);
                                fwrite(&vecLogicas[x], sizeof (struct_ebr), 1, nuevodisco);
                            }
                            rewind(nuevodisco);
                            fclose(nuevodisco);
                            printf("Se creo la particion correctamente \n");
                        } else {
                            printf("No se encontro un espacio disponible para crear esta particion \n");
                        }
                    } else {
                        printf("Ya existe una particion con ese nombre \n");
                    }
                }else {
                    printf("La particion que intenta crear sobrepasa el tamaño de la extendida \n");
                }
            } else {
                printf("No se puede crear una particion logica debido a que no existe una particion extendida \n");
            }
        }
    } else { //Si el disco es nulo, no existe ese disco
        printf("No existe el disco \n");
    }
}

int CantidadParticionesPrimarias(struct_mbr mbr) {
    int cantidad = 0, i;
    for (i = 0; i < 4; i++) {
        if ((mbr.mbr_particiones[i].part_type == 'P' || mbr.mbr_particiones[i].part_type == 'p') && mbr.mbr_particiones[i].part_status == '1') {

            cantidad++;
        }
    }
    return cantidad;
}

int CantidadParticionesExtendidas(struct_mbr mbr) {
    int cantidad = 0, i;
    for (i = 0; i < 4; i++) {
        if ((mbr.mbr_particiones[i].part_type == 'E' || mbr.mbr_particiones[i].part_type == 'e') && mbr.mbr_particiones[i].part_status == '1') {

            cantidad++;
        }
    }
    return cantidad;
}

int ContarSlash(char path[]) {
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
