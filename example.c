#define SPDEBUG_IMPLEMENTATION
#include "spdebug.h"

int main() {
    // Макрос для инициализации библиотеки и лог-файла.
    // Его использовать необязательно, но если использовать,
    // то лог-файл не будет создан.
    $init;

    // Вывод логов с разными уровнями.
    // Функции поддерживают форматирование вывода, как в функции printf.
    $debug("Hello World");
    int a = 2, b = 2;
    $info("%d + %d = %d", a, b, a + b);
    $warn("Some warning");
    $err("Some error");
    $fatal("Some fatal error");

    // Детальный вывод строк (с индексами, экранированием непечатных символов)
    const char* str = "Hello\tSPDebug2\n\xea";
    $vs(str);

    // Hex Dump объектов (автоматическое определление размера)
    int arr[5] = {67, 42, 52, 57, 376};
    $xdi(arr);

    return 0;
}
