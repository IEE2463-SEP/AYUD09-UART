#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xil_types.h"
#include "sleep.h"
#include <stdlib.h>
#include <time.h>
#define LEDS_DEVICE_ID	XPAR_AXI_LEDS_0_DEVICE_ID
XGpio LEDInst;
int status;
char rut_entrar[12];
char contrasena_entrar[12];
int digipass_entrar;
// Estructura para representar una cuenta bancaria
struct CuentaBancaria {
    char rut[12];
    char contrasena[12];
    int saldo;
};

// Función para realizar una transferencia
int realizarTransferencia(struct CuentaBancaria* cuenta, int monto, int claveDigipass) {
    if (cuenta == NULL || claveDigipass != digipass_entrar) {
        xil_printf("Error: Clave del Digipass incorrecta.\n");
        return -1.0; // Transferencia fallida
    }

    if (cuenta->saldo < monto) {
        xil_printf("Error: Saldo insuficiente para realizar la transferencia.\n");
        return -1.0; // Transferencia fallida
    }

    // Verificar el destinatario (aquí puedes agregar más lógica)
    cuenta->saldo -= monto;
    xil_printf("Transferencia exitosa. Nuevo saldo: %.2f\n", cuenta->saldo);
    return cuenta->saldo;
}

int main() {
    init_platform();

    // Inicializar GPIO para controlar LEDs
    status = XGpio_Initialize(&LEDInst, LEDS_DEVICE_ID);
    if (status != XST_SUCCESS) return XST_FAILURE;

    // Estructura de la cuenta del usuario
    struct CuentaBancaria miCuenta;
    strcpy(miCuenta.rut, "12345678-9");
    strcpy(miCuenta.contrasena, "12345");
    miCuenta.saldo = 10000;



    while (1) {
    	xil_printf("Bienvenido al banco digital 'bitis'.\n");
    	xil_printf("Rut:\n");
    	scanf("%s", &rut_entrar);
    	xil_printf("contraseña:\n");
    	scanf("%s", &contrasena_entrar);
    	if (strcmp(rut_entrar, miCuenta.rut) == 0 && strcmp(contrasena_entrar, miCuenta.contrasena) == 0){
    		while (1) {
    			int opcion;
    			xil_printf("Saldo: %d\n", miCuenta.saldo);
    			xil_printf("Opciones:\n");
    			xil_printf("1- Realizar transferencia\n");
    			xil_printf("2- Cerrar sesión\n");
    			scanf("%d", &opcion);
    			if (opcion == 1) {
    				char rutDestino[12];
    				int monto;
    				xil_printf("Ingrese el RUT de destino:\n ");
    				scanf("%s", &rutDestino);
    				xil_printf("Ingrese el monto de la transferencia:\n ");
    				scanf("%d", &monto);
    				int claveDigipass = rand()%16;
    				XGpio_DiscreteWrite(&LEDInst, 1, claveDigipass);
    				xil_printf("Ingrese la clave del Digipass en formato decimal:\n ");
    				scanf("%d", &digipass_entrar);
    				int nuevoSaldo = realizarTransferencia(&miCuenta, monto, claveDigipass);
    				if (nuevoSaldo != -1.0) {
    					miCuenta.saldo = nuevoSaldo;
    					}
    				}
    			else if (opcion == 2) {
    				xil_printf("Sesión cerrada. Gracias por utilizar el banco digital 'bitis'.\n");
    				break;
    			}
    			else {
    				xil_printf("Opción no válida. Por favor, elija una opción válida.\n");
    			}
    		}
    	}
    }
    cleanup_platform();
    return 0;

}

