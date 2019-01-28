#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <string.h>

struct Stack {
    int top;
    unsigned capacity;
    int* array;
};

struct Stack* createStack(unsigned capacity) {
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (int*)malloc(stack->capacity * sizeof(int));
    return stack;
}

int isFull(struct Stack* stack) {
    return stack->top == stack->capacity - 1;
}

int isEmpty(struct Stack* stack) {
    return stack->top == -1;
}

void push(struct Stack* stack, int item) {
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
    printf("%d pushed to the stack\n", item);
}

int pop(struct Stack* stack) {
    if (isEmpty(stack))
        return INT32_MIN;
    return stack->array[stack->top--];
}

void displayStack(struct Stack* stack) {
    for (int i = 0; i < stack->top + 1; i++) {
        printf("%d ", stack->array[i]);
    }
    printf("\n");
}

void peek(struct Stack* stack) {
    printf("%d\n", stack->array[stack->top]);
}

void split_input(char* input, char* command, char* argument) {
//    printf("String: %s\n", input);
    int j = 0;
    int flag = 0;
    for (int i = 0; i < strlen(input); i++) {
        if (input[i] == '\n') {
            break;
        } else if (input[i] == ' ' || input[i] == '\0') {
            flag = 1;
            j = 0;
        } else {
            if (flag == 0)
                command[j] = input[i];
            else
                argument[j] = input[i];
            j++;
        }
    }
//    printf("command: %s argument: %s\n", command, argument);
}

void child(int fds) {
    struct Stack* stack;

    while(1) {
        char* input = malloc(1024);
        char* command = malloc(1024);
        char* argument = malloc(1024);

        read(fds, input, 1024);
//        printf("String: %s\n", input);
        split_input(input, command, argument);
//        printf("command: %s argument: %s\n", command, argument);

        if (strcmp(command, "create") == 0) {
            stack = createStack(5);
        } else if (strcmp(command, "push") == 0) {
            push(stack, atoi(argument));
        } else if (strcmp(command, "display") == 0) {
            displayStack(stack);
        } else if (strcmp(command, "pop") == 0) {
            pop(stack);
        } else if (strcmp(command, "peek") == 0) {
            peek(stack);
        } else if (strcmp(command, "empty") == 0) {
            printf("%d\n", isEmpty(stack));
        } else if (strcmp(command, "stack_size") == 0) {
            printf("Stack size is: %d\n", stack->top + 1);
        } else if (strcmp(command, "print") == 0) {
            printf("Top: %d\n", stack->top);
        } else {
            printf("Wrong input. Terminating...\n");
            exit(EXIT_SUCCESS);
        }

        free(input);
        free(command);
        free(argument);
    }
}

void parent(int fds) {
    char * input = malloc(1024);

    while (1) {
        fgets(input, 1024, stdin);
        write(fds, input, 1024);
    }
}

int main() {
    int fds[2];
    if (pipe(fds) < 0)
        perror("pipe");

    int p_read = fds[0];
    int p_write = fds[1];

    int pid = fork();

    if (pid == 0) {
        child(p_read);
    } else {
        parent(p_write);
    }
}