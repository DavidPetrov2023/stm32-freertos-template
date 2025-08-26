#include "FreeRTOS.h"
#include "task.h"

#include "uart_api.h" /* app uses only the clean API */
#include "uart_app.h"

#ifndef LINE_BUF_SIZE
#define LINE_BUF_SIZE 64u
#endif

#ifndef HELLO_PERIOD_MS
#define HELLO_PERIOD_MS 1000u
#endif

#ifndef UART_TASK_STACK
#define UART_TASK_STACK 256
#endif

#ifndef UART_TASK_PRIO
#define UART_TASK_PRIO (tskIDLE_PRIORITY + 2)
#endif

/* UART worker task: echoes completed lines and sends a periodic hello. */
static void UartTask(void *arg)
{
    (void) arg;

    uint8_t line[LINE_BUF_SIZE];
    size_t idx = 0;

    static const uint8_t hello_msg[] = "ahoj\r\n";
    TickType_t lastHello = xTaskGetTickCount();

    for (;;) {
        /* 1) Periodic output */
        if (xTaskGetTickCount() - lastHello >= pdMS_TO_TICKS(HELLO_PERIOD_MS)) {
            (void) g_uart2.write(hello_msg, sizeof(hello_msg) - 1);
            lastHello = xTaskGetTickCount();
        }

        /* 2) Read bytes with a small timeout */
        uint8_t ch;
        if (g_uart2.getc(&ch, 20)) {
            if (ch == '\r' || ch == '\n') {
                if (idx > 0) {
                    (void) g_uart2.write(line, idx);
                    (void) g_uart2.write((const uint8_t *) "\r\n", 2);
                    idx = 0;
                }
            } else if (idx < sizeof(line)) {
                line[idx++] = ch;
            } else {
                /* Optional overflow policy: reset index */
                idx = 0;
            }
        }
    }
}

void uart_app_start(void)
{
    /* Initialize the API (internally sets up StreamBuffer and driver). */
    g_uart2.init();

    /* Spawn the UART task. */
    (void) xTaskCreate(UartTask, "uart", UART_TASK_STACK, NULL, UART_TASK_PRIO, NULL);
}
