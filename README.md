# AYUD09 · UART

> Comunicación serial entre el PC y la Zybo: habilitar la **UART** del Zynq, abrir una terminal (la de Vitis o una externa) y usarla como consola para programar en C una **máquina expendedora** con estructuras, punteros y funciones.

Ayudante a cargo: **Ernesto Ferrante** — ernesto.ferrante@uc.cl

Esta ayudantía se divide en dos partes:

| | Qué es | Cuándo se hace |
| :--- | :--- | :--- |
| **Actividades previas** | Un *Block Design* con el Zynq7 Processing System (con la UART habilitada) y un AXI GPIO conectado a los 4 LEDs, y sobre él un proyecto *Hello World* en Vitis que imprime por la UART y se lee desde una terminal. | **Antes**, en su casa. |
| **Ejercicio propuesto** | Programar una **máquina expendedora** que se maneja completamente desde la consola serial: muestra los productos, vende, entrega el vuelto en monedas, indica con los LEDs el producto entregado y tiene un modo administrador con clave para reponer stock. | **Durante** la ayudantía. |

---

## 🎥 Antes de la ayudantía

Debes llegar a la sesión con las **actividades previas ya desarrolladas y funcionando en la tarjeta**: un proyecto de Vivado para la **Zybo Z7-10** con un *Block Design* que conecta el **Zynq7 Processing System** y un **AXI GPIO** para los 4 LEDs (`leds_4bits`), verificando que la **UART1** del Zynq esté habilitada (MIO 48..49, 115200 baudios); el archivo de *constraints* incluido y editado según ese *Block Design*; el *HDL Wrapper* y el *bitstream* generados y el hardware exportado. Ya en Vitis, un *Platform Project* creado con el `.xsa` y un *Application Project* a partir de la plantilla **Hello World** (no *Empty Application*, porque *Hello World* deja configurada la UART como entrada y salida estándar). Por último, abrir una terminal serial, la de Vitis (*Window → Show View → Terminal*) o una externa como **CoolTerm**, en el puerto COM de la Zybo con **115200 baudios, 8 bits de datos, sin paridad y 1 bit de parada (8N1)**, y comprobar que el mensaje del *Hello World* llega al PC.

Estas actividades están resueltas paso a paso en este video, grabado el año **2023 por el ex ayudante del curso Agustín Kamke**:

[![Video de la ayudantía 09](https://img.youtube.com/vi/3dn__gzqBcg/hqdefault.jpg)](https://youtu.be/3dn__gzqBcg)

**Apoyarse en el video es opcional.** Puede seguirlo completo, usarlo sólo para destrabar un punto puntual, o resolver las actividades por su cuenta: eso lo decide usted. Lo que no es opcional es llegar con el diseño andando y la terminal comunicándose con la Zybo, porque el tiempo de la ayudantía se destinará por completo al ejercicio propuesto.

---

## 📂 Material

| Archivo | Descripción |
| :--- | :--- |
| [AYUD09-UART.pdf](https://github.com/IEE2463-SEP/AYUD09-UART/blob/HEAD/AYUD09-UART.pdf) | Enunciado de la ayudantía: objetivos, funciones útiles para la consola, actividades de la máquina expendedora, ejemplo de ejecución y desafíos opcionales. |
| [Zybo-Z7-Master.xdc](https://github.com/IEE2463-SEP/AYUD09-UART/blob/HEAD/Zybo-Z7-Master.xdc) | Constraints de la tarjeta, con los 4 LEDs (`leds[0]` a `leds[3]`) habilitados. La UART va por pines MIO del Zynq, así que no aparece en este archivo. |
| [AYUD09.zip](https://github.com/IEE2463-SEP/AYUD09-UART/blob/HEAD/AYUD09.zip) | Proyecto de Vivado de las actividades previas, con el *Block Design* (Zynq7 con UART1 a 115200 y AXI GPIO a los LEDs), el *bitstream* y el `.xsa` ya generados. |
| [ayud9_wrapper.xsa](https://github.com/IEE2463-SEP/AYUD09-UART/blob/HEAD/ayud9_wrapper.xsa) | Hardware exportado de ese proyecto, para crear directamente el *Platform Project* en Vitis. |
| [helloworld.c](https://github.com/IEE2463-SEP/AYUD09-UART/blob/HEAD/helloworld.c) | Ejercicio de una versión anterior de esta ayudantía (un banco digital con *digipass* en los LEDs). No es la solución de la máquina expendedora, pero sirve de ejemplo de `struct`, `scanf`, `strcmp` y `XGpio_DiscreteWrite` sobre la consola UART. |
| [CoolTermWin64Bit.zip](https://github.com/IEE2463-SEP/AYUD09-UART/blob/HEAD/CoolTermWin64Bit.zip) | Terminal serial externa para Windows, alternativa a la terminal de Vitis. |

---

## 🧪 Durante la ayudantía

El **ejercicio propuesto** es el trabajo de la sesión, y fue propuesto y desarrollado por el ayudante de este semestre, **Ernesto Ferrante**. La idea es programar una **máquina expendedora** que se controla completamente desde la consola serial. Se usa exactamente el mismo hardware de las actividades previas (el AXI GPIO de los LEDs en el canal 1 y la UART como entrada y salida estándar), así que **no es necesario volver a Vivado**:

**1. Estructura y datos.** Una `struct Producto` con `codigo` (string de largo 4), `nombre` (string de largo 16), `precio` y `stock`, y un arreglo global con 4 productos: `A1` Agua $800 (3), `A2` Bebida $1200 (2), `B1` Galletas $950 (1) y `B2` Chocolate $1000 (0). La función `void mostrarProductos(void)` los imprime en la consola.

**2. Búsqueda.** `int buscarProducto(char *codigo)` retorna el índice del producto con ese código, o −1 si no existe.

**3. Compra.** `void comprar(void)` pide el código y el pago, avisa si el producto no existe, está agotado o el pago no alcanza, y si todo está bien descuenta el stock, entrega el producto y calcula el vuelto.

**4. Vuelto.** `void darVuelto(int vuelto)` desglosa el vuelto en la menor cantidad posible de monedas de $500, $100, $50 y $10.

**5. Entrega con LEDs.** `void entregar(int i)` hace parpadear 5 veces el LED del producto `i` (250 ms encendido, 250 ms apagado) e imprime un punto con `outbyte` en cada parpadeo: `Entregando Agua ..... listo!`

**6. Modo administrador.** `void reponer(void)` pide la clave `"1234"` y luego el código y la cantidad **en una misma línea** (por ejemplo `B2 5`) con un solo `scanf`, valida ambos y suma la cantidad al stock.

**7. Menú principal.** El `main` inicializa el GPIO, apaga los LEDs y repite en un ciclo infinito la lista de productos y el menú. La opción se lee con `inbyte()`, de modo que basta presionar `1` (comprar) o `2` (reponer) sin Enter; cualquier otra tecla muestra `Opcion invalida`, y los `\r` y `\n` que quedan de un Enter anterior se ignoran.

> 💡 `xil_printf` no imprime decimales (`%f`) y el salto de línea en la terminal es `\r\n`. Al leer un string con `scanf(" %3s", texto)` el arreglo se pasa sin `&`. Para usar `usleep` hay que incluir `"sleep.h"`.

Si termina antes, el enunciado propone tres **desafíos opcionales**: mostrar en binario en los LEDs el stock del último producto comprado, bloquear el modo administrador por 10 s tras 3 claves incorrectas, y agregar una opción `3` con el total recaudado por la máquina.

### 📤 Entrega y bonificación

El ejercicio propuesto se sube a **Canvas hasta el viernes 23 de octubre a las 14:50**. Entregarlo dentro de plazo otorga **una décima (+0,1)** en la nota del **Proyecto 2**.

---

<sub>IEE2463 · Sistemas Electrónicos Programables</sub>
