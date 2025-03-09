int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Uso: ordena <método> <quantidade> <situação> [-P]\n");
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);

    switch (metodo) {
        case 1:
            printf("Método 1!\n");
            break;
        case 2:
            printf("Método 2!\n");
            break;
        case 3:
            printf("Método 3!\n");
            break;
        default:
            printf("Método inválido!\n");
            return 1;
    }

    return 0;
}
