#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <dirent.h> 
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h> 

extern char **environ;

void print_prompt() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("\x1b[38;2;255;128;0m%s>-\x1b[38;2;0;255;0m|EASY SHELL|->\x1b[38;2;255;255;255m", cwd);
    } else {
        perror("getcwd() error");
    }
}

void execute_cd(char *args[]) {
    if (args[1] == NULL) {
        printf("%s\n", getenv("PWD"));
    } else if (chdir(args[1]) != 0) {
        perror("myshell");
    } else {
        setenv("PWD", args[1], 1);
    }
}

void execute_dir(char *args[]) {
    DIR *dir;
    struct dirent *entry;
    if (args[1] == NULL) {
        args[1] = ".";
    }
    if ((dir = opendir(args[1])) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            printf("%s\n", entry->d_name);
        }
        closedir(dir);
    } else {
        perror("myshell");
    }
}

void execute_environ() {
    for (char **env = environ; *env != 0; env++) {
        printf("%s\n", *env);
    }
}

void execute_pause() {
    printf("Press Enter to continue...\n");
    while (getchar() != '\n');
}

void execute_quit() {
    exit(0);
}

void execute_echo(char *args[]) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
}

void execute_help() {
    printf("\n| \x1b[38;2;255;128;0mmyshell \x1b[38;2;255;255;255m- Простая оболочка.\n\n");
    printf("| \x1b[38;2;255;255;255mПоддерживаемые команды:\n\n");
    printf("| \x1b[38;2;255;128;0mcd <directory>\n");
    printf("| \x1b[38;2;255;128;0mclr\n");
    printf("| \x1b[38;2;255;128;0mdir <directory>\n");
    printf("| \x1b[38;2;255;128;0menviron\n");
    printf("| \x1b[38;2;255;128;0mecho <comment>\n");
    printf("| \x1b[38;2;255;128;0mhelp\n");
    printf("| \x1b[38;2;255;128;0mpause\n");
    printf("| \x1b[38;2;255;128;0mquit\n\n");
}

void execute_clr() {
    printf("\033[H\033[J");
}

void execute_external_command(char *args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        perror("myshell");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("myshell");
    } else {
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char *args[100];

    setenv("shell", argv[0], 1);

    while (1) {
        print_prompt();
        read = getline(&line, &len, stdin);
        if (read == -1) {
            break;
        }
        line[strcspn(line, "\n")] = 0;

        int i = 0;
        args[i] = strtok(line, " ");
        while (args[i] != NULL) {
            args[++i] = strtok(NULL, " ");
        }

        if (args[0] == NULL) {
            continue;
        } else if (strcmp(args[0], "cd") == 0) {
            execute_cd(args);
        } else if (strcmp(args[0], "clr") == 0) {
            execute_clr();
        } else if (strcmp(args[0], "dir") == 0) {
            execute_dir(args);
        } else if (strcmp(args[0], "environ") == 0) {
            execute_environ();
        } else if (strcmp(args[0], "echo") == 0) {
            execute_echo(args);
        } else if (strcmp(args[0], "help") == 0) {
            execute_help();
        } else if (strcmp(args[0], "pause") == 0) {
            execute_pause();
        } else if (strcmp(args[0], "quit") == 0) {
            execute_quit();
        } else {
            execute_external_command(args);
        }
    }
}